# TODO: Allow PS commands to be input if not resolved as CDLS

function cdls {

	param ( [string[]]$Inputs = "" )
	
	$Repeat = $false
	
	while ($true) {
		if (-not  $Inputs) {
			$Repeat = $true
			
			$THE_STRING = "CDLS (Powershell) "+(Get-Location).Path+"> "
			Write-Host $THE_STRING -NoNewLine
			$cmd = Read-Host
			$Inputs = @()
			$Inputs += $cmd
			
			if (-not $Inputs) {
				cdls.exe "."
				$Repeat = $false
				return
			}
		}
		
		
		$output = & cdls.exe $Inputs
		echo $output

		$directory_line = echo $output | Select-String -Pattern "Directory: "
		$dir_string = $directory_line.ToString()
		$directory = $dir_string.Substring(11)

		cd $directory
		
		$Inputs = $null
		
		if (-not $Repeat) {
			break
		}
	}
}