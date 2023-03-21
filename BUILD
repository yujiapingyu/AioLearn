load('@com_google_protobuf//:protobuf.bzl', 'cc_proto_library')

cc_library(
    name = 'decorator',
    srcs = glob(['src/decorator/*.cpp', 'src/decorator/*.h']),
    copts = ['-g'],
    visibility = ["//visibility:public"],
)

cc_library(
    name = 'proxy',
    srcs = glob(['src/proxy/*.cpp', 'src/proxy/*.h']),
    copts = ['-g'],
    visibility = ["//visibility:public"],
)


cc_binary(
    name = 'main_work',
    srcs = glob(['src/*.cpp', 'src/*.h']),
    linkopts = ["-lunwind", "-ltcmalloc"],
    copts = ['-g'],
    deps = ['decorator', 'proxy']
)
