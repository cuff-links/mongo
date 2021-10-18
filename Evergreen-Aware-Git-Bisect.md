## TLDR
`bisect` will print the **last known passing version** and **first known failing version** of _mongo_ for a `--branch`, `--variant` and `--lookback` period, with respect to a user provided shell `--script`.
## Usage
`bisect` can be invoked from within the mongo repo like so:
```
buildscripts/resmoke.py bisect
  --branch   ["master", "v5.1", "v5.0", etc.]
  --variant  ["enterprise-rhel-80-64-bit-dynamic-required", "ubuntu1804-debug-asan", etc.]
  --lookback [30, 90, 180, etc]
  --script    [shell script to indicate presence of bug]
  --python-installation [(optional) path to python3.7+]
```
To use this command, all that is needed is to write the shell `--script` which runs against a mongo version and returns `0` upon successful execution. The `--script` will be run from within the `build/resmoke-bisect` directory described [below](https://github.com/mongodb/mongo/wiki/Evergreen-Aware-Git-Bisect#bisect).
## Bisect
All versions for the given `--branch`, `--variant` and `--lookback` period are fetched from Evergreen. 

The _repo, binaries, artifacts and virtual environment_ are downloaded to the `build/resmoke-bisect` directory for the _middle version_. This is what the `build/resmoke-bisect` directory may look like — where `mongo_repo` is the github repo and `bisect_venv` is the virtual environment:
```
bisect_venv		mongo_repo		mongobridge		mongod			mongokerberos		
mongoqd			mongotmock		wt                      mongo			mongoauditdecrypt	
mongocryptd		mongodecrypt		mongoldap		mongos			mqlrun
```
The `--script` is run from within this `build/resmoke-bisect` directory. If it completes successfully, `bisect` continues on newer versions. Else, if it fails, `bisect` continues on older versions. This is repeated until the algorithm completes.

The **last known passing version** and **first known failing version** print to `stdout`
## Notes
1. This `bisect` command assumes a perfect partition between passing & failing versions. ie: `[Pass, Pass, Pass, Fail, Fail, Fail]`. If there is not a perfect partition, this command will not work as expected. If there is not a perfect partition, try modifying the `--lookback` period or shell `--script`.

2. Bisecting to beyond a year may not be supported since Evergreen artifacts are only stored for a year.

3. Older versions will have different syntax, so you may need to run `resmoke.py` instead of `resmoke.py run` if the latter fails in the shell script. 

## Help
```
buildscripts/resmoke.py bisect --help
```