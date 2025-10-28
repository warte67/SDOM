#!/usr/bin/env bash
# ==========================================================
#  SDOM version generator
#  Generates include/SDOM/SDOM_Version.hpp from the template.
#
#  - Automatically increments the patch version.
#  - Fills all metadata placeholders:
#    @MAJOR@, @MINOR@, @PATCH@, @CODENAME@,
#    @BUILD@, @DATE@, @HASH@, @BRANCH@,
#    @COMPILER@, @PLATFORM@
# ==========================================================

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEMPLATE="$ROOT_DIR/include/SDOM/SDOM_Version.hpp.in"
OUTPUT="$ROOT_DIR/include/SDOM/SDOM_Version.hpp"
PATCH_FILE="$ROOT_DIR/.version_patch"

# ---- Configurable defaults ----
MAJOR=0
MINOR=1
CODENAME="PreAlpha"

# ---- Load or bump patch ----
if [[ ! -f "$PATCH_FILE" ]]; then
    PATCH=0
else
    PATCH=$(<"$PATCH_FILE")
    PATCH=$((PATCH + 1))
fi
echo "$PATCH" > "$PATCH_FILE"

# ---- Generate build metadata ----
DATE=$(date +%Y-%m-%d_%H:%M:%S)
HASH=$(git rev-parse --short HEAD 2>/dev/null || echo "nogit")
BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown")
COMPILER=$(g++ --version 2>/dev/null | head -n 1 | sed 's/^[[:space:]]*//')
PLATFORM=$(uname -s)-$(uname -m)
BUILD="${DATE}_${HASH}"

# ---- Ensure output directory exists ----
mkdir -p "$(dirname "$OUTPUT")"

# ---- Substitute template variables ----
sed \
  -e "s|@MAJOR@|$MAJOR|g" \
  -e "s|@MINOR@|$MINOR|g" \
  -e "s|@PATCH@|$PATCH|g" \
  -e "s|@CODENAME@|$CODENAME|g" \
  -e "s|@BUILD@|$BUILD|g" \
  -e "s|@DATE@|$DATE|g" \
  -e "s|@HASH@|$HASH|g" \
  -e "s|@BRANCH@|$BRANCH|g" \
  -e "s|@COMPILER@|$COMPILER|g" \
  -e "s|@PLATFORM@|$PLATFORM|g" \
  "$TEMPLATE" > "$OUTPUT"

# ---- Output summary ----
echo "✅ Generated SDOM_Version.hpp v$MAJOR.$MINOR.$PATCH"
echo "   Codename : $CODENAME"
echo "   Commit   : $HASH"
echo "   Branch   : $BRANCH"
echo "   Compiler : $COMPILER"
echo "   Platform : $PLATFORM"
echo "   Built    : $DATE"
