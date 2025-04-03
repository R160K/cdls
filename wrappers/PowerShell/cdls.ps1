$com_arg = ""

# Check if command line arguments are provided
if ($args.Count -gt 0) {
    $com_arg = args[1]
} else {
    Write-Host "No arguments provided."
}

$CDLS_OUTPUT = cdls $args[1]
$DIRECTORY_LINE = echo $CDLS_OUTPUT | Select-String -Pattern "Directory: "

$DIRECTORY = $($DIRECTORY_LINE -split "\s+")[1]

echo $CDLS_OUTPUT
cd $DIRECTORY