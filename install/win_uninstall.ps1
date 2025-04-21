# TODO: Removal from PYTHONPATH not working properly

param (
	[string]$ScriptPath = (Get-Location).Path
	)

$ScriptName = $ScriptPath + "\uninstall.ps1"

# Check if running as admin
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    # Relaunch as admin
    Start-Process powershell -Verb RunAs -ArgumentList "-NoProfile -ExecutionPolicy Bypass -File `"$ScriptName`" -ScriptPath `"$ScriptPath`"" -WindowStyle "Hidden"
    
	Write-Host "uninstall must be run by admin. Restarting..."
	exit
}

# Your script code here
Write-Host "Running as Administrator"



#Uninstall
cd $ScriptPath

rmdir -r "C:\Program Files\cdls"


# Remove main directory from PATH

#Get original PATH and split into array
$FullPath = [System.Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::Machine)
$PathElements = $FullPath -split ";"

# Check for C:\Program Files\cdls in system PATH
if ($PathElements -contains "C:\Program Files\cdls") {
	$PathElements = $PathElements | Where-Object { $_ -ne "C:\Program Files\cdls" }
	Write-Host "cdls to be removed from system path."
}


# Join up array and replace environment variable
$NewPath = $PathElements -join ";"
[System.Environment]::SetEnvironmentVariable("PATH", $NewPath, [System.EnvironmentVariableTarget]::Machine)



# Remove python wrapper from system PYTHONPATH
$FullPath = [System.Environment]::GetEnvironmentVariable("PYTHONPATH", [System.EnvironmentVariableTarget]::Machine)
$PathElements = $FullPath -split ";"

# Check for C:\Program Files\cdls in system PYTHONPATH
if ($PathElements -contains "C:\Program Files\cdls\wrappers") {
   $PathElements = $PathElements | Where-Object { $_ -ne "C:\Program Files\cdls\wrappers" }
   Write-Host "wrappers to be removed from system python path."
}

# Join up array and replace environment variable
$NewPath = $PathElements -join ";"
[System.Environment]::SetEnvironmentVariable("PYTHONPATH", $NewPath, [System.EnvironmentVariableTarget]::Machine)


if (Get-Command MyFunction -CommandType Function -ErrorAction SilentlyContinue) {
    Remove-Item function:MyFunction
}


$filePath = $PROFILE.AllUsersAllHosts
$lineToRemove = ". cdls_ps_console.ps1"

if (Test-Path $filePath) {
    $content = Get-Content $filePath
    if ($content -contains $lineToRemove) {
        $newContent = $content | Where-Object { $_ -ne $lineToRemove }
        Set-Content -Path $filePath -Value $newContent
        Write-Host "Line removed."
    } else {
        Write-Host "Line not found in the file."
    }
} else {
    Write-Host "File does not exist."
}