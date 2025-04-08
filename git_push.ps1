( param [string]$Message = "")

if (-not $Message) {
	$Message = Read-Host "Commit message"
}

if (-not $Message) {
	exit
}

git add .
git commit -m $Message
git push origin main