This `benchRun` command is designed as a QA baseline performance measurement tool; it is *not* designed to be a "benchmark".

``` sourceCode
db.foo.drop();
db.foo.insert( { _id : 1 } )

ops = [{op: "findOne", ns: "test.foo", query: {_id: 1}},
       {op: "update", ns: "test.foo", query: {_id: 1}, update: {$inc: {x: 1}}}]

for ( var x = 1; x <= 128; x *= 2) {
    res = benchRun( {
        parallel : x ,
        seconds : 5 ,
        ops : ops
    } );
    print( "threads: " + x + "\t queries/sec: " + res.query );
}
```

Dynamic Values
--------------

``` sourceCode
// benchmark updates using the $inc operator
res = benchRun( {
    ops : [ {
        ns : "test.foo" ,
        op : "update" ,
        query : { _id : { "#RAND_INT" : [ 0 , 100 ] } } ,
        update : { $inc : { x : 1 } }
    } ] ,
    parallel : 2 ,
    seconds : 1
} );
print( "threads: 2\t update/sec: " + res.update );
// benchmark inserts with random strings
res = benchRun( {
    ops : [ {
        ns : "test.foo" ,
        op : "insert" ,
        doc : { y : { "#RAND_STRING" : [ 10 ] } }
    } ] ,
    parallel : 2 ,
    seconds : 1
} );
print( "threads: 2\t insert/sec: " + res.insert );
```

Options
-------

-   host

    The hostname of the machine mongod is running on (defaults to localhost).

-   username

    The username to use when authenticating to mongod (only use if running with `auth`).

-   password

    The password to use when authenticating to mongod (only use if running with `auth`).

-   db

    The database to authenticate to (only necessary if running with `auth`).

-   ops

    A list of objects describing the operations to run (documented below).

-   parallel

    The number of threads to run (defaults to single thread).

-   seconds

    The amount of time to run the tests for (defaults to one second).

-   hideResults

    Prevents results from each operation executed from being logged. Default value is `true`. Overridden by setting `showResult` on a specific operation.

-   handleErrors

    Prevents a thread from stopping on error. Default value is `false`. Overridden by setting `handleError` on a specific operation.

-   hideErrors

    Prevents errors from being logged. Default value is `false`. Overridden by setting `showError` for a specific operation.

-   throwGLE

    Causes `benchRun` to actually throw an exception on errors in updates/inserts/deletes rather than handling them. Default value is `false` to have `benchRun` to handle the errors itself. `throwGLE` can also be set on specific operations.

Operation Options
-----------------

-   ns

    The namespace of the collection you are running the operation on, should be of the form `"db.collection"`.

-   op

    The type of operation can be `"findOne"`, `"find"`, `"insert"`, `"update"`, `"remove"`, `"createIndex"`, `"dropIndex"` or `"command"`.

-   query

    The query object to use when querying or updating documents.

-   update

    The update object (same as 2nd argument of `update()` function).

-   doc

    The document to insert into the database (only for insert and remove).

-   safe

    `boolean` specifies whether to use safe writes (only for update and insert).

-   delay

    `integer` specifies the time to wait between executing the operation.

-   skip

    Only applies to find operations. `integer` specifies the number of documents to skip in a query.

-   limit

    Only applies to find operations. `integer` specifies the number of elements a query should return.

-   batchSize

    Only applies to find operations. `integer` specifies the number of elements the returned cursor should cache.

-   filter

    Only applies to find operations. Document specifies which fields should be returned.

-   expected

    Only applies to find operations. `integer` specifies the expected number of elements returned. If the count is not as expected the operation will throw an error. Turning on the `handleErrors` option allows `benchRun` to continue if the count was not as expected.

-   multi

    > `boolean` specifies whether or not the update or delete should apply to multiple documents. Default value is `false`.

-   upsert

    > `boolean` specifies whether or not an update operation should be run as an upsert. Default value is `false`.

-   key

    > Only works for and is required by the `createIndex` and `dropIndex` operations. The definition for the index to create or drop.

-   showResult

    `boolean` specifies whether to log the result of each call of the operation.

Dynamic Operators
-----------------

``` sourceCode
{ "#RAND_INT" : [ min , max , <multiplier> ] }
```

-   `[ 0 , 10 , 4 ]` would produce random numbers between `0` and `10` and then multiply by `4`.

``` sourceCode
{ "#RAND_STRING" : [ length ] }
```

-   `[ 3 ]` would produce a string of `3` random characters.

Dynamic operators generate random strings, random ints, etc but don't work in second level objects, just main level.

-   This is fine:

    ``` sourceCode
    var complexDoc3 = { info: "#RAND_STRING": [30] } }
    ```

-   This is only going to insert a value called `"#RAND_STRING"` with an array as a key:

    ``` sourceCode
    var complexDoc3 = { info: { inner_field: { "#RAND_STRING": [30] } } }
    ```

More info:

-   [Bench Test 1](http://github.com/mongodb/mongo/blob/master/jstests/core/bench_test1.js)
-   [Bench Test 2](http://github.com/mongodb/mongo/blob/master/jstests/core/bench_test2.js)
-   [Bench Test 3](http://github.com/mongodb/mongo/blob/master/jstests/core/bench_test3.js)

