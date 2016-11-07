# build libevert and evertims for Linux / MacOS
# ---------------------------------------------

set -e

OUTPUTDIR=linux
BUILDDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# ------------------------------------------------------------
# Build libevert
# ------------------------------------------------------------

echo 'Building libevert...'
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
make && make install
