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