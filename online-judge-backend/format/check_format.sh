#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
SRC_DIR="${ROOT_DIR}/src"

FAILED=0
mapfile -t FILES < <(find "$SRC_DIR" -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) | sort)

if (( ${#FILES[@]} == 0 )); then
    echo "未找到需要检查的文件"
    exit 0
fi

for file in "${FILES[@]}"; do
    if ! clang-format "$file" | diff -q "$file" - >/dev/null; then
        echo "格式不符合规范: ${file#$ROOT_DIR/}"
        FAILED=1
    fi
done

if (( FAILED )); then
    echo "❌ 发现格式问题，请运行 ./format/format.sh 修复"
else
    echo "✅ 所有文件格式正确！"
fi

exit $FAILED
