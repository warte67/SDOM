#!/usr/bin/env bash
set -euo pipefail

# ------------------------------------------------------------
# 1. Resolve root path of the repository no matter where run from
# ------------------------------------------------------------
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

TEMPLATE="$ROOT_DIR/include/SDOM/SDOM_Version.hpp.in"
OUTPUT="$ROOT_DIR/include/SDOM/SDOM_Version.hpp"
PATCH_FILE="$ROOT_DIR/.version_patch"
README="$ROOT_DIR/README.md"

# ------------------------------------------------------------
# 2. Metadata setup
# ------------------------------------------------------------
MAJOR=0
MINOR=5
CODENAME="early pre-alpha"
PATCH=0

# --- Version increment logic ---
if [[ ! -f "$PATCH_FILE" ]]; then
    echo "$MAJOR $MINOR $PATCH" > "$PATCH_FILE"
else
    read -r LAST_MAJOR LAST_MINOR PATCH < "$PATCH_FILE" || PATCH=0
    if [[ "$MAJOR" != "$LAST_MAJOR" ]]; then
        MINOR=0; PATCH=0
    elif [[ "$MINOR" != "$LAST_MINOR" ]]; then
        PATCH=0
    else
        PATCH=$((PATCH + 1))
    fi
    echo "$MAJOR $MINOR $PATCH" > "$PATCH_FILE"
fi

# --- Build metadata ---
DATE=$(date +%Y-%m-%d_%H:%M:%S)
HASH=$(git rev-parse --short HEAD 2>/dev/null || echo "nogit")
BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown")
COMPILER=$(g++ --version 2>/dev/null | head -n 1 | sed 's/^[[:space:]]*//')
PLATFORM=$(uname -s)-$(uname -m)
BUILD="${DATE}_${HASH}"

# ------------------------------------------------------------
# 3. Generate SDOM_Version.hpp
# ------------------------------------------------------------
mkdir -p "$(dirname "$OUTPUT")"
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

echo "✅ Generated SDOM_Version.hpp v$MAJOR.$MINOR.$PATCH"
echo "   Codename : $CODENAME"
echo "   Commit   : $HASH"
echo "   Branch   : $BRANCH"
echo "   Compiler : $COMPILER"
echo "   Platform : $PLATFORM"
echo "   Built    : $DATE"

# ------------------------------------------------------------
# 4. Parse generated header safely
# ------------------------------------------------------------
VERSION_HEADER="$OUTPUT"
if [ -f "$VERSION_HEADER" ]; then
    echo "📄 Reading version header from: $VERSION_HEADER"

    SDOM_VERSION_MAJOR=$(grep -E '^#define[[:space:]]+SDOM_VERSION_MAJOR' "$VERSION_HEADER" | awk '{print $3}')
    SDOM_VERSION_MINOR=$(grep -E '^#define[[:space:]]+SDOM_VERSION_MINOR' "$VERSION_HEADER" | awk '{print $3}')
    SDOM_VERSION_PATCH=$(grep -E '^#define[[:space:]]+SDOM_VERSION_PATCH' "$VERSION_HEADER" | awk '{print $3}')
    SDOM_VERSION_STRING="${SDOM_VERSION_MAJOR}.${SDOM_VERSION_MINOR}.${SDOM_VERSION_PATCH}"

    SDOM_VERSION_CODENAME=$(grep -E '^#define[[:space:]]+SDOM_VERSION_CODENAME' "$VERSION_HEADER" | sed -E 's/.*"([^"]+)".*/\1/')
    SDOM_VERSION_BUILD_DATE=$(grep -E '^#define[[:space:]]+SDOM_VERSION_BUILD_DATE' "$VERSION_HEADER" | sed -E 's/.*"([^"]+)".*/\1/')
    SDOM_BUILD_PLATFORM=$(grep -E '^#define[[:space:]]+SDOM_BUILD_PLATFORM' "$VERSION_HEADER" | sed -E 's/.*"([^"]+)".*/\1/')
    SDOM_BUILD_COMPILER=$(grep -E '^#define[[:space:]]+SDOM_BUILD_COMPILER' "$VERSION_HEADER" | sed -E 's/.*"([^"]+)".*/\1/')
else
    echo "⚠️  Version header not found at: $VERSION_HEADER"
fi


# ------------------------------------------------------------
# 5. Update README.md (only interactively or if forced)
# ------------------------------------------------------------
if [[ "${SDOM_SKIP_README:-0}" -eq 0 ]]; then
    : "${SDOM_VERSION_STRING:=unknown}"
    : "${SDOM_VERSION_CODENAME:=unknown}"
    : "${SDOM_VERSION_BUILD_DATE:=unknown}"
    : "${SDOM_BUILD_PLATFORM:=unknown}"
    : "${SDOM_BUILD_COMPILER:=unknown}"

    VERSION_BLOCK=$(cat <<EOF
<!-- BEGIN_VERSION_BLOCK -->
**SDOM ${SDOM_VERSION_STRING} (${SDOM_VERSION_CODENAME})**  
**Build Date:** ${SDOM_VERSION_BUILD_DATE}  
**Platform:** ${SDOM_BUILD_PLATFORM}  
**Compiler:** ${SDOM_BUILD_COMPILER}
<!-- END_VERSION_BLOCK -->
EOF
)

    if grep -q "<!-- BEGIN_VERSION_BLOCK -->" "$README"; then
        awk -v block="$VERSION_BLOCK" '
            /<!-- BEGIN_VERSION_BLOCK -->/ {print block; skip=1; next}
            /<!-- END_VERSION_BLOCK -->/ {skip=0; next}
            skip==0 {print}
        ' "$README" > "${README}.tmp" && mv "${README}.tmp" "$README"
    else
        echo -e "\n$VERSION_BLOCK" >> "$README"
    fi

    echo "📝 Updated README.md successfully!"
fi
