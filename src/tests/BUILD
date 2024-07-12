cc_test(
    name = "request_queue_test",
    srcs = ["request_queue_test.cc"],
    size = "small",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//attention_broker_server:attention_broker_server_lib",
        "@mbedcrypto//:lib",
    ],
    linkstatic = 1
)

cc_test(
    name = "request_selector_test",
    srcs = ["request_selector_test.cc"],
    size = "small",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//attention_broker_server:attention_broker_server_lib",
        "@mbedcrypto//:lib",
    ],
    linkstatic = 1
)

cc_test(
    name = "attention_broker_server_test",
    srcs = ["attention_broker_server_test.cc"],
    size = "small",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//attention_broker_server:attention_broker_server_lib",
        "@mbedcrypto//:lib",
    ],
    linkstatic = 1
)

cc_test(
    name = "worker_threads_test",
    srcs = ["worker_threads_test.cc", "test_utils.cc", "test_utils.h"],
    size = "small",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//attention_broker_server:attention_broker_server_lib",
        "@mbedcrypto//:lib",
    ],
    linkstatic = 1
)

cc_test(
    name = "handle_trie_test",
    srcs = ["handle_trie_test.cc", "test_utils.cc", "test_utils.h"],
    size = "medium",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//attention_broker_server:attention_broker_server_lib",
        "@mbedcrypto//:lib",
    ],
    linkstatic = 1
)

cc_test(
    name = "hebbian_network_test",
    srcs = ["hebbian_network_test.cc", "test_utils.cc", "test_utils.h"],
    size = "medium",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//attention_broker_server:attention_broker_server_lib",
        "@mbedcrypto//:lib",
    ],
    linkstatic = 1
)

cc_test(
    name = "stimulus_spreader_test",
    srcs = ["stimulus_spreader_test.cc", "test_utils.cc", "test_utils.h"],
    size = "small",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "@com_github_singnet_das_proto//:attention_broker_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//attention_broker_server:attention_broker_server_lib",
        "@mbedcrypto//:lib",
    ],
    linkstatic = 1
)
