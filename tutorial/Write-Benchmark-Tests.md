MongoDB has a function level benchmarking library that is based on [Google Benchmark v1.3.0]
(https://github.com/google/benchmark/tree/v1.3.0), with a few changes to provide a more familiar 
interface for MongoDB developers. Compared with system-level performance tests that rely on 
full-fledged MongoDB servers or even clusters, function-level benchmarks make it easier to isolate 
certain code segments that may be difficult to test through a complete MongoDB server.

The MongoDB Benchmarks follow most of the conventions and best practices of Google Benchmark, with 
some deviations to make the user experience closer to that of MongoDB C++ unit tests.

## Writing a Benchmark

#### The Basics: 

```cpp
// my_bm.cpp

#include <benchmark/benchmark.h>

#include "mongo/util/processinfo.h"

namespace mongo {
namespace {

static void BM_Foo(benchmark::State& state) {
  if (state.thread_index == 0) {
    // Setup code here. None of the threads will start until all have
    // reached the start of the benchmark loop.
  }

  for (auto keepRunning : state) {
    // Test code goes here.
  }

  if (state.thread_index == 0) {
    // Teardown code here.
  }
}

BENCHMARK(BM_Foo)
  ->Range(1, 1<<4)
  ->Threads(1, ProcessInfo::getNumAvailableCores());


class MyFixture : public benchmark::Fixture {
public:
    void SetUp(benchmark::State& state) override {
		...
    }

    void TearDown(benchmark::State& state) override {
		...
    }
};

BENCHMARK_F(MyFixture, BM_Bar)(benchmark::State& state) {
   for (auto keepRunning : state) {
     // Test code goes here.
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_Bar)-> ... (same options as BENCHMARK())

}  // namespace
}  // namespace mongo

```

```python
# SConscript

env.Benchmark(
    target=my_bm',
    source=[
        'my_bm.cpp',
    ],
    LIBDEPS=[
    	...
    ],
)

```

Preferred style:

* benchmark functions should be named `BM_UpperCamelCase`.
* benchmark file names should end with `_bm`, excluding the file extension.
* SCons `Benchmark` targets *must* end with `_bm`.

There are a number of existing tests that serve as good examples:
  
* [`clock_source_bm.cpp`](https://github.com/mongodb/mongo/blob/master/src/mongo/util/clock_source_bm.cpp)
* [`system_resource_canary_bm.cpp`](https://github.com/mongodb/mongo/blob/master/src/mongo/unittest/system_resource_canary_bm.cpp)

#### Best Practices
Following the best practices below will ensure that your test has a high signal to noise ratio.

* Benchmark with all optimizations enabled, as you would compile a production build. This means 
using the toolchain g++, static linking, `dbg=on`, `opt=off`, etc.
* Use `benchmark::DoNotOptimize()` liberally to prevent your code from being optimized away; when 
in doubt, use it. 
  * There are a number of other mechanisms available to prevent unexpected optimizations:
  * `benchmark::ClobberMemory()` can be used to "flush" memory and provides a read/write barrier 
  to prevent instructions from being reordered across it.
  * `asm("")` can be used to prevent a function that has no side-effects from being optimized 
  away. See the `noinline` keyword on [this page from the GCC documentation]
  (https://gcc.gnu.org/onlinedocs/gcc-5.3.0/gcc/Function-Attributes.html) for more detail.
* Avoid using RNGs. If one is absolutely necessary, use a fixed seed.
* Avoid disk IO.
* Avoid non-loopback networking.
* Avoid tests that stress multiple resources (e.g. CPU and memory access).
* Avoid tests that take more than 0.1 seconds in a single iteration. Consider breaking each one up 
into multiple tests. ("iteration" here refers to a single run of a `keepRunning` loop, not a 
single call to a `BM_` function, which can take much longer.)

#### Advanced Usages
If you like to explore other available functionality the Benchmark framework provides, please 
refer to 
[Google Benchmark's execellent documentation](https://github.com/google/benchmark/blob/v1.3.0/README.md). 
The rule of thumb is that if a functionality is provided by both Google Benchmark and MongoDB, 
use the MongoDB version. There are a few differences as a result of this rule, including:

1. `BENCHMARK_MAIN` does not need to be explicitly declared.
1. `mongo::ProcessInfo` should be used for getting hardware information in favor of Google 
Benchmark's version.

## Running a Benchmark

#### Running a benchmark through resmoke.py

`python2 buildscripts/resmoke.py --suite=benchmarks [/optional/path/to/benchmark/test/binary]`

The results from the above command will be printed to the console and look like the following.

```
Run on (8 X 2800 MHz CPU s)
2018-03-08 14:55:12
-----------------------------------------------------------------------------------
Benchmark                                            Time           CPU Iterations
-----------------------------------------------------------------------------------
BM_ClockNow/poll period:0/threads:1                 29 ns         29 ns   24998482
BM_ClockNow/poll period:0/threads:1                 29 ns         29 ns   24998482
BM_ClockNow/poll period:0/threads:1                 29 ns         29 ns   24998482
BM_ClockNow/poll period:0/threads:1_mean            29 ns         29 ns   24998482
BM_ClockNow/poll period:0/threads:1_median          29 ns         29 ns   24998482
BM_ClockNow/poll period:0/threads:1_stddev           0 ns          0 ns   24998482
BM_ClockNow/poll period:0/threads:2                 14 ns         28 ns   25178950
BM_ClockNow/poll period:0/threads:2                 14 ns         27 ns   25178950
BM_ClockNow/poll period:0/threads:2                 14 ns         28 ns   25178950
BM_ClockNow/poll period:0/threads:2_mean            14 ns         28 ns   25178950
BM_ClockNow/poll period:0/threads:2_median          14 ns         28 ns   25178950
BM_ClockNow/poll period:0/threads:2_stddev           0 ns          0 ns   25178950
```

If you see a warning for CPU frequency scaling, consider turning CPU scaling off for more stable 
results. `cpufrequtils` is a popular tool to do this on Linux.

A few Benchmark specific options have been added to resmoke.py

```
--benchmarkMinTimeSecs[=5]         replaces --repeat; set a higher number to make a test run longer.
--benchmarkRepetitions[=3]         can be changed to 1 to make tests finish faster. Mean/Media/StdDev will not be available.
```

#### Preventing unexpected optimizations
If a test is taking less than 2 nano-seconds per iteration, or if you believe one or more lines of 
code never touches memory (i.e. it directly and indirectly affects only registers), none of the 
compiler hints may be useful in preventing code being optimized away. In this case it's prudent 
to manually check that the compiler is not optimizing out your code. This can be done by looking 
at the disassembly of a benchmark binary. 

The disassembly for an empty, optimized, and unstripped benchmark contains something like this:

```objdump
1000013d9:	e8 52 58 00 00 	callq	22610 <__ZN9benchmark5State16StartKeepRunningEv>
1000013de:	48 89 df 	movq	%rbx, %rdi
1000013e1:	48 83 c4 08 	addq	$8, %rsp
1000013e5:	5b 	popq	%rbx
1000013e6:	5d 	popq	%rbp
1000013e7:	e9 04 59 00 00 	jmp	22788 <__ZN9benchmark5State17FinishKeepRunningEv>
```
The keywords to look out for are `callq`, `StartKeepRunning`, and `FinishKeepRunning`. If there's 
not much happening between `StartKeepRunning` and `FinishKeepRunning`, your test is likely doing 
nothing. The `addq` on line 3 is part of Google Benchmark's book-keeping and not the benchmark 
itself.

#### Evergreen Integration

If you're writing benchmarks for a new feature, putting them in a new resmoke suite is recommended 
to ensure faster feedback. The resmoke suite definitions are in the mongo repository under 
`buildscripts/resmokeconfig/suites/`.

In your new suite YAML definition, make sure to always include the `system_resource_canary_bm*` 
test and the `CombineBenchmarkResults` hook.

```yaml
# benchmarks_my_feature_name.yml

test_kind: benchmark_test

selector:
  root: build/benchmarks.txt
  include_files:
  # The trailing asterisk is for catching the [.exe] suffix on Windows.
  - build/**/system_resource_canary_bm*
  - build/**/path/to/my/benchmarks*

executor:
  config: {}
  hooks:
  - class: CombineBenchmarkResults
```

Also Add your files to the `excluded_files` section of `benchmarks.yml`, which is a fallback suite
to pick up orphaned benchmarks.
```yaml
...
selector:
  ...
  exclude_files:
  - build/**/path/to/my/benchmarks*
...
```

Use the following template to add a Evergreen task for the `benchmarks_my_feature_name` suite. 
Ensure that when adding the task to Enterprise RHEL 6, use the specially tuned perf distro, 
which will provide a better signal to noise ratio.

Unless the suite is for testing platform-specific codepaths, it should be added to Enterprise 
RHEL 6.2 (on the centos6-perf distro), Enterprise RHEL 7.0 (on the rhel70-small distro), and 
Enterprise Windows 2008R2 (on the windows-64-vs2015-small distro) build variants to ensure 
coverage of major MongoDB platforms.

```yaml
# etc/evergreen.yml

- <<: *benchmark_template
  name: benchmarks_my_feature_name
  commands:
  - func: "do benchmark setup"
  - func: "run tests"
    vars:
      resmoke_args: --suites=benchmarks_my_feature_name
      run_multiple_jobs: false
  - func: "send benchmark results"


# Add the task to a build variant.
  - name: benchmarks_my_feature_name
    distros:
    - centos6-perf
```

## Understanding Benchmark Results in Evergreen

#### Interpreting Graphs in the Perf Plugin
The main difference between benchmarks and most other existing performance tests is that the 
result is presented as latency, not throughput. This means lower numbers on the graphs are better, 
which is not visually intuitive. To ensure consistency with existing tests, the benchmarks run 
through resmoke.py will return `0 - actual_latency` in the JSON report file (but not the console 
output); this preserves the "higher is better" semantics, but the `y` axis on the graphs will be 
negative numbers.

Another difference is that the absolute scale is different between latency and throughput graphs, 
despite higher being better for both. Since latency is the inverse of throughput, a 50% increase 
in latency results in only a 33% drop in throughput.

#### Displaying user-defined counters
Google Benchmark allows you to 
[define custom counters](https://github.com/google/benchmark/tree/v1.3.0#user-defined-counters), 
where each counter is a `double`-like object. The counters are stored in the perf dashboard's 
"historic data" json file but is not visualized at the moment. Note that all benchmark results are 
negated, including counters.
