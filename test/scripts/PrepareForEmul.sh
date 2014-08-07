export TCS_CFG="release"
#export TCS_CFG="DEBUG"
export PORT=x86
export SDK_HOME=$HOME/tizen-sdk
export SYSROOT="$SDK_HOME/platforms/tizen2.2/rootstraps/tizen-emulator-2.2.native"
export CFLAGS="-I$SYSROOT/usr/include"
export LD_FLAGS="--sysroot=$SYSROOT -L$SYSROOT/lib -L$SYSROOT/usr/lib -lc-2.13 -lpthread-2.13 -lc_nonshared"
export TCS_CC="$SDK_HOME/tools/i386-linux-gnueabi-gcc-4.5/bin/i386-linux-gnueabi-gcc"
export TCS_LD="$SDK_HOME/tools/i386-linux-gnueabi-gcc-4.5/bin/i386-linux-gnueabi-gcc"
export TCS_AR="$SDK_HOME/tools/i386-linux-gnueabi-gcc-4.5/bin/i386-linux-gnueabi-ar"
export TCS_STRIP="$SDK_HOME/tools/i386-linux-gnueabi-gcc-4.5/bin/i386-linux-gnueabi-strip"

# Paths relative to test/scripts folder. Change or create new macro for other folders.
export PKG_CONFIG_PATH="$SYSROOT/usr/lib/pkgconfig/:$SYSROOT/usr/lib"
