#!/usr/bin/env bash
set -euo pipefail

# repeat_tests.sh
# Usage: ./repeat_tests.sh [iterations] [optional-filter]
# Runs ./prog multiple times, saves logs to repeat_logs/, and reports how many runs failed.

# Parse positional args but allow the script flag --save-error-logs anywhere in the args.
# Usage: ./repeat_tests.sh [iterations] [optional-filter] [--save-error-logs]
SAVE_ERROR_LOGS=0
ITER=""
FILTER=""
for arg in "$@"; do
  case "$arg" in
    --save-error-logs)
      SAVE_ERROR_LOGS=1
      ;;
    *)
      if [ -z "$ITER" ]; then
        ITER="$arg"
      elif [ -z "$FILTER" ]; then
        FILTER="$arg"
      else
        # ignore extra args
        :
      fi
      ;;
  esac
done
ITER=${ITER:-20}
FILTER=${FILTER:-}
# By default the script writes per-run logs to repeat_logs/.
# For each run we keep the per-run log only if that run failed; successful run logs are removed.
BIN="./prog"
# Default flag to make the program stop after running tests so the app
# doesn't require manual closing. Can be overridden by setting STOP_FLAG
# in the environment before invoking the script.
STOP_FLAG="--stop_after_tests"
# Build a command array so flags with spaces are handled safely.
# Build a command array so flags with spaces are handled safely.
CMD=("$BIN")
# Only append STOP_FLAG if non-empty so users can override by setting
# STOP_FLAG empty in the environment (e.g. STOP_FLAG= ./repeat_tests.sh)
if [ -n "${STOP_FLAG:-}" ]; then
  CMD+=("$STOP_FLAG")
fi
OUT_DIR="repeat_logs"
# Only create the output directory when the user asked to save error logs.
if [ "$SAVE_ERROR_LOGS" -eq 1 ]; then
  mkdir -p "$OUT_DIR"
  echo "Running $ITER iterations of ${CMD[*]}. Logs -> $OUT_DIR/"
else
  echo "Running $ITER iterations of ${CMD[*]}. Not saving logs (use --save-error-logs to save failing logs)."
fi

fail_count=0
failed_runs=()

for i in $(seq 1 "$ITER"); do
  echo "=== RUN $i ==="
  LOG="$OUT_DIR/run_$i.log"
  if [ "$SAVE_ERROR_LOGS" -eq 1 ]; then
    # save full output for post-run inspection
    if [ -n "$FILTER" ]; then
      "${CMD[@]}" | tee "$LOG"
      rc=${PIPESTATUS[0]:-$?}
    else
      "${CMD[@]}" | tee "$LOG"
      rc=${PIPESTATUS[0]:-$?}
    fi
  else
    # do not save files; capture the program output so we can inspect it
    # on failure. We still print the output to the terminal for visibility.
    output="$("${CMD[@]}" 2>&1)"
    rc=$?
    # Print the captured output to the terminal so users can see progress
    echo "$output"
  fi

  # Consider a run failed if the binary returned non-zero or the log contains "[FAILED]" or "Lua test" failures
  # Determine if this run failed by return code or by failure markers in the log
  failed=0
  if [ "$rc" -ne 0 ]; then
    failed=1
  else
    # If we're saving logs, inspect the saved log file. Otherwise inspect the
    # captured output stored in the 'output' variable.
    if [ "$SAVE_ERROR_LOGS" -eq 1 ]; then
      if grep -q "\[FAILED\]" "$LOG" || grep -q "Lua test[0-9]* failed" "$LOG"; then
        failed=1
      fi
    else
      if echo "${output:-}" | grep -q "\[FAILED\]" || echo "${output:-}" | grep -q "Lua test[0-9]* failed"; then
        failed=1
      fi
    fi
  fi

  if [ "$failed" -eq 1 ]; then
    fail_count=$((fail_count+1))
    failed_runs+=("$i")
    echo "--- Run $i: FAIL (rc=$rc) ---"
    # If saving is enabled we keep the failing log; otherwise nothing to keep.
    if [ "$SAVE_ERROR_LOGS" -eq 1 ]; then
      echo "Failing run log kept: $LOG"
    else
      echo "Logs are not being saved. Rerun with --save-error-logs to preserve the failing run output."
    fi
    # stop the loop on first failure
    break
  else
    echo "--- Run $i: PASS ---"
    # remove successful run log to save space (only when saving is enabled)
    if [ "$SAVE_ERROR_LOGS" -eq 1 ]; then
      rm -f "$LOG" || true
    fi
  fi
  # small pause to avoid hammering resources
  sleep 0.1
done

echo
echo "Summary: $ITER runs, $fail_count failed."
  if [ "$fail_count" -ne 0 ]; then
  echo "Failed runs: ${failed_runs[*]}"
  echo "Showing tail of first failed run log:"
  first=${failed_runs[0]}
    if [ "${NO_LOGS:-0}" -eq 1 ]; then
      # In NO_LOGS mode we captured the last run output in the 'output' variable
      printf "--- tail of last run (no-log mode) ---\n"
      echo "${output:-}" | tail -n 200 || true
    else
      if [ "$SAVE_ERROR_LOGS" -eq 1 ]; then
        printf "--- tail of %s ---\n" "$OUT_DIR/run_${first}.log"
        tail -n 200 "$OUT_DIR/run_${first}.log" || true
      else
        # We didn't save logs, but we captured the failing run output in 'output'.
        printf "--- tail of last run (captured output) ---\n"
        echo "${output:-}" | tail -n 200 || true
      fi
    fi
fi

exit $fail_count
