g++ -std=c++23 -o ..\bin\cdls.exe ./main.cpp
g++ -shared -static -o ..\bin\cdls_lib.dll ./main.cpp
rm C:\bin\cdls.exe
mklink /H C:\bin\cdls.exe ..\bin\cdls.exe

if ($args[0] -eq "run")
{
	. ..\bin\app.exe
}