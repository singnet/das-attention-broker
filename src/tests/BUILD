cc_test(
    name = "attention_broker_test",
    srcs = ["attention_broker_test.cc"],
    size = "small",
    copts = [
      "-Iexternal/gtest/googletest/include",
      "-Iexternal/gtest/googletest",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        "//:attention_broker",
    ],
)

