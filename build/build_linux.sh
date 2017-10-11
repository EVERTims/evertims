# build libevert and evertims for Linux / MacOS
# ---------------------------------------------

set -e

BUILDDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# install deps
sudo apt-get install git
sudo apt-get install cmake
# openGL
sudo apt-get install freeglut3-dev
# bsd
sudo apt-get install libbsd-dev

# ------------------------------------------------------------
# Build libevert
# ------------------------------------------------------------
$BUILDDIR/../libevert/build/build_linux.sh

# ------------------------------------------------------------
# Build evertims
# ------------------------------------------------------------
$BUILDDIR/../evertims/build/build_linux.sh
