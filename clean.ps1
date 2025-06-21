
$env:BUILD_TYPE="Debug"
#$env:BUILD_TYPE="Release"

Set-Location src
make BUILD_TYPE=$BUILD_TYPE gbc-clean
Set-Location ..
Remove-Item -Path ".\obj" -Recurse -Force
