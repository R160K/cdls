# A wrapper script to run cdls and change the cwd after

# TODO: Make -v and -h work
# for version & help
# (-w might be getting removed by default, maybe replaced with -c console mode)

# On load, check if an alternative path has been given
param (
	[Alias("p")]
	[string]$Path = "cdls_ex.exe" # Default Windows install path is "C:\Program Files\cdls\cdls_ex.exe"
)

# Location of $CDLS_PATH
$CDLS_PATH = $Path


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