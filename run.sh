#!/bin/sh

unset GTK_PATH

# uncomment to run Debug build
java -jar /usr/local/lib/CrossZGB/env/Emulicious/Emulicious.jar bin/gbc/rom_Debug.gbc
# uncomment to run Release build
#java -jar /usr/local/lib/CrossZGB/env/Emulicious/Emulicious.jar bin/gbc/rom.gbc


