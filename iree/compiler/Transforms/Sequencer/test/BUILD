# Tests specific to the sequencer.

load("//iree:build_defs.bzl", "iree_glob_lit_tests", "iree_setup_lit_package")

package(
    default_visibility = ["//visibility:public"],
    licenses = ["notice"],  # Apache 2.0
)

iree_setup_lit_package(
    data = [
        "//iree/tools:iree-opt",
    ],
)

iree_glob_lit_tests()
