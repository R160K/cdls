$output = & cdls "..."
echo $output

$directory_line = echo $output | Select-String -Pattern "Directory: "
$directory = $($directory_line -split "\s+")[1]

cd $directory