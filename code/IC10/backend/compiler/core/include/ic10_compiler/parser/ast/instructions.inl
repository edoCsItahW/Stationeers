// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file instructions.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/31 00:11
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef STATIONEERS_INSTRUCTIONS_INL
#define STATIONEERS_INSTRUCTIONS_INL
#pragma once

namespace stationeers::ic10 {

    // NullaryInstructionBase

    template<FString K>
    NullaryInstructionBase<K>::NullaryInstructionBase(Pos pos) noexcept
        : AST<NullaryInstructionBase>{pos} {}

    template<FString K>
    Pos NullaryInstructionBase<K>::end() const {
        return endPos(this->position, keyword.size, keyword.size);
    }

    template<FString K>
    std::string NullaryInstructionBase<K>::toString() const {
        return std::string(keyword.value.data(), keyword.value.size());
    }

    template<FString K>
    std::string NullaryInstructionBase<K>::toJSON() const {
        return jsonBase();
    }

    template<FString K>
    template<FString... Vs, AstJsonAble... Params>
        requires(sizeof...(Vs) == sizeof...(Params))
    std::string NullaryInstructionBase<K>::jsonBase(Params&&... params) const {
        return this->AST<NullaryInstructionBase>::template jsonBase<"keyword", Vs...>(
            keyword, std::forward<Params>(params)...
        );
    }

    // UnaryInstructionBase

    template<FString K, OperandType V1>
    UnaryInstructionBase<K, V1>::UnaryInstructionBase(Pos pos, O1 o1)
        : NullaryInstructionBase<K>{pos}
        , operand1(o1)
        , args{o1} {}

    template<FString K, OperandType V1>
    Pos UnaryInstructionBase<K, V1>::end() const {
        return call(operand1, [](auto&& o) { return o.end(); });
    }

    template<FString K, OperandType V1>
    std::string UnaryInstructionBase<K, V1>::toString() const {
        if constexpr (IsVariant<O1>)
            return std::format(
                "{} {}", NullaryInstructionBase<K>::toString(),
                call(operand1, [](auto&& o) { return o.toString(); })
            );

        else
            return std::format("{} {}", NullaryInstructionBase<K>::toString(), operand1.toString());
    }

    template<FString K, OperandType V1>
    std::string UnaryInstructionBase<K, V1>::toJSON() const {
        return jsonBase();
    }

    template<FString K, OperandType V1>
    template<FString... Vs, AstJsonAble... Params>
        requires(sizeof...(Vs) == sizeof...(Params))
    std::string UnaryInstructionBase<K, V1>::jsonBase(Params&&... params) const {
        return this->NullaryInstructionBase<K>::template jsonBase<"operand1", "type1", Vs...>(
            operand1, static_cast<int>(type1), std::forward<Params>(params)...
        );
    }

    // BinaryInstructionBase

    template<FString K, OperandType V1, OperandType V2>
    BinaryInstructionBase<K, V1, V2>::BinaryInstructionBase(Pos pos, O1 o1, O2 o2)
        : UnaryInstructionBase<K, V1>{pos, o1}
        , operand2(o2)
        , args(o1, o2) {}

    template<FString K, OperandType V1, OperandType V2>
    Pos BinaryInstructionBase<K, V1, V2>::end() const {
        return call(operand2, [](auto&& o) { return o.end(); });
    }

    template<FString K, OperandType V1, OperandType V2>
    std::string BinaryInstructionBase<K, V1, V2>::toString() const {
        if constexpr (IsVariant<O2>)
            return std::format(
                "{} {}", UnaryInstructionBase<K, V1>::toString(),
                call(operand2, [](auto&& o) { return o.toString(); })
            );

        else
            return std::format(
                "{} {}", UnaryInstructionBase<K, V1>::operand1.toString(), operand2.toString()
            );
    }

    template<FString K, OperandType V1, OperandType V2>
    std::string BinaryInstructionBase<K, V1, V2>::toJSON() const {
        return jsonBase();
    }

    template<FString K, OperandType V1, OperandType V2>
    template<FString... Vs, AstJsonAble... Params>
        requires(sizeof...(Vs) == sizeof...(Params))
    std::string BinaryInstructionBase<K, V1, V2>::jsonBase(Params&&... params) const {
        return this->UnaryInstructionBase<K, V1>::template jsonBase<"operand2", "type2", Vs...>(
            operand2, static_cast<int>(type2), std::forward<Params>(params)...
        );
    }

    // TernaryInstructionBase

    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    TernaryInstructionBase<K, V1, V2, V3>::TernaryInstructionBase(Pos pos, O1 o1, O2 o2, O3 o3)
        : BinaryInstructionBase<K, V1, V2>{pos, o1, o2}
        , operand3(o3)
        , args(o1, o2, o3) {}

    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    Pos TernaryInstructionBase<K, V1, V2, V3>::end() const {
        return call(operand3, [](auto&& o) { return o.end(); });
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    std::string TernaryInstructionBase<K, V1, V2, V3>::toString() const {
        if constexpr (IsVariant<O3>)
            return std::format(
                "{} {}", BinaryInstructionBase<K, V1, V2>::toString(),
                call(operand3, [](auto&& o) { return o.toString(); })
            );

        else
            return std::format(
                "{} {}", BinaryInstructionBase<K, V1, V2>::operand2.toString(), operand3.toString()
            );
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    std::string TernaryInstructionBase<K, V1, V2, V3>::toJSON() const {
        return jsonBase();
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    template<FString... Vs, AstJsonAble... Params>
        requires(sizeof...(Vs) == sizeof...(Params))
    std::string TernaryInstructionBase<K, V1, V2, V3>::jsonBase(Params&&... params) const {
        return this
            ->BinaryInstructionBase<K, V1, V2>::template jsonBase<"operand3", "type3", Vs...>(
                operand3, static_cast<int>(type3), std::forward<Params>(params)...
            );
    }

    // QuaternaryInstructionBase

    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    QuaternaryInstructionBase<K, V1, V2, V3, V4>::QuaternaryInstructionBase(
        Pos pos, O1 o1, O2 o2, O3 o3, O4 o4
    )
        : TernaryInstructionBase<K, V1, V2, V3>{pos, o1, o2, o3}
        , operand4(o4)
        , args(o1, o2, o3, o4) {}

    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    Pos QuaternaryInstructionBase<K, V1, V2, V3, V4>::end() const {
        return call(operand4, [](auto&& o) { return o.end(); });
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    std::string QuaternaryInstructionBase<K, V1, V2, V3, V4>::toString() const {
        if constexpr (IsVariant<O4>)
            return std::format(
                "{} {}", TernaryInstructionBase<K, V1, V2, V3>::toString(),
                call(operand4, [](auto&& o) { return o.toString(); })
            );

        else
            return std::format(
                "{} {}", TernaryInstructionBase<K, V1, V2, V3>::operand3.toString(),
                operand4.toString()
            );
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    std::string QuaternaryInstructionBase<K, V1, V2, V3, V4>::toJSON() const {
        return jsonBase();
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    template<FString... Vs, AstJsonAble... Params>
        requires(sizeof...(Vs) == sizeof...(Params))
    std::string QuaternaryInstructionBase<K, V1, V2, V3, V4>::jsonBase(Params&&... params) const {
        return this
            ->TernaryInstructionBase<K, V1, V2, V3>::template jsonBase<"operand4", "type4", Vs...>(
                operand4, static_cast<int>(type4), std::forward<Params>(params)...
            );
    }

    // QuinaryInstructionBase

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
    QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::QuinaryInstructionBase(
        Pos pos, O1 o1, O2 o2, O3 o3, O4 o4, O5 o5
    )
        : QuaternaryInstructionBase<K, V1, V2, V3, V4>{pos, o1, o2, o3, o4}
        , operand5(o5)
        , args(o1, o2, o3, o4, o5) {}

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
    Pos QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::end() const {
        return call(operand5, [](auto&& o) { return o.end(); });
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
    std::string QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::toString() const {
        if constexpr (IsVariant<O5>)
            return std::format(
                "{} {}", QuaternaryInstructionBase<K, V1, V2, V3, V4>::toString(),
                call(operand5, [](auto&& o) { return o.toString(); })
            );
        else
            return std::format(
                "{} {}", QuaternaryInstructionBase<K, V1, V2, V3, V4>::toString(),
                operand5.toString()
            );
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
    std::string QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::toJSON() const {
        return jsonBase();
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
    template<FString... Vs, AstJsonAble... Params>
        requires(sizeof...(Vs) == sizeof...(Params))
    std::string QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::jsonBase(Params&&... params) const {
        return this->QuaternaryInstructionBase<K, V1, V2, V3, V4>::template jsonBase<
            "operand5", "type5", Vs...>(
            operand5, static_cast<int>(type5), std::forward<Params>(params)...
        );
    }

    // SenaryInstructionBase

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
    SenaryInstructionBase<K, V1, V2, V3, V4, V5, V6>::SenaryInstructionBase(
        Pos pos, O1 o1, O2 o2, O3 o3, O4 o4, O5 o5, O6 o6
    )
        : QuinaryInstructionBase<K, V1, V2, V3, V4, V5>{pos, o1, o2, o3, o4, o5}
        , operand6(o6)
        , args(o1, o2, o3, o4, o5, o6) {}

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
    Pos SenaryInstructionBase<K, V1, V2, V3, V4, V5, V6>::end() const {
        return call(operand6, [](auto&& o) { return o.end(); });
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
    std::string SenaryInstructionBase<K, V1, V2, V3, V4, V5, V6>::toString() const {
        if constexpr (IsVariant<O6>)
            return std::format(
                "{} {}", QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::toString(),
                call(operand6, [](auto&& o) { return o.toString(); })
            );
        else
            return std::format(
                "{} {}", QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::toString(),
                operand6.toString()
            );
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
    std::string SenaryInstructionBase<K, V1, V2, V3, V4, V5, V6>::toJSON() const {
        return jsonBase();
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
    template<FString... Vs, AstJsonAble... Params>
        requires(sizeof...(Vs) == sizeof...(Params))
    std::string SenaryInstructionBase<K, V1, V2, V3, V4, V5, V6>::jsonBase(
        Params&&... params
    ) const {
        return this->QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::template jsonBase<
            "operand6", "type6", Vs...>(
            operand6, static_cast<int>(type6), std::forward<Params>(params)...
        );
    }

}  // namespace stationeers::ic10

#endif  // STATIONEERS_INSTRUCTIONS_INL
