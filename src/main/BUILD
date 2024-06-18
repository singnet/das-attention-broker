package(default_visibility = ["//visibility:public"])

cc_library(
    name = "main_lib",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    deps = [
        "//attention_broker_server:attention_broker_server_lib",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
    ],
)
