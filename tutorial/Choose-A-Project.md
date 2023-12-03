MongoDB, Inc. officially supports the following projects: the core MongoDB database (commonly referred to as the server or the kernel), ten drivers, and a MongoDB-Hadoop connector. These projects are open-source and open to your contributions.

MongoDB projects are available on GitHub. The get the latest source from Github, follow the links to the projects.


Links to Project on GitHub| Project Documentation
----|----
[MongoDB Server](https://github.com/mongodb/mongo) | [MongoDB Server Manual](http://docs.mongodb.org/manual/)
[MongoDB Tools](https://github.com/mongodb/mongo-tools) | [MongoDB Tools Documentation](http://docs.mongodb.org/manual/reference/program/)
[C Driver](https://github.com/mongodb/mongo-c-driver) | [C Driver Documentation](http://docs.mongodb.org/ecosystem/drivers/c/)
[C++ Driver](https://github.com/mongodb/mongo-cxx-driver/) | [C++ Driver Documentation](http://docs.mongodb.org/ecosystem/drivers/cpp/)
[C\# Driver](https://github.com/mongodb/mongo-csharp-driver) | [C\# Driver Documentation](http://docs.mongodb.org/ecosystem/drivers/csharp/)
[Java Driver](https://github.com/mongodb/mongo-java-driver) | [Java Driver Documentation](http://docs.mongodb.org/ecosystem/drivers/java/)
[Node Driver](https://github.com/mongodb/node-mongodb-native) | [Node Driver Documentation](http://www.mongodb.com/docs/drivers/node/current/)
[Perl Driver](https://github.com/mongodb/mongo-perl-driver) | [Perl Driver Documentation](http://docs.mongodb.org/ecosystem/drivers/perl/)
[PHP Driver](https://github.com/mongodb/mongo-php-driver) | [PHP Driver Documentation](http://docs.mongodb.org/ecosystem/drivers/php/)
[Python Driver](https://github.com/mongodb/mongo-python-driver) | [Python Driver Documentation](http://docs.mongodb.org/ecosystem/drivers/python/)
[Ruby Driver](https://github.com/mongodb/mongo-ruby-driver) | [Ruby Driver Documentation](http://www.mongodb.com/docs/drivers/ruby-drivers/)
[Casbah (Scala Driver)](https://github.com/mongodb/casbah) | [Casbah (Scala Driver) Documentation](http://docs.mongodb.org/ecosystem/drivers/scala/)
[Hadoop Connector](https://github.com/mongodb/mongo-hadoop) | [Hadoop Connector Documentation](http://docs.mongodb.org/ecosystem/tools/hadoop/)

Start with a Driver
-------------------

Unless you are already very familiar with C++ and the MongoDB database, or already know that you specifically want to work on the Core Server, we highly recommend that you start by contributing to one of the Drivers. Why?

-   Smaller codebase(s) than the core server.  
    Each driver has a relatively small codebase. This means it is easier to get familiar with all the parts and how they work together. Drivers are individually less complex than the core server, and they are similar in function to each other.

-   Fewer requirements to get a pull request approved.  
    There are fewer moving parts in a driver than the core server, due to the smaller codebase. This makes it easier for us to tell if your change is going to break something else, whether that something else is an existing part of the driver or a planned new feature.

-   More likely to be in a language with which you are comfortable.  
    With drivers for different languages, you're likely to be comfortable in at least one of those languages.

-   Apache License.  
    Among other things, you do not have to sign the contributor agreement when contributing to the drivers, which can hold up the pull request process.


