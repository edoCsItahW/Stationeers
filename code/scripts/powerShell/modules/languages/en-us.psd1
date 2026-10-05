@{
    Build = @{
        Configure = @{
            Head = "Configure CMake"
            Error = "CMake configuration failed."
            Skip = "Build directory already exists, skipping CMake configuration."
            RemoveCache = "Removed old CMake cache to apply new configuration options."
        }
        Build = @{
            Head = "Building target"
            Target = "Building target: {0}"
            Config = "Configuration: {0}"
            Cache = "Target '{0}' cache hit (IMPORTED), skipping build"
            Error = "CMake build failed with exit code {0}"
        }
        Copy = @{
            SourceNotFound = "Source file '{0}' not found"
            Error = "Failed to copy file with exit code {0}"
            Success = "Copied '{0}' to '{1}'"
        }
        StdLib = @{
            Head = "Sync standard library"
            SourceNotFound = "Standard library source '{0}' not found"
            NoMarker = "Standard library has no date marker (first line should be '# stdLib.ic YYYY-MM-DD'): '{0}'"
            Stale = "Standard library copy is out of sync: source date '{0}', copy date '{1}'"
            Synced = "Standard library synced (date {0} -> {1})"
        }
    }
    Node = @{
        Copy = "Copy .node artifacts"
        Test = "jest tests"
        Error = "Script execution failed with error: '{0}'"
    }
    Python = @{
        Copy = "Copy .pyd artifacts"
        Test = "pytest tests"
        Error = "Script execution failed with error: '{0}'"
    }
    Java = @{
        Copy = "Copy .dll artifacts"
        Test = "junit tests"
        Error = "Script execution failed with error: '{0}'"
    }
    Core = @{
        Test = "gtest tests"
        Error = "Script execution failed with error: '{0}'"
    }
}