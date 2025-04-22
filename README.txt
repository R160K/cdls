cdls is a directory walker; a simple productivity tool to view and navigate directories quickly.

A major part of the program is changing the current working directory, which as a standalone program is pointless in the context of the parent shell. So this is best implemented via wrapper scripts.

This app will list the contents of given directory, with all subdirectories given indices that can be input instead of a name.

If launched with args, will run and exit. If run without args, will begin a loop until "" is passed.

Usage:
	cdls[_ex] [option] [command1] [command2] ... [commandN]
	
Options:
	-h, --help : Displays help.
	-v, --version : Displays the current version.
	-w : Wrapper-mode; presents output in a way that is easier for wrapper scripts to read. (now default)
	-c : Console-mode; looks better running as a standalone program.

Commands:

1. Entering the index of a directory will cdls to that directory. 0 will resolve to ".". Negative integers will be translated into dot strings and regress backwards up the directory tree.

2. Entering a path (including relative path such as simply the name of a subdirectory in the cwd) will cdls to that path.

3. Dot strings: entering a number of dots more than 1 will move up the directory tree, from the cwd, by the number of dots minus 1. A single dot will cdls to the current working directory.
3.1 A single dot followed by an integer will translate to a dot string of the specified number of dots. A negative integer has an equivalent function.

4. Comma strings: entering a string of commas will move down the directory tree, from root, by the number of commas minus 1.
4.1 As with dot strings, a single comma followed by an integer will be interpreted as a string of the specified number of commas.

5. Enter a partial directory name (case sensitive parse first, then case insensitive) to auto-complete and cdls to that directory.

6. Enter a blank command (or exit) to quit back to parent shell.


---------------------------------------------------

BUILDING: Section to be updated. Ad-hoc build scripts should be replaced with something like CMake.

---------------------------------------------------


INSTALLING:

As one of the major functions of this code is to change the cwd, shell-specific wrappers are an important part of the project. There are currently wrappers for PowerShell on Windows and bash on Unix. There is also a Python module to access cdls functions via a dynamic library.

To install:
(Assuming binaries are already built)

cdls_ex(.exe)
1. Place executable (cdls.exe/cdls.bin) somewhere; most wrappers expect it to be called "cdls_ex.exe" (Windows) or "cdls_ex" (Unix) and be available in the PATH. On Linux it is recommended to place it in one of the binary folders (/bin, /usr/bin etc.) On Windows it is recommended to create a folder "C:\Program Files\cdls" and add it to PATH.

wrapper.sh - Linux
2. On Linux: place the wrapper.sh file somewhere. You could place it (if available) in /etc/profile.d to have it loaded automatically when the shell starts; or place it somewhere like /lib/cdls/wrappers/sh/cdls.sh and load it in e.g. ~/.bashrc or the global bash init script: ". /lib/cdls/wrappers/sh/cdls.sh"
2.1 wrapper.sh looks for the executable in three places: firstly, if a path to the file is passed as a first commandline argument, that takes precedence. Secondly, if the $CDLS_EXE_PATH environment variable is set, it uses that. If neither of those are provided, it uses normal resolution (local dir followed by PATH).

console.ps1 - PowerShell
3. On Windows: place the console.ps1 file somewhere; recommended is "C:\Program Files\cdls\wrappers\PowerShell\cdls.ps1". Then add it to an appropriate profile file, $profile for local user, $PROFILE.AllUsersAllHosts for global scope. ". [PATH].ps1"

cdls_lib.dll/.so
4. If utilising the library (e.g. for the Python module), it is recommended to place it in /lib or /usr/lib (Linux) or "C:\Program Files\cdls" (Windows), and have it available on PATH.

cdls.py
5. To install the Python module and have it available, it is recommended to place it somewhere like "C:\Program Files\cdls\wrappers\Python\cdls.py" (Windows) or /(usr/)lib/cdls/wrappers/python/cdls.py (Linux), and permanently add it to the local or system $PYTHONPATH environment variable.