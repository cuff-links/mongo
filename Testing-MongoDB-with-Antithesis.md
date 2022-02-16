## What is Antithesis?

Antithesis is a third party vendor with an environment that can perform superior network fuzzing compared to existing `resmoke` infrastructure (ie: `ContinuousStepdown` hook). Antithesis can spin up various multi-container MongoDB topologies in their environment and run test suites. **Real network fuzzing** is performed between any subset of containers while a test suite runs, and any detected server crashes are reported back.

## ContinuousStepdown vs. Antithesis

The `ContinuousStepdown` hook works by sending a `replSetStepDown` command to all repl sets in a cluster based on some time interval. This is loosely similar to the cluster experiencing network faults.

On the other hand, Antithesis provides an environment that can trigger **real network faults**. 

These are the current, configurable fault scenario settings in Antithesis:
* **Fuzzer Input (input)** : every ~1 second(s) the fuzzer provided a new input
* **Packet Latency (background)** : 20 ms (mean) and a standard deviation of 50
* **Packet Drop (background)** : probability of 0.05
* **Network Partition (event-based)** : every ~4 seconds the services were partitioned in up to 2 groups for a duration minimum of 0.3 seconds and maximum of 3 seconds.
* **Network Teardowns (event-based)** : every ~0.3 seconds service(s) were disconnected and reconnected in a different order based on a duration minimum of 0.5 seconds and maximum of 2.5 seconds. Each service had a probability of 0.033 to be included in the event
* **Packet dropping, queuing, and latency (event options)** : Partitions & Teardowns randomly choose between dropping packets, queuing packets for release at the end of the event, or slowing packets with up to a mean of 300 ms of latency at a standard deviation of 300

## concurrency_sharded_with_stepdowns_and_balancer

The existing `concurrency_sharded_with_stepdowns_and_balancer` test suite uses the `ContinuousStepdown` hook, meaning the test suite aims to simulate network failures. This makes it a prime candidate for testing in Antithesis. The [antithesis_suite script](https://github.com/10gen/mongo/blob/ad5977c28ad442c22e9b4a9b2c7198fd3129f046/buildscripts/antithesis_suite.py) is a script that generates antithesis compatible suites for all of the existing suites. The generated suites are all prefixed with `antithesis_[suite_name]` & these are the suites that can run in Antithesis. Currently, we run the antithesis compatible version of this test suite against a 2 shard MongoDB cluster. Antithesis uses the sharded cluster [docker-compose.yml](https://github.com/10gen/mongo/blob/ad5977c28ad442c22e9b4a9b2c7198fd3129f046/buildscripts/antithesis/topologies/sharded_cluster/docker-compose.yml) file to spin up the system under test.

_System under test based off of [docker-compose.yml](https://github.com/10gen/mongo/blob/ad5977c28ad442c22e9b4a9b2c7198fd3129f046/buildscripts/antithesis/topologies/sharded_cluster/docker-compose.yml)_
![Antithesis Example](https://user-images.githubusercontent.com/62212154/154159775-e0edaa58-c88d-4464-8c64-7a778bd3f7cd.jpg)

The workload container contains `resmoke` and is the entry point to run the test suite. The workload container is excluded from network fuzzing. Antithesis can run the following command from the workload container to start a test run:

```
buildscripts/resmoke.py run --suite antithesis_concurrency_sharded_with_stepdowns_and_balancer --shellConnString "mongodb://mongos:27017"
```

## Results

Running the `concurrency_sharded_with_stepdowns_and_balancer` test suite in antithesis has surfaced novel failures that were not detected using the `ContinuousStepdown` hook. Additionally, bugs that have historically been difficult to reproduce have been reproduced consistently in Antithesis. Antithesis has shown improved code coverage and improved rate of discovering new failures. The goal is to continue to find use cases for Antithesis and expand usage.

## How to leverage Antithesis

See the [README.md](https://github.com/10gen/mongo/blob/ad5977c28ad442c22e9b4a9b2c7198fd3129f046/buildscripts/antithesis/README.md) for more information on how to leverage Antithesis for testing today.