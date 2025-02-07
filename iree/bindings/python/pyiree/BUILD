# Copyright 2019 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("//iree:build_defs.bzl", "iree_py_extension")

package(
    default_visibility = ["//visibility:public"],
    licenses = ["notice"],  # Apache 2.0
)

exports_files(["requirements.txt"])

iree_py_extension(
    name = "binding",
    srcs = [
        "binding.cc",
        "binding.h",
        "compiler.cc",
        "compiler.h",
        "status_utils.cc",
        "status_utils.h",
    ],
    copts = [
        "-fexceptions",
    ],
    features = ["-use_header_modules"],
    deps = [
        "//iree/base:flatbuffer_util",
        "//iree/base:status",
        "//iree/compiler/Translation/Sequencer",
        "//iree/schemas",
        "@iree_pybind11//:pybind11",
        "@llvm//:support",
        "@local_config_mlir//:IR",
        "@local_config_mlir//:Parser",
    ],
)

py_test(
    name = "compiler_test",
    srcs = ["compiler_test.py"],
    python_version = "PY3",
    deps = [
        ":binding",
        "//iree/bindings/python:pathsetup",
        "@absl_py//absl/testing:absltest",
    ],
)
