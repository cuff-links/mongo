To aid testing, MongoDB has the "configureFailpoint" command that can alter server behavior in a wide variety of ways, mostly simulating types of failure that are difficult to cause reliably in tests. To enable the "configureFailpoint" command, mongod must be started like:
```
mongod --setParameter enableTestCommands=1
```
Same for mongos.

There are dozens of failpoints, the most powerful is named "failCommand", introduced in [mongod 4.0](https://jira.mongodb.org/browse/SERVER-34551) and [mongos 4.1.5](https://jira.mongodb.org/browse/SERVER-35518). Here is a basic example of shell code that causes the "find" command to fail with error code 2:
```js
db.adminCommand({
    configureFailPoint: "failCommand",
    mode: "alwaysOn",
    data: {errorCode: 2, failCommands: ["find"]}
});
// Returns {ok: 0, count: 0, code: 2}
db.runCommand({find: "collection"});
db.adminCommand({configureFailPoint: "failCommand", mode: "off"});
```
A jsTest that uses this failpoint might look like this:
```js
assert.commandWorked(db.adminCommand({
    configureFailPoint: "failCommand",
    mode: "alwaysOn",
    data: {errorCode: ErrorCodes.BadValue, failCommands: ["find"]}
}));
assert.commandFailedWithCode(db.runCommand({find: "collection"}), ErrorCodes.BadValue);
db.adminCommand({configureFailPoint: "failCommand", mode: "off"});
```
As shown above, the configureFailPoint command returns "count" which is the number of times the failpoint has been entered up until that point. To wait for a failpoint to be hit, use the waitForFailPoint command. The command takes in the target number of times and maxTimeMS, and blocks until the fail point has been entered that number of times or until it times out. So in the example above, one can wait for the "failCommand" failpoint by doing: 
```js
let count = db.adminCommand({
    configureFailPoint: "failCommand",
    mode: "alwaysOn",
    data: {errorCode: 2, failCommands: ["find"]}
}).count;
db.adminCommand({waitForFailPoint: 'failCommand', timesEntered: count+1, maxTimeMS: 1000});
db.adminCommand({configureFailPoint: "failCommand", mode: "off"});
```

# configureFailpoint parameters

The configureFailpoint command itself is undocumented, the following are parameters relevant to the "failCommand" failpoint:

- mode (required): One of "alwaysOn", "off", a document like ``{times: N}``, where "N" is a positive integer, or a document like ``{activationProbability: D}``, where "D" is a double between 0 and 1.
- data: A subdocument containing parameters for the "failCommand" failpoint (or any failpoint). Required unless mode is "off".

If the mode is like ``{times: N}``, the failpoint will automatically switch to mode "off" after firing the specified number of times. (See "failInternalCommands" below.)

# failCommand parameters

Each parameter is passed as a field in the "data" document provided to "configureFailPoint". The "data" document is ignored if mode is "off".

- failCommands (required): Array of command names that should be affected by the failpoint.
- closeConnection: Whether the server should hang up when the client sends an affected command. Default false.
- errorCode: The error code to include in the server's reply to an affected command.
- writeConcernError: Subdocument with arbitrary contents. Pass something like ``{code: N, errmsg: "foo"}``.
- threadName: Which thread should be affected by the failpoint. [New in mongod 4.1.6](https://jira.mongodb.org/browse/SERVER-38054).
- failInternalCommands: Whether commands from mongod or mongos servers should be affected. If false and mode is "times", commands from mongod or mongos do not count against the "times" argument. Default false. [New in MongoDB 4.0.6 and 4.1.7](https://jira.mongodb.org/browse/SERVER-34943).
- errorLabels: Array of error labels to be included in the server's reply to an affected command. Passing in an empty array suppresses all error labels that would otherwise be returned by the server. The existence of the "errorLabels" field in the failCommand failpoint completely overrides the server's normal error labels adding behaviors for the affected commands. [New in MongoDB 4.3.1](https://jira.mongodb.org/browse/SERVER-43941).

If closeConnection is true, errorCode and writeConcernError have no effect. If closeConnection is false and errorCode is set, writeConcernError has no effect. If only writeConcernError is set, it affects all write commands but no other commands. If closeConnection, errorCode, and writeConcernError are all omitted, the failpoint has no effect.

Threadname can be obtained by code like the following (from failcommand_failpoint.js):
```js
const getThreadName = function() {
    let myUri = adminDB.runCommand({whatsmyuri: 1}).you;
    return adminDB.aggregate([{$currentOp: {localOps: true}}, {$match: {client: myUri}}])
        .toArray()[0]
        .desc;
};
```

The behavior of the configureFailpoint command itself is never affected by the failCommand failpoint.

The logic to decide whether a command is affected by the failCommand failpoint is implemented in ``CommandHelpers::shouldActivateFailCommandFailPoint()``, and its effect upon the command is implemented in ``CommandHelpers::evaluateFailCommandFailPoint()``.
