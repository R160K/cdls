# A wrapper script to run cdls and change the cwd after

# TODO: Make -v and -h work
# for version & help

#TODO: Add alternative path for testing

# Location of $CDLS_PATH
$CDLS_PATH = "cdls_ex.exe"


function cdls {

	param (
	[switch]$w,
	[Parameter(ValueFromRemainingArguments = $true)]
	[string[]]$Inputs
	)

	# Add -w to list of arguments if not already present
	if ($Inputs.Length -eq 0 -or -not ($Inputs[0].Trim() -like "-*")) {
		$Inputs = @("-w") + $Inputs
	}

	
	# Run app and get last line
	& $CDLS_PATH $Inputs | Tee-Object -Variable output
	$lastLine = $output[-1]

	# Change cwd
	cd $lastLine
	
}