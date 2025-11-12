#!/usr/bin/env bash
set -euo pipefail

# Safe wrapper to run the DataRegistry C binding generator and a compile-check
# - Attempts to compile a small standalone driver (linking installed libSDOM
#   or compiling local sources as a fallback)
# - Runs the driver with a hard timeout, captures stdout/stderr to a log
# - Lists generated files and prints compile stderr if present
#
# Usage:
#   tools/run_c_binding_gen.sh [outdir]
# Example:
#   tools/run_c_binding_gen.sh examples/test/build/test_out_dataregistry

OUTDIR="${1:-examples/test/api_gen}"
LOGDIR="build/logs"
mkdir -p "$OUTDIR" "$LOGDIR" tools
TS=$(date +%Y%m%d_%H%M%S)
LOG="$LOGDIR/run_c_binding_gen_$TS.log"
ERR1="tools/compile_step1.err"
ERR2="tools/compile_step2.err"
EXE="tools/run_dataregistry_test"
EXE_LOCAL="tools/run_dataregistry_test_local"

echo "[run_c_binding_gen] Output dir: $OUTDIR" | tee "$LOG"

# Clean output dir (be conservative)
if [ -d "$OUTDIR" ]; then
  echo "[run_c_binding_gen] Cleaning existing output directory: $OUTDIR" | tee -a "$LOG"
  rm -rf "$OUTDIR"/* || true
fi
mkdir -p "$OUTDIR"

echo "[run_c_binding_gen] Prefer local-built helper when available (avoid linking installed libSDOM)" | tee -a "$LOG"
# If a locally-built helper binary exists, prefer it outright.
if [ -x "$EXE_LOCAL" ]; then
  echo "[run_c_binding_gen] Using existing local helper: $EXE_LOCAL" | tee -a "$LOG"
  RUN="$EXE_LOCAL"
else
  # Try building the helper from local sources first
  echo "[run_c_binding_gen] Attempting to compile helper from local sources..." | tee -a "$LOG"
  if g++ -std=c++17 -I./include tools/run_dataregistry_test.cpp src/SDOM_DataRegistry.cpp src/SDOM_CBindingGenerator.cpp -pthread -o "$EXE_LOCAL" 2> "$ERR2"; then
    echo "[run_c_binding_gen] Compiled $EXE_LOCAL (local sources)" | tee -a "$LOG"
    RUN="$EXE_LOCAL"
  else
    echo "[run_c_binding_gen] Local compile failed; see $ERR2; attempting to compile/link against installed libSDOM..." | tee -a "$LOG"
    if g++ -std=c++17 -I./include -I"$HOME/.local/include" tools/run_dataregistry_test.cpp -L"$HOME/.local/lib" -lSDOM -o "$EXE" 2> "$ERR1"; then
      echo "[run_c_binding_gen] Compiled $EXE (linked to libSDOM)" | tee -a "$LOG"
      RUN="$EXE"
    else
      echo "[run_c_binding_gen] Both local and installed-link compile attempts failed. See $ERR1 and $ERR2" | tee -a "$LOG"
      echo "---- $ERR1 ----" >> "$LOG"
      cat "$ERR1" >> "$LOG" || true
      echo "---- $ERR2 ----" >> "$LOG"
      cat "$ERR2" >> "$LOG" || true
      exit 2
    fi
  fi
fi

echo "[run_c_binding_gen] Running generator (timeout 20s)..." | tee -a "$LOG"
export SDOM_RUN_GEN_COMPILE=1
if timeout 20s "$RUN" "$OUTDIR" >>"$LOG" 2>&1; then
  echo "[run_c_binding_gen] Run finished successfully" | tee -a "$LOG"
  RC=0
else
  RC=$?
  echo "[run_c_binding_gen] Run exited with status $RC (timeout or error). See $LOG" | tee -a "$LOG"
fi

echo "[run_c_binding_gen] Listing output dir: $OUTDIR" | tee -a "$LOG"
ls -la "$OUTDIR" | tee -a "$LOG" || true

# Also check the helper's internal default output directory used by the
# standalone driver, since the helper currently writes to a fixed path.
DEFAULT_HELPER_OUT="build/test_out_dataregistry_standalone"
if [ "$OUTDIR" != "$DEFAULT_HELPER_OUT" ]; then
  echo "[run_c_binding_gen] Also listing helper default output dir: $DEFAULT_HELPER_OUT" | tee -a "$LOG"
  ls -la "$DEFAULT_HELPER_OUT" | tee -a "$LOG" || true
fi
if [ -f "$OUTDIR/compile_err.txt" ]; then
  echo "[run_c_binding_gen] ---- compile_err.txt ----" | tee -a "$LOG"
  sed -n '1,200p' "$OUTDIR/compile_err.txt" | tee -a "$LOG"
fi

echo "[run_c_binding_gen] Full log: $LOG"
exit "$RC"
