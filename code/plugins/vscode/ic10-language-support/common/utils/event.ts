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
        if (event)
            this.events.delete(event);
        else
            this.events.clear();

    }
}