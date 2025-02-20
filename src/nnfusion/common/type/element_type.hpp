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

//================================================================================================
// ElementType
//================================================================================================

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "half/include/half.hpp"
#include "nnfusion/common/serialize/nnf_pbtypes.pb.h"
#include "nnfusion/common/type/bfloat16.hpp"
#include "nnfusion/util/errors.hpp"

namespace nnfusion
{
    namespace element
    {
        using half = half_float::half;

        class Type;

        extern const Type dynamic;
        extern const Type boolean;
        extern const Type character;
        extern const Type bf16;
        extern const Type f16;
        extern const Type f32;
        extern const Type f64;
        extern const Type i8;
        extern const Type i16;
        extern const Type i32;
        extern const Type i64;
        extern const Type u8;
        extern const Type u16;
        extern const Type u32;
        extern const Type u64;

        class Type
        {
        public:
            Type() {}
            Type(const Type&) = default;
            Type(size_t bitwidth,
                 bool is_real,
                 bool is_signed,
                 bool is_quantized,
                 const std::string& cname);
            Type& operator=(const Type&);
            virtual ~Type() {}
            const std::string& c_type_string() const;
            size_t size() const;
            size_t hash() const;
            bool is_static() const { return (*this != dynamic); }
            bool is_dynamic() const { return !is_static(); }
            bool is_real() const { return m_is_real; }
            bool is_integral() const { return !is_real(); }
            bool is_signed() const { return m_is_signed; }
            bool is_quantized() const { return m_is_quantized; }
            size_t bitwidth() const { return m_bitwidth; }
            bool operator==(const Type& other) const;
            bool operator!=(const Type& other) const { return !(*this == other); }
            bool operator<(const Type& other) const;
            friend std::ostream& operator<<(std::ostream&, const Type&);
            static std::vector<const Type*> get_known_types();
            static bool nnfusion_element_type_to_pbtype(const Type& ng_et,
                                                        nnfusion::serialize::PBType& dtype);
            static bool nnfusion_element_type_to_dtype_string(const Type& ng_et,
                                                              std::string& dtype);
            static bool dtype_string_to_nnfusion_element_type(const std::string& dtype,
                                                              Type& ng_et);
            static std::string extract_value(const Type& ng_et, const void* data_ptr);

            /// Returns true if the type is floating point, else false.
            bool get_is_real() const { return m_is_real; }
            /// \brief Merges two element types t1 and t2, writing the result into dst and
            ///        returning true if successful, else returning false.
            ///
            ///        To "merge" two element types t1 and t2 is to find the least restrictive
            ///        element type t that is no more restrictive than t1 and t2, if t exists.
            ///        More simply:
            ///
            ///           merge(dst,element::Type::dynamic,t)
            ///              writes t to dst and returns true
            ///
            ///           merge(dst,t,element::Type::dynamic)
            ///              writes t to dst and returns true
            ///
            ///           merge(dst,t1,t2) where t1, t2 both static and equal
            ///              writes t1 to dst and returns true
            ///
            ///           merge(dst,t1,t2) where t1, t2 both static and unequal
            ///              does nothing to dst, and returns false
            static bool merge(element::Type& dst, const element::Type& t1, const element::Type& t2);

        private:
            size_t m_bitwidth{0};
            bool m_is_real{false};
            bool m_is_signed{false};
            bool m_is_quantized{false};
            std::string m_cname{"dynamic"};
        };

        template <typename T>
        const Type& from()
        {
            throw nnfusion::errors::InvalidArgument("Unknown type");
        }
        template <>
        const Type& from<char>();
        template <>
        const Type& from<bool>();
        template <>
        const Type& from<float>();
        template <>
        const Type& from<double>();
        template <>
        const Type& from<int8_t>();
        template <>
        const Type& from<int16_t>();
        template <>
        const Type& from<int32_t>();
        template <>
        const Type& from<int64_t>();
        template <>
        const Type& from<uint8_t>();
        template <>
        const Type& from<uint16_t>();
        template <>
        const Type& from<uint32_t>();
        template <>
        const Type& from<uint64_t>();
        template <>
        const Type& from<nnfusion::bfloat16>();
        template <>
        const Type& from<half>();

        std::ostream& operator<<(std::ostream& out, const nnfusion::element::Type& obj);
    }
}
