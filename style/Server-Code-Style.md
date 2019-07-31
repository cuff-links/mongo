Basics
------

-   Use spaces, no literal tabs.
-   4 spaces per indentation.
-   Limit lines to 100 columns.
-   Use LF (Unix-style) line endings, not CR-LF (DOS). git has a config option in Windows to convert line endings automatically (`core.autocrlf`)
-   Starting with MongoDB 3.2 code, all code changes must be formatted by Clang-Format 3.6.0 before they are checked in. See clang-format-label for more information.
-   All new files added to the MongoDB code base should use the following copyright notice and SSPL license language, substituting the current year as appropriate:

    ``` sourceCode
    /**
     *    Copyright (C) 2018-present MongoDB, Inc.
     *
     *    This program is free software: you can redistribute it and/or modify
     *    it under the terms of the Server Side Public License, version 1,
     *    as published by MongoDB, Inc.
     *
     *    This program is distributed in the hope that it will be useful,
     *    but WITHOUT ANY WARRANTY; without even the implied warranty of
     *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     *    Server Side Public License for more details.
     *
     *    You should have received a copy of the Server Side Public License
     *    along with this program. If not, see
     *    <http://www.mongodb.com/licensing/server-side-public-license>.
     *
     *    As a special exception, the copyright holders give permission to link the
     *    code of portions of this program with the OpenSSL library under certain
     *    conditions as described in each individual source file and distribute
     *    linked combinations including the program with the OpenSSL library. You
     *    must comply with the Server Side Public License in all respects for
     *    all of the code used other than as permitted herein. If you modify file(s)
     *    with this exception, you may extend this exception to your version of the
     *    file(s), but you are not obligated to do so. If you do not wish to do so,
     *    delete this exception statement from your version. If you delete this
     *    exception statement from all source files in the program, then also delete
     *    it in the license file.
     */
    ```

-   For anything that isn't explicitly covered here, default to the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) and the [Google JavaScript Style Guide](https://google.github.io/styleguide/javascriptguide.xml).

Case
----

Use camelCase for most `varNames`. Use ProperCase for names of classes and structs. Use camelCase for instances of such classes.

Comments
--------

Refer to getting-started-coding-style-guidelines for basic guidelines. Otherwise, default to [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml#Comments) for placement of comments.

Inlines
-------

-   If you must have long inline functions, put them in a [-inl.h](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml#The_-inl.h_Files) file.
-   If your inline function is a single line long, put it and its decl on separate lines:

    ``` sourceCode
    int getLength() const {
        return _length;
    }
    ```

-   If a function is not performance sensitive, and it isn't one line long, put it in the cpp file. Keep code out of headers.

Strings
-------

See util/mongoutils/str.h and bson/stringdata.h

-   Use

    ``` sourceCode
    str::startsWith()
    str::endsWith()
    ```

    and not `strstr()`.

-   Use

    ``` sourceCode
    << 'c'
    ```

    and not `<< "c"`.

-   Use

    ``` sourceCode
    str[0] == '\0'
    ```

    and not `strlen(str) == 0`.

See server-string-manipulation.

Brackets
--------

``` sourceCode
if (0) {
} else if (0) {
} else {
}

do {
} while (0);
```

Class Members
-------------

``` sourceCode
class Foo {
private:
    int _bar;
};
```

Functions
---------

### Declaration

``` sourceCode
void foo(int v, MyType myItem);
```

Avoid declarations of extern functions in source files. Instead, `#include` a proper `.h` file. Be sure to match the header filename to the source filename where the function definition appears.

### Definition

``` sourceCode
void foo(int v, MyType myItem) {
}
```

Namespaces
----------

``` sourceCode
namespace foo {
// Contents of namespace are not indented.
int foo;
namespace bar {
int bar;
}  // namespace bar
}  // namespace foo
```

Assertions
----------

See Kernel exception architecture &lt;server-exception-architecture&gt;

Return Early
------------

-   BAD

    ``` sourceCode
    int foo() {
       if (x) {
         ...
       }
    }
    ```

-   GOOD

    ``` sourceCode
    int foo() {
       if (!x)
         return;

       ...
    }
    ```

Keeps indentation levels down and makes more readable.

Numeric Constants
-----------------

Large, round numeric constants should be written in multiplied form so that you never need to count digits.

``` sourceCode
const int tenMillion = 10 * 1000 * 1000;
const int megabyte = 1024 * 1024;
```

Explicit Constructors
---------------------

To avoid implicit type conversion, use the `explicit` keyword before constructors that take a single parameter.

\#includes
----------

-   Use "double quotes" for local code, &lt;angle brackets&gt; for 3rd party or library headers.

    ``` sourceCode
    examples:
    #include "mongo/platform/basic.h"
    #include <boost/thread.h>
    #include <vector>
    ```

-   Always use forward relative path from `mongo/src/`; do not use `..`

    ``` sourceCode
    correct:
    #include "mongo/db/namespace_details.h"

    incorrect:
    #include "../db/namespace_details.h"
    ```

For `cpp` Files
---------------

-   Include mongo/platform/basic.h first. blank line.
-   Include your .h file next, if applicable. blank line.
-   Include third party headers next, sorted. blank line.
-   Include local headers last, sorted.

``` sourceCode
example for classy.cpp:
#include "mongo/platform/basic.h"

#include "mongo/db/classy.h"

#include <boost/thread/thread.hpp>
#include <cstdio>
#include <string>

#include "mongo/db/db.h"
#include "mongo/db/namespace_details.h"
#include "mongo/util/concurrency/qlock.h"
```

For `h` files
-------------

-   `#pragma once` at the top, after the licence
-   Include third party headers first, sorted. blank line.
-   Include local headers last, sorted.
-   It's generally preferable to `#include` any headers you need. Forward-declaring type tags from other headers is permissible only when doing so is necessary, such as to break an `#include` cycle. Forward declaring symbols other than type tags, such as functions or variables, is forbidden.
-   Do not include platform/basic.h in a header; assume that all source files will include it prior to including the header.

For `js` files
-------------

-   Disable formatting for [template literals](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Template_literals)
``` sourceCode
// clang-format off
newCode = `load("${overridesFile}"); (${jsCode})();`;
// clang-format on
```

File Names
----------

-   Class definitions should go in a header file with the same name as the class. Insert `_` in place of a capital letter. Do not use capital letters in filenames. For example:
    -   ClassyClass's definition goes in classy\_class.h.
    -   ClassyClass's member function implementations go in classy\_class.cpp.
-   Do not be afraid to make another file, even if it is small. This is preferable to inserting your new class into a preexisting file.

Casting
-------

-   Do not use C-style casts, ever.
-   Use `static_cast<>` as needed. Use `const_cast<>` when no other solutions will work.
-   Be aware that `dynamic_cast<>`, unlike other casts, is done at run-time and calls a function. You should always check the return status of `dynamic_cast<>` for `null`.
-   `reinterpret_cast<>` should be used sparingly and is typically done for converting structures to raw bytes for use with I/O drivers.
-   When down-casting from a base type where the program logic guarantees that the runtime type is correct, consider using `checked_cast<>` from `mongo/base/checked_cast.h`. It is equivalent to static\_cast in release builds, but adds an invariant to debug builds that ensures the cast is valid.

RAII and Bare vs. Smart Pointers
--------------------------------

-   Aspire to embrace RAII (Resource Aquisition Is Initialization)
-   When writing functions that take or return bare pointers, document the ownership semantics in a header comment.

    -   Is the caller responsible for managing returned pointer's memory?
    -   Does the callee take ownership of the pointed-to parameter, or does the caller retain ownership?

    Prefer caller-retains ownership of parameters and takes ownership of returned pointers, but use the appropriate policy for each situation.

-   Generally, bare calls to `delete` and `free()` are red flags
    -   except in destructors
-   Use smart pointers such as `std::unique_ptr` and `std::shared_ptr` (know the difference between them!) to avoid memory leaks and ensure all `new`'s and `malloc`'s are paired with `delete`'s and `free`'s
-   Use `ON_BLOCK_EXIT` or `ScopeGuard` to protect other resources that must be released
    -   e.g. `fopen`/`fclose` pairs
    -   Or, write an object to do this for you via constructor and destructor

Fields that Include Units
-------------------------

In things like `serverStatus`, include the units in the stat name if there is any chance of ambiguity. For example:

-   `writtenMB`
-   `timeMs`

We should have standards for these -- i.e. megabytes should always be `MB` and not `Mb` and `Megabytes` in different places. So the standards are:

-   For bytes: use `MB` and show in megabytes unless you know it will be tiny. Note you can use a float so `0.1MB` is fine to show.
-   For time: use millis (`Ms`) for time by default. You can also use Secs and a float for times you know will be very long.
-   For microseconds, use `Micros` as the suffix, e.g., `timeMicros`.

ESLint
------

As of the 3.2 code, MongoDB uses [ESLint](http://eslint.org/) version 2.3.0 to lint JS code. ESLint is a JS linting tool that uses the config file located at `.eslintrc.yml`, in the root of the mongo repository, to control the linting of the JS code.

> **important**
>
> All JS files must be linted by ESLint before they are formatted by clang-format.

[Plugins](http://eslint.org/docs/user-guide/integrations) are available for most editors that will automatically run ESLint on file save. It is recommended to use one of these plugins.

Use the Python script `buildscripts/eslint.py` to check that the JS code is linted correctly as well as to fix linting errors in the code.

To lint JS code:  
``` sourceCode
python buildscripts/eslint.py lint
```

To auto-fix JS code:
``` sourceCode
python buildscripts/eslint.py fix
```


Clang-Format
------------

As of the 3.5 code, MongoDB uses [Clang-Format](http://clang.llvm.org/docs/ClangFormat.html) version 3.9 to enforce coding style. Clang-Format is a C/C++ & JS code formatting tool that uses the config files located at `src/mongo/.clang-format` & `jstests/.clang-format` to control the format of the code.

> **important**
>
> All code changes must be formatted by clang-format before they are checked in.

Plugins are available for most editors that will automatically run clang-format on file save. It is recommended to use one of these plugins.

Use the Python script `buildscripts/clang_format.py` to check that the C/C++ & JS code is formatted correctly as well as to reformat the code.

To lint code:  
``` sourceCode
python buildscripts/clang_format.py lint
```

To format code:  
``` sourceCode
python buildscripts/clang_format.py format
```

