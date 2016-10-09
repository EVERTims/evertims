# build libevert and evertims for MacOS (Xcode)
# ---------------------------------------------

# compile both (evert then evertims) with xcode
echo 'Building libevert...'
xcodebuild -project ../lib/libevert/build/macos/evert.xcodeproj

echo 'Building EVERTims...'
xcodebuild -project macos/evertims.xcodeproj
