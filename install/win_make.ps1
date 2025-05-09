#Build on Windows

#TODO: Consider making this script official (or migrating it to CMake)

#Check if g++ is installed
if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
	Write-Host "g++ is not installed. Please install it first."
	exit
}

#TODO: Allow alternative build tools (cl)
#TODO: Check all files exist before continuing

$config = Get-Content "config.json" | ConvertFrom-Json
$VERSION = $config.VERSION

#Build the exe and dll
$V_FLAG = "-DAPP_VERSION=\`"$VERSION\`"" #Adds VERSION as string to APP_VERSION
g++ -std=c++23 -o .\bin\cdls.exe $V_FLAG .\src\main.cpp
g++ -std=c++20 -shared -static -o .\bin\cdls_lib.dll $V_FLAG .\src\main.cpp

#Copy to the shared builds for Windows x64
$VERSION_BUILD = $VERSION -replace '\.', '_'
cp .\bin\cdls.exe ".\bin\cdls_win_x86_64_v$($VERSION_BUILD).exe"
cp .\bin\cdls.exe ".\bin\cdls_lib_win_x86_64_v$($VERSION_BUILD).dll"


#Ask about install
$response = Read-Host "Would you like to install? (y/N)"

if ($response -eq "Y" -or $response -eq "y") {
	./install.ps1
}

#Ask about commit and push
$response = Read-Host "Would you like to commit? (y/N)"

if ($response -eq "Y" -or $response -eq "y") {
	$response = Read-Host "Commit message:"

	#Check if git is installed
	#TODO

	git add .
	git commit -m $response

	$response = Read-Host "Would you like to push? (Y/n)"
	if (-not ($response -eq "N" -or $response -eq "n")) {
		git push
	}
}


#TODO: Add options to update version number
#Perhaps at start