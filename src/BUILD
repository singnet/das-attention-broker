#load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "attention_broker",
    srcs = [],
    defines = ["BAZEL_BUILD"],
    deps = [
        "//attention_broker_server:attention_broker_server_lib",
        "//main:main_lib",
        "//utils:utils_lib",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "@mbedcrypto//:lib",
    ],
)
