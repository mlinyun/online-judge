#!/bin/bash

# 获取脚本所在目录的上级目录（项目根目录）
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "开始格式化项目代码..."
echo "项目根目录: $PROJECT_ROOT"

# 格式化 src 和 include 目录中所有 C++ 源文件
find "$PROJECT_ROOT/src" "$PROJECT_ROOT/include" -type f \
    \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.cc" \) \
    -exec clang-format -i {} \;

echo "✅ 代码格式化完成！"
