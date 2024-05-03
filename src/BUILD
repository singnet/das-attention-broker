#load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "attention_broker",
    srcs = [
        "main.cc",
        "attention_broker.h",
        "attention_broker.cc",
    ],
    defines = ["BAZEL_BUILD"],
    deps = [
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
    ],
    visibility = ["//visibility:public"],
)
