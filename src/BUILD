#load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "attention_broker",
    srcs = [],
    defines = ["BAZEL_BUILD"],
    deps = [
        "//attention_broker_server:attention_broker_server_lib",
        "//cache_node:cache_node_lib",
        "//atom_space_node:atom_space_node_lib",
        "//main:main_lib",
        "//commons:commons_lib",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_singnet_das_proto//:atom_space_node_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
    ],
)
