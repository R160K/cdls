#!/bin/sh

EXE_PATH=cdls_ex
#EXE_PATH = /bin/cdls_ex

cdls() {
	cd $(cdls_ex -w "$@" | tee /dev/tty | tail -n 1)
}

export -f cdls