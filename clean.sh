#!/bin/sh

BUILD_TYPE=Debug
#BUILD_TYPE=Release

cd src
make BUILD_TYPE=$BUILD_TYPE gbc-clean
cd ..
rm -rf obj
