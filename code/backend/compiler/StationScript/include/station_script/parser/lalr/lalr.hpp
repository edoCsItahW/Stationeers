// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file lalr.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/06/29
 * @if zh
 * @brief C++23编译期LALR(1)分析表生成器
 * @details 使用C++23 constexpr和模板元编程技术，在编译期生成LALR(1)语法分析器的ACTION/GOTO表。
 *          所有数据结构均使用std::array，确保完全编译期计算。
 *
 * @par 核心功能:
 * - 编译期文法定义与验证
 * - FIRST集计算
 * - LR(1)项目集规范族构造
 * - LALR(1)同心项合并
 * - ACTION/GOTO分析表生成
 *
 * @par 使用方式:
 * 1. 定义终结符枚举和非终结符枚举（必须包含 _COUNT_ 哨兵值）
 * 2. 使用 Grammar<NT, T, NumProds, MaxRHS> 定义文法
 * 3. 使用 LALRGenerator::generate() 生成分析表
 *
 * @note 所有计算均在编译期完成，无运行时开销
 * @requires C++20 或更高版本
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief C++23 compile-time LALR(1) parse table generator
 * @details Uses C++23 constexpr and template metaprogramming to generate LALR(1)
 *          parser ACTION/GOTO tables at compile time.
 *          All data structures use std::array for true compile-time computation.
 *
 * @note All computations are done at compile time with zero runtime overhead
 * @requires C++20 or later
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */

#ifndef STATION_SCRIPT_LALR_HPP
#define STATION_SCRIPT_LALR_HPP
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <type_traits>

namespace stationeers::station_script::lalr {

// ============================================================================
//  Symbol representation
// ============================================================================

enum class SymType : uint8_t { Terminal, NonTerminal };

template <typename NT, typename T>
struct Symbol {
    SymType type;
    uint32_t index;

    constexpr Symbol() : type(SymType::Terminal), index(0) {}
    constexpr Symbol(NT nt) : type(SymType::NonTerminal), index(static_cast<uint32_t>(nt)) {}
    constexpr Symbol(T t) : type(SymType::Terminal), index(static_cast<uint32_t>(t)) {}

    constexpr bool isTerminal() const { return type == SymType::Terminal; }
    constexpr bool isNonTerminal() const { return type == SymType::NonTerminal; }
    constexpr NT asNonTerminal() const { return static_cast<NT>(index); }
    constexpr T asTerminal() const { return static_cast<T>(index); }

    constexpr bool operator==(const Symbol& other) const {
        return type == other.type && index == other.index;
    }
    constexpr bool operator!=(const Symbol& other) const { return !(*this == other); }
};

// ============================================================================
//  Fixed-size production (with max RHS size)
// ============================================================================

template <typename NT, typename T, size_t MaxRHSSize>
struct Production {
    NT lhs{};
    std::array<Symbol<NT, T>, MaxRHSSize> rhs{};
    size_t rhsSize = 0;

    constexpr Production() = default;

    constexpr Production(NT left, std::initializer_list<Symbol<NT, T>> right) : lhs(left) {
        rhsSize = 0;
        for (const auto& s : right) {
            if (rhsSize < MaxRHSSize) {
                rhs[rhsSize++] = s;
            }
        }
    }

    constexpr size_t size() const { return rhsSize; }
};

// ============================================================================
//  Grammar (fixed-size)
// ============================================================================

template <typename NT, typename T, size_t NumProductions, size_t MaxRHSSize>
struct Grammar {
    using ProdType = Production<NT, T, MaxRHSSize>;
    std::array<ProdType, NumProductions> prods{};
    size_t count = 0;

    constexpr Grammar() = default;

    constexpr void add(const ProdType& p) {
        if (count < NumProductions) {
            prods[count++] = p;
        }
    }

    constexpr size_t size() const { return count; }
    constexpr const ProdType& operator[](size_t i) const { return prods[i]; }
};

// ============================================================================
//  BitSet - fixed-size bitmap set
// ============================================================================

template <size_t MaxSize>
struct BitSet {
    static constexpr size_t kWordCount = (MaxSize + 63) / 64;
    std::array<uint64_t, kWordCount> bits{};

    constexpr BitSet() = default;

    constexpr void set(size_t idx) { bits[idx / 64] |= 1ULL << (idx % 64); }
    constexpr void clear(size_t idx) { bits[idx / 64] &= ~(1ULL << (idx % 64)); }
    [[nodiscard]] constexpr bool test(size_t idx) const {
        return (bits[idx / 64] & (1ULL << (idx % 64))) != 0;
    }
    [[nodiscard]] constexpr bool empty() const {
        for (auto w : bits) if (w != 0) return false;
        return true;
    }
    constexpr void unite(const BitSet& other) {
        for (size_t i = 0; i < kWordCount; ++i) bits[i] |= other.bits[i];
    }
    [[nodiscard]] constexpr bool operator==(const BitSet& other) const { return bits == other.bits; }
    [[nodiscard]] constexpr bool operator!=(const BitSet& other) const { return bits != other.bits; }
};

// ============================================================================
//  LR(1) Item & ItemSet
// ============================================================================

template <size_t TermCount>
struct LR1Item {
    uint32_t prodIdx = 0;
    uint32_t dotPos = 0;
    BitSet<TermCount> lookahead{};

    constexpr bool coreEquals(const LR1Item& other) const {
        return prodIdx == other.prodIdx && dotPos == other.dotPos;
    }
    constexpr bool operator==(const LR1Item& other) const {
        return coreEquals(other) && lookahead == other.lookahead;
    }
};

template <size_t TermCount, size_t MaxItems>
struct ItemSet {
    std::array<LR1Item<TermCount>, MaxItems> items{};
    size_t count = 0;

    constexpr void add(const LR1Item<TermCount>& item) {
        for (size_t i = 0; i < count; ++i) {
            if (items[i].coreEquals(item)) {
                items[i].lookahead.unite(item.lookahead);
                return;
            }
        }
        if (count < MaxItems) items[count++] = item;
    }

    [[nodiscard]] constexpr bool contains(const LR1Item<TermCount>& item) const {
        for (size_t i = 0; i < count; ++i) {
            if (items[i].coreEquals(item)) {
                auto merged = items[i].lookahead;
                merged.unite(item.lookahead);
                if (merged == items[i].lookahead) return true;
            }
        }
        return false;
    }

    [[nodiscard]] constexpr bool coreEquals(const ItemSet& other) const {
        if (count != other.count) return false;
        for (size_t i = 0; i < count; ++i) {
            bool found = false;
            for (size_t j = 0; j < other.count; ++j) {
                if (items[i].coreEquals(other.items[j])) { found = true; break; }
            }
            if (!found) return false;
        }
        return true;
    }

    [[nodiscard]] constexpr bool operator==(const ItemSet& other) const {
        if (count != other.count) return false;
        for (size_t i = 0; i < count; ++i) {
            bool found = false;
            for (size_t j = 0; j < other.count; ++j) {
                if (items[i] == other.items[j]) { found = true; break; }
            }
            if (!found) return false;
        }
        return true;
    }
};

// ============================================================================
//  Action types
// ============================================================================

enum class ActionType : uint8_t { Error, Shift, Reduce, Accept };

struct ActionEntry {
    ActionType type = ActionType::Error;
    uint32_t value = 0;

    constexpr ActionEntry() = default;
    static constexpr ActionEntry shift(uint32_t state) {
        ActionEntry e; e.type = ActionType::Shift; e.value = state; return e;
    }
    static constexpr ActionEntry reduce(uint32_t prodIdx) {
        ActionEntry e; e.type = ActionType::Reduce; e.value = prodIdx; return e;
    }
    static constexpr ActionEntry accept() {
        ActionEntry e; e.type = ActionType::Accept; return e;
    }
    static constexpr ActionEntry error() { return ActionEntry{}; }

    constexpr bool operator==(const ActionEntry& other) const {
        return type == other.type && value == other.value;
    }
};

// ============================================================================
//  LALR Table (fixed size)
// ============================================================================

template <typename NT, typename T, size_t MaxStates>
struct LALRTable {
    static constexpr size_t kTermCount = static_cast<size_t>(T::_COUNT_);
    static constexpr size_t kNonTermCount = static_cast<size_t>(NT::_COUNT_);

    size_t numStates = 0;
    std::array<std::array<ActionEntry, kTermCount>, MaxStates> action{};
    std::array<std::array<int32_t, kNonTermCount>, MaxStates> gotoTable{};

    constexpr LALRTable() {
        for (size_t s = 0; s < MaxStates; ++s) {
            for (size_t nt = 0; nt < kNonTermCount; ++nt) {
                gotoTable[s][nt] = -1;
            }
        }
    }

    [[nodiscard]] constexpr size_t stateCount() const { return numStates; }
};

// ============================================================================
//  FIRST set computation
// ============================================================================

namespace detail {

template <typename NT, typename T>
struct FirstSets {
    static constexpr size_t kTermCount = static_cast<size_t>(T::_COUNT_);
    static constexpr size_t kNonTermCount = static_cast<size_t>(NT::_COUNT_);

    std::array<BitSet<kTermCount>, kNonTermCount> first{};
    std::array<bool, kNonTermCount> epsilon{};
};

template <typename NT, typename T, size_t NumProds, size_t MaxRHS>
constexpr auto computeFirstSets(const Grammar<NT, T, NumProds, MaxRHS>& g) {
    FirstSets<NT, T> result{};
    constexpr size_t kTermCount = static_cast<size_t>(T::_COUNT_);

    bool changed = true;
    while (changed) {
        changed = false;
        for (size_t p = 0; p < g.count; ++p) {
            const auto& prod = g[p];
            size_t lhsIdx = static_cast<size_t>(prod.lhs);
            bool allEps = true;

            for (size_t i = 0; i < prod.rhsSize; ++i) {
                const auto& sym = prod.rhs[i];
                if (sym.isTerminal()) {
                    size_t t = static_cast<size_t>(sym.asTerminal());
                    if (!result.first[lhsIdx].test(t)) {
                        result.first[lhsIdx].set(t);
                        changed = true;
                    }
                    allEps = false;
                    break;
                } else {
                    size_t nt = static_cast<size_t>(sym.asNonTerminal());
                    for (size_t t = 0; t < kTermCount; ++t) {
                        if (result.first[nt].test(t) && !result.first[lhsIdx].test(t)) {
                            result.first[lhsIdx].set(t);
                            changed = true;
                        }
                    }
                    if (!result.epsilon[nt]) { allEps = false; break; }
                }
            }

            if (allEps && !result.epsilon[lhsIdx]) {
                result.epsilon[lhsIdx] = true;
                changed = true;
            }
        }
    }
    return result;
}

template <typename NT, typename T, typename FirstSetsType, typename ProdType>
constexpr BitSet<static_cast<size_t>(T::_COUNT_)> firstOfBeta(
    const FirstSetsType& fs,
    const ProdType& prod,
    size_t dotPos,
    const BitSet<static_cast<size_t>(T::_COUNT_)>& follow
) {
    constexpr size_t kTermCount = static_cast<size_t>(T::_COUNT_);
    BitSet<kTermCount> result;
    bool allEps = true;

    for (size_t i = dotPos + 1; i < prod.rhsSize; ++i) {
        const auto& sym = prod.rhs[i];
        if (sym.isTerminal()) {
            result.set(static_cast<size_t>(sym.asTerminal()));
            allEps = false;
            break;
        } else {
            size_t nt = static_cast<size_t>(sym.asNonTerminal());
            for (size_t t = 0; t < kTermCount; ++t) {
                if (fs.first[nt].test(t)) result.set(t);
            }
            if (!fs.epsilon[nt]) { allEps = false; break; }
        }
    }
    if (allEps) result.unite(follow);
    return result;
}

}  // namespace detail

// ============================================================================
//  LALR Generator
// ============================================================================

template <typename NT, typename T, NT StartSymbol, T EofSymbol>
class LALRGenerator {
public:
    static constexpr size_t kTermCount = static_cast<size_t>(T::_COUNT_);
    static constexpr size_t kNonTermCount = static_cast<size_t>(NT::_COUNT_);
    static constexpr size_t kEofIdx = static_cast<size_t>(EofSymbol);

    static_assert(std::is_enum_v<NT>);
    static_assert(std::is_enum_v<T>);

    /**
     * @if zh
     * @brief 生成LALR(1)分析表
     * @tparam NumProds 产生式数量
     * @tparam MaxRHS 产生式右部最大长度
     * @tparam MaxStates 最大状态数
     * @tparam MaxItems 每个状态最大项目数
     * @param grammar 文法
     * @return LALR(1)分析表
     * @elseif en
     * @brief Generate LALR(1) parse table
     * @tparam NumProds Number of productions
     * @tparam MaxRHS Max RHS length
     * @tparam MaxStates Max number of states
     * @tparam MaxItems Max items per state
     * @param grammar The grammar
     * @return LALR(1) parse table
     * @endif
     */
    template <size_t NumProds, size_t MaxRHS, size_t MaxStates, size_t MaxItems>
    static constexpr LALRTable<NT, T, MaxStates> generate(
        const Grammar<NT, T, NumProds, MaxRHS>& grammar
    ) {
        auto augmented = augmentGrammar(grammar);
        auto firstSets = detail::computeFirstSets(augmented);

        auto [states, numStates] = buildLR1Canonical<MaxStates, MaxItems>(augmented, firstSets);
        auto [lalrStates, numLalrStates] = mergeStates<MaxStates, MaxItems>(states, numStates);
        return buildTable<MaxStates, MaxItems>(lalrStates, numLalrStates, augmented, firstSets);
    }

private:
    template <size_t NumProds, size_t MaxRHS>
    static constexpr auto augmentGrammar(const Grammar<NT, T, NumProds, MaxRHS>& g) {
        Grammar<NT, T, NumProds + 1, MaxRHS> aug;

        Production<NT, T, MaxRHS> augProd;
        augProd.lhs = NT::_AUGMENTED_;
        augProd.rhs[0] = Symbol<NT, T>{StartSymbol};
        augProd.rhsSize = 1;
        aug.add(augProd);

        for (size_t i = 0; i < g.count; ++i) {
            aug.add(g[i]);
        }
        return aug;
    }

    template <size_t MaxStates, size_t MaxItems, typename GrammarType, typename FirstSetsType>
    static constexpr std::pair<std::array<ItemSet<kTermCount, MaxItems>, MaxStates>, size_t>
    buildLR1Canonical(const GrammarType& g, const FirstSetsType& fs) {
        std::array<ItemSet<kTermCount, MaxItems>, MaxStates> states{};
        size_t numStates = 0;

        LR1Item<kTermCount> init;
        init.prodIdx = 0;
        init.dotPos = 0;
        init.lookahead.set(kEofIdx);

        ItemSet<kTermCount, MaxItems> initSet;
        initSet.add(init);
        closure(initSet, g, fs);

        states[numStates++] = initSet;

        bool changed = true;
        while (changed) {
            changed = false;
            for (size_t s = 0; s < numStates; ++s) {
                for (size_t i = 0; i < states[s].count; ++i) {
                    const auto& item = states[s].items[i];
                    const auto& prod = g[item.prodIdx];
                    if (item.dotPos >= prod.rhsSize) continue;

                    const auto& sym = prod.rhs[item.dotPos];
                    auto gset = gotoState(states[s], sym, g, fs);
                    if (gset.count == 0) continue;

                    int found = -1;
                    for (size_t es = 0; es < numStates; ++es) {
                        if (states[es] == gset) { found = static_cast<int>(es); break; }
                    }
                    if (found < 0 && numStates < MaxStates) {
                        states[numStates++] = gset;
                        changed = true;
                    }
                }
            }
        }
        return {states, numStates};
    }

    template <typename ItemSetType, typename GrammarType, typename FirstSetsType>
    static constexpr void closure(
        ItemSetType& itemSet,
        const GrammarType& g,
        const FirstSetsType& fs
    ) {
        bool changed = true;
        while (changed) {
            changed = false;
            size_t prev = itemSet.count;

            for (size_t i = 0; i < prev; ++i) {
                const auto& item = itemSet.items[i];
                const auto& prod = g[item.prodIdx];
                if (item.dotPos >= prod.rhsSize) continue;

                const auto& nextSym = prod.rhs[item.dotPos];
                if (!nextSym.isNonTerminal()) continue;

                size_t ntIdx = static_cast<size_t>(nextSym.asNonTerminal());

                auto betaFirst = detail::firstOfBeta<NT, T>(fs, prod, item.dotPos, item.lookahead);

                for (size_t p = 0; p < g.count; ++p) {
                    if (static_cast<size_t>(g[p].lhs) == ntIdx) {
                        LR1Item<kTermCount> ni;
                        ni.prodIdx = static_cast<uint32_t>(p);
                        ni.dotPos = 0;
                        ni.lookahead = betaFirst;
                        if (!itemSet.contains(ni)) {
                            itemSet.add(ni);
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    template <typename ItemSetType, typename GrammarType, typename FirstSetsType>
    static constexpr ItemSet<kTermCount, ItemSetType{}.items.size()> gotoState(
        const ItemSetType& itemSet,
        const Symbol<NT, T>& sym,
        const GrammarType& g,
        const FirstSetsType& fs
    ) {
        ItemSet<kTermCount, ItemSetType{}.items.size()> result;
        for (size_t i = 0; i < itemSet.count; ++i) {
            const auto& item = itemSet.items[i];
            const auto& prod = g[item.prodIdx];
            if (item.dotPos >= prod.rhsSize) continue;
            if (prod.rhs[item.dotPos] != sym) continue;

            auto ni = item;
            ni.dotPos++;
            result.add(ni);
        }
        if (result.count > 0) closure(result, g, fs);
        return result;
    }

    template <size_t MaxStates, size_t MaxItems>
    static constexpr std::pair<std::array<ItemSet<kTermCount, MaxItems>, MaxStates>, size_t>
    mergeStates(const std::array<ItemSet<kTermCount, MaxItems>, MaxStates>& states, size_t numStates) {
        std::array<ItemSet<kTermCount, MaxItems>, MaxStates> merged{};
        size_t numMerged = 0;

        for (size_t i = 0; i < numStates; ++i) {
            int target = -1;
            for (size_t j = 0; j < numMerged; ++j) {
                if (states[i].coreEquals(merged[j])) { target = static_cast<int>(j); break; }
            }
            if (target < 0) {
                merged[numMerged++] = states[i];
            } else {
                for (size_t k = 0; k < states[i].count; ++k) {
                    merged[target].add(states[i].items[k]);
                }
            }
        }
        return {merged, numMerged};
    }

    template <size_t MaxStates, size_t MaxItems, typename GrammarType, typename FirstSetsType>
    static constexpr LALRTable<NT, T, MaxStates> buildTable(
        const std::array<ItemSet<kTermCount, MaxItems>, MaxStates>& states,
        size_t numStates,
        const GrammarType& g,
        const FirstSetsType& fs
    ) {
        LALRTable<NT, T, MaxStates> table;
        table.numStates = numStates;

        for (size_t s = 0; s < numStates; ++s) {
            const auto& state = states[s];

            for (size_t i = 0; i < state.count; ++i) {
                const auto& item = state.items[i];
                const auto& prod = g[item.prodIdx];

                if (item.dotPos < prod.rhsSize) {
                    const auto& nextSym = prod.rhs[item.dotPos];
                    if (nextSym.isTerminal()) {
                        size_t t = static_cast<size_t>(nextSym.asTerminal());
                        int gs = findGotoState(states, numStates, s, nextSym, g, fs);
                        if (gs >= 0 && table.action[s][t].type == ActionType::Error) {
                            table.action[s][t] = ActionEntry::shift(static_cast<uint32_t>(gs));
                        }
                    }
                } else {
                    if (item.prodIdx == 0) {
                        if (item.lookahead.test(kEofIdx)) {
                            table.action[s][kEofIdx] = ActionEntry::accept();
                        }
                    } else {
                        for (size_t t = 0; t < kTermCount; ++t) {
                            if (item.lookahead.test(t) && table.action[s][t].type == ActionType::Error) {
                                table.action[s][t] = ActionEntry::reduce(item.prodIdx);
                            }
                        }
                    }
                }
            }

            for (size_t nt = 0; nt < kNonTermCount; ++nt) {
                Symbol<NT, T> ntSym{static_cast<NT>(nt)};
                int gs = findGotoState(states, numStates, s, ntSym, g, fs);
                if (gs >= 0) table.gotoTable[s][nt] = static_cast<int32_t>(gs);
            }
        }
        return table;
    }

    template <size_t MaxStates, size_t MaxItems, typename GrammarType, typename FirstSetsType>
    static constexpr int findGotoState(
        const std::array<ItemSet<kTermCount, MaxItems>, MaxStates>& states,
        size_t numStates,
        size_t from,
        const Symbol<NT, T>& sym,
        const GrammarType& g,
        const FirstSetsType& fs
    ) {
        ItemSet<kTermCount, MaxItems> gset;
        const auto& st = states[from];
        for (size_t i = 0; i < st.count; ++i) {
            const auto& item = st.items[i];
            const auto& prod = g[item.prodIdx];
            if (item.dotPos >= prod.rhsSize) continue;
            if (prod.rhs[item.dotPos] != sym) continue;
            auto ni = item;
            ni.dotPos++;
            gset.add(ni);
        }
        if (gset.count == 0) return -1;
        closure(gset, g, fs);

        for (size_t s = 0; s < numStates; ++s) {
            if (states[s].coreEquals(gset)) {
                bool ok = true;
                for (size_t gi = 0; gi < gset.count; ++gi) {
                    bool found = false;
                    for (size_t si = 0; si < states[s].count; ++si) {
                        if (gset.items[gi].coreEquals(states[s].items[si])) {
                            auto comb = states[s].items[si].lookahead;
                            comb.unite(gset.items[gi].lookahead);
                            if (comb == states[s].items[si].lookahead) { found = true; break; }
                        }
                    }
                    if (!found) { ok = false; break; }
                }
                if (ok) return static_cast<int>(s);
            }
        }
        return -1;
    }
};

}  // namespace stationeers::station_script::lalr

#endif  // STATION_SCRIPT_LALR_HPP
