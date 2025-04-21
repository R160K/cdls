cdls is a directory walker; a simple productivity tool to view and navigate directories quickly.

Still in development. A major part of the program is changing the current working directory, which as a standalone program is pointless in the context of the parent shell. So this is best implemented via wrapper scripts.

This app will list the contents of given directory, with all subdirectories given indices that can be input instead of a name.

If launched with args, will run and exit. If run without args, will begin a loop until "" is passed.

Usage:
	cdls[_ex] [option] [command1] [command2] ... [commandN]
	
Options:
	-h : Displays help.
	-v : Displays the current version.
	-w : Wrapper-mode; presents output in a way that is easier for wrapper scripts to read.

Commands:

1. Entering the index of a directory will cdls to that directory. 0 will resolve to ".". Negative integers will be translated into dot strings and regress backwards up the directory tree.

2. Entering a path (including relative path such as simply the name of a subdirectory in the cwd) will cdls to that path.

3. Dot strings: entering a number of dots more than 1 will move up the directory tree, from the cwd, by the number of dots minus 1. A single dot will cdls to the current working directory.
3.1 A single dot followed by an integer will translate to a dot string of the specified number of dots. A negative integer has an equivalent function.

4. Comma strings: entering a string of commas will move down the directory tree, from root, by the number of commas minus 1.
4.1 As with dot strings, a single comma followed by an integer will be interpreted as a string of the specified number of commas.

5. Enter a partial directory name (case sensitive parse first, then case insensitive) to auto-complete and cdls to that directory.

6. Enter a blank command (or exit) to quit back to parent shell.


ADDENDUM: As one of the major functions of this code is to change the cwd, shell-specific wrappers are an important part of the project.
