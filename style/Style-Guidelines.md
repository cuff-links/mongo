This page presents some coding style and general guidelines that you should strive to follow when contributing code.

Consistency
-----------

The **most important** guideline is to be consistent with the existing style in the files. This rule applies to all projects and overrides the actual style guide on most occasions. For example, follow the spacing or naming conventions in the file you are editing.

> **tip**
>
> If your IDE has auto-formatting, turn off the feature.

Rules of Thumb for Contributing Code
------------------------------------

-   Add tests for your new functionality.
-   Ensure your code is readable.

    Readability helps the project maintainers and other contributors understand your code. This includes commenting your code. See guidelines-comments.

-   Maintain backwards compatibility.
    -   Avoid changes that break the project's functionality and cause existing tests to fail unexpectedly.
    -   Avoid changes that affect the project's compatibility with other projects. For example, each driver has to be compatible with many versions of the associated language.
-   Ensure cross-platform compatibility.

    We recognize that you may not be able to test your changes across different operating system. Just be aware that we do test for cross-platform compatibility and failing the tests could be a reason for not accepting your pull requests.

General Guidelines for Commenting Code
--------------------------------------

-   Use complete sentences with proper capitalization and punctuation.
-   Use declarative comments instead of imperatives; i.e. comments should state what the function does.

    For example, if a function computes the product of two numbers, the comment should state:

    ``` sourceCode
    This function computes the product of two floats.
    ```

    Avoid using the imperative `Compute the product of two floats.`

-   Every module, class, method, type, or global variable should have a one-line comment of its functionality. A method should have a one-line description of what it does, including its input and output.

    Basically, the usage for a class should be apparent from reading the header.

-   Use in-line comments to describe "how" the code works if not self-evident. For example, if the order of the commands is important for some reason that is unclear, put a comment. Or, if the code works in a way different from what a casual reader would expect, put a comment.

> **note**
>
> If a driver has a different convention for commenting, follow the specific convention for the driver.
