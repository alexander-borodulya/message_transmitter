#!/usr/bin/env bash

SOURCES_DIR=`pwd`
BUILD_DIR=$PWD/../build-mt
if [[ ! -d $BUILD_DIR ]]; then
    mkdir $BUILD_DIR
fi
cd $BUILD_DIR
cmake $SOURCES_DIR
cmake --build . --target all
cd $SOURCES_DIR
