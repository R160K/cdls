#Python wrapper for cdls

# TODO: A debug option (uses cdls_lib in ../../bin)

# TODO: Add a config file that specifies install location (in same location as this script)

import ctypes
import os


# Set lib_path
lib_path = ""

if os.name == "nt":
    # Running on Windows
    os.add_dll_directory(r"C:\Program Files\cdls")
    lib_path = r"cdls_lib.dll"
else:
    # Assume is running on UNIX
    lib_path = r"cdls_lib.so"


# Import DLL
clib = ctypes.CDLL(lib_path)


#Constants
DEFAULT_COMMAND = ctypes.c_char_p.in_dll(clib, "DEFAULT_COMMAND").value
VERSION = ctypes.c_char_p.in_dll(clib, "VERSION").value

#SEPARATOR = ctypes.c_char.in_dll(clib, "SEPARATOR").value


# Get input and output types set up
clib.LS.argtypes = []
clib.LS.restype = None

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
    return clib.CD(command)

def CDLS(command = DEFAULT_COMMAND):
    command = to_bytes(command)

    return clib.CDLS(command)
