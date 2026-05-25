#!/bin/bash

cat > include/generated/__BLD.h <<EOF
#ifndef BUILD_NUMBER
#define BUILD_NUMBER $(cat .version)
#endif
EOF
if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
	git_hash="$(git rev-parse HEAD)"
else
	git_hash="<none>"
fi
cat >> include/generated/__BLD.h <<EOF
#ifndef GIT_SOURCE_HASH
#define GIT_SOURCE_HASH "$git_hash"
#endif
EOF
