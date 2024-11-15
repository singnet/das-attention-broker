#!/bin/bash

/opt/bazel/bazelisk build --jobs 6 --noenable_bzlmod :attention_broker
mv bazel-bin/attention_broker ../bin
/opt/bazel/bazelisk build --jobs 6 --noenable_bzlmod :query_broker
mv bazel-bin/query_broker ../bin
