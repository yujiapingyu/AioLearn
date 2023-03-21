load('@com_google_protobuf//:protobuf.bzl', 'cc_proto_library')

cc_proto_library(
    name = 'proto',
    srcs = glob(['proto/*.proto']),
    visibility = ["//visibility:public"],
)


cc_binary(
    name = 'main_work',
    srcs = glob(['src/*.cpp', 'src/*.h']),
    linkopts = ["-lunwind", "-ltcmalloc", "-laio"],
    copts = ['-g'],
    deps = ['@brpc//:brpc', ':proto', '@com_github_google_glog//:glog']
)

cc_binary(
    name = 'test_aio',
    srcs = glob(['test/*.cpp', 'test/*.h']),
    linkopts = ["-lunwind", "-ltcmalloc", "-laio"],
    copts = ['-g'],
    deps = []
)