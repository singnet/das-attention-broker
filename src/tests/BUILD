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
        "//commons:commons_lib",
        "@mbedcrypto//:lib",
    ],
    linkstatic = 1
)

cc_test(
    name = "hebbian_network_updater_test",
    srcs = ["hebbian_network_updater_test.cc", "test_utils.cc", "test_utils.h"],
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

cc_test(
    name = "leadership_broker_test",
    srcs = ["leadership_broker_test.cc", "test_utils.cc", "test_utils.h"],
    size = "small",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "//atom_space_node:atom_space_node_lib",
        "//hasher:hasher_lib",
    ],
    linkstatic = 1
)

cc_test(
    name = "message_broker_test",
    srcs = ["message_broker_test.cc", "test_utils.cc", "test_utils.h"],
    size = "small",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "//cache_node:cache_node_lib",
        "//atom_space_node:atom_space_node_lib",
        "//hasher:hasher_lib",
    ],
    linkstatic = 1
)

cc_test(
    name = "cache_node_test",
    srcs = ["cache_node_test.cc", "test_utils.cc", "test_utils.h"],
    size = "small",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "//cache_node:cache_node_lib",
        "//atom_space_node:atom_space_node_lib",
        "//hasher:hasher_lib",
    ],
    linkstatic = 1
)
