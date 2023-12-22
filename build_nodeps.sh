#!/bin/bash

CWD=$(pwd)


# Activate Emscripten
cd $CWD/deps/emsdk
source emsdk_env.sh

# Build bindings
cd $CWD
emcc -o wasmzint.js \
zintbindings.cpp \
$CWD/deps/zlib/build/libz.a \
$CWD/deps/libpng/build/libpng16.a \
$CWD/deps/zint/build/backend/libzint.a \
-l embind \
-s MODULARIZE \
-s'EXPORT_NAME="wasmzint"' \
-I $CWD/deps/zint/backend \
-O3

emcc -o wasmzint_sf.js \
zintbindings.cpp \
$CWD/deps/zlib/build/libz.a \
$CWD/deps/libpng/build/libpng16.a \
$CWD/deps/zint/build/backend/libzint.a \
-l embind \
-s MODULARIZE \
-s'EXPORT_NAME="wasmzint"' \
-sSINGLE_FILE \
-I $CWD/deps/zint/backend \
-O3

emcc -o wasmzint_sf_es6.js \
zintbindings.cpp \
$CWD/deps/zlib/build/libz.a \
$CWD/deps/libpng/build/libpng16.a \
$CWD/deps/zint/build/backend/libzint.a \
-l embind \
-s MODULARIZE \
-s'EXPORT_NAME="wasmzint"' \
-sSINGLE_FILE \
-sEXPORT_ES6=1 \
-I $CWD/deps/zint/backend \
-O3