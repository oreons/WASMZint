#!/bin/bash

CWD=$(pwd)

# Build dependencies
cd deps
./build_dependencies.sh

# Build bindings
cd $CWD
./build_nodeps.sh
