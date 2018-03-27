As you make changes, you will need to build the server and [run the appropriate test suites](Test-The-Mongodb-Server).

> **note**
>
> As of MongoDB 3.0, the MongoDB tools, except for `mongosniff` and `mongoperf`, are now written in Go and have their own build process. For these tools written in Go, you must build them separately from the server. See building-tools-from-source for build instructions.

Though the exact dependencies and semantics differ by operating system, building the server from source has the same basic steps.

Dependencies & Installation
---------------------------

The requirements for building MongoDB vary by branch:

To build the master branch, you will need:
- A modern and complete C++11 compiler. One of the following is required:
  - VS2015 Update 2 or newer
  - GCC 5.3.0
  - Clang 3.4 (or Apple XCode 5.1.1 Clang) or newer
- Python 2.7


To build the 3.2 and 3.0 branches, you will need:
- A C++11 compiler. One of the following is required:
  - VS2013 Update 4 or newer. Note that VS2015 is currently not compatible with the 3.0 and 3.2 branches. You must use VS2013.
  - GCC 4.8.2 or newer. Note that versions of GCC newer than 4.8.2 may produce warnings when building these branches, which are promoted to errors. Please use the --disable-warnings-as-errors build option if necessary to allow the build to succeed despite the warnings.
- Python 2.7
- SCons 2.3.0 or newer

To build the 2.6 branch, you will need:
- A C++03 compiler, or VS2010 or newer.

MongoDB uses [SCons](http://www.scons.org/) to build the Server and the shell. While it is unnecessary to know all the intricacies of SCons, glancing over the documentation may be helpful. SCons is unlikely to come preinstalled on your system, so make sure to install it using your preferred package manager.

> **note**
>
> MongoDB uses a customized built-in version of `tcmalloc` to achieve significant performance gains. Building with `--use-system-tcmalloc` produces binaries that lack these performance gains.

Other basic requirements, independent of system, are to install [Git](http://git-scm.com/) and a recent 2.7.x version of [Python](https://www.python.org/).

### Compiler Requirements

To build MongoDB you must use one of the following minimum compiler versions:

-   Windows: Visual Studio 2013 (i.e. MSVC12) Update 4
-   Linux: GCC 4.8.2
-   OS X: Clang 3.4 of XCode 5

### System Specific Requirements

Further requirements are system-dependent and for both SCons and running the tests:

-   On Linux, you will need to install a compiler `gcc` or `clang`, as well as `glibc` headers which are usually included in a package named `glibc-devel`.
    -   On Debian and Ubuntu systems, you must install the `libssl-dev` package to compile with SSL support.
    -   On Red Hat and CentOS systems, you must install the `openssl-devel` package to compile with SSL support.
-   On Mac OS X, you will need to install `XCode`, specifically the command line tools.
-   On Windows, you will need to install `Visual Studio 2013` or later. When running on Windows machines, it is recommended to install either [pywin32](http://sourceforge.net/projects/pywin32/) or [ActivePython](http://www.activestate.com/activepython/downloads) if you wish to do parallel builds.

    If using Visual Studio 2013 Express and you receive scons error regarding the unavailability of atomic types, you may need to install [Visual C++ 2008 Redistributable Package](http://www.microsoft.com/en-au/download/details.aspx?id=29).

-   On FreeBSD, the included compiler (`gcc` or `clang`) should suffice, but you will need to install `devel/libexecinfo` from ports or via [pkgng](http://www.freebsd.org/doc/handbook/pkgng-intro.html).
-   On OpenBSD 5.4 or later, you will need to install `gcc` from ports (`lang/gcc`) or by using [pkg\_add](http://www.openbsd.org/cgi-bin/man.cgi?query=pkg_add&sektion=1) You will also need to install `devel/libexecinfo`.

    When you run SCons, you will also need to pass additional arguments to `scons` to use the compiler from ports instead of the system compiler, i.e. `--cc=egcc --cxx=eg++`.


Installing pip requirements
---------------------------

```
# Windows
pip.exe install -r buildscripts\requirements.txt

# macOS / Linux
pip install -r buildscripts/requirements.txt
```

All of these commands should be run from the root of the MongoDB repository.


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

Windows Specific Instructions
-----------------------------

Install Git from <http://git-scm.com>. When you install git, choose Checkout Windows-style commit Unix-style line endings. This will set the git option `core.autocrlf` to `true`. To verify the setting after installation, run the following command.

``` sourceCode
git config --get core.autocrlf
```

SCons requires Python 2.7.x. On Windows, [ActiveState Python Community Edition](http://www.activestate.com/activepython/downloads) is recommended to support parallel builds. Install either the 64-bit (x64) or 32-bit (x86) version depending on the machine architecture.

To install SCons 2.3.4 with ActiveState Python Community Edition, download the zip distribution of SCons, not the self-extracting exe installer labelled Windows. You must use the zip file, because self-extracting installer cannot find the 64-bit install of ActiveState Python. Unpack the SCons zip file, and if Python is installed at the default path, use the following command:

``` sourceCode
c:\python27\python.exe setup.py install
```

MongoDB version 2.4.x supports VS 2010. MongoDB 2.6.x, and 2.7.x support VS 2010, VS 2012, and VS 2013. MongoDB version 3.0.x requires VS 2013.

Finally, to build MongoDB for Windows, you will need to specify whether SCons will target 32-bit or 64-bit. SCons will target 32-bit builds by default. For 64-bit builds, run the following command:

``` sourceCode
scons --64bit
```

For 32-bit builds, run the following command:

``` sourceCode
scons --32bit
```

Building with Visual Studio 2013 & Later
----------------------------------------

To build with SCons, first start a `VS2013 x64 Native Tools Command Prompt`, and within this command shell environment, run the SCons commands listed above.

In a typical Visual Studio installation, the batch file to set up the VS 2013 environment has the following path:

``` sourceCode
%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\vc\bin\amd64\vcvars64.bat
```

It is also available in the `Visual Studio Tools` folder in the Start Menu.

If using Visual Studio 2013 Express and you receive scons error regarding the unavailability of atomic types, you may need to install [Visual C++ 2008 Redistributable Package](http://www.microsoft.com/en-au/download/details.aspx?id=29).