#!/bin/bash
# Record the path of the release5.0_psram_octal_120m.patch file
PATCH_PATH="$(pwd)/release5.0_psram_octal_120m.patch"

# Check if the IDF_PATH environment variable is set
if [[ -z "$IDF_PATH" ]]; then
  echo "Error: You must run this script within the ESP-IDF virtual environment." >&2
  exit 1
fi

# Change to the IDF_PATH directory
cd "$IDF_PATH" || exit 1

# Check if the Git repository's release version is 5.0 or 5.0.1
GIT_VERSION=$(git describe --tags)
if [[ "$GIT_VERSION" != "v5.0" && "$GIT_VERSION" != "v5.0.1" ]]; then
  echo "Error: This script and patch have only been tested with ESP-IDF release version 5.0." >&2
  exit 1
fi

# Apply patch
if git apply --whitespace=fix "$PATCH_PATH"; then
  echo "Patch applied successfully"
else
  echo "Error: Failed to apply the patch $PATCH_PATH." >&2
  exit 1
fi
# Check if the patch is applied correctly
git status;
