If there is a fuzzer-generated test failure, a _minimized jstestfuzz test_ will be generated from the original to reduce test size & complexity.  Minimized jstestfuzz tests will be available for download in the files section in the Evergreen task page. 

You can run the minimized test with the following command after setting up a repro environment:

`buildscripts/resmoke.py run --suites=generational_fuzzer['', _last_lts, _last_continuous] /local/path/to/minimized/test/minimizer-outputs-minimizedtest-[build_id].js`

If the above does not work, you can find the exact resmoke invocation by searching for the string "_resmoke.py invocation for local usage_" in the "Task Logs" and add the local file path to the minimized test as a positional argument.

For more information on running tests from Evergreen locally, see the [Wiki: Running Tests from Evergreen Tasks Locally](https://github.com/mongodb/mongo/wiki/Running-Tests-from-Evergreen-Tasks-Locally).