// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file analyser_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/22 18:07
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "analyser_adapter.hpp"
#include "ast_adapter.hpp"
#include "common_node/error_adapter.hpp"
#include "ic10/semantic/analyser.hpp"
#include "symbol_table_adapter.hpp"

namespace stationeers::ic10 {

    AnalyserAdapter::AnalyserAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Object AnalyserAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Analyser",
            {
                StaticMethod<&analyse>("analyse"),
                InstanceMethod<&AnalyserAdapter::visit>("visit"),
                InstanceAccessor<&AnalyserAdapter::getSymbolTable>("symbolTable"),
                InstanceAccessor<&AnalyserAdapter::getErrors>("errors"),
            }
        );

        node::FunctionReference* constructor = new node::FunctionReference();

        *constructor = node::Persistent(func);

        constructor->SuppressDestruct();

        (void)exports.Set("Analyser", func);

        return exports;
    }

    node::Value AnalyserAdapter::analyse(const node::CallbackInfo& info) {
        Arguments args(info);

        auto program = ProgramAdapter::from(args.getWithCheck<node::Object>(0));

        auto deferred = node::Promise::Deferred::New(info.Env());

        auto* worker = new TaskWorker<void>(deferred, [program] -> Task<> {
            return Analyser::analyse(program);
        });

        worker->Queue();

        return deferred.Promise();
    }

    node::Value AnalyserAdapter::getSymbolTable(const node::CallbackInfo& info) {
        return SymbolTableAdapter::to(info.Env(), analyser_.getSymbolTable());
    }

    node::Value AnalyserAdapter::getErrors(const node::CallbackInfo& info) {
        auto errors = analyser_.getErrors();

        auto size = errors.size();
        auto result = node::Array::New(info.Env(), size);

        for (std::size_t i = 0; i < size; i++)
            result[i] = ErrorAdapter::to(info.Env(), errors[i]);

        return result;
    }

    node::Value AnalyserAdapter::visit(const node::CallbackInfo& info) {
        Arguments args(info);

        auto program = ProgramAdapter::from(args.getWithCheck<node::Object>(0));

        auto deferred = node::Promise::Deferred::New(info.Env());

        auto* worker = new TaskWorker<void>(deferred, [this, program] -> Task<> {
            return analyser_.visit(program);
        });

        worker->Queue();

        return deferred.Promise();
    }

}  // namespace stationeers::ic10
