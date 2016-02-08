Class Design Guidelines
-----------------------

-   Do not use multiple inheritance. If you need the service of several classes, use delegation. The only possible but highly unlikely exception to this is if your class inherits from other pure abstract classes.
-   Have a comment before a class that explains its purpose. Perhaps the class name is so clear that this is obvious. Then some commentary on what you are up to.
-   Only add members and methods to a class if they make sense with regards to the bullet above. If you find yourself unsure to where to hook a piece of logic, rethink the class and surrounding classes' purposes.
-   Class names and method names are to be descriptive of what they do. Avoid generic overloaded names (e.g., write, add, ...) to make grep easier (and maybe reading too).
-   Don't put implementation details in the header unless the user of the class needs to know them. Sometimes single line inline implementations are good "documentation".
-   Be aware than many methods in the codebase can throw a `DBException`. If a class should never throw (e.g. can be called in a destructor), that should be clear.
-   Write unit tests for each class you create. If you can't easily write a unit test for the class, that is a strong hint it has way too many external dependencies.
-   Do not create early hierarchies. An early hierarchy is a one where there is only one type of derived class. If you need to separate functionality, use delegation instead. In that case, make sure to test separately.
-   Avoid `friend`.
-   When declaring overriding methods in derived classes, use only one of `final` or `override`. Do not re-iterate `virtual`.
-   Default to making classes *non-assignable and non-copyable*. Use MONGO\_DISALLOW\_COPYING().

