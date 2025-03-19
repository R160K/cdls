#Python wrapper for cdls

import ctypes
import os


lib_path = os.path.join(os.getcwd(), "C:/Program Files/cdls/cdls_lib.dll")

clib = ctypes.CDLL(lib_path)

clib.CDLS.argtypes = [ctypes.c_char_p]
clib.CDLS.restype = ctypes.c_int

