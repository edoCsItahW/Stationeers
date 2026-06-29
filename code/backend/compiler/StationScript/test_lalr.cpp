// Test file for compile-time LALR(1) generator
// Build with: g++ -std=c++20 -o test_lalr test_lalr.cpp

#include <iostream>
#include <iomanip>
#include <string_view>

#include "station_script/parser/lalr/lalr.hpp"

using namespace stationeers::station_script::lalr;

// Test grammar: classic expression grammar
// E -> E + T | T
// T -> T * F | F
// F -> id | ( E )

enum class Term : size_t {
    PLUS,       // +
    TIMES,      // *
    LPAREN,     // (
    RPAREN,     // )
    ID,         // id
    END,        // $ (EOF)
    _COUNT_     // sentinel
};

enum class NonTerm : size_t {
    _AUGMENTED_,  // S' (augmented start symbol)
    E,            // E
    T,            // T
    F,            // F
    _COUNT_       // sentinel
};

constexpr Grammar<NonTerm, Term, 6, 3> makeExprGrammar() {
    Grammar<NonTerm, Term, 6, 3> g;

    // E -> T
    Production<NonTerm, Term, 3> p1;
    p1.lhs = NonTerm::E;
    p1.rhs[0] = Symbol<NonTerm, Term>{NonTerm::T};
    p1.rhsSize = 1;
    g.add(p1);

    // E -> E + T
    Production<NonTerm, Term, 3> p2;
    p2.lhs = NonTerm::E;
    p2.rhs[0] = Symbol<NonTerm, Term>{NonTerm::E};
    p2.rhs[1] = Symbol<NonTerm, Term>{Term::PLUS};
    p2.rhs[2] = Symbol<NonTerm, Term>{NonTerm::T};
    p2.rhsSize = 3;
    g.add(p2);

    // T -> F
    Production<NonTerm, Term, 3> p3;
    p3.lhs = NonTerm::T;
    p3.rhs[0] = Symbol<NonTerm, Term>{NonTerm::F};
    p3.rhsSize = 1;
    g.add(p3);

    // T -> T * F
    Production<NonTerm, Term, 3> p4;
    p4.lhs = NonTerm::T;
    p4.rhs[0] = Symbol<NonTerm, Term>{NonTerm::T};
    p4.rhs[1] = Symbol<NonTerm, Term>{Term::TIMES};
    p4.rhs[2] = Symbol<NonTerm, Term>{NonTerm::F};
    p4.rhsSize = 3;
    g.add(p4);

    // F -> id
    Production<NonTerm, Term, 3> p5;
    p5.lhs = NonTerm::F;
    p5.rhs[0] = Symbol<NonTerm, Term>{Term::ID};
    p5.rhsSize = 1;
    g.add(p5);

    // F -> ( E )
    Production<NonTerm, Term, 3> p6;
    p6.lhs = NonTerm::F;
    p6.rhs[0] = Symbol<NonTerm, Term>{Term::LPAREN};
    p6.rhs[1] = Symbol<NonTerm, Term>{NonTerm::E};
    p6.rhs[2] = Symbol<NonTerm, Term>{Term::RPAREN};
    p6.rhsSize = 3;
    g.add(p6);

    return g;
}

const char* termName(Term t) {
    switch (t) {
        case Term::PLUS: return "+";
        case Term::TIMES: return "*";
        case Term::LPAREN: return "(";
        case Term::RPAREN: return ")";
        case Term::ID: return "id";
        case Term::END: return "$";
        default: return "?";
    }
}

const char* nonTermName(NonTerm nt) {
    switch (nt) {
        case NonTerm::_AUGMENTED_: return "S'";
        case NonTerm::E: return "E";
        case NonTerm::T: return "T";
        case NonTerm::F: return "F";
        default: return "?";
    }
}

void printTable(const LALRTable<NonTerm, Term, 128>& table) {
    std::cout << "LALR(1) Parse Table (" << table.numStates << " states)\n";
    std::cout << std::string(100, '=') << "\n\n";

    // Print ACTION table header
    std::cout << std::setw(6) << "State";
    std::cout << " | ACTION";
    for (size_t t = 0; t < static_cast<size_t>(Term::_COUNT_); ++t) {
        std::cout << std::setw(8) << termName(static_cast<Term>(t));
    }
    std::cout << " | GOTO";
    for (size_t nt = 1; nt < static_cast<size_t>(NonTerm::_COUNT_); ++nt) {
        std::cout << std::setw(6) << nonTermName(static_cast<NonTerm>(nt));
    }
    std::cout << "\n";
    std::cout << std::string(100, '-') << "\n";

    for (size_t s = 0; s < table.numStates; ++s) {
        std::cout << std::setw(6) << s;
        std::cout << " |";
        for (size_t t = 0; t < static_cast<size_t>(Term::_COUNT_); ++t) {
            const auto& entry = table.action[s][t];
            std::string str;
            switch (entry.type) {
                case ActionType::Error: str = ""; break;
                case ActionType::Shift: str = "s" + std::to_string(entry.value); break;
                case ActionType::Reduce: str = "r" + std::to_string(entry.value); break;
                case ActionType::Accept: str = "acc"; break;
            }
            std::cout << std::setw(8) << str;
        }
        std::cout << " |";
        for (size_t nt = 1; nt < static_cast<size_t>(NonTerm::_COUNT_); ++nt) {
            int32_t g = table.gotoTable[s][nt];
            std::string str = (g >= 0) ? std::to_string(g) : "";
            std::cout << std::setw(6) << str;
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "=== StationScript LALR(1) Compile-Time Generator Test ===\n\n";

    // Build grammar
    constexpr auto grammar = makeExprGrammar();
    std::cout << "Grammar: " << grammar.count << " productions\n";
    for (size_t i = 0; i < grammar.count; ++i) {
        const auto& prod = grammar[i];
        std::cout << "  " << (i + 1) << ". " << nonTermName(prod.lhs) << " ->";
        for (size_t j = 0; j < prod.rhsSize; ++j) {
            const auto& sym = prod.rhs[j];
            if (sym.isTerminal()) {
                std::cout << " " << termName(sym.asTerminal());
            } else {
                std::cout << " " << nonTermName(sym.asNonTerminal());
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    // Generate LALR(1) table
    constexpr auto table = LALRGenerator<NonTerm, Term, NonTerm::E, Term::END>
        ::generate<6, 3, 128, 256>(grammar);

    std::cout << "Generated table with " << table.numStates << " states\n\n";

    printTable(table);

    std::cout << "\n=== Verification ===\n";
    bool ok = true;

    // State 0, ID should be shift
    if (table.action[0][static_cast<size_t>(Term::ID)].type != ActionType::Shift) {
        std::cout << "FAIL: State 0, ID should be shift\n";
        ok = false;
    } else {
        std::cout << "PASS: State 0, ID = s" 
                  << table.action[0][static_cast<size_t>(Term::ID)].value << "\n";
    }

    // State 0, LPAREN should be shift
    if (table.action[0][static_cast<size_t>(Term::LPAREN)].type != ActionType::Shift) {
        std::cout << "FAIL: State 0, ( should be shift\n";
        ok = false;
    } else {
        std::cout << "PASS: State 0, ( = s" 
                  << table.action[0][static_cast<size_t>(Term::LPAREN)].value << "\n";
    }

    // GOTO[0, E] should be valid
    if (table.gotoTable[0][static_cast<size_t>(NonTerm::E)] < 0) {
        std::cout << "FAIL: GOTO[0, E] should be valid\n";
        ok = false;
    } else {
        std::cout << "PASS: GOTO[0, E] = " 
                  << table.gotoTable[0][static_cast<size_t>(NonTerm::E)] << "\n";
    }

    // GOTO[0, T] should be valid
    if (table.gotoTable[0][static_cast<size_t>(NonTerm::T)] < 0) {
        std::cout << "FAIL: GOTO[0, T] should be valid\n";
        ok = false;
    } else {
        std::cout << "PASS: GOTO[0, T] = " 
                  << table.gotoTable[0][static_cast<size_t>(NonTerm::T)] << "\n";
    }

    // GOTO[0, F] should be valid
    if (table.gotoTable[0][static_cast<size_t>(NonTerm::F)] < 0) {
        std::cout << "FAIL: GOTO[0, F] should be valid\n";
        ok = false;
    } else {
        std::cout << "PASS: GOTO[0, F] = " 
                  << table.gotoTable[0][static_cast<size_t>(NonTerm::F)] << "\n";
    }

    // Some state should have accept action on $
    bool hasAccept = false;
    for (size_t s = 0; s < table.numStates; ++s) {
        if (table.action[s][static_cast<size_t>(Term::END)].type == ActionType::Accept) {
            hasAccept = true;
            std::cout << "PASS: State " << s << " has accept action on $\n";
            break;
        }
    }
    if (!hasAccept) {
        std::cout << "FAIL: No state has accept action\n";
        ok = false;
    }

    std::cout << "\n";
    if (ok) {
        std::cout << "All basic checks PASSED!\n";
    } else {
        std::cout << "Some checks FAILED!\n";
    }

    return ok ? 0 : 1;
}
