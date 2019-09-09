#!/usr/bin/env bash

SOURCES_DIR=`pwd`
BUILD_DIR=$PWD/../build-mt
if [[ -d $BUILD_DIR ]]; then
    rm -fr $BUILD_DIR
fi
