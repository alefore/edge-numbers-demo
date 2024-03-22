cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        "@edge//:bigint",
        "@edge//:numbers",
    ],
)

cc_binary(
    name = "calculator",
    srcs = ["calculator.cc"],
    linkopts = [
        "-lreadline",
        "-lhistory",
    ],
    deps = [
        "@com_github_google_glog//:glog",
        "@edge//:vm",
    ],
)
