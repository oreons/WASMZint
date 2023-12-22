git submodule update --init --recursive
set "CWD=%~dp0"
set "CWD=%CWD:\=/%"

cd %CWD%emsdk
call emsdk.bat install 3.1.9
call emsdk.bat activate 3.1.9
call emsdk_env.bat

REM Build Zlib
cd  %CWD%zlib
mkdir build
cd build
call emcmake cmake ../ ^
     -DCMAKE_BUILD_TYPE=Release
call emmake make -j

REM Build libpng
cd  %CWD%libpng
mkdir build
cd build
call emcmake cmake ../ ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DZLIB_LIBRARY="%CWD%zlib/build/libz.a" ^
     -DZLIB_INCLUDE_DIR="%CWD%zlib/;%CWD%zlib/build"
call emmake make -j

REM Build Zint
cd %CWD%zint
git checkout master
mkdir build
cd build
call emcmake cmake ../ ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DZLIB_LIBRARY="%CWD%zlib/build/libz.a" ^
     -DZLIB_INCLUDE_DIR="%CWD%zlib/;%CWD%zlib/build" ^
     -DPNG_LIBRARY="%CWD%libpng/build/libpng.a" ^
     -DPNG_PNG_INCLUDE_DIR="%CWD%libpng/;%CWD%libpng/build"
call emmake make -j
cd %~dp0
