# A wrapper script to run cdls and change the cwd after

# TODO: Make -v and -h work
# for version & help

# On load, check if an alternative path has been given
# param (
	# [Alias("p")]
	# [string]$Path = "cdls_ex.exe" # Default Windows install path is "C:\Program Files\cdls\cdls_ex.exe"
# )

param (
	[Alias("p")]
	[string]$Path
)

# Location of $CDLS_PATH
if ($Path) {
	$CDLS_PATH = $Path
} elseif ($env:CDLS_EXE_PATH) {
	$CDLS_PATH = $env:CDLS_EXE_PATH
} else {
	$CDLS_PATH = "cdls_ex.exe"
}

echo "Looking for cdls executable at: $CDLS_PATH"

function cdls {

	param (
	[Parameter(ValueFromRemainingArguments = $true)]
	[string[]]$Inputs
	)


	# Run app with parameters and get last line
	& $CDLS_PATH $Inputs | Tee-Object -Variable output
	$lastLine = $output[-1]

	# Change cwd
	cd $lastLine
	
}