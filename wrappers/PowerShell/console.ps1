# A wrapper script to run cdls and change the cwd after

# TODO: Make -v and -h work
# for version & help

# On load, check if an alternative path has been given
param (
	[Alias("p")]
	[string]$Path = "cdls_ex.exe" # Default Windows install path is "C:\Program Files\cdls\cdls_ex.exe"
)

# Location of $CDLS_PATH
$CDLS_PATH = $Path


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