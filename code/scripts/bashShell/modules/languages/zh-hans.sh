export TRANSLATIONS='{
  "Build": {
    "Configure": {
      "Head": "配置 CMake",
      "Error": "配置CMake失败。",
      "Skip": "构建目录已存在，跳过 CMake 配置。",
      "RemoveCache": "已删除旧的CMake缓存，以应用新配置选项。"
    },
    "Build": {
      "Head": "构建目标",
      "Target": "构建目标: {0}",
      "Config": "配置: {0}",
      "Cache": "目标 '{0}' 缓存命中（IMPORTED），跳过构建",
      "Error": "CMake 构建失败，退出码 {0}"
    },
    "Copy": {
      "Head": "复制工件",
      "SourceNotFound": "找不到源文件 {0}",
      "Error": "复制文件失败，退出码 {0}",
      "Success": "已将 {0} 复制到 {1}"
    },
    "StdLib": {
      "Head": "同步标准库",
      "SourceNotFound": "找不到标准库源文件 {0}",
      "NoMarker": "标准库缺少日期标志（首行应为 '# stdLib.ic YYYY-MM-DD'）：{0}",
      "Stale": "标准库副本与源不同步：源日期 {0}，副本日期 {1}",
      "Synced": "标准库已同步（日期 {0} → {1}）",
      "Skipped": "该目标不发布标准库，跳过同步（目标 {0}）"
    },
    "Test": {
      "Head": "运行测试"
    },
    "ConfigFileNotFound": "未找到 build-info.json 配置文件。",
    "TargetNotFound": "未找到目标 {0}",
    "UnknownTarget": "未知目标 {0}"
  },
  "Node": {
    "Copy": "复制.node工件",
    "Test": "jest测试",
    "Error": "脚本执行失败，退出码 {0}"
  },
  "Python": {
    "Copy": "复制.pyd工件",
    "Test": "pytest测试",
    "Error": "脚本执行失败，退出码 {0}"
  },
  "Java": {
    "Copy": "复制.so工件",
    "Test": "junit测试",
    "Error": "脚本执行失败，退出码 {0}"
  },
  "Core": {
    "Test": "gtest测试",
    "Error": "脚本执行失败，退出码 {0}"
  }
}'