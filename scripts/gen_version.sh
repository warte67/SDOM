#!/usr/bin/env bash
# ==========================================================
#  SDOM version generator
#  Generates include/SDOM/SDOM_Version.hpp from the template.
# ==========================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEMPLATE="$ROOT_DIR/include/SDOM/SDOM_Version.hpp.in"
OUTPUT="$ROOT_DIR/include/SDOM/SDOM_Version.hpp"
PATCH_FILE="$ROOT_DIR/.version_patch"

# ---- Configurable defaults ----
MAJOR=0
MINOR=1
CODENAME="pre-alpha"

# ---- Load or bump patch ----
if [[ ! -f "$PATCH_FILE" ]]; then
    PATCH=0
else
    PATCH=$(<"$PATCH_FILE")
    PATCH=$((PATCH + 1))
fi
echo "$PATCH" > "$PATCH_FILE"

# ---- Generate build metadata ----
BUILD_DATE=$(date +%Y-%m-%d_%H:%M:%S)
GIT_HASH=$(git rev-parse --short HEAD 2>/dev/null || echo "nogit")

# ---- Substitute template variables ----
mkdir -p "$(dirname "$OUTPUT")"

sed \
  -e "s/@MAJOR@/$MAJOR/" \
  -e "s/@MINOR@/$MINOR/" \
  -e "s/@PATCH@/$PATCH/" \
  -e "s/@CODENAME@/$CODENAME/" \
  -e "s/@BUILD@/${BUILD_DATE}_${GIT_HASH}/" \
  "$TEMPLATE" > "$OUTPUT"

echo "✅ Generated SDOM_Version.hpp v$MAJOR.$MINOR.$PATCH ($CODENAME, $BUILD_DATE, $GIT_HASH)"


