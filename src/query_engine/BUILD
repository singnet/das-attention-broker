package(default_visibility = ["//visibility:public"])

cc_library(
    name = "query_engine_lib",
    srcs = glob(["*.cc", "query_element/*.cc"]),
    hdrs = glob(["*.h", "query_element/*.h"]),
    includes = [".", "query_element"],
    deps = [
        "//attention_broker_server:attention_broker_server_lib",
        "//hasher:hasher_lib",
        "//utils:utils_lib",
        "@com_github_singnet_das_node//:atomspacenode",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
    ],
)
