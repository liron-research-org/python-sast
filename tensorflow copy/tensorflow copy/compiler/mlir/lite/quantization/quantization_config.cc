/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/compiler/mlir/lite/quantization/quantization_config.h"

#include <algorithm>
#include <ios>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "tensorflow/core/framework/types.pb.h"

// Is this dtype a quantization type from TensorFlow.
static bool IsQuantizationType(tensorflow::DataType dtype) {
  switch (dtype) {
    case tensorflow::DT_QINT8:
    case tensorflow::DT_QUINT8:
    case tensorflow::DT_QINT16:
    case tensorflow::DT_QUINT16:
    case tensorflow::DT_QINT32:
      return true;
    default:
      return false;
  }
}

namespace mlir {
namespace quant {
namespace {
bool GetBooleanSpecs(const std::string& bool_val) {
  bool result;
  std::stringstream iss(bool_val);
  iss >> std::boolalpha >> result;
  return result;
}
}  // namespace

void ParseCustomOpSpecs(absl::string_view node_names,
                        const CustomOpUpdateOptions& update_option,
                        CustomOpMap& custom_op_map) {
  if (node_names.empty()) return;

  std::vector<std::string> custom_nodes = absl::StrSplit(node_names, ',');

  for (auto& cur_node : custom_nodes) {
    std::vector<std::string> node_infos = absl::StrSplit(cur_node, '=');
    std::string node_name = node_infos[0];
    auto node_specification = node_infos[1];
    CustomOpInfo new_node_info;
    switch (update_option) {
      case CustomOpUpdateOptions::kInputIndices: {
        std::vector<std::string> indices =
            absl::StrSplit(node_specification, '-');
        for (auto& cur_index : indices) {
          custom_op_map[node_name].quantizable_input_indices.push_back(
              std::stoi(cur_index));
        }
        break;
      }
      case CustomOpUpdateOptions::kWeightOnly:
        custom_op_map[node_name].is_weight_only =
            GetBooleanSpecs(node_specification);
        break;
      case CustomOpUpdateOptions::kNoSideEffect:
        custom_op_map[node_name].no_side_effect =
            GetBooleanSpecs(node_specification);
        break;
    }
  }
}

bool ParseInputNodeQuantSpecs(absl::string_view node_names,
                              absl::string_view min_values,
                              absl::string_view max_values,
                              absl::string_view inference_type,
                              QuantizationSpecs* quant_specs) {
  std::vector<std::string> input_nodes = absl::StrSplit(node_names, ',');
  std::vector<std::optional<double>> node_mins;
  if (!min_values.empty()) {
    std::vector<std::string> node_mins_str = absl::StrSplit(min_values, ',');
    for (int i = 0, e = node_mins_str.size(); i < e; i++) {
      double value;
      if (!absl::SimpleAtod(node_mins_str[i], &value)) {
        return true;
      }
      node_mins.push_back(value);
    }
  }

  std::vector<std::optional<double>> node_maxs;
  if (!max_values.empty()) {
    std::vector<std::string> node_maxs_str = absl::StrSplit(max_values, ',');
    for (int i = 0, e = node_maxs_str.size(); i < e; i++) {
      double value;
      if (!absl::SimpleAtod(node_maxs_str[i], &value)) {
        llvm::errs() << "Unexpected mins: " << node_maxs_str[i] << "\n";
        return true;
      }
      node_maxs.push_back(value);
    }
  }

  tensorflow::DataType final_type = tensorflow::DT_FLOAT;
  if (!inference_type.empty() &&
      !DataType_Parse(std::string(inference_type), &final_type)) {
    return true;
  }
  return GetInputNodeQuantSpecs(input_nodes, node_mins, node_maxs, final_type,
                                quant_specs);
}

bool GetInputNodeQuantSpecs(const std::vector<std::string>& node_names,
                            const std::vector<std::optional<double>>& node_mins,
                            const std::vector<std::optional<double>>& node_maxs,
                            tensorflow::DataType inference_type,
                            QuantizationSpecs* quant_specs) {
  quant_specs->inference_type = inference_type;

  // If min/max are not specified, just return;
  if (node_mins.empty() || node_maxs.empty()) return false;

  // Otherwise make sure min/max has the same size as inputs.
  if (IsQuantizationType(inference_type)) {
    // min/max should have same size as inputs, or shouldn't be specified.
    if (node_names.size() != node_mins.size() ||
        node_names.size() != node_maxs.size()) {
      return true;
    }
    for (int i = 0, e = node_names.size(); i != e; ++i) {
      quant_specs->input_ranges.push_back({node_mins[i], node_maxs[i]});
    }
    return false;
  }
  if (!node_mins.empty()) {
    llvm::dbgs() << "Ignored input_min_values.";
  }
  if (!node_maxs.empty()) {
    llvm::dbgs() << "Ignored input_max_values.";
  }
  return false;
}

std::string GetQDQQuantModeString(QDQConversionMode mode) {
  switch (mode) {
    case QDQConversionMode::kQDQStatic:
      return "Static";
    case QDQConversionMode::kQDQDynamic:
      return "Dynamic";
    default:
      return "NoQDQ";
  }
}

}  // namespace quant
}  // namespace mlir
