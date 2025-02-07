// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef IREE_VM_BYTECODE_VALIDATOR_H_
#define IREE_VM_BYTECODE_VALIDATOR_H_

#include "iree/base/status.h"
#include "iree/schemas/bytecode_def_generated.h"
#include "iree/vm/bytecode_module.h"

namespace iree {
namespace vm {

// Validates bytecode such that success indicates the bytecode does not
// reference undefined types, functions, or required imports and all imports can
// be resolved with matching signatures.
class BytecodeValidator {
 public:
  static Status Validate(const BytecodeModule& module,
                         const BytecodeDef& bytecode_def);
};

}  // namespace vm
}  // namespace iree

#endif  // IREE_VM_BYTECODE_VALIDATOR_H_
