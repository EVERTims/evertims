# build libevert and evertims for MacOS (Xcode)
# ---------------------------------------------

OUTPUTDIR=macos
BUILDDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# compile both evertims (Xcode)
echo 'Building EVERTims...'
xcodebuild -project $BUILDDIR/macos/evertims.xcodeproj
