#!/usr/bin/env bash
# Export all Mermaid code blocks in docs/*.md to PNG and SVG using mermaid-cli (mmdc).
# Requirements:
#   - Node.js
#   - mermaid-cli: npm install -g @mermaid-js/mermaid-cli
# Usage:
#   bash scripts/export_mermaid.sh
# Outputs:
#   - PNG/SVG files under docs/diagrams/<md-basename>/diagram-<n>.png|svg

set -euo pipefail

# Determine Mermaid CLI invocation: prefer mmdc, else npx-based fallback
MERMAID_CMD=""
if command -v mmdc >/dev/null 2>&1; then
  MERMAID_CMD="mmdc"
elif command -v npx >/dev/null 2>&1; then
  # Use npx to run mermaid-cli without global install
  MERMAID_CMD="npx -y @mermaid-js/mermaid-cli@latest"
else
  echo "ERROR: Neither 'mmdc' nor 'npx' found in PATH."
  echo "Install one of the following:"
  echo "  - pacman -S nodejs npm   (then use the built-in npx)"
  echo "  - npm install -g @mermaid-js/mermaid-cli   (requires permissions)"
  echo "  - yay -S mermaid-cli   (AUR package)"
  exit 1
fi

# If the environment variable for Puppeteer executable isn't set, try to
# auto-detect a Chromium/Chrome binary and export it so puppeteer-core can
# find and use it. This avoids needing Puppeteer to download a browser and
# fixes "Could not find Chrome" errors when a system Chromium is installed.
if [[ -z "${PUPPETEER_EXECUTABLE_PATH:-}" ]]; then
  if command -v chromium >/dev/null 2>&1; then
    export PUPPETEER_EXECUTABLE_PATH="$(command -v chromium)"
  elif command -v chromium-browser >/dev/null 2>&1; then
    export PUPPETEER_EXECUTABLE_PATH="$(command -v chromium-browser)"
  elif command -v google-chrome >/dev/null 2>&1; then
    export PUPPETEER_EXECUTABLE_PATH="$(command -v google-chrome)"
  fi
  if [[ -n "${PUPPETEER_EXECUTABLE_PATH:-}" ]]; then
    echo "[mermaid] Using Puppeteer executable at: ${PUPPETEER_EXECUTABLE_PATH}"
  fi
fi

DOCS_DIR="docs"
OUT_DIR="docs/diagrams"
mkdir -p "$OUT_DIR"

# Collect Markdown files recursively under docs/
mapfile -t md_files < <(find "$DOCS_DIR" -type f -name '*.md' | sort)

extract_and_export() {
  local md_file="$1"
  local base
  base=$(basename "$md_file" .md)
  local dest_dir="$OUT_DIR/$base"
  mkdir -p "$dest_dir"

  # Extract Mermaid blocks between ```mermaid ... ```
  # We'll number them in order of appearance.
  awk 'BEGIN{inblock=0;count=0} \
    /^```mermaid(\..*|[[:space:]].*|)[[:space:]]*$/ {inblock=1;count++; fname=sprintf("%s/diagram-%02d.mmd", dest, count); print "# extracting to " fname > "/dev/stderr"; next} \
    /^```mermaid-norender[[:space:]]*$/ {inblock=1;count++; fname=sprintf("%s/diagram-%02d.mmd", dest, count); print "# extracting (norender) to " fname > "/dev/stderr"; next} \
    /^```[[:space:]]*$/ && inblock==1 {inblock=0; next} \
    { if(inblock==1) print > fname }' dest="$dest_dir" "$md_file"

  # Sanitize: remove any lingering code fences and Windows newlines
  if compgen -G "$dest_dir/diagram-*.mmd" > /dev/null; then
    sed -i -e '/^```/d' -e 's/^```.*$//' -e 's/\r$//' "$dest_dir"/diagram-*.mmd || true
    # If any file begins with a stray language tag like "```mermaid.radar", remove it fully
    # Ensure files start with a Mermaid directive or diagram keyword; otherwise leave as-is (Mermaid will parse flowchart/sequence/etc.)
  fi

  # Export all .mmd files in dest_dir
  local mm
  for mm in "$dest_dir"/*.mmd; do
    [ -e "$mm" ] || continue
    local name
    name=$(basename "$mm" .mmd)

    # Skip empty files (whitespace-only) which can happen when extraction found
    # a mermaid fence with no content or the content isn't actual mermaid.
    if ! grep -q '[^[:space:]]' "$mm"; then
      echo "[mermaid] Skipping empty/whitespace-only file: $mm"
      continue
    fi

    # Skip files that clearly aren't mermaid content: YAML front-matter,
    # HTML wrappers, or other non-mermaid fragments. This prevents
    # mermaid-cli from trying to render files that will always fail with
    # "UnknownDiagramError" or YAML/front-matter parsing errors.
    #
    # Heuristics used:
    #  - Files starting with '---' (YAML front-matter) are skipped.
    #  - Files that contain obvious HTML tags like <details>, <div>, or <script>
    #    are skipped.
    #  - Files that do not contain any mermaid diagram keywords
    #    (graph, flowchart, sequenceDiagram, classDiagram, stateDiagram,
    #     gantt, pie, journey) are skipped.
    firstline=$(head -n 1 "$mm" || true)
    if [[ "$firstline" =~ ^--- ]]; then
      echo "[mermaid] Skipping file with YAML front-matter: $mm"
      continue
    fi
    if grep -qiE '<(details|div|script|pre|code)\b|</' "$mm"; then
      echo "[mermaid] Skipping file containing HTML/markup: $mm"
      continue
    fi
    if ! grep -qE '^[[:space:]]*(graph|flowchart|sequenceDiagram|classDiagram|stateDiagram|gantt|pie|journey)\b' "$mm" && \
       ! grep -qE '^[[:space:]]*%%[[:space:]]*mermaid' "$mm"; then
      echo "[mermaid] Skipping non-mermaid file (no diagram keywords): $mm"
      continue
    fi

    # Prefer SVG output (more portable, doesn't require rasterization).
    # If SVG export fails, warn but continue. PNG export is attempted when
    # possible, but failure to produce PNG will not abort the whole run.
    echo "[mermaid] Rendering $mm -> ${name}.svg"
    if ! $MERMAID_CMD ${MERMAID_OPTS:-} -i "$mm" -o "$dest_dir/${name}.svg"; then
      echo "[mermaid] Warning: SVG export failed for $mm (continuing)."
    fi

    # Attempt PNG export optionally. Setting MERMAID_SKIP_PNG=1 will skip PNG
    # rendering which avoids Puppeteer/Chromium requirements and speeds up runs.
    if [[ -z "${MERMAID_SKIP_PNG:-}" ]]; then
      echo "[mermaid] Attempting PNG export $mm -> ${name}.png"
      if ! $MERMAID_CMD ${MERMAID_OPTS:-} -i "$mm" -o "$dest_dir/${name}.png"; then
        echo "[mermaid] Warning: PNG export failed for $mm. PNG rendering requires a Chromium/Chrome installation available to Puppeteer."
        echo "[mermaid] To enable PNG rendering, either install a system Chromium/Chrome or install Puppeteer's browsers cache with e.g.:"
        echo "  npx -y @puppeteer/browsers install chrome"
        echo "Or install Chrome/Chromium via your package manager (e.g. apt, pacman, yum) and ensure it's on PATH."
      fi
    else
      echo "[mermaid] Skipping PNG export for $mm (MERMAID_SKIP_PNG set)."
    fi
  done
}

for f in "${md_files[@]}"; do
  echo "Processing $f"
  extract_and_export "$f"
done

echo "Done. See $OUT_DIR for outputs."
