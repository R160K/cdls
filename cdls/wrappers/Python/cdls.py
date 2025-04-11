#Python wrapper for cdls

# TODO: Add dict of constants

import ctypes
import os

# TODO: amend this to point to correct location (perhaps change during build/install process
ADJUSTMENT = "../../bin/cdls_lib.dll"
lib_path = os.path.join(os.getcwd(), ADJUSTMENT)
# Import DLL
clib = ctypes.CDLL(lib_path)


#Constants
DEFAULT_COMMAND = ctypes.c_char_p.in_dll(clib, "DEFAULT_COMMAND").value

# Get input and output types set up
clib.LS.argtypes = []
clib.LS.restype = ctypes.c_int

clib.CD.argtypes = [ctypes.c_char_p]
clib.CD.restype = ctypes.c_int

clib.CDLS.argtypes = [ctypes.c_char_p]
clib.CDLS.restype = ctypes.c_int


# Make sure an input is bytes
def to_bytes(data):
    if isinstance(data, str):
        return data.encode('utf-8')
    elif isinstance(data, bytes):
        return data
    else:
        raise TypeError("Input must be a string or bytes.")
    
    
def LS():
    clib.LS()

def CD(command = DEFAULT_COMMAND):
    command = to_bytes(command)
    clib.CD(command)

def CDLS(command = DEFAULT_COMMAND):
    command = to_bytes(command)
    
    clib.CDLS(command, verbose)