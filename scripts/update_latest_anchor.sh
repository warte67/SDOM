#!/usr/bin/env bash
# update_latest_anchor.sh
# -------------------------------------------------------------
# Automatically moves the <a id="latest-update"></a> anchor in
# docs/progress.md so it appears immediately above the most
# recent daily heading (e.g., "## October 31, 2025").
#
# Run this at the start of each new day before logging progress.
# -------------------------------------------------------------
set -euo pipefail

# Syncs the <a id="latest-update"></a> anchor in docs/progress.md.
# Behavior:
# 1) If a heading for today (e.g., "## … October 31, 2025") already exists,
#    remove any existing anchors and insert the anchor immediately above that heading.
# 2) Otherwise, if a line "#### end-of-day" exists, insert two lines just
#    before it:
#        <a id="latest-update"></a>
#        ## 🗓️ {Month} {D}, {YYYY}
#    (Then you can start typing under the freshly created heading.)
# 3) Otherwise, fall back to placing the anchor above the most recent daily heading.

PROG_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
REPO_ROOT=$(cd -- "${PROG_DIR}/.." && pwd)
FILE="${REPO_ROOT}/docs/progress.md"

if [[ ! -f "${FILE}" ]]; then
  echo "[update_latest_anchor] File not found: ${FILE}" >&2
  exit 0
fi

tmp_out=$(mktemp)

# Allow override for testing (e.g., SDOM_TODAY="November 1, 2025")
if [[ -n "${SDOM_TODAY:-}" ]]; then
  TODAY="$SDOM_TODAY"
else
  TODAY=$(date '+%B %-d, %Y' 2>/dev/null || date '+%B %e, %Y')
fi

awk -v TODAY="$TODAY" '
  BEGIN {
    months = "January|February|March|April|May|June|July|August|September|October|November|December"
    heading_re = "^##[[:space:]]+.*(" months ") [0-9]{1,2}, [0-9]{4}"
    last_idx = 0
    out_n = 0
    today_idx = 0
    eod_idx = 0
  }
  {
    # Drop any existing latest-update anchor lines
    if ($0 ~ /^<a id="latest-update"><\/a>$/) {
      next
    }
    out_n++
    lines[out_n] = $0
    if ($0 ~ heading_re) {
      last_idx = out_n
    }
    # Track explicit end-of-day marker location (keep the last one we see)
    if ($0 ~ /^####[[:space:]]+end-of-day(.*)$/) {
      eod_idx = out_n
    }
    # Detect a heading that contains TODAY
    if ($0 ~ /^##[[:space:]]+/ && index($0, TODAY) > 0) {
      today_idx = out_n
    }
  }
  END {
    # Fallback: if no level-2 headings matched, try older style daily headings
    if (last_idx == 0) {
      for (i = 1; i <= out_n; i++) {
        if (lines[i] ~ /^###[[:space:]]*\[[A-Za-z]+ [0-9]{1,2}, [0-9]{4}\]/) {
          last_idx = i
        }
      }
    }

    # Decide insertion strategy
    insert_anchor_at = 0
    insert_new_heading = 0
    new_heading_line = ""

    if (today_idx > 0) {
      insert_anchor_at = today_idx
    } else if (eod_idx > 0) {
      insert_anchor_at = eod_idx
      insert_new_heading = 1
      new_heading_line = "## 🗓️ " TODAY
    } else if (last_idx > 0) {
      insert_anchor_at = last_idx
    } else {
      # No obvious place; just print as-is
      for (i = 1; i <= out_n; i++) print lines[i]
      exit 0
    }

    for (i = 1; i <= out_n; i++) {
      if (i == insert_anchor_at) {
        print "<a id=\"latest-update\"></a>"
        if (insert_new_heading) {
          print new_heading_line
          print ""  # extra blank line before end-of-day marker
        }
      }
      print lines[i]
    }
  }
' "${FILE}" >"${tmp_out}"

if ! cmp -s "${FILE}" "${tmp_out}"; then
  mv "${tmp_out}" "${FILE}"
  echo "[update_latest_anchor] Updated: ${FILE}"
else
  rm -f "${tmp_out}"
  echo "[update_latest_anchor] No change needed"
fi
