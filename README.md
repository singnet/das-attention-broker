# DAS - Attention Broker

The Attention Broker is the DAS component which keeps track of atom's
importance values attached to different contexts and update those values
according to the queries made by users using context specific hebbian
networks.

DAS query engine can use those importance values to control caching policies
and to better process pattern matcher queries.

## How to build and run the server

From the main repository directory:

```bash
$ ./scripts/docker_image_build.sh
$ ./scripts/build.sh
$ ./scripts/run.sh PORT
```

## How to run unit tests

From the main repository directory:

```bash
$ ./scripts/unit_tests.sh
```
