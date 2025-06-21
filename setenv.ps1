
# Change ZGB_HOME to the directory where you extracted CrossZGB
[System.Environment]::SetEnvironmentVariable("ZGB_HOME", "C:\Tools\CrossZGB", "User")
[System.Environment]::SetEnvironmentVariable("ZGB_PATH", $env:ZGB_HOME+"\ZGB\common", "User")
[System.Environment]::SetEnvironmentVariable("GBDK_HOME", $env:ZGB_HOME+"\gbdk", "User")
[System.Environment]::SetEnvironmentVariable("PATH", $env:ZGB_HOME+"\env\make-3.81-bin\bin;"+$env:ZGB_HOME+"\gbdk\bin", "User")