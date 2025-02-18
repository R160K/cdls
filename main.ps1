#!/bin/pwsh

#Productivity script to cd to a folder, list its contents and allow the user to enter other directories recursively

#TODO: ADD LINE ENDING TO EXIT AFTER COMMAND (e.g. /)
#TODO: ADD PARENT SCRIPT HISTORY

#TODO: Make sure it works properly on linux with ls -al and zero/negative indices for . and ..
#TODO: doesn't work on PowerShell on Linux (which doesn't really matter anyway)

#Accept a path parameter
param (
	[string]$Path
)

try {
	if ($Path) {
		cd $Path
	}
} catch {
	#TODO: Give more detail on precise error
	Write-Error "Invalid path: $Path"
	exit 1
}


#CONSTANTS
$COMMA_STRING_REGEX = "^\,(\-?[0-9]*|\,*)$"
$DOT_STRING_REGEX = "^\.(\-?[0-9]*|\.*)$"

#Get system specific separator ("/" or "\")
$separator = [System.IO.Path]::DirectorySeparatorChar


#Check if input is valid directory
function ChkDir {
	param (
		[string]$testDir
	)
	
	try {
		Set-Location -Path $testDir -ErrorAction Stop
		return $true
	} catch {
		return $false
	}
}


#Convert a dot string to a dot path
function Dot-Path {
	param ( [string]$dotString )
	
	#Check if an integer expression has been input and create dot string
	$int_check = $dotString -replace ".", ""
	
	if ($int_check) {
		$number = [int]$int_check + 1
		$dotString = -join ("." * $number)
	}
	
	#Convert dot string to path
	if ($dotString.Length -lt 2) { #Is valid Windows path as is
		return $dotString
	} else {
		#Get number of directory layers to generate
		$layers = $dotString.Length - 1
		
		$returnPath = (@("..") * $layers) -join $separator
		
		return $returnPath
	}
}

#Convert a comma string to a path
#Each comma in a comma string represents a subdirectory of the current path, beginning at the root
function Comma-Path {
    param ( [string]$comma_string,
            [string]$path )
	
	
	#Count the commas (assume only commas entered)
	$number = $comma_string.Length
	
	
	#Check if an integer expression has been input, in which case that should be the count
	$int_check = $comma_string -replace ",", ""
	
	if ($int_check) {
		$number = [int]$int_check
	}
	
	
    #Get bits of path by splitting across slashes
    $bits = $path -split "\\|/"
	

    #Check the input number and verify that it's in range
    if ($number -lt 1 -or $number -gt $bits.Length)
    {
        #Write error and return original path
        Write-Error "Index $number out of range."
        return $path
    }

    #Extract required subset and add blank final element to ensure functionality of join
    $parts = $bits[0..($number - 1)] + ""

    #Combine the bits of path using the correct separator up to the specified subdirectory
    $soughtPath = $parts -join $separator


    return $soughtPath
}


#Gets list of child items in directory, and gives directories an index
function Enumerate {
	#Try to ls directory and give option
	$items = ls

	#Variable to count number of directories
	$dir_num = [int]0
	
	# Add an index column and format output
	$indexedItems = $items | ForEach-Object {
		#Check if item is directory and if so apply an index
		$index = ""
		if ($_.PSIsContainer) { 
			$index = [array]::IndexOf($items, $_) + 1
			$dir_num += 1
		}

		[PSCustomObject]@{
			Index = $index
			Name = $_.Name
			Size = if ($_.PSIsContainer) { "DIR" } else { "{0:N0}" -f $_.Length }
			LastWriteTime = $_.LastWriteTime
		}
	}
	
	#Display the table
	$indexedItems | Format-Table -AutoSize
	
	#Try to get an integer value, throw to a normal expression if not
	Write-Host "(Enter an index, path, dot string, comma string or expression:)"
	
	#Ask user for input
	$strings = @("CDLS (Powershell) ", (Get-Location).Path, "> ")
	Write-Host ($strings -join "") -NoNewline
	$input = Read-Host
	
	#If blank response, exit
	if ($input -eq "") {
		exit 0
	}
	
	#Check if input is an integer, a dot string or an expression
	if ([int]::TryParse($input, [ref]$null)) { #An integer has been entered
		$chosen = [int]$input
		
		#Check if integer is within range or is negative
		if ($chosen -le 0) { #Negative integer, convert to dot string
		
			[int]$stops = ($chosen / -1) + 1
			$dotString = -join ("." * $stops)
			
			#Get path from dot string
			$dotPath = Dot-Path $dotString
			cd $dotPath
			
		} elseif ($chosen -le $dir_num) { #Positive integer within range
		
			#cd to selected subdirectory
			cd $indexedItems[$chosen - 1].Name
			
		} else {
			Write-Error "Index out of range."
		}
		
	} elseif ($input -match $DOT_STRING_REGEX) { #A dot string has been entered
	
		#Get path from dot string
		$dotPath = Dot-Path $input
		
		cd $dotPath
		
	} elseif ($input -match $COMMA_STRING_REGEX) { #A comma string has been entered
		
		#Get path from comma string
		$soughtPath = Comma-Path $input (Get-Location).Path
		
		cd $soughtPath
		
	} else { #Not an integer or a dot/comma string, try something else
	
		if (-not (ChkDir $input)) { # If valid dir, will cd in function
			Invoke-Expression $input
		}
	}
	
}



while ($true) {
	Enumerate
}