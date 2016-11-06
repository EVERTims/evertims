# build libevert and evertims for Linux / MacOS
# ---------------------------------------------

set -e

OUTPUTDIR=linux
BUILDDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

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
BINDIR=bin
cd ../..
if [ ! -d "$BINDIR" ]; then
	mkdir $BINDIR
fi
cd $BINDIR

TARGET=ims
if [ -f "$TARGET" ]; then
	echo 'Removing old ims binary..'
	rm $TARGET
fi
# ln -s ../build/linux/ims .
mv ../build/linux/ims .

echo 'Done'
