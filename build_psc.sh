#!/bin/bash

export PKG_CONFIG_LIBDIR="$PSL1GHT/ppu/lib/pkgconfig"
export PKG_CONFIG_PATH="$PS3DEV/portlibs/ppu/lib/pkgconfig"
PKG_CONFIG="pkg-config --static"

#make VERBOSE=1 pkg
make pkg
