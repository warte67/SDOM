#!/usr/bin/env bash
# Helper to run the ASan-instrumented test binary with leak suppressions for external libraries.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE="$(cd "${SCRIPT_DIR}/../.." && pwd)"
SUPP="${SCRIPT_DIR}/asan_lsan.supp"

# export ASAN_OPTIONS="detect_leaks=1:${ASAN_OPTIONS:-}"
# export LSAN_OPTIONS="suppressions=${SUPP}:${LSAN_OPTIONS:-}"
export ASAN_OPTIONS="detect_leaks=1:color=always:print_suppressions=1:${ASAN_OPTIONS-}"
export LSAN_OPTIONS="suppressions=${SUPP}:${LSAN_OPTIONS-}"


echo "Using ASan suppressions: ${SUPP}"

# prog is emitted to SCRIPT_DIR (not build/). If not found, check build tree.
BIN="${SCRIPT_DIR}/prog"
if [[ ! -x "$BIN" ]]; then
  ALT_BIN="${SCRIPT_DIR}/build/prog"
  if [[ -x "$ALT_BIN" ]]; then
    BIN="$ALT_BIN"
  else
    echo "ASan binary not found at ${BIN} or ${ALT_BIN}. Build with ./compile asan (optionally release) first." >&2
    exit 1
  fi
fi

exec "$BIN" "$@"
