This is a simple productivity tool to speed up directory navigation in the command line.


It is most effective as a native PowerShell script ./scripts/powershell_native.ps1).
Also a C++ program (./bin/app.exe).

The C++ executable is found in ./bin, the PowershellScript is found in ./scripts, the C++ source is found in ./src
Everything else is still in development and is experimental.


The script, when given a directory path (and as the name might suggest), performs cd to the path and then ls (ls -la), adds an index for each directory, then asks for an input command. If no directory is given, defaults to ".".


Commands:

1. Entering the index of a directory will cdls to that directory (recursive). 0 will resolve to ".". Negative integers will be translated into dot strings and regress backwards up the directory tree.

2. Entering a path (including relative path such as simply the name of a subdirectory in the cwd) will cdls to that path.

3. Dot strings: entering a number of dots more than 1 will move up the directory tree, from the cwd, by the number of dots minus 1. A single dot will cdls to the current working directory.
3.1 A single dot followed by an integer will translate to a dot string of the specified number of dots. A negative integer has an equivalent function.

4. Comma strings: entering a string of commas will move down the directory tree, from root, by the number of commas minus 1.
4.1 As with dot strings, a single comma followed by an integer will be interpreted as a string of the specified number of commas.

5. Anything other than an integer, existing path or a comma or dot string will be invoked as a regular command (PowerShell). Will end execution (C++).

6. Enter a blank command (or exit) to quit back to parent shell.