export TCS_CFG=release
export PORT=arm
export SDK_HOME=$HOME/tizen-sdk
SYSROOT="$SDK_HOME/tools/smart-build-interface/../../platforms/tizen2.2/rootstraps/tizen-device-2.2.native"
export CFLAGS="-I$SYSROOT/usr/include"
export LD_FLAGS="--sysroot $SYSROOT -B $SYSROOT/usr/lib -L$SYSROOT/lib -L$SYSROOT/usr/lib -lc-2.13 -lpthread-2.13 -lc_nonshared"
export TCS_CC="$SDK_HOME/tools/arm-linux-gnueabi-gcc-4.5/bin/arm-linux-gnueabi-gcc"
export TCS_LD="$SDK_HOME/tools/arm-linux-gnueabi-gcc-4.5/bin/arm-linux-gnueabi-gcc"
export TCS_AR="$SDK_HOME/tools/arm-linux-gnueabi-gcc-4.5/bin/arm-linux-gnueabi-ar"

export TCS_STRIP="$SDK_HOME/tools/arm-linux-gnueabi-gcc-4.5/bin/arm-linux-gnueabi-strip"

echo "TCS_CC in Prepare: " $TCS_CC

# Paths relative to test/scripts folder. Change or create new macro for other folders.
export PKG_CONFIG_PATH="$SYSROOT/usr/lib/pkgconfig/:$SYSROOT/usr/lib"