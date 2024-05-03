#!/bin/bash

/opt/bazel/bazelisk build --jobs 6 --noenable_bzlmod :attention-broker
mv bazel-bin/attention-broker ../bin
/opt/bazel/bazelisk clean
rm -f bazel-src bazel-out bazel-testlogs bazel-bin
