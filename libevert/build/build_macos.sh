# build libevert and evertims for MacOS (Xcode)
# ---------------------------------------------

OUTPUTDIR=macos
BUILDDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# compile both (evert then evertims) with xcode
echo 'Building libevert...'
xcodebuild -project $BUILDDIR/macos/evert.xcodeproj