JIRA is the system we use to track feature requests, bug reports and other requested work. Each project has an associated JIRA project:

-   [MongoDB Server JIRA Project](https://jira.mongodb.org/browse/SERVER)
-   [MongoDB Tools Jira Project](https://jira.mongodb.org/browse/TOOLS)
-   [C Driver JIRA Project](https://jira.mongodb.org/browse/CDRIVER)
-   [C++ Driver JIRA Project](https://jira.mongodb.org/browse/CXX)
-   [C\# Driver JIRA Project](https://jira.mongodb.org/browse/CSHARP)
-   [Java Driver JIRA Project](https://jira.mongodb.org/browse/JAVA)
-   [Node Driver JIRA Project](https://jira.mongodb.org/browse/NODE)
-   [Perl Driver JIRA Project](https://jira.mongodb.org/browse/PERL)
-   [PHP Driver JIRA Project](https://jira.mongodb.org/browse/PHP)
-   [Python Driver JIRA Project](https://jira.mongodb.org/browse/PYTHON)
-   [Ruby Driver JIRA Project](https://jira.mongodb.org/browse/RUBY)
-   [Scala Driver JIRA Project](https://jira.mongodb.org/browse/SCALA)
-   [Hadoop Connector JIRA Project](https://jira.mongodb.org/browse/HADOOP)

Guidelines for Choosing a SERVER Ticket
---------------------------------------

Anyone can file a JIRA ticket, and some tickets may be vague, underspecified, or out of date. Some tickets may be feature requests that cannot or will not be implemented because the request may be incompatible with planned changes or is a feature that would have a greater negative impact than positive.

The following lists some guidelines to help you choose a ticket:

1.  **Choose a ticket that is unassigned or only assigned to a user starting with "Backlog".**
2.  **Do not choose a ticket with status other than "Open".
3.  **Aim for a ticket with Fix Version of the next version.** For example, if the current stable release is 4.4, you want to aim for something in the "4.5 Required" or "4.5 Desired" fix version.
4.  **Choose a ticket with detailed and specific description.** A more thorough specification improves your chances of implementing the changes in a non-breaking way.
5.  **Avoid tickets with debates over the implementation details in the comments.** Approvals are less likely for changes based on a controversial tickets.
6.  **Look for tickets without any or fewer cross-dependencies.** A ticket that has numerous links to other tickets, depends on other tickets, or have other tickets depend upon it require more research and can be difficult to implement correctly without being in close contact with the persons implementing the others tickets.

    If you're just starting out, try to pick a ticket that has fewer cross-dependencies.

7.  **Verify approval of New Feature requests.** To avoid [feature creep](http://en.wikipedia.org/wiki/Feature_creep), requests for New Feature undergo a review and approval process. If choosing a New Feature request ticket, verify that we have approved the request.

Scope Consideration
-------------------

It is important to keep your changes to the scope of the JIRA ticket. This helps us accurately track changes.

For instance, do not correct typos or trailing whitespace or anything else outside of the scope of the ticket. If your IDE of choice has auto-formatting, turn that feature off.

If you go outside the scope of the ticket, your changes are unlikely to be merged.

SERVER-Specific JIRA FAQs
-------------------------

-   What do the JIRA ticket Fix version numbers mean?  

    We break down specific versions into subcategories. Ordered from most to least pressing, these are:

    1.  &lt;Version&gt; Desired (for whichever is due out next, e.g. 3.3.)
    2.  Backlog

-   What does the [\#neweng label](https://jira.mongodb.org/secure/IssueNavigator.jspa?reset=true&jqlQuery=labels+%3D+neweng) mean?  
    This label exists for tickets that we think are a good entry point for engineers who are new to the codebase. The tickets are small in scope, provide a good learning experience, and/or are otherwise appropriate for a new server contributor.

-   Is there any way to get feedback on the ticket I picked?  
    Yes. Once you have picked a ticket, you can email the [mongodb-dev mailing list](https://groups.google.com/forum/#!forum/mongodb-dev) to get feedback on your chosen ticket. This mailing list is specifically for anyone interested in contributing to the database or building third party tools around and is a great resource for feedback throughout the process of contributing to the server.