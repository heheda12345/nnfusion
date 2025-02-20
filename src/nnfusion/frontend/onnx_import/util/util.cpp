//*****************************************************************************
// Copyright 2017-2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

//----------------------------------------------------------------------------------------------
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Licensed under the MIT License. See License.txt in the project root for license information.
//----------------------------------------------------------------------------------------------

#include "util.hpp"
#include "../core/node.hpp"
#include "../core/tensor.hpp"
namespace nnfusion
{
    namespace frontend
    {
        namespace onnx_import
        {
            bool ONNXDataTypeToNNFusionElementType(const onnx::TensorProto_DataType onnx_dt,
                                                   nnfusion::element::Type* nnfusion_et)
            {
                switch (onnx_dt)
                {
                case onnx::TensorProto_DataType::TensorProto_DataType_BOOL:
                    *nnfusion_et = element::boolean;
                    break;
                case onnx::TensorProto_DataType::TensorProto_DataType_FLOAT:
                case onnx::TensorProto_DataType::TensorProto_DataType_FLOAT16:
                    *nnfusion_et = element::f32;
                    break;
                case onnx::TensorProto_DataType::TensorProto_DataType_DOUBLE:
                    *nnfusion_et = element::f64;
                    break;
                case onnx::TensorProto_DataType::TensorProto_DataType_INT8:
                    *nnfusion_et = element::i8;
                    break;
                case onnx::TensorProto_DataType::TensorProto_DataType_INT16:
                    *nnfusion_et = element::i16;
                    break;
                case onnx::TensorProto_DataType::TensorProto_DataType_INT32:
                    *nnfusion_et = element::i32;
                    break;
                case onnx::TensorProto_DataType::TensorProto_DataType_INT64:
                    *nnfusion_et = element::i64;
                    break;
                case onnx::TensorProto_DataType::TensorProto_DataType_UINT8:
                    *nnfusion_et = element::u8;
                    break;
                case onnx::TensorProto_DataType::TensorProto_DataType_UINT16:
                    *nnfusion_et = element::u16;
                    break;
                case onnx::TensorProto_DataType::TensorProto_DataType_UINT32:
                    *nnfusion_et = element::u32;
                    break;
                case onnx::TensorProto_DataType::TensorProto_DataType_UINT64:
                    *nnfusion_et = element::u64;
                    break;
                default:
                    NNFUSION_CHECK_FAIL() << "unsupported onnx element type: "
                                          << onnx::TensorProto_DataType_Name(onnx_dt);
                    return false;
                }
                return true;
            }

            template <typename T>
            std::shared_ptr<op::Constant>
                make_constant_op(const element::Type& type, const Shape shape, const Tensor& tensor)
            {
                return std::make_shared<op::Constant>(type, shape, tensor.get_data<T>());
            }

            std::shared_ptr<op::Constant> make_constant_op(const onnx::TensorProto_DataType onnx_et,
                                                           const Shape shape,
                                                           const Tensor& tensor)
            {
                switch (onnx_et)
                {
                case onnx::TensorProto_DataType::TensorProto_DataType_BOOL:
                    return make_constant_op<bool>(element::boolean, shape, tensor);
                case onnx::TensorProto_DataType::TensorProto_DataType_FLOAT:
                case onnx::TensorProto_DataType::TensorProto_DataType_FLOAT16:
                    return make_constant_op<float>(element::f32, shape, tensor);
                case onnx::TensorProto_DataType::TensorProto_DataType_DOUBLE:
                    return make_constant_op<double>(element::f64, shape, tensor);
                case onnx::TensorProto_DataType::TensorProto_DataType_INT8:
                    return make_constant_op<int8_t>(element::i8, shape, tensor);
                case onnx::TensorProto_DataType::TensorProto_DataType_INT16:
                    return make_constant_op<int16_t>(element::i16, shape, tensor);
                case onnx::TensorProto_DataType::TensorProto_DataType_INT32:
                    return make_constant_op<int32_t>(element::i32, shape, tensor);
                case onnx::TensorProto_DataType::TensorProto_DataType_INT64:
                    return make_constant_op<int64_t>(element::i64, shape, tensor);
                case onnx::TensorProto_DataType::TensorProto_DataType_UINT8:
                    return make_constant_op<uint8_t>(element::u8, shape, tensor);
                case onnx::TensorProto_DataType::TensorProto_DataType_UINT16:
                    return make_constant_op<uint16_t>(element::u16, shape, tensor);
                case onnx::TensorProto_DataType::TensorProto_DataType_UINT32:
                    return make_constant_op<uint32_t>(element::u32, shape, tensor);
                case onnx::TensorProto_DataType::TensorProto_DataType_UINT64:
                    return make_constant_op<uint64_t>(element::u64, shape, tensor);
                default:
                    NNFUSION_CHECK_FAIL() << "unsupported value info element type: "
                                          << onnx::TensorProto_DataType_Name(onnx_et);
                }
                return make_constant_op<float>(element::f32, shape, tensor);
            }

            std::shared_ptr<graph::GNode> GetInputNode(const NodeMap& all_ng_nodes,
                                                       const onnx::NodeProto& node,
                                                       size_t input_idx)
            {
                std::shared_ptr<graph::GNode> result = nullptr;
                try
                {
                    result = all_ng_nodes.at(node.input(input_idx)).at(0).gnode;
                }
                catch (const std::out_of_range&)
                {
                    if (node.input(input_idx) == "")
                    {
                        return nullptr;
                    }
                    NNFUSION_CHECK_FAIL() << "Input Ngraph op not found for "
                                          << node.input(input_idx);
                }
                return result;
            }

            // TODO: replace all GetInputNode to GetInputIndex
            GNodeIndex GetInputIndex(const NodeMap& all_ng_nodes,
                                     const onnx::NodeProto& node,
                                     size_t input_idx)
            {
                GNodeIndex result{nullptr};
                try
                {
                    result = all_ng_nodes.at(node.input(input_idx)).at(0);
                }
                catch (const std::out_of_range&)
                {
                    if (node.input(input_idx) == "")
                    {
                        return GNodeIndex{nullptr};
                    }
                    NNFUSION_CHECK_FAIL() << "Input Ngraph op not found for "
                                          << node.input(input_idx);
                }
                return result;
            }

            graph::GNodeVector GetAllInputNode(const NodeMap& all_ng_nodes,
                                               const onnx::NodeProto& node)
            {
                graph::GNodeVector nodes;
                for (size_t i = 0; i < node.input_size(); i++)
                {
                    nodes.push_back(GetInputNode(all_ng_nodes, node, i));
                }
                return nodes;
            }

            GNodeIndexVector GetAllInputIndex(const NodeMap& all_ng_nodes,
                                              const onnx::NodeProto& node)
            {
                GNodeIndexVector indexes;
                for (size_t i = 0; i < node.input_size(); i++)
                {
                    indexes.push_back(GetInputIndex(all_ng_nodes, node, i));
                }
                return indexes;
            }

            Shape get_kernel_shape(const Node& node,
                                   const std::shared_ptr<graph::GNode> input_gnode) {
                NNFUSION_CHECK(input_gnode->get_output_size() == 1);
                return get_kernel_shape(node, input_gnode, 0);
            }

            Shape get_kernel_shape(const Node& node,
                                   const std::shared_ptr<graph::GNode> input_gnode,
                                   int index)
            {
                std::size_t input_spacial_dims = input_gnode->get_output_shape(index).size() - 2;
                return node.get_attribute_value<std::vector<std::size_t>>(
                    "kernel_shape", std::vector<std::size_t>(input_spacial_dims, 1UL));
            }

            Strides get_strides(const Node& node, const Shape& kernel_shape)
            {
                return get_strides_helper(node, "strides", kernel_shape);
            }

            Strides get_strides(const Node& node, const std::shared_ptr<graph::GNode> input_gnode)
            {
                NNFUSION_CHECK(input_gnode->get_output_size() == 1);
                return get_strides(node, input_gnode, 0);
            }

            Strides get_strides(const Node& node, const std::shared_ptr<graph::GNode> input_gnode, int index)
            {
                return get_strides(node, get_kernel_shape(node, input_gnode, index));
            }

            Strides get_dilations(const Node& node, const std::shared_ptr<graph::GNode> input_gnode)
            {
                NNFUSION_CHECK(input_gnode->get_output_size() == 1);
                return get_dilations(node, input_gnode, 0);
            }

            Strides get_dilations(const Node& node, const std::shared_ptr<graph::GNode> input_gnode, int index)
            {
                return get_strides_helper(node, "dilations", get_kernel_shape(node, input_gnode, index));
            }

            std::pair<CoordinateDiff, CoordinateDiff> get_pads(const Node& node,
                                                               const Shape& kernel_shape)
            {
                CoordinateDiff pads;
                if (node.has_attribute("pads"))
                {
                    auto pads_int64 = node.get_attribute_value<std::vector<int64_t>>("pads");
                    pads = CoordinateDiff{std::begin(pads_int64), std::end(pads_int64)};
                }
                else
                {
                    std::string auto_pad{node.get_attribute_value<std::string>("auto_pad", "")};
                    if (!auto_pad.empty())
                    {
                        pads = get_auto_pads(kernel_shape, auto_pad);
                    }
                }
                if (pads.empty())
                {
                    pads = CoordinateDiff(static_cast<std::ptrdiff_t>(kernel_shape.size()), 0UL);
                }

                if (pads.size() != kernel_shape.size() * 2)
                {
                    // Paddings specified in (H, W, C) format.
                    return {pads, pads};
                }
                else
                {
                    return {{std::begin(pads) + pads.size() / 2, std::end(pads)},
                            {std::begin(pads), std::begin(pads) + pads.size() / 2}};
                }
            }

            CoordinateDiff get_auto_pads(const Shape& kernel_shape, const std::string& auto_pad)
            {
                CoordinateDiff pads;

                // Add padding to the input to match the size of output size.
                auto pad_value = [](size_t dim) { return (static_cast<float>(dim) - 1.f) / 2.f; };

                if (auto_pad == "SAME_UPPER")
                {
                    for (size_t dim : kernel_shape)
                    {
                        pads.emplace_back(std::floor(pad_value(dim)));
                    }
                    for (size_t dim : kernel_shape)
                    {
                        pads.emplace_back(std::ceil(pad_value(dim)));
                    }
                }
                else if (auto_pad == "SAME_LOWER")
                {
                    for (size_t dim : kernel_shape)
                    {
                        pads.emplace_back(std::ceil(pad_value(dim)));
                    }
                    for (size_t dim : kernel_shape)
                    {
                        pads.emplace_back(std::floor(pad_value(dim)));
                    }
                }

                return pads;
            }

            Strides get_strides_helper(const Node& node,
                                       const std::string& name,
                                       const Shape& kernel_shape)
            {
                return node.get_attribute_value<std::vector<std::size_t>>(
                    name, std::vector<std::size_t>(kernel_shape.size(), 1UL));
            }

            std::unordered_set<std::string> extract_input(const onnx::GraphProto& graph_proto, bool ignore_graph_input)
            {
                std::unordered_set<std::string> node_inputs;
                std::unordered_set<std::string> node_outputs;

                for (auto node_proto : graph_proto.node())
                {
                    for (size_t i = 0; i < node_proto.input_size(); i++)
                    {
                        node_inputs.insert(node_proto.input(i));
                    }
                    for (size_t i = 0; i < node_proto.output_size(); i++)
                    {
                        node_outputs.insert(node_proto.output(i));
                    }
                    if (node_proto.op_type() == "If") {
                        Node node(node_proto);
                        auto then_inputs = extract_input(node.get_attribute_value<onnx::GraphProto>("then_branch"));
                        node_inputs.insert(then_inputs.begin(), then_inputs.end());
                        auto else_inputs = extract_input(node.get_attribute_value<onnx::GraphProto>("else_branch"));
                        node_inputs.insert(else_inputs.begin(), else_inputs.end());
                    } else if (node_proto.op_type() == "Loop") {
                        Node node(node_proto);
                        auto body_inputs = extract_input(node.get_attribute_value<onnx::GraphProto>("body"));
                        node_inputs.insert(body_inputs.begin(), body_inputs.end());
                    } else if (node_proto.op_type() == "Recursion") {
                        Node node(node_proto);
                        auto body_inputs = extract_input(node.get_attribute_value<onnx::GraphProto>("body"));
                        node_inputs.insert(body_inputs.begin(), body_inputs.end());
                    }
                }

                std::unordered_set<std::string> graph_inputs;
                for (auto item : node_inputs)
                {
                    if (node_outputs.find(item) == node_outputs.end())
                    {
                        graph_inputs.insert(item);
                    }
                }

                std::unordered_set<std::string> existing_inputs;
                for (auto input_proto : graph_proto.input())
                {
                    existing_inputs.insert(input_proto.name());
                }

                std::unordered_set<std::string> missing_inputs;

                for (auto input : graph_inputs)
                {
                    if (existing_inputs.find(input) == existing_inputs.end())
                    {
                        missing_inputs.insert(input);
                        // std::cout << input << std::endl;
                    }
                }

                return missing_inputs;
            }

            onnx::GraphProto complete_graphproto(const onnx::GraphProto& graph_proto, const onnx::GraphProto& full_graph_proto)
            {
                onnx::GraphProto completed_graphproto(graph_proto);
                std::unordered_set<std::string> missing_inputs = extract_input(graph_proto);

                std::unordered_map<std::string, size_t> full_graph_initializer_index;
                for (size_t i = 0; i < full_graph_proto.initializer().size(); i++) {
                    auto& initializer = full_graph_proto.initializer(i);
                    full_graph_initializer_index[initializer.name()] = i;
                }

                for (auto item : missing_inputs)
                {
                    if (full_graph_initializer_index.find(item) != full_graph_initializer_index.end()) {
                        auto initializer = completed_graphproto.add_initializer();
                        initializer->CopyFrom(full_graph_proto.initializer(full_graph_initializer_index[item]));
                        NNFUSION_LOG(INFO) << "Copy initializer to inner graph: " << item;
                    } else {
                        auto input = completed_graphproto.add_input();
                        input->set_name(item);
                    }
                }

                // std::cout << completed_graphproto.DebugString() << std::endl;

                return completed_graphproto;
            }
            std::shared_ptr<graph::GNode> find_node_from_graph(const graph::Graph::Pointer graph,
                                                               const std::string& name)
            {
                for (auto node : graph->get_nodes())
                {
                    if (node->get_name() == name)
                        return node;
                }
                return nullptr;
            }

        } // namespace onnx_import
    }     // namespace frontend
} // namespace nnfusion
