g++ -std=c++23 -o ..\bin\app.exe ./main.cpp
rm C:\bin\cdls.exe
mklink /H C:\bin\cdls.exe ..\bin\app.exe

if ($args[0] -eq "run")
{
	. ..\bin\app.exe
}
