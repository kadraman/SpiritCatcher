
$env:BUILD_TYPE="Debug"
#$env:BUILD_TYPE="Release"

Set-Location src
make BUILD_TYPE=$BUILD_TYPE gbc
Set-Location ..
