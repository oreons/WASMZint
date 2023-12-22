set "CWD=%~dp0"
set "CWD=%CWD:\=/%"

REM Activate Emscripten
cd %CWD%deps/emsdk
call emsdk_env.bat

REM Build bindings
set "CWD=%~dp0"
set "CWD=%CWD:\=/%"
cd %CWD%
call emcc -o wasmzint.js ^
     zintbindings.cpp ^
     -l embind ^
     %CWD%deps/zlib/build/libz.a ^
     %CWD%deps/libpng/build/libpng16.a ^
     %CWD%deps/zint/build/backend/libzint.a  ^
     -sALLOW_MEMORY_GROWTH=1 ^
     -sMODULARIZE ^
     -sEXPORT_NAME="wasmzint" ^
     -I %CWD%deps/zint/backend ^
     -O3
   
call emcc -o wasmzint_sf.js ^
     zintbindings.cpp ^
     -l embind ^
     %CWD%deps/zlib/build/libz.a ^
     %CWD%deps/libpng/build/libpng16.a ^
     %CWD%deps/zint/build/backend/libzint.a  ^
     -sALLOW_MEMORY_GROWTH=1 ^
     -sMODULARIZE ^
     -sEXPORT_NAME="wasmzint" ^
     -sSINGLE_FILE ^
     -I %CWD%deps/zint/backend ^
     -O3

call emcc -o wasmzint_sf_es6.js ^
     zintbindings.cpp ^
     -l embind ^
     %CWD%deps/zlib/build/libz.a ^
     %CWD%deps/libpng/build/libpng16.a ^
     %CWD%deps/zint/build/backend/libzint.a  ^
     -sALLOW_MEMORY_GROWTH=1 ^
     -sMODULARIZE ^
     -sEXPORT_NAME="wasmzint" ^
     -sSINGLE_FILE ^
     -sEXPORT_ES6=1 ^
     -I %CWD%deps/zint/backend ^
     -O3
