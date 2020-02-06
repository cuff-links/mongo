As you make changes, you will need to build the server and [run the appropriate test suites](Test-The-Mongodb-Server).

> **note**
>
> As of MongoDB 3.0, the MongoDB tools, except for `mongosniff` and `mongoperf`, are now written in Go and have their own build process. For these tools written in Go, you must build them separately from the server. See building-tools-from-source for build instructions.

Though the exact dependencies and semantics differ by operating system, building the server from source has the same basic steps.

Dependencies & Installation
---------------------------

The requirements for building MongoDB vary by branch:

To build the master branch, you will need:
- A modern and complete C++17 compiler. One of the following is required:
  - VS 2017 version 15.9 or newer
  - GCC 8.0 or newer
  - Clang 7 (or Apple XCode 10 Clang) or newer
- On Linux and macOS, the libcurl library and header is required. MacOS includes libcurl.
  - Fedora/RHEL - `dnf install libcurl-devel`
  - Ubuntu/Debian - `apt-get install libcurl-dev`
- Python 3.7

To build the 3.4, 3.6, and 4.0 branches, you will need:
- A modern and complete C++11/14 compiler. One of the following is required:
  - VS2015 Update 2 or newer
  - GCC 5.4.0
  - Clang 3.4 (or Apple XCode 5.1.1 Clang) or newer
- On Linux and macOS, the libcurl library and header is required. MacOS includes libcurl.
  - Fedora/RHEL - `dnf install libcurl-devel`
  - Ubuntu/Debian - `apt-get install libcurl-dev`
- Python 2.7


To build the 3.2 and 3.0 branches, you will need:
- A C++11 compiler. One of the following is required:
  - VS2013 Update 4 or newer. Note that VS2015 is currently not compatible with the 3.0 and 3.2 branches. You must use VS2013.
  - GCC 4.8.2 or newer. Note that versions of GCC newer than 4.8.2 may produce warnings when building these branches, which are promoted to errors. Please use the --disable-warnings-as-errors build option if necessary to allow the build to succeed despite the warnings.
- Python 2.7
- SCons 2.3.0 or newer

MongoDB uses [SCons](http://www.scons.org/) to build the Server and the shell. While it is unnecessary to know all the intricacies of SCons, glancing over the documentation may be helpful. SCons is unlikely to come preinstalled on your system, so make sure to install it using your preferred package manager.

> **note**
>
> MongoDB uses a customized built-in version of `tcmalloc` to achieve significant performance gains. Building with `--use-system-tcmalloc` produces binaries that lack these performance gains.

Other basic requirements, independent of system, are to install [Git](http://git-scm.com/) and a recent 2.7.x version of [Python](https://www.python.org/).

### System Specific Requirements

Further requirements are system-dependent and for both SCons and running the tests:

-   On Linux, you will need to install a compiler `gcc` or `clang`, as well as `glibc` headers which are usually included in a package named `glibc-devel` and `libcurl` headers.
    -   On Debian and Ubuntu systems, you must install the `libssl-dev` package to compile with SSL support.
    -   On Red Hat and CentOS systems, you must install the `openssl-devel` package to compile with SSL support.
-   On Mac OS X, you will need to install `XCode`, specifically the command line tools.
-   On Windows, you will need to install Visual Studio. When running on Windows machines, it is recommended to install either [pywin32](http://sourceforge.net/projects/pywin32/) or [ActivePython](http://www.activestate.com/activepython/downloads) if you wish to do parallel builds.

-   On FreeBSD, the included compiler (`gcc` or `clang`) should suffice, but you will need to install `devel/libexecinfo` from ports or via [pkgng](http://www.freebsd.org/doc/handbook/pkgng-intro.html).
-   On OpenBSD 5.4 or later, you will need to install `gcc` from ports (`lang/gcc`) or by using [pkg\_add](http://www.openbsd.org/cgi-bin/man.cgi?query=pkg_add&sektion=1) You will also need to install `devel/libexecinfo`.

    When you run SCons, you will also need to pass additional arguments to `scons` to use the compiler from ports instead of the system compiler, i.e. `--cc=egcc --cxx=eg++`.

-  When targeting 64-bit ARM systems (aarch64) you must either explicitly select a CPU targeting that includes CRC32 support by adding `CCFLAGS=-march=armv8-a+crc` to your SCons invocation, or disable hardware CRC32 acceleration with the flag `--use-hardware-crc32=off`. Note that on older branches this flag may be somewhat confusingly called `--use-s390x-crc32=off`, but will still affect ARM builds.

Installing pip requirements
---------------------------

```
# Windows
pip.exe install --user -r etc\pip\dev-requirements.txt

# macOS / Linux assuming python = python 2.7
pip install --user -r etc/pip/dev-requirements.txt
```

All of these commands should be run from the root of the MongoDB repository.

> **note**
> 
> In the past, instead of `etc/pip/dev-requirements.txt`, there was `buildscripts/requirements.txt`.
>
> The `--user` flag installs modules in the user install location. (See [pip documentation](https://pip.pypa.io/en/stable/user_guide/#user-installs).) This flag is not necessary if you have write access to your python distribution's site-packages. Virtualenv environments are recommended to maintain stable and isolated python distributions.

Building With SCons
-------------------

Building the server executables consists of passing SCons a target; for example, when we build the MongoDB database server executable:

``` sourceCode
buildscripts/scons.py mongod
```

On Windows, this will instead be `scons mongod.exe`.

Other executables you may need to build include mongos or mongo, depending on what you are working on. You can pass SCons multiple targets in a list if necessary:

``` sourceCode
buildscripts/scons.py mongod mongo mongos
```

Alternatively, you may build everything, which includes mongo, mongod, mongos, and all the C++ unit tests:

``` sourceCode
buildscripts/scons.py all
```

All of these commands should be run from the root of the MongoDB repository.

> **note**
>
> If you are building an older version of MongoDB with a newer compiler, such as MongoDB 2.6 with Clang 3.5 on Mac OS X 10.10, you may need to include the `--disable-warnings-as-errors` option to your SCons invocation, as in the following:
>
> ``` sourceCode
> scons all --disable-warnings-as-errors
> ```
>
> This ensures that novel warnings in newer compilers do not prevent you from building older versions of MongoDB.

While certain top level build artifacts (like mongod and mongos) get copied to the top of the source tree during a build, other files like the C++ unit tests are generated underneath a subdirectory called `build`. The `build` directory is created at the top level of the source tree the first time you build anything.

Building can take a while, depending on which targets you are building and the capabilities of your computer. Without getting into all the intricacies of SCons, here are some flags to get you started:

-   `--help`  
    This options provides help on both the intrinsic and local options. Read this.

-   `-j <N>` This option controls the number of parallel jobs SCons will use. Depending on what the local hardware is, you probably want to set this from anything between 1 and 32. Keep in mind that this isn't just compilation, but also linking. Having 32 concurrent linking jobs running can make your machine pretty sluggish. Find a number that works for you, such as the number of cpus. When running on Windows machines, it is recommended to install either [pywin32](http://sourceforge.net/projects/pywin32/) or [ActivePython](http://www.activestate.com/activepython/downloads) if you wish to do parallel builds.
-   `--ssl`  
    This option builds MongoDB with SSL support.

The following table lists some SCons aliases that are helpful with testing:


Alias|  Effect
----|----
all|  Builds everything (core and all tests)
dbtest|  Builds the `dbtest` program.
lint|  Runs the code linter
msi|  Builds the Windows MSI installer.
core|  Builds mongod, mongos, and the mongo shell.
install|  Installs to the directory used with the `--prefix` option, or to `/usr/local` if no `--prefix` was specified.
unittests|  Builds the unit tests. The preferred way to run the unit tests is either with resmoke.py or by running the unit test executables. For examples, see run-unit-tests.

Please note that using `ccache` with SCons does not work as expected (see https://jira.mongodb.org/browse/SERVER-38389). We advise not using `ccache` to enable build caching, but to instead use the SCons built-in caching mechanism by adding `--cache` to your SCons invocation. If you must use `ccache`, you can partially work around it by setting `MAXLINELENGTH` to a very high value on the SCons command line. Note that if you use `ccache` by setting, say CC=`ccache g++`, SCons may become unable to detect when your compiler binary changes and may not rebuild files when necessary.

Windows Specific Instructions
-----------------------------

Install Git from <http://git-scm.com>. When you install git, choose Checkout Windows-style commit Unix-style line endings. This will set the git option `core.autocrlf` to `true`. To verify the setting after installation, run the following command.

``` sourceCode
git config --get core.autocrlf
```

SCons requires Python 2.7.x. On Windows, [ActiveState Python Community Edition](http://www.activestate.com/activepython/downloads) is recommended to support parallel builds. Install either the 64-bit (x64) or 32-bit (x86) version depending on the machine architecture.
