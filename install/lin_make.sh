#!/bin/bash

# TODO: Fix PYTHONPATH
# TODO: Work out if /etc/profile is best global option (non-login shells)
# TODO: Updat /etc/bash.bashrc to point to /etc/profile.d/cdls_wrapper.sh

# TODO: Get version
VERSION=$(jq -r ".VERSION" config.json)
VERSION_BUILD="${VERSION//./_}"

#Platform can be overruled using environment variable
if [ $CDLS_BUILD_PLATFORM ]; then
    PLATFORM="$CDLS_BUILD_PLATFORM"
else
    PLATFORM="arm_aarch64"
fi

echo "Building for platform $PLATFORM..."

# Build executable and library
g++ -std=c++23 -o ./bin/cdls.bin -DAPP_VERSION=\"$VERSION\" ./src/main.cpp

g++ -c -fPIC -DBUILDING_SHARED -DAPP_VERSION=\"$VERSION\" ./src/main.cpp -o ./bin/main.o -std=c++20
g++ -shared -o ./bin/cdls_lib.so ./bin/main.o
rm ./bin/main.o

# Copy to shared builds for Unix on appropriate platform
cp ./bin/cdls.bin "./bin/cdls_unix_${PLATFORM}_v${VERSION_BUILD}.bin"
cp ./bin/cdls_lib.so "./bin/cdls_lib_unix_${PLATFORM}_v${VERSION_BUILD}.so"


echo "Binaries built."
exit
