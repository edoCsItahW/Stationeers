/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file event.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/21 23:58
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

type Listener = (...args: any[]) => void;

/**
 * @summary 轻量级发布/订阅事件发射器
 *
 * @summary Lightweight pub/sub event emitter
 *
 * @desc 提供 on（订阅）、off（取消订阅）、emit（触发）和 clear（清理）功能。
 * on() 返回取消订阅函数，避免持有 listener 引用。emit 时复制监听器列表后再遍历，防止回调中修改导致漏触发。
 *
 * @desc Provides on (subscribe), off (unsubscribe), emit (fire), and clear.
 * on() returns an unsubscribe function to avoid holding listener references.
 * emit copies the listener list before iterating to prevent skipped callbacks during mutation.
 * */
export class EventEmitter {
    private events = new Map<string, Listener[]>();

    on(event: string, listener: Listener): () => void {
        const listeners = this.events.get(event) || [];
        listeners.push(listener);
        this.events.set(event, listeners);
        // 返回取消订阅函数
        return () => this.off(event, listener);
    }

    off(event: string, listener: Listener): void {
        const listeners = this.events.get(event);
        if (!listeners) return;
        const index = listeners.indexOf(listener);
        if (index > -1) listeners.splice(index, 1);
    }

    emit(event: string, ...args: any[]): void {
        const listeners = this.events.get(event);
        if (!listeners) return;
        // 复制一份，避免回调中修改影响遍历
        [...listeners].forEach(fn => fn(...args));
    }

    // 清空所有监听（用于测试或卸载）
    clear(event?: string): void {
        if (event) this.events.delete(event);
        else this.events.clear();
    }
}
