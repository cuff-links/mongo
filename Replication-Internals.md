# Replication in MongoDB

Replication is the set of systems used to continuously copy data from a primary server to secondary servers so if the primary server fails a secondary server can take over soon. This process is intended to be mostly transparent to the user, with drivers taking care of routing queries to the requested replica. Replication in MongoDB is facilitated through [**replica sets**](https://docs.mongodb.com/manual/replication/). 

**The architecture guide has moved to mongo repo and will be maintained there. Please refer to the [README](https://github.com/mongodb/mongo/blob/master/src/mongo/db/repl/README.md) for an up-to-date version.**