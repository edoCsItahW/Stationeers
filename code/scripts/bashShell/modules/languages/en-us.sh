export TRANSLATIONS='{
  "Build": {
    "Configure": {
      "Head": "Configure CMake",
      "Error": "CMake configuration failed.",
      "Skip": "Build directory exists, skipping CMake configure.",
      "RemoveCache": "Removed old CMake cache to apply new options."
    },
    "Build": {
      "Head": "Building target",
      "Target": "Building target: {0}",
      "Config": "Configuration: {0}",
      "Cache": "Target '{0}' cache hit (IMPORTED), skipping build",
      "Error": "CMake build failed with exit code {0}"
    },
    "Copy": {
      "Head": "Copying artifact",
      "SourceNotFound": "Source file {0} not found",
      "Error": "Failed to copy file with exit code {0}",
      "Success": "Copied {0} to {1}"
    },
    "StdLib": {
      "Head": "Syncing standard library",
      "SourceNotFound": "Standard library source {0} not found",
      "NoMarker": "Standard library has no date marker (first line should be '# stdLib.ic YYYY-MM-DD'): {0}",
      "Stale": "Standard library copy is out of sync: source date {0}, copy date {1}",
      "Synced": "Standard library synced (date {0} -> {1})",
      "Skipped": "This target publishes no standard library, skipping the sync (target {0})"
    },
    "Test": {
      "Head": "Running tests"
    },
    "ConfigFileNotFound": "build-info.json configuration file not found.",
    "TargetNotFound": "Target {0} not found",
    "UnknownTarget": "Unknown target {0}"
  },
  "Node": {
    "Copy": "Copying .node artifact",
    "Test": "jest tests",
    "Error": "Script execution failed with exit code {0}"
  },
  "Python": {
    "Copy": "Copying .pyd artifact",
    "Test": "pytest tests",
    "Error": "Script execution failed with exit code {0}"
  },
  "Java": {
    "Copy": "Copying .so artifact",
    "Test": "junit tests",
    "Error": "Script execution failed with exit code {0}"
  },
  "Core": {
    "Test": "gtest tests",
    "Error": "Script execution failed with exit code {0}"
  }
}'