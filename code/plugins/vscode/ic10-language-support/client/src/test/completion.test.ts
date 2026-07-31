/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

import * as vscode from 'vscode';
import * as assert from 'assert';
import { getDocUri, activate } from './helper';

suite('Should do completion', () => {
	const docUri = getDocUri('completion.txt');

	test('Completes JS/TS in txt file', async () => {
		await testCompletion(docUri, new vscode.Position(0, 0), {
			items: [
				{ label: 'JavaScript', kind: vscode.CompletionItemKind.Text },
				{ label: 'TypeScript', kind: vscode.CompletionItemKind.Text }
			]
		});
	});
});

async function testCompletion(
	docUri: vscode.Uri,
	position: vscode.Position,
	expectedCompletionList: vscode.CompletionList
) {
	await activate(docUri);

	// Executing the command `vscode.executeCompletionItemProvider` to simulate triggering completion
	const actualCompletionList = (await vscode.commands.executeCommand(
		'vscode.executeCompletionItemProvider',
		docUri,
		position
	)) as vscode.CompletionList;

	assert.ok(actualCompletionList.items.length >= 2);
	expectedCompletionList.items.forEach((expectedItem, i) => {
		const actualItem = actualCompletionList.items[i];
		assert.equal(actualItem.label, expectedItem.label);
		assert.equal(actualItem.kind, expectedItem.kind);
	});
}
