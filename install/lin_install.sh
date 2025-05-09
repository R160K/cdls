#!/bin/bash
# Install
sudo cp ./bin/cdls_lib.so /lib/cdls_lib.so
sudo cp ./bin/cdls.bin /bin/cdls_ex

echo "Binaries built and installed!"

exit # Fix so can install after

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
