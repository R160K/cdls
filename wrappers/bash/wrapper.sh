#!/bin/sh

EXE_PATH=cdls_ex
#EXE_PATH = /bin/cdls_ex

# Check if command-line arg has been passed with new EXE_PATH
if [ $# -gt 0 ]; then
    EXE_PATH=$1
# Else check if the $CDLS_EXEC_PATH environment variable has been set
elif [ $CDLS_EXE_PATH ]; then
    EXE_PATH=$CDLS_EXE_PATH
fi

echo $EXE_PATH

cdls() {
	cd $($EXE_PATH -w "$@" | tee /dev/tty | tail -n 1)
}

export -f cdls
