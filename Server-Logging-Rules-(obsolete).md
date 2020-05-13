WARNING
-----------

This document is largely obsolete. Please refer to 
[src/mongo/logv2/README.md](https://github.com/mongodb/mongo/blob/master/src/mongo/logv2/README.md)
for up-to-date guidance on logging.

***


Basic Rules
-----------

-   `cout`/`cerr` should never be used
-   Include mongo/util/log.h to use the `LOG()` helper macro
-   Declare a log component *before* the first `#include` statement in a `.cpp` file to assign a default log component to log messages.

    ``` sourceCode
    #define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kStorage

    #include ...
    ```

Normal Logging
--------------

-   Debugging with levels of verbosity. See the `-v` command line option (default level is 0). If the global log level is less than `x`, no functions in the stream are executed.

    ``` sourceCode
    LOG( int x ) << ...
    ```

-   Informational

    ``` sourceCode
    log() << ...
    ```

-   Warnings

    -   recoverable, e.g. replica set node down

    ``` sourceCode
    warning() << ...
    ```

-   Errors

    -   unexpected system state (disk full)
    -   internal code errors

    ``` sourceCode
    error() << ...
    ```


