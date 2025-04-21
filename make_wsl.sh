#!/bin/bash

# TODO: Fix PYTHONPATH
# TODO: Work out if /etc/profile is best global option (non-login shells)
# TODO: Updat /etc/bash.bashrc to point to /etc/profile.d/cdls_wrapper.sh

# TODO: Get version
VERSION=$(jq -r ".VERSION" config.json)
VERSION_BUILD="${VERSION//./_}"

# Build executable and library
g++ -std=c++23 -o ./bin/cdls.bin -DAPP_VERSION=\"$VERSION\" ./src/main.cpp

g++ -c -fPIC -DBUILDING_SHARED -DAPP_VERSION=\"$VERSION\" ./src/main.cpp -o ./bin/main.o -std=c++20
g++ -shared -o ./bin/cdls_lib.so ./bin/main.o
rm ./bin/main.o

# Copy to shared builds for Unix on x86
cp ./bin/cdls.bin "./bin/cdls_unix_x86_64_v$VERSION.bin"
cp ./bin/cdls_lib.so "./bin/cdls__lib_unix_x86_64_v$VERSION.so"


# Install
sudo cp ./bin/cdls_lib.so /lib/cdls_lib.so
sudo cp ./bin/cdls.bin /bin/cdls_ex

echo "Binaries built and installed!"

# Create directory for python wrapper module
sudo mkdir /lib/cdls
sudo mkdir /lib/cdls/wrappers
sudo mkdir /lib/cdls/wrappers/python

# Copy python wrapper module
sudo cp ./wrappers/Python/cdls.py /lib/cdls/wrappers/python/cdls.py
echo "Attempting to amend PYTHONPATH"

# Amend python path
TARGET="/lib/cdls/wrappers/python/"
IFS=':' read -ra PYTHONPATH_ARR <<< "$PYTHONPATH"

found=false
for path in "${PYTHONPATH_ARR[@]}"; do
  if [[ "$path" == "$TARGET" ]]; then
    found=true
    break
  fi
done

if ! $found; then
  export PYTHONPATH="${PYTHONPATH:+$PYTHONPATH:}$TARGET"
fi

echo "PYTHONPATH=$PYTHONPATH"


# Add bash wrapper
sudo cp ./wrappers/bash/wrapper.sh /etc/profile.d/cdls_wrapper.sh