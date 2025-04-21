param (
	[string]$ScriptPath = (Get-Location).Path
	)

$ScriptName = $ScriptPath + "\install.ps1"

# Check if running as admin
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    # Relaunch as admin
    Start-Process powershell -Verb RunAs -ArgumentList "-NoProfile -ExecutionPolicy Bypass -File `"$ScriptName`" -ScriptPath `"$ScriptPath`"" -WindowStyle "Hidden"
    
	Write-Host "install must be run by admin. Restarting..."
	exit
}

# Your script code here
Write-Host "Running as Administrator"



#Install
cd $ScriptPath

rmdir -r "C:\Program Files\cdls"
mkdir -f "C:\Program Files\cdls"

cp $ScriptPath\bin\cdls.exe "C:\Program Files\cdls\cdls_ex.exe"
cp  $ScriptPath\bin\cdls_lib.dll "C:\Program Files\cdls"

mkdir -f "C:\Program Files\cdls\wrappers"
cp $ScriptPath\wrappers\Python\cdls.py "C:\Program Files\cdls\wrappers"
cp $ScriptPath\wrappers\PowerShell\console.ps1 "C:\Program Files\cdls\wrappers\cdls_ps_console.ps1"




# Add main directory to PATH

#Get original PATH and split into array
$FullPath = [System.Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::Machine)
$PathElements = $FullPath -split ";"

# Check for C:\Program Files\cdls in system PATH
if (-not ($PathElements -contains "C:\Program Files\cdls")) {
	$PathElements += "C:\Program Files\cdls"
	Write-Host "cdls to be added to system path."
}

# Check for C:\Program Files\cdls\wrappers in system PATH
# if (-not ($PathElements -contains "C:\Program Files\cdls\wrappers")) {
   ##$NewPath += ";C:\Program Files\cdls\wrappers"
   # $PathElements += "C:\Program Files\cdls\wrappers"
   # Write-Host "wrappers to be added to system path."
# }

# Join up array and replace environment variable
$NewPath = $PathElements -join ";"
[System.Environment]::SetEnvironmentVariable("PATH", $NewPath, [System.EnvironmentVariableTarget]::Machine)



# Add python wrapper to system PYTHONPATH
$FullPath = [System.Environment]::GetEnvironmentVariable("PYTHONPATH", [System.EnvironmentVariableTarget]::Machine)
$PathElements = $FullPath -split ";"

# Check for C:\Program Files\cdls in system PYTHONPATH
if (-not ($PathElements -contains "C:\Program Files\cdls\wrappers")) {
   #$NewPath += ";C:\Program Files\cdls"
   $PathElements += "C:\Program Files\cdls\wrappers"
   Write-Host "wrappers to be added to system python path."
}

# Join up array and replace environment variable
$NewPath = $PathElements -join ";"
[System.Environment]::SetEnvironmentVariable("PYTHONPATH", $NewPath, [System.EnvironmentVariableTarget]::Machine)



# Add powershell function to global profile
$filePath = $PROFILE.AllUsersAllHosts
$lineToAdd = ". cdls_ps_console.ps1"

# Ensure the directory exists
$dir = Split-Path $filePath
if (-not (Test-Path $dir)) {
    New-Item -ItemType Directory -Path $dir -Force | Out-Null
}

# If the file doesn't exist, create it and write the line
if (-not (Test-Path $filePath)) {
    Set-Content -Path $filePath -Value $lineToAdd
    Write-Host "Profile created and line added."
}
else {
    # Check if the line already exists
    $content = Get-Content -Path $filePath
    if ($content -notcontains $lineToAdd) {
        Add-Content -Path $filePath -Value $lineToAdd
        Write-Host "Line added to existing profile."
    } else {
        Write-Host "Line already exists in the profile."
    }
}