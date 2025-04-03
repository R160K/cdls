#Python wrapper for cdls


import ctypes
import os

ADJUSTMENT = "../../bin/cdls_lib.dll"
lib_path = os.path.join(os.getcwd(), ADJUSTMENT)
# Import DLL
clib = ctypes.CDLL(lib_path)


#Constants
DEFAULT_COMMAND = ctypes.c_char_p.in_dll(clib, "DEFAULT_COMMAND").value

# Get input and output types set up
clib.LS.argtypes = [ctypes.c_bool]

clib.CD.argtypes = [ctypes.c_char_p]
clib.CD.restype = ctypes.c_int

clib.CDLS.argtypes = [ctypes.c_char_p, ctypes.c_bool]
clib.CDLS.restype = ctypes.c_int


# Make sure an input is bytes
def to_bytes(data):
    if isinstance(data, str):
        return data.encode('utf-8')
    elif isinstance(data, bytes):
        return data
    else:
        raise TypeError("Input must be a string or bytes.")
    
    
def LS(verbose = True):
    clib.LS(verbose)

def CD(command = DEFAULT_COMMAND):
    command = to_bytes(command)
    clib.CD(command)

def CDLS(command = DEFAULT_COMMAND, verbose = True):
    if command == None:
        command = clib.DEFAULT_COMMAND
    else:
        command = to_bytes(command)
    
    clib.CDLS(command, verbose)

def flush():
    clib.flush()