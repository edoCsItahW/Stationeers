// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file base.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 22:41
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_node/base.hpp"

namespace stationeers {

    // Arguments

    Arguments::Arguments(const node::CallbackInfo& info)
        : info_(info) {}

    const node::Value Arguments::operator[](std::size_t index) const { return info_[index]; }

    const node::Value Arguments::get(std::size_t index) const {
        if (index < info_.Length()) return info_[index];

        return info_.Env().Undefined();
    }

    // TaskWorker

    TaskWorker<void>::TaskWorker(node::Promise::Deferred deferred, TaskFactory factory)
        : AsyncWorker(deferred.Env())
        , deferred_(deferred)
        , factory_(factory)
        , flag_(false) {}

    void TaskWorker<void>::Execute() {
        try {
            Task<> task = factory_();

            auto future = task.getFuture();

            if (auto result = future.get(); result.has_value())
                flag_ = true;

            else {
                if (result.error()) std::rethrow_exception(result.error());

                throw std::runtime_error("Unknown error");
            }

        } catch (const std::exception& e) { SetError(e.what()); } catch (...) {
            SetError("Unknown error");
        }
    }

    void TaskWorker<void>::OnOK() {
        if (flag_)
            deferred_.Resolve(Env().Undefined());

        else
            deferred_.Reject(node::Error::New(Env(), "Task failed without exception").Value());
    }

    void TaskWorker<void>::OnError(const Napi::Error& e) {
        deferred_.Resolve(e.Value());
    }

}  // namespace stationeers
