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

// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "../op.hpp"
#include "nnfusion/core/graph/graph.hpp"
#include "nnfusion/engine/interpreter.hpp"

namespace nnfusion
{
    namespace op
    {
        /// \brief If control-flow operation, with same definition as https://github.com/onnx/onnx/blob/master/docs/Changelog.md#If-1.
        class If : public Op
        {
        public:
            /// \brief Constructs an if operation
            ///
            /// \param then_branch_graph The then_branch graph.<br>
            /// `[f]`
            /// \param else_branch_graph The else_branch graph.<br>
            /// `[f]`
            If(std::shared_ptr<nnfusion::graph::Graph>& then_branch_graph,
               std::shared_ptr<nnfusion::graph::Graph>& else_branch_graph,
               const std::vector<nnfusion::PartialShape>& output_shapes,
               const std::vector<nnfusion::element::Type>& output_types);

            void validate_and_infer_types(std::shared_ptr<graph::GNode> gnode) override;
            std::shared_ptr<nnfusion::graph::Graph> get_then_branch_graph();
            std::shared_ptr<nnfusion::graph::Graph> get_else_branch_graph();
            TranslationUnit::Pointer get_then_branch_tu();
            void set_then_branch_tu(TranslationUnit::Pointer);
            TranslationUnit::Pointer get_else_branch_tu();
            void set_else_branch_tu(TranslationUnit::Pointer);
            std::unordered_map<std::string, int> get_output_map() { return m_output_map; }
            void set_output_map(std::unordered_map<std::string, int> map)
            {
                m_output_map = std::move(map);
            }

            Shape get_output_shape(int i) { return m_output_shapes[i].to_shape(); }
            void set_output_shape(int i, const Shape& shape) { m_output_shapes[i] = PartialShape(shape); }

        protected:
            std::shared_ptr<nnfusion::graph::Graph> m_then_branch_graph;
            std::shared_ptr<nnfusion::graph::Graph> m_else_branch_graph;
            TranslationUnit::Pointer m_then_branch_tu, m_else_branch_tu;
            std::vector<nnfusion::PartialShape> m_output_shapes;
            std::vector<nnfusion::element::Type> m_output_types;
            std::unordered_map<std::string, int> m_output_map;
        };
    } // namespace op
} // namespace nnfusion
