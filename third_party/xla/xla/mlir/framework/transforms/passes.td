/* Copyright 2021 The OpenXLA Authors.

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

include "mlir/Pass/PassBase.td"

def LegalizeXLAFrameworkToLLVM : Pass<"xla-legalize-xla-framework-to-llvm", "ModuleOp"> {
  let summary = "Legalize XLAFramework to bare pointers and LLVM memref descriptors";
  let dependentDialects = ["::mlir::LLVM::LLVMDialect"];
  let constructor = "::mlir::xla_framework::CreateLegalizeXLAFrameworkToLLVMPass()";
}

def OutlineWithXLAFramework : Pass<"outline-with-xla-framework", "ModuleOp"> {
  let summary = "Create XLAFramework based wrapper function to match XLA:CPU's interface";
  let dependentDialects = ["::mlir::xla_framework::XLAFrameworkDialect"];
  let constructor = "::mlir::xla_framework::CreateOutlineWithXLAFrameworkPass()";
}
