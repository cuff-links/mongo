# Deprecation Note!

There is a [newer sharding internals guide](https://github.com/mongodb/mongo/blob/master/src/mongo/db/s/README.md) that lives in the codebase itself that was written in spring 2020 as of the v4.4 release of MongoDB.

The guide below was written in spring 2017 as of the v3.4 release of MongoDB.

# Sharding

This document provides MongoDB server engineers and community contributors with a "mental map" of sharding components and code.

A reader should have a [basic understanding of sharding](https://docs.mongodb.com/manual/sharding/) from a user's point of view.

**Links to code** reference the [3.4 release of MongoDB](https://docs.mongodb.com/manual/release-notes/3.4/).

**Links to online documentation** reference the [official MongoDB docs](https://docs.mongodb.com/manual/).

**Links to internal Google documents** may be only accessible by members of MongoDB's engineering team.

## Overview

The main goals of sharding are to:
* allow collections to be partitioned and distributed across shards
* allow a cluster to be scaled by adding or removing shards

Sharding also provides some features, such as:
* auto-balancing and auto-splitting of chunks
* designating [zones](https://docs.mongodb.com/manual/core/zone-sharding/) for chunk ranges

In broad strokes, the architecture as of v3.4 is as follows:

### routing table

The routing table describes where in the cluster (that is, on which shards) data in a collection is located.

Unsharded collections are always placed on the [primary shard](https://docs.mongodb.com/manual/core/sharded-cluster-shards/#primary-shard) for their database. For routing to unsharded collections, the routing table contains a mapping of **databases to their primary shards**.

For sharded collections, which are subdivided into chunks, the routing table contains a mapping of **chunks to shards**.

### config server

The config server is a special replica set that stores the **authoritative** copy of the routing table.

The config server also stores the authoritative copy of other metadata about the cluster, such as the list of databases, sharded collections, and shards, as well as settings for the auto-balancer, auto-split, and zones.

See the full list of information that config servers are authoritative for [here](https://docs.mongodb.com/manual/reference/config-database).

The data on the config server is not itself partitioned.

### mongos

Mongos nodes are router nodes which have no persisted state.

A mongos node is responsible for routing reads and writes to shards, and forwarding requests to modify metadata to the config servers.

Mongos nodes **cache** the routing table and shard list in memory so they can route reads and writes.

### shards

Shards are nodes that store the actual data.

Shards also **cache** the routing table and shard list in memory in order to perform chunk migrations, [sharded aggregations](https://docs.mongodb.com/manual/core/aggregation-pipeline-sharded-collections/), and [sharded mapReduce](https://docs.mongodb.com/manual/core/map-reduce-sharded-collections/).

## Caching the Routing Table: Databases, Collections, Chunks

All three types of nodes (mongos, shards, and config servers) cache the routing table in memory. (Only the config server, which is authoritative for the routing table, additionally stores the routing table information in the on-disk collections [config.databases](https://docs.mongodb.com/manual/reference/config-database/#config.databases), [config.collections](https://docs.mongodb.com/manual/reference/config-database/#config.collections), and [config.chunks](https://docs.mongodb.com/manual/reference/config-database/#config.chunks)).

A routing table cache becomes stale (and must be refreshed from the authoritative copy on the config servers) when:
* a sharded collection is dropped (and perhaps recreated with the same name)
* a collection becomes sharded or unsharded
* a chunk in the collection migrates from one shard to another

In code, the `CatalogCache` is the root of the hierarchy of the in-memory routing table cache.

The `CatalogCache` contains a map of database names to `DBConfig` objects. Each `DBConfig` object contains a map of collection names to `CollectionInfo` objects. Finally, each `CollectionInfo` object contains a `ChunkManager`, which has a map of chunks to shards:

`CatalogCache` -> `DBConfig` -> `CollectionInfo` -> `ChunkManager`

For the first two ways of becoming stale, the `CatalogCache` contains methods to invalidate and reload an entire `DBConfig`, which includes a full reload of the `ChunkManager`.
For the third case, the `DBConfig` contains methods to perform an incremental reload of the `ChunkManager`.

The `CatalogCache` uses methods on the `ShardingCatalogClient` to load data from the config server, and the `ChunkManager` uses the `ConfigDiffTracker` to perform the incremental chunk reload logic.

**Classes:** [CatalogCache](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/catalog/catalog_cache.h), [DBConfig](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/config.h), [CollectionInfo](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/config.cpp#L52-L58), [ChunkManager](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/chunk_manager.h), [ShardingCatalogClient](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/catalog/sharding_catalog_client.h), [ConfigDiffTracker](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/chunk_diff.h)

#### Additional Chunk Cache on Shards

On each shard, there is an additional in-memory cache of the chunk metadata **just for the chunks that the shard owns**.

While the general-purpose routing table is used for routing reads and writes, this additional cache of chunks is used by shards to:
* filter incoming sharded requests
* after donating a chunk, prevent data that is still in use by active requests from being deleted

In code, the root of this additional cache is `ShardingState`, which more generally performs sharding initialization on shards (see below).

The `ShardingState` contains a map of collection name to `CollectionShardingState`. The `CollectionShardingState` has a `MetadataManager`, which tracks the current and still-in-use chunks through instances of `CollectionMetadata`.

`ShardingState` -> `CollectionShardingState` -> `MetadataManager` -> `CollectionMetadata`

**Classes:** [ShardingState](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/sharding_state.h), [CollectionShardingState](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/collection_sharding_state.h), [MetadataManager](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/metadata_manager.h), [CollectionMetadata](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/collection_metadata.h)

## Sharded Commands and Shard Versions

A shard's "shardVersion" **for a collection** is equal to the ChunkVersion of the highest chunk in that collection the shard owns. (The shard has a shardVersion for each sharded collection it owns chunks for).

When a node sends a read or write request to a shard, it must include the shardVersion it believes the shard is at (or "ChunkVersion::UNSHARDED" if it believes the collection is unsharded).

For requests that include a shardVersion ("versioned" requests), the shard compares the sent version with what it believes its own version to be through the [\_checkShardVersionOk](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/collection_sharding_state.cpp#L362-L452) method on the `CollectionShardingState`.

If the sent version is **lower** than the shard's own version, the shard returns an error indicating that the sent version was stale. On receiving a stale version error, a sender refreshes its routing table, re-routes for the operation (if necessary; perhaps the routing table change means a different set of shards should be routed to), and re-sends the requests.

If the sent version is **higher** than the shard's own version, the shard refreshes its routing table.

**Classes:** [ChunkVersion](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/chunk_version.h)

<!-- **Internal Documents:** [Shard Versioning Story](https://docs.google.com/a/10gen.com/document/d/1JaClNLh5ENH51X_4xQKUc9E9mNiZHSfRErawEsnROJI/edit?usp=sharing)
-->

## Chunk Migration Protocol

Most importantly, **a migration causes the shardVersion (for the collection of the migrated chunk) to be incremented** on both the donor and recipient shards (see details in the "migration protocol summary" below). Thus, when a node contacts a shard that has just participated in a migration, the shard will return a stale version error, causing the sending node to refresh its routing table.

A moveChunk command for a chunk is issued against the shard that currently owns the chunk (the "donor" shard).

The moveChunk command specifies the desired _recipient_ shard. On receiving a moveChunk command, the donor shard coordinates with the recipient shard to transfer the data. (**The sender of the moveChunk command does not contact the recipient shard at all**).

On the donor shard, migration logic is handled by the `MigrationSourceManager`, and on the recipient shard, it is handled by the `MigrationDestinationManager`.

#### Migration Protocol Summary

See the linked documents for full diagrams of the commands involved.

In general terms, first, the donor shard sends the `_recvChunkStart` command to the recipient.

The recipient then repeatedly calls the `_migrateClone` command on the donor to "bulk clone" the documents in the chunk range. It continues until it receives an empty response.

Meanwhile, the donor shard repeatedly calls the `_recvChunkStatus` command on the recipient to find out when the donor has finished the bulk clone.

Once it sees the recipient is ready, the donor enters the "critical section." This means the donor does not accept any reads or writes.

After entering the "critical section," the donor sends `_recvChunkCommit` to the recipient.

The donor may have accepted some writes between the final `_migrateClone` and entering the critical section. So, the recipient repeatedly calls `_transferMods` to clone any such remaining writes in the chunk's range. After this, the recipient responds "OK" for `_recvChunkCommit`, and the recipient's end of the migration is complete.

On hearing "OK" for `_recvChunkCommit`, the donor sends the `_configsvrCommitChunkMigration` command to the config server to update the **authoritative** routing table.

The routing table is updated by bumping the ChunkVersion of the donated chunk, as well as some arbitrary chunk on the donor shard (in order to bump the shardVersions of both the donor and recipient).

Once the donor hears "OK" from the config server, it exits the critical section and **refreshes its routing table**. The migration is complete.

The **recipient shard only refreshes its routing table lazily**, when some node sends a request to it with a higher shardVersion than it knows about.

**Classes:** [MigrationSourceManager](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/migration_source_manager.h), [MigrationDestinationManager](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/migration_destination_manager.h)

**Commands:** [\_recvChunkStart](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/migration_destination_manager_legacy_commands.cpp#L59-L187), [\_migrateClone](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/migration_chunk_cloner_source_legacy_commands.cpp#L118-L180), [\_recvChunkStatus](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/migration_destination_manager_legacy_commands.cpp#L189-L227), [\_transferMods](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/migration_chunk_cloner_source_legacy_commands.cpp#L182-L225), [\_recvChunkCommit](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/migration_destination_manager_legacy_commands.cpp#L229-L273), [applyOps](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/commands/apply_ops_cmd.cpp)

**External Documentation:** [moveParanoia](https://docs.mongodb.com/manual/reference/program/mongod/#cmdoption--moveParanoia)

**Internal Documents:** [Authoritative Chunk Info on Shards](https://docs.google.com/a/10gen.com/document/d/1cDr3e3UMPEdUaluNju7NpqXfZvYromuKHrykep99JAc/edit?usp=sharing), [Proposal for Shard-Negotiated Migrations](https://docs.google.com/a/10gen.com/presentation/d/1WezsY1207mj_o7hHZZPpAKz75TDHVswb71Kx66b_oYw/edit?usp=sharing) (good diagrams of the migration protocol)

## The RangeDeleter

The migration protocol does not necessarily cause the data on the donor shard to be synchronously deleted after the migration completes (unless it is opted-in via the `_waitForDelete` field in the [moveChunk command](https://docs.mongodb.com/manual/reference/command/moveChunk)). This is because it would require waiting for active queries using data in the donated range to complete.

Instead, the `RangeDeleter` runs in a separate thread on the shard primary. The `RangeDeleter` deletes data in the donated chunk's range once all cursors that were active at the start of the migration have been exhausted.

If the shard primary crashes or fails over before the RangeDeleter has a chance to delete a range, the documents in that range remain "orphaned" on the donor shard. This is because the RangeDeleter does not persist or replicate the ranges it has yet to clean.

However, there is a `cleanupOrphaned` command that can be issued against the new primary to delete any such orphaned documents.

**Classes:** [RangeDeleter](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/range_deleter.h)

**Commands:** [cleanupOrphaned](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/cleanup_orphaned_cmd.cpp)

**Internal Documents:** [Improved Range Deleter design](https://drive.google.com/open?id=1OPAtZasLskYpbWJNuAmACE7LjtPekmyJ4tsFurQkK7Y)

## The Balancer

In order to automatically enforce zone boundaries and keep the chunks approximately balanced across shards, there is a `Balancer` on the config server that runs in its own thread.

The `Balancer` uses the `BalancerPolicy` class to determine which chunks to move based on:
* zone boundaries
* shards that need to be 'drained' of chunks because they have been requested to be removed
* whether the number of chunks per shard is imbalanced

The `Balancer` uses the `MigrationManager` class to actually schedule migrations against shards.

As of v3.4, migrations are allowed to happen in parallel as long as they do not involve the same shard (as either the donor or recipient). Thus, at most numShards/2 migrations can be active at a time. The `Balancer` does not keep track of which shards are participating in active migrations; it is the shard's responsibility to reject a migration request (as either the donor or recipient) if it is currently participating in another migration.

Any requests from external clients to do a manual migration are forwarded by mongos to the config server, where they either "join" an existing migration if an identical one exists, or cause the migration to be initiated by the `Balancer`.

**Classes:** [Balancer](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/balancer/balancer.h), [BalancerPolicy](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/balancer/balancer_policy.h), [MigrationManager](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/balancer/migration_manager.h)

**Commands:** [balancerStop, balancerStart, balancerStatus](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/commands/cluster_control_balancer_cmd.cpp), [\_configsvrMoveChunk](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/config/configsvr_move_chunk_command.cpp)

**Internal Documents:** [Parallel Autobalancing design](https://drive.google.com/open?id=1LFRaPUflytwsglhQNsIwqViqAsNRGS-yymbMPLmNn44), [v3.4 Migration and moveChunk changes](https://docs.google.com/a/10gen.com/document/d/1OXH-kqAPZE1vIhF1trjgbKozpE-4w6N4LWVJTrrDl8o/edit?usp=sharing), [Shard Zoning design](https://drive.google.com/open?id=1qxvzNVqBxnWUqfjMACCH5s1Hb6ZHdgOVO8lX0-cL-Y0)

## Chunk Auto-Splitting

As of v3.4, on every insert and update, mongos checks whether a chunk has exceeded the [chunkSize setting](https://docs.mongodb.com/manual/tutorial/modify-chunk-size-in-sharded-cluster/). If so, it synchronously (on the thread that did the write) splits the chunk on the shard.

Additionally, when shardCollection is called on an empty collection, [the collection is "pre-split" into chunks](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/commands/cluster_shard_collection_cmd.cpp#L453-L503).

Note that there is **no automatic chunk merging** mechanism.

**Internal Documents:** [Moving Auto Split Logic to Mongod design](https://drive.google.com/open?id=1ZPVukksl-oHJQtfDn5fjdLgxM3BgklE_8RKIP9UzQxg)

## Caching the Shard List

Similarly to the routing table, all three types of nodes (mongos, shards, and config servers) cache the shard list in memory. (Only the config server, which is authoritative for the shard list, additionally stores the shard list in [an on-disk collection](https://docs.mongodb.com/manual/reference/config-database/#config.shards)).

In code, the in-memory shard list is represented by the `ShardRegistry`. The `ShardRegistry` maintains a map of shardId or hostname to `Shard` objects.

`Shard` objects can be used to run single synchronous commands against shards. They also encompass retry logic on retriable errors for idempotent and nonidempotent operations.

`Shard` objects use a `RemoteCommandTargeter` to enforce [readPreference](https://docs.mongodb.com/manual/core/read-preference/).

For replica set shards, `RemoteCommandTargeter` uses a `ReplicaSetMonitor` to keep an up-to-date view of the replica set hosts. The `ReplicaSetMonitor` periodically pings the replica set hosts to find out about a new primary, added secondaries, or unreachable hosts.

`ShardRegistry` -> `Shard` -> `RemoteCommandTargeter` -> `ReplicaSetMonitor`

As of v3.4, there is no incremental refresh of the `ShardRegistry`. The `ShardRegistry` is always fully reloaded when it is refreshed, meaning the `Shard` and `RemoteCommandTargeter` instances get thrown out and recreated each time. Therefore, because building a `ReplicaSetMonitor` is a heavy-weight task, a `ReplicaSetMonitor` is not owned by the `Shard` that uses it. Instead, `ReplicaSetMonitors` are registered with a separate `ReplicaSetMonitorManager` so they can exist across `ShardRegistry` reloads.

Finally, running commands against shards through the `Shard` class synchronously updates the shard's `ReplicaSetMonitor` with the response status. Some, but not all, other code paths also call this updateReplSetMonitor() hook (one place is the [find command path](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/query/async_results_merger.cpp#L492)).

**Classes:** [ShardRegistry](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/client/shard_registry.h), [Shard](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/client/shard.h), [RemoteCommandTargeter](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/client/remote_command_targeter.h), [ReplicaSetMonitor](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/client/replica_set_monitor.h), [ReplicaSetMonitorManager](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/client/replica_set_monitor_manager.h)

## Sharding Metadata Commands

Sharding metadata commands broadly include dropping/creating databases, collections, and indexes; sharding/unsharding collections; and adding/removing shards. There are also some read-only metadata operations, such as listCollections, listDatabases, and listIndexes.

Most metadata operations that modify metadata are forwarded by mongos to the config server:

Prior to v3.4, the business logic for **all** metadata operations were carried out by mongos using direct reads and writes to the config database on the config servers.

This required mongos to take a "distributed lock", which was stored on the config server, to ensure conflicting operations from another mongos could not proceed. The "distributed lock" is inefficient, because if the mongos that holds it crashes, all other mongos's must wait for the lock to time out before overtaking the lock. It is also complicated, because it requires the mongos holding the lock to continuously ping the lock to prove the mongos's liveness.

In v3.4, the business logic for **some** of these operations was moved to the config server, so that dependence on the distributed lock can eventually be removed. Mongos's were made to just forward the request to the config server.

For the operations that have been moved to the config server, the business logic is encapsulated in the `ShardingCatalogManager`. For operations still remaining on mongos, the business logic is in the `ShardingCatalogClient`.

It is intended that in the 3.6 and 3.8 releases, all the metadata operations will be moved to the config server.

**Classes:** [ShardingCatalogManager](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/catalog/sharding_catalog_manager.h), [ShardingCatalogClient](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/catalog/sharding_catalog_client.h)

**Internal Documents:** [Move All Metadata Commands to the Config Server](https://docs.google.com/a/10gen.com/document/d/1EUC55ZT7xTfeWGQk1WxsbS1r-bEsW0VEcXa-R1PdI00/edit?usp=sharing), [Catalog Classes & Server Metadata Access](https://docs.google.com/a/10gen.com/document/d/1DxOkQEd1J9K-U2nG55gTNAYXN7u_ktboTjvnWUwdWs0/edit?usp=sharing)

## Sharding Component Initialization 

Because many sharding components (e.g., components used to maintain the routing table and shard list caches) are used by all three types of nodes (mongos, shards, and config servers), they are initialized on each type of node.

These common components are mostly owned by the `Grid` or by other sharding components.

Mongos nodes initialize components on the `Grid` at startup, through the [initializeSharding() method](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/server.cpp#L173-L217).

Config servers initialize components on the `Grid` as well as start the Balancer thread [at startup](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/db.cpp#L757-L763).

Shard nodes initialize components on the `Grid` only once they receive a shardIdentity document, which is inserted in their admin.system.version collection by the config server when the shard is added to the cluster. Sharding initialization is [triggered through an OpObserver](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/s/collection_sharding_state.cpp#L73-L74) for inserts to shards' admin.system.version collection.

If a shard is started up with a shardIdentity document already on disk, it initializes components on the `Grid` [at startup](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/db/db.cpp#L736-L738).

**Classes:** [Grid](https://github.com/mongodb/mongo/blob/r3.4.2-rc0/src/mongo/s/grid.h)

**Internal Documents:** [All Nodes Shard Aware Usability](https://docs.google.com/a/10gen.com/document/d/1_ContYLvjxIwXSnazDgaeqbPuNbFcb5swzcVNBIorrs/edit?usp=sharing)