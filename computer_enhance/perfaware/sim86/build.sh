#!/bin/bash

# This is for use in the computer_enhance/perfaware/sim86 directory
# to build the shared library and copy it to the shared directory on macOS.

# Stop script on any error
set -e

# Create build directory if it doesn't exist
[ ! -d build ] && mkdir build
pushd build

# Compile with clang -std=c++11 for debug
clang -std=c++11 -g ../sim86.cpp -o sim86_clang_debug.exe

# Compile with clang -std=c++11 for release
clang -std=c++11 -O3 -g ../sim86.cpp -o sim86_clang_release.exe

# Preprocess and format headers
clang -P -E ../sim86_lib.h | clang-format --style="Microsoft" > ../shared/sim86_shared.h
clang -P -E ../sim86_instruction_table_standalone.h | clang-format --style="Microsoft" > sim86_instruction_table_standalone.h

# Compile shared library for debug
clang -std=c++11 -g -fPIC -shared ../sim86_lib.cpp -o libsim86_shared_debug.dylib -Wl,-install_name,libsim86_shared_debug.dylib
# Export functions if needed, e.g., -Wl,-exported_symbol,_Sim86_Decode8086Instruction

# Compile shared library for release
clang -std=c++11 -O3 -g -fPIC -shared ../sim86_lib.cpp -o libsim86_shared_release.dylib -Wl,-install_name,libsim86_shared_release.dylib
# Export functions if needed

# Copy shared library files to the shared directory
cp libsim86_shared*.dylib ../shared

# Compile shared library test
# clang -std=c++11 -g ../shared/shared_library_test.cpp -o shared_library_test.exe

popd
