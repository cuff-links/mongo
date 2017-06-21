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
    * For bug fixes, the discussion should be a brief one to confirm that your proposed fix makes
sense. Once you and the MongoDB engineer have aligned you should open a pull request against the
MongoDB repository as per the 'Pull Request Expectations' section of this document.
    * All syntactic and new feature changes, however, go through our design review process (whether
the change is from an external pull request or a MongoDB engineer). So a MongoDB engineer will first
discuss your proposal (including the syntax, output, whether it impacts sharding, etc.) to confirm
that the implementation fits our roadmap. Once you have come to an agreement, this engineer will
act as the 'champion' for your proposal in our internal design review process.
    * The design review group meets weekly so it is possible there will be a lag between the initial
conversation with your champion and final feedback on the direction of the proposed
feature/improvement. Once the design review group has reached consensus, the MongoDB engineer will
let you know and you can begin typing with the confidence that, once done and passing tests,
your request will be pulled ASAP.
4. Make the code changes and open a pull request. Note that almost all changes require tests to ensure that the code works as expected. MongoDB developers will review the code changes and run our automation tests against a patch build. If everything looks good, the pull request can be merged.

##### A note on “Fix Version/s”
The SERVER JIRA project uses the “Fix Version/s” field to indicate the current scheduling for tickets. Here is what they mean:
- X.Y.Z or “X.Y Required/Desired”, where X, Y and Z are numbers: a ticket has been scheduled for the X.Y version of MongoDB.
- “Needs Triage”: a ticket needs to be scheduled
- “Backlog”: a ticket to be considered in a future round of planning
- “Features we’re not sure of”: tickets representing changes to MongoDB that are currently not being considered for implementation, but may in the future.

### Other Resources for help with MongoDB
**Documentation**

https://docs.mongodb.org/

**Mailing Lists and IRC**

https://www.mongodb.org/community

**MongoDB University**

https://university.mongodb.com/  