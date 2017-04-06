# ![logo](https://media.mongodb.org/mongodb-logo-web-tagline.png)

This wiki is a guide to contributing to MongoDB Server. For help with MongoDB Server see the links in [Other Resources](#other-resources-for-help-with-mongodb).

The MongoDB Server is written in C++ with an extensive suite of tests in JavaScript. To get started with the server, you will want to see [Building with Server](Build-Mongodb-From-Source). For help with the tools, see [Building the Tools](Build-Tools-From-Source) or http://github.com/mongodb/mongo-tools. If you are looking to instead get started with drivers that connect to the server, check out our list of [official drivers](https://docs.mongodb.org/ecosystem/drivers/) for more information.

### Contributing to MongoDB
MongoDB welcomes community contributions! If you’re interested in making a contribution to MongoDB, please follow the steps below before you start writing any code:

1. Sign the [contributor's agreement](http://www.mongodb.com/contributor). This will allow us to review and accept contributions.
2. Reach out to us to discuss your contribution. Either find an open JIRA ticket for an existing issue and comment in it, or create a new JIRA ticket that describes the change you would like to work on. If in doubt feel free to always create a new ticket.
    * New to MongoDB development? The SERVER JIRA project, tickets of an appropriate complexity for new engineers are marked with a “neweng” label. For more details, see [Choosing a Project](https://github.com/mongodb/mongo/wiki/Choose-A-Project).
    * See [“A note on Fix Version/s”](#a-note-on-fix-versions) below to see how to find work that’s currently scheduled in our roadmap
3. Begin a discussion on the ticket describing the intended changes. MongoDB developers can help hash out the approach and make sure everyone is on the same page before too much time is spent writing the code changes and associated tests.
4. Make the code changes and open a pull request. Note that almost all changes require tests to ensure that the code works as expected. MongoDB developers will review the code changes and run our automation tests against a patch build. If everything looks good, the pull request can be merged.

##### A note on “Fix Version/s”
The SERVER JIRA project uses the “Fix Version/s” field to indicate the current scheduling for tickets. Here is what they mean:
- X.Y.Z or “X.Y Required/Desired”, where X, Y and Z are numbers: a ticket has been scheduled for the X.Y version of MongoDB.
- “Needs Triage”: a ticket needs to be scheduled
- “Planning Bucket A”, “Planned but not scheduled”: a ticket to be considered in a future round of planning
- “Features we’re not sure of”: tickets representing changes to MongoDB that are currently not being considered for implementation, but may in the future.

### Other Resources for help with MongoDB
**Documentation**

https://docs.mongodb.org/

**Mailing Lists and IRC**

https://www.mongodb.org/community

**MongoDB University**

https://university.mongodb.com/  