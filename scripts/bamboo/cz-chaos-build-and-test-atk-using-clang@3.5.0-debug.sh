#!/bin/bash


COMPILER="clang@3.5.0"

if [[ $HOSTNAME == rz* ]]; then
    HC="host-configs/rzmerl-chaos_5_x86_64_ib-${COMPILER}.cmake"
else
    HC="host-configs/surface-chaos_5_x86_64_ib-${COMPILER}.cmake"
fi

BT="Debug"
BP="atk_build"
IP="atk_install"
COMP_OPT=""
BUILD_OPT=""



echo "Configuring..."
echo "-----------------------------------------------------------------------"
./scripts/config-build.py -ecc -hc $HC -bt $BT -bp $BP -ip $IP $COMP_OPT $BUILD_OPT    
if [ $? -ne 0 ]; then
    echo "Error: config-build.py failed"
    exit 1
fi
echo "-----------------------------------------------------------------------"

cd $BP
    echo "Building..."
    echo "-----------------------------------------------------------------------"
    make VERBOSE=1 -j16
    if [ $? -ne 0 ]; then
        echo "Error: 'make' failed"
        exit 1
    fi
    echo "-----------------------------------------------------------------------"

    echo "Running tests..."
    echo "-----------------------------------------------------------------------"
    make test ARGS="-T Test"
    if [ $? -ne 0 ]; then
        echo "Error: 'make test' failed"
        exit 1
    fi
    echo "-----------------------------------------------------------------------"

    echo "Installing files..."
    echo "-----------------------------------------------------------------------"
    make VERBOSE=1 install
    if [ $? -ne 0 ]; then
        echo "Error: 'make install' failed"
        exit 1
    fi
    echo "-----------------------------------------------------------------------"
