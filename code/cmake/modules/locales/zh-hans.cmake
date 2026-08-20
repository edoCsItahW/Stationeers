# Locale: zh-hans (简体中文)
# MSG_ID 格式: ST_{Level}{Component}{Number}
#   Level    = I | D | W | E (Info / Doc / Warning / Error)
#   Component= 组件全名 (Functions, CommonSettings, Node, Dependencies,
#                       Pybind11, Fbjni, ...) — 不允许单字母简写,避免冲突
#   Number   = 两位十进制序号
# --------------------------------------------------------------


# CMakeLists.txt

## code/CMakeLists.txt
set(ST_DC0 "启用ccache/sccache")
set(ST_DC1 "启用并行编译")
set(ST_DC2 "启用工件缓存")
set(ST_DC3 "启用三方依赖缓存")

# ---- Functions ----
set(ST_EFunctions0 "源码目录'{0}'不存在")
set(ST_EFunctions1 "在特性路径中找不到'{1}'的父路径'{0}'")
set(ST_EFunctions2 "在特性路径中找不到根")
set(ST_IFunctions3 "目标'{0}'缓存命中（哈希'{1}'）")
set(ST_IFunctions4 "目标'{0}'缓存未命中，从源代码构建（期望哈希'{1}'）")
set(ST_DFunctions5 "将'{0}'的缓存更新为'{1}'")
set(ST_EFunctions6 "文件'{0}'不存在")

# ---- CommonSettings ----
set(ST_ICommonSettings0 "使用'{0}'，位于'{1}'")

# ---- Node (FindNode + FindNodeAddonApi) ----
# Doc strings (D 前缀) — 用于 option / cache 变量说明
set(ST_DNode0 "Node.js可执行文件")
set(ST_DNode1 "Node.js可执行文件不存在")
set(ST_DNode2 "获取Node.js版本失败")
set(ST_DNode3 "自动安装缺失的Node.js开发文件和node-addon-api")
set(ST_DNode8 "尝试自动安装后找不到Node.js头文件。")
set(ST_DNode9 "找不到Node.js导入库（Windows）")
set(ST_DNode18 "包管理器命令")
set(ST_DNode19 "安装包管理器命令")
set(ST_DNode20 "包管理器的执行命令")
set(ST_DNode21 "用于包管理器的DLX命令")
set(ST_DNode22 "包管理器是否为pnpm")
set(ST_DNode23 "包管理器查找已完成")
set(ST_DNode24 "包管理器保存模式命令")
set(ST_DNode25 "缓存node-addon-api头文件目录")
# Info strings (I 前缀) — st_l_info / st_l_info_fmt
set(ST_INode4 "缓存中找不到Node.js头文件。尝试通过node-gyp进行安装...")
set(ST_INode7 "node-gyp成功完成。")
set(ST_INode10 "找不到node-addon-api。正在尝试安装...")
set(ST_INode13 "FindNode: 版本'{0}'")
set(ST_INode14 "FindNode: 包含目录'{0}'")
set(ST_INode15 "FindNode: 库'{0}'")
set(ST_INode16 "FindNode: node-addon-api位于'{0}'")
set(ST_INode17 "FindNode: node-addon-api未找到（可选）")
# Warning strings (W 前缀) — st_l_warning / st_l_warning_fmt
set(ST_WNode5 "未找到pnpm或npm。无法自动安装依赖项。")
set(ST_WNode6 "'node-gyp install'失败（错误：{0}）。你可能需要手动安装。")
set(ST_WNode11 "未能安装node-addon-api（错误：{0}）。在不使用node-addon-api的情况下继续。")
set(ST_WNode12 "没有可用于安装node-addon-api的包管理器。")

# ---- Dependencies ----
set(ST_EDependencies0 "ensure_package需要NAME, REPO, TAG, INSTALL_PREFIX")
set(ST_IDependencies1 "找到'{0}'（系统或{1}）")
set(ST_IDependencies2 "未找到'{0}'。从'{1}'获取...")

# ---- Pybind11 ----
# Doc strings (D 前缀)
set(ST_DPybind110 "如果未找到，则通过pip自动安装pybind11")
set(ST_DPybind112 "pybind11 CMake配置的路径")
set(ST_DPybind1112 "pybind11包含目录")
# Info strings (I 前缀)
set(ST_IPybind111 "FindPybind11: 尝试通过Python检测pybind11...")
set(ST_IPybind113 "FindPybind11: 自动检测pybind11_DIR = '{0}'")
set(ST_IPybind114 "FindPybind11: 自动检测失败（结果='{0}'，错误='{1}）")
set(ST_IPybind115 "FindPybind11: 未找到pybind11，尝试通过pip安装...")
set(ST_IPybind116 "FindPybind11: pip安装成功，重新检测...")
set(ST_IPybind117 "FindPybind11: 安装后自动检测pybind11_DIR = '{0}'")
set(ST_IPybind118 "FindPybind11: 安装后仍无法获取pybind11 CMake目录。")
set(ST_IPybind119 "FindPybind11: pip安装失败（错误：{0}）。请手动安装pybind11。")
set(ST_IPybind1110 "FindPybind11: 未找到pybind11。请通过`pip install pybind11`安装或设置pybind11_DIR。")
set(ST_IPybind1111 "FindPybind11: 目标pybind11::pybind11缺失。请检查你的pybind11安装。")
set(ST_IPybind1113 "FindPybind11: Python可执行文件'{0}'")
set(ST_IPybind1114 "FindPybind11: Pybind11目录'{0}'")
set(ST_IPybind1115 "FindPybind11: 包含目录'{0}'")
set(ST_IPybind1116 "FindPybind11: 目标'pybind11::pybind11'自动提供Python头文件和库。")

# ---- Fbjni ----
# Doc strings (D 前缀)
set(ST_DFbjni0 "JDK根目录")
set(ST_DFbjni1 "JDK安装根")
set(ST_DFbjni3 "如果找不到fbjni，则自动下载并构建fbjni。")
set(ST_DFbjni4 "fbjni自动安装的Git标签")
set(ST_DFbjni5 "自动构建fbjni的安装路径前缀")
set(ST_DFbjni6 "fbjni源码目录路径")
# Info strings (I 前缀)
set(ST_IFbjni0 "FindFbjni: 找到现有fbjni源码于'{0}'（跳过下载）")
set(ST_IFbjni1 "FindFbjni: 未找到fbjni源码，从{0}下载...")
set(ST_IFbjni2 "FindFbjni: 正在修补fbjni CMakeLists.txt（SHARED -> STATIC，移除PDB安装规则）")
set(ST_IFbjni3 "FindFbjni: fbjni构建成功（静态库）")
set(ST_IFbjni4 "FindFbjni: fbjni_INCLUDE_DIRS = '{0}'")
set(ST_IFbjni5 "FindFbjni: fbjni_LIBRARIES = '{0}'")
set(ST_IFbjni6 "FindFbjni: 版本 = '{0}'")
set(ST_IFbjni7 "FindFbjni: 未找到fbjni。如需启用自动下载，请设置-DFBJNI_AUTO_INSTALL=ON")
# Warning strings (W 前缀)
set(ST_WFbjni2 "未找到JAVA_HOME。fbjni可能无法构建。")
set(ST_WFbjni3 "FindFbjni: 未能创建fbjni目标。")

# ---- GoogleTest ----
# Doc strings (D 前缀)
set(ST_DGoogleTest0 "如果找不到GoogleTest，则自动下载并构建GoogleTest。")
set(ST_DGoogleTest1 "GoogleTest自动安装的Git标签")
set(ST_DGoogleTest2 "自动构建GoogleTest的安装路径前缀")
set(ST_DGoogleTest3 "GoogleTest源码目录路径")
# Info strings (I 前缀)
set(ST_IGoogleTest0 "FindGoogleTest: 找到现有GoogleTest源码于'{0}'（跳过下载）")
set(ST_IGoogleTest1 "FindGoogleTest: 未找到GoogleTest源码，从{0}下载...")
set(ST_IGoogleTest2 "FindGoogleTest: GoogleTest构建成功")
set(ST_IGoogleTest3 "FindGoogleTest: 未找到GoogleTest。如需启用自动下载，请设置-DGOOGLETEST_AUTO_INSTALL=ON")
set(ST_IGoogleTest4 "FindGoogleTest: GoogleTest_INCLUDE_DIRS = '{0}'")
set(ST_IGoogleTest5 "FindGoogleTest: GoogleTest_LIBRARIES = '{0}'")
set(ST_IGoogleTest6 "FindGoogleTest: 版本 = '{0}'")
# Warning strings (W 前缀)
set(ST_WGoogleTest0 "FindGoogleTest: 未能创建gtest目标。")
