export PROJECTS="libk kernel libc libm libpthread coresrv"

export PATH="$HOME/opt/cross/bin:$PATH"

export MAKE=${MAKE:-make}
export HOST=${HOST:-$(./default-host.sh)}

export AR=${HOST}-ar
export AS=${HOST}-as
export CC=${HOST}-gcc
export SYSROOT=`$CC -print-sysroot`

export PREFIX=/usr
export EXEC_PREFIX=$PREFIX
export BOOTDIR=/boot
export LIBDIR=$EXEC_PREFIX/lib
export INCLUDEDIR=$PREFIX/include

export CFLAGS='-O0 -g'
export CPPFLAGS=''
