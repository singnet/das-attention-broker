#!/bin/bash

/opt/bazel/bazelisk build --jobs 6 --noenable_bzlmod :link_creation_engine
mv bazel-bin/link_creation_engine ../bin
/opt/bazel/bazelisk build --jobs 6 --noenable_bzlmod :word_query
mv bazel-bin/word_query ../bin
/opt/bazel/bazelisk build --jobs 6 --noenable_bzlmod :attention_broker
mv bazel-bin/attention_broker ../bin
/opt/bazel/bazelisk build --jobs 6 --noenable_bzlmod :query_broker
mv bazel-bin/query_broker ../bin
/opt/bazel/bazelisk build --jobs 6 --noenable_bzlmod :query
mv bazel-bin/query ../bin
