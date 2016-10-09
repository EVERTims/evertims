# build libevert and evertims for Linux / MacOS
# ---------------------------------------------

OUTPUTDIR=linux
BUILDDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd BUILDDIR

# ------------------------------------------------------------
# Build libevert
# ------------------------------------------------------------

echo 'Building libevert...'
cd ../lib/libevert/build

# rm old / create build directory
if [ -d "$OUTPUTDIR" ]; then
	echo 'Removing old build directories...'
	rm -Rf $OUTPUTDIR
fi
mkdir $OUTPUTDIR

# build
cd $OUTPUTDIR
cmake ../../
make && make install

# ------------------------------------------------------------
# Build EVERTims
# ------------------------------------------------------------

echo 'Building EVERTims...'
cd $BUILDDIR

# rm old / create build directory
if [ -d "$OUTPUTDIR" ]; then
	echo 'Removing old build directories...'
	rm -Rf $OUTPUTDIR
fi
mkdir $OUTPUTDIR

# build
cd $OUTPUTDIR
cmake ../../
make

# create symbolic link of binary to parent folder
cd ../../bin
TARGET=ims
if [ -f "$TARGET" ]; then
	echo 'Removing old ims binary..'
	rm $TARGET
fi
ln -s ../build/linux/ims .

echo 'Done'