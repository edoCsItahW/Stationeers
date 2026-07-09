#ifndef COMPILER_IC10_PCH_HPP
#define COMPILER_IC10_PCH_HPP
#pragma once

// 标准库
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <expected>
#include <filesystem>
#include <fstream>
#include <format>
#include <iostream>
#include <memory>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

// 公共库
#include "common/utils/common.hpp"
#include "common/utils/fstr.hpp"
#include "common/utils/position.hpp"
#include "common/exception/error.hpp"
#include "common/exception/diagnostic.hpp"

// IC10核心头文件
#include "ic10/lexer/token.hpp"
#include "ic10/locals/local.hpp"

#endif
