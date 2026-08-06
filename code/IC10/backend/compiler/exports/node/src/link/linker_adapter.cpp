// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file linker_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/20 22:18
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

#include "ic10_compiler_node/link/linker_adapter.hpp"
#include "common_node/diagnostic_adapter.hpp"
#include "ic10_compiler/link/linker.hpp"
#include "ic10_compiler_node/parser/ast_adapter.hpp"
#include "ic10_compiler_node/semantic/symbol_table_adapter.hpp"
#include "ic10_compiler_node/semantic/type_table_adapter.hpp"

namespace stationeers::ic10 {

    LinkerAdapter::LinkerAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Object LinkerAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Linker",
            {
#ifdef _MSC_VER
                InstanceAccessor("diagnostics", &LinkerAdapter::getDiagnostics, nullptr),
                InstanceAccessor("units", &LinkerAdapter::getUnits, nullptr),
                InstanceAccessor("typeTable", &LinkerAdapter::getTypeTable, nullptr),
#else
                InstanceAccessor<&LinkerAdapter::getDiagnostics>("diagnostics"),
                InstanceAccessor<&LinkerAdapter::getUnits>("units"),
                InstanceAccessor<&LinkerAdapter::getTypeTable>("typeTable"),
#endif
                InstanceMethod<&LinkerAdapter::addUnit>("addUnit"),
                InstanceMethod<&LinkerAdapter::link>("link"),
            }
        );

        auto constructor = std::make_unique<node::FunctionReference>();
        *constructor     = node::Persistent(func);
        constructor->SuppressDestruct();

        (void)exports.Set("Linker", func);

        return exports;
    }

    node::Value LinkerAdapter::addUnit(const node::CallbackInfo& info) {
        Arguments args(info);


        if (args[0].IsObject()) {
            auto program = ProgramAdapter::from(args.getWithCheck<node::Object>(0));

            if (info.Length() > 1 && args[1].IsString()) {
                auto path = args.getWithCheck<node::String>(1).Utf8Value();
                linker_.addUnit(std::move(program), path);

            } else
                linker_.addUnit(std::move(program));


        } else if (args[0].IsString()) {
            auto source = args.getWithCheck<node::String>(0).Utf8Value();

            if (info.Length() > 1 && args[1].IsString()) {
                auto path = args.getWithCheck<node::String>(1).Utf8Value();
                linker_.addUnit(source, path);

            } else
                linker_.addUnit(source);

        } else
            Napi::TypeError::New(info.Env(), "addUnit requires Program or string argument")
                .ThrowAsJavaScriptException();


        return info.Env().Undefined();
    }

    node::Value LinkerAdapter::link(const node::CallbackInfo& info) {
        auto& symbolTable = linker_.link();
        return SymbolTableAdapter::to(info.Env(), symbolTable);
    }

    node::Value LinkerAdapter::getDiagnostics(const node::CallbackInfo& info) {
        auto diagnostics = linker_.getDiagnostics();
        auto size        = diagnostics.size();
        auto result      = node::Array::New(info.Env(), size);

        for (std::size_t i = 0; i < size; i++)
            result[i] = DiagnosticAdapter::to(info.Env(), diagnostics[i]);


        return result;
    }

    node::Value LinkerAdapter::getUnits(const node::CallbackInfo& info) {
        auto units  = linker_.getUnits();
        auto size   = units.size();
        auto result = node::Array::New(info.Env(), size);

        for (std::size_t i = 0; i < size; i++) {
            auto obj = node::Object::New(info.Env());
            obj.Set("path", units[i].path);
            auto diagArray = node::Array::New(info.Env());

            if (units[i].diagnostics)
                for (std::size_t j = 0; j < units[i].diagnostics->size(); j++)
                    diagArray[j] = DiagnosticAdapter::to(info.Env(), (*units[i].diagnostics)[j]);

            (void)obj.Set("diagnostics", diagArray);

            result[i] = obj;
        }

        return result;
    }

    node::Value LinkerAdapter::getTypeTable(const node::CallbackInfo& info) {
        return TypeTableAdapter::to(info.Env(), linker_.getTypeTable());
    }

}  // namespace stationeers::ic10
