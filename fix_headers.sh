#!/bin/bash
# Script to update all header files to use Qt stubs when Qt is not available

cd /workspaces/x16BDstacks/src

# List of all header files to update
HEADERS=$(find . -name "*.h" -type f)

for file in $HEADERS; do
    # Skip if already has USE_QT_STUBS
    if grep -q "USE_QT_STUBS" "$file"; then
        continue
    fi
    
    # Check if file includes Qt headers
    if grep -q "#include <Q" "$file"; then
        # Create temp file with replacements
        sed -i '1i\
#ifdef USE_QT_STUBS\
#include "qt_stubs.h"\
#else
' "$file"
        
        # Close the ifdef after Qt includes
        sed -i '/#include <Q/a\
#endif' "$file" 2>/dev/null || true
    fi
done

echo "Headers updated!"
