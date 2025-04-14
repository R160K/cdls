#Python wrapper for cdls

# TODO: Make a function to point this at a specific library file
# Right now assumes is being run in own directory

import ctypes
import os

CDLS_LIB_PATH_WIN = "C:/Users/Owner/Documents/Programming/cdls/bin/cdls_lib.dll"
CDLS_LIB_PATH_UNIX = "~/Programming/cdls/bin/cdls_lib.so"

if os.name == "nt":
    # Running on Windows
    lib_path = CDLS_LIB_PATH_WIN
else:
    # Assume is running on UNIX
    lib_path = CDLS_LIB_PATH_UNIX
    
#lib_path = os.path.join(os.getcwd(), ADJUSTMENT)
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
    
    clib.CDLS(command)