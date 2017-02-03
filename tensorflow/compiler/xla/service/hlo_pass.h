/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_COMPILER_XLA_SERVICE_HLO_PASS_H_
#define TENSORFLOW_COMPILER_XLA_SERVICE_HLO_PASS_H_

#include <string>

#include "tensorflow/compiler/xla/service/hlo_module.h"
#include "tensorflow/compiler/xla/status_macros.h"
#include "tensorflow/compiler/xla/statusor.h"
#include "tensorflow/compiler/xla/types.h"
#include "tensorflow/core/platform/macros.h"

namespace xla {

// Base class for HLO passes. These are used with the HloPassPipeline to
// organize a sequence of passes.
class HloPass {
 public:
  explicit HloPass(const string& name) : name_(name) {}
  virtual ~HloPass() {}

  const string& name() const { return name_; }

  // Run the pass on the given HLO module.  Return whether it modified the
  // module.
  virtual StatusOr<bool> Run(HloModule* module) = 0;

 private:
  const string name_;

  TF_DISALLOW_COPY_AND_ASSIGN(HloPass);
};

// Do an HLO pass to a fix point.
template <typename Pass>
class HloPassFix : public Pass {
 public:
  template <typename... Args>
  explicit HloPassFix(Args&&... args) : Pass(args...) {}

  StatusOr<bool> Run(HloModule* module) override {
    bool changed = false;
    bool changed_this_iteration = true;
    while (changed_this_iteration) {
      TF_ASSIGN_OR_RETURN(changed_this_iteration, Pass::Run(module));
      changed |= changed_this_iteration;
    }
    return changed;
  }
};

}  // namespace xla

#endif  // TENSORFLOW_COMPILER_XLA_SERVICE_HLO_PASS_H_