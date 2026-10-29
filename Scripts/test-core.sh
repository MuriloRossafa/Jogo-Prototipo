#!/usr/bin/env bash
set -euo pipefail
project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
test_binary="$(mktemp /tmp/other-side-tests.XXXXXX)"
trap 'rm -f "$test_binary"' EXIT
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined "$project_root/Tests/CaseSimulationTests.cpp" -o "$test_binary"
"$test_binary"
