# build libevert and evertims for Linux / MacOS
# ---------------------------------------------

set -e

# Function to check and install a package
install_if_not_present() {
    PACKAGE=$1
    dpkg-query -W -f='${Status}' $PACKAGE 2>/dev/null | grep -q "ok installed"
    if [ $? -ne 0 ]; then
        echo "$PACKAGE is not installed. Installing..."
        sudo apt-get update
        sudo apt-get install -y $PACKAGE
    else
        echo "$PACKAGE is already installed."
    fi
}

# Check and install libbsd-dev
install_if_not_present libbsd-dev

# Check and install freeglut3-dev
install_if_not_present freeglut3-dev

BUILDDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# ------------------------------------------------------------
# Build libevert
# ------------------------------------------------------------
$BUILDDIR/../libevert/build/build_linux.sh

# ------------------------------------------------------------
# Build evertims
# ------------------------------------------------------------
$BUILDDIR/../evertims/build/build_linux.sh
