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
  $MERMAID_CMD ${MERMAID_OPTS:-} -i "$mm" -o "$dest_dir/${name}.png" || { echo "Failed PNG export for $mm"; exit 1; }
  $MERMAID_CMD ${MERMAID_OPTS:-} -i "$mm" -o "$dest_dir/${name}.svg" || { echo "Failed SVG export for $mm"; exit 1; }
  done
}

for f in "${md_files[@]}"; do
  echo "Processing $f"
  extract_and_export "$f"
done

echo "Done. See $OUT_DIR for outputs."
