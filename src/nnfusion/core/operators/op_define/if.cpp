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

#include "if.hpp"

using namespace std;
using namespace nnfusion::op;

If::If(std::shared_ptr<nnfusion::graph::Graph>& then_branch_graph,
       std::shared_ptr<nnfusion::graph::Graph>& else_branch_graph,
       const std::vector<nnfusion::PartialShape>& output_shapes,
       const std::vector<nnfusion::element::Type>& output_types)
    : Op("If")
    , m_then_branch_graph(then_branch_graph)
    , m_else_branch_graph(else_branch_graph)
    , m_output_shapes(output_shapes)
    , m_output_types(output_types)
{
}

std::shared_ptr<nnfusion::graph::Graph> If::get_then_branch_graph()
{
    return m_then_branch_graph;
}

std::shared_ptr<nnfusion::graph::Graph> If::get_else_branch_graph()
{
    return m_else_branch_graph;
}

TranslationUnit::Pointer If::get_then_branch_tu()
{
    return m_then_branch_tu;
}
void If::set_then_branch_tu(TranslationUnit::Pointer tu)
{
    m_then_branch_tu = tu;
}
TranslationUnit::Pointer If::get_else_branch_tu()
{
    return m_else_branch_tu;
}
void If::set_else_branch_tu(TranslationUnit::Pointer tu)
{
    m_else_branch_tu = tu;
}

void If::validate_and_infer_types(std::shared_ptr<graph::GNode> gnode)
{
    nnfusion::Shape cond_shape = gnode->get_input_shape(0);
    nnfusion::element::Type cond_et = gnode->get_input_element_type(0);
    NNFUSION_CHECK(shape_size(cond_shape) == 1)
        << "The condition tensor of the If operation mush be scalar.";
    NNFUSION_CHECK(cond_et == nnfusion::element::boolean)
        << "The condition tensor of the If operation mush be boolean.";

    auto then_branch_outputs = m_then_branch_graph->get_outputs();
    auto else_branch_outputs = m_else_branch_graph->get_outputs();
    NNFUSION_CHECK(then_branch_outputs.size() == else_branch_outputs.size())
        << "The outputs in the then_branch and else_branch must have the same shape and "
           "same data type.";
    for (size_t i = 0; i < then_branch_outputs.size(); i++)
    {
        gnode->set_output_type_and_shape(i, m_output_types[i], m_output_shapes[i]);
    }
}
