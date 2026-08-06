# How to Package

<details>
<summary>中文</summary>

# 如何打包

1. 安装esbuild（直接使用pkg会导致找不到vscode-languageserver与vscode-languageserver/node）
   ```cmd
   pnpm i --save-dev esbuild@latest
   ```
   
2. 安装pkg
   ```cmd
   pnpm i --save-dev @yao-pkg/pkg@latest
   ```
   
3. 安装verdaccio
   ```cmd
   pnpm i --save-dev verdaccio@latest 
   ```
   
4. 新建一个终端，确保不阻塞下一步命令，启动verdaccio
   ```cmd
   pnpm exec verdaccio
   ```
   
5. 添加用户
   ```cmd
   npm adduser --registry http://localhost:4873
   ```
   
6. 发布common包到本地仓库（在common目录下）
   ```cmd
   npm publish --registry=http://localhost:4873
   ```
   
7. 回到server目录，精确安装common 1.0.0包
   ```cmd
   pnpm add common@1.0.0 --registry http://localhost:4873
   ```
   
8. 编译为javascript文件
   ```cmd
   pnpm run build
   ```
   
9. 使用esbuild打包为单文件
   ```cmd
   pnpm exec esbuild ./out/server/server.js --bundle --platform=node --outfile=bundle.js --external:ic10-node-api 
   ```

10. 使用pkg打包（使用镜像源加速）
   ```cmd
   set NODE_MIRROR=https://mirrors.huaweicloud.com/nodejs/ && pkg bundle.js --targets node26-win-x64 --output ic10-lsp.exe
   ```

11. 将package.json中common的版本改回`file:../common`

12. 删除__%APPDATA%\verdaccio\storage__的缓存

## 使用node新特性SEA打包

参见[SEA官方文档](https://nodejs.cn/api/single-executable-applications.html)同时确保本地node版本支持该功能

目前缺点在于，当lsp运行时会产生关于SEA的警告信息

上述实现中pkg相关步骤在实际操作中是耗时最久的，其获取node的速度以小时计，相比下SEA会快很多

1. 上述2、10步骤无需执行

2. 在原第10步位置改为手动编辑bundle.js（位于server目录位置）
   > [!TIP]
   > 建议使用IDE简化操作
   - 在文件开头，"use strict"之后添加
      ```js
      const fs = require("fs");
      const os = require("os");
      const path = require("path");
      const { getRawAsset } = require("node:sea");
         
      const tmpFile = path.join(os.tmpdir(), "ic10-node-api.node");
      fs.writeFileSync(tmpFile, new Uint8Array(getRawAsset("ic10-node-api.node")));
      const api = { exports: {} };
      process.dlopen(api, tmpFile);
         
      global.ic10NodeApi = api.exports;
      ```
   - 将*bundle.js*中所有`require("ic10-node-api")`的地方，替换为`global.ic10NodeApi`
   - 将*bundle.js*中所有`require("ic10-node-api/static/stdLib.ic.json")`替换为`JSON.parse(require('node:sea').getAsset('stdLib.ic.json', 'utf8'))`

3. 随后使用node打包
   ```cmd
   node --build-sea sea-config.json
   ```
   
4. 可选11、12步的清理

</details>

# How to Package

1. **Install esbuild** (using `pkg` directly will cause issues with `vscode-languageserver` and `vscode-languageserver/node`):
   ```cmd
   pnpm i --save-dev esbuild@latest
   ```

2. **Install pkg**:
   ```cmd
   pnpm i --save-dev @yao-pkg/pkg@latest
   ```

3. **Install verdaccio**:
   ```cmd
   pnpm i --save-dev verdaccio@latest
   ```

4. **Start verdaccio** in a new terminal (make sure it doesn't block the next commands):
   ```cmd
   pnpm exec verdaccio
   ```

5. **Add a user**:
   ```cmd
   npm adduser --registry http://localhost:4873
   ```

6. **Publish the `common` package** to the local registry (inside the `common` directory):
   ```cmd
   npm publish --registry=http://localhost:4873
   ```

7. **Go back to the `server` directory** and install the `common` package at exact version `1.0.0`:
   ```cmd
   pnpm add common@1.0.0 --registry http://localhost:4873
   ```

8. **Compile to JavaScript files**:
   ```cmd
   pnpm run build
   ```

9. **Bundle into a single file** using esbuild:
   ```cmd
   pnpm exec esbuild ./out/server/server.js --bundle --platform=node --outfile=bundle.js --external:ic10-node-api
   ```

10. **Package with pkg**:
    ```cmd
    pkg bundle.js --targets node26-win-x64 --output ic10-lsp.exe
    ```

11. **Revert** the `common` version in `package.json` back to `file:../common`.

12. **Delete the verdaccio cache** at `%APPDATA%\verdaccio\storage`.

---

## Packaging with Node.js SEA (Single Executable Applications)

See the [official SEA documentation](https://nodejs.org/api/single-executable-applications.html) and make sure your local Node.js version supports this feature.

In the above process, the steps involving `pkg` are by far the most time‑consuming in practice – downloading Node.js binaries can take hours. In comparison, SEA is much faster.

1. **Steps 2 and 10 above do not need to be executed.**

2. **At the original step 10, instead, manually edit `bundle.js`**
   > [!TIP]
   > It is recommended to use an IDE to simplify the operations.
   - At the beginning of the file, after `"use strict"`, add:
     ```js
     const fs = require("fs");
     const os = require("os");
     const path = require("path");
     const { getRawAsset } = require("node:sea");
     
     const tmpFile = path.join(os.tmpdir(), "ic10-node-api.node");
     fs.writeFileSync(tmpFile, new Uint8Array(getRawAsset("ic10-node-api.node")));
     const api = { exports: {} };
     process.dlopen(api, tmpFile);
     
     global.ic10NodeApi = api.exports;
     ```
   - Replace **all occurrences** of `require("ic10-node-api")` in `bundle.js` with `global.ic10NodeApi`.
   - Replace **all occurrences** of `require("ic10-node-api/static/stdLib.ic.json")` with `JSON.parse(require('node:sea').getAsset('stdLib.ic.json', 'utf8'))`.

3. **Then package using Node.js**:
   ```cmd
   node --build-sea sea-config.json
   ```

4. **Optionally, perform cleanup steps 11 and 12** above.
