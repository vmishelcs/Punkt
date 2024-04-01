#!/usr/bin/env bash

SCRIPT_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )";

PUNKT_COMPILER_EXE_PATH="${SCRIPT_DIR}/build/applications/punkt_compiler";

echo "creating an alias 'punktc' for ${PUNKT_COMPILER_EXE_PATH}."
echo "run this once."

echo "alias punktc=${PUNKT_COMPILER_EXE_PATH}" >> ~/.bash_aliases;
