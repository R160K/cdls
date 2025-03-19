#!/bin/python

import ctypes
import os

LIB_PATH = os.path.join(os.getcwd(), "test.dll")

cdls_lib = ctypes.CDLL(LIB_PATH)