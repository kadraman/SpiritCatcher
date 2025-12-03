#!/bin/sh

# This assumes Emullicious has been installed to:
# 	$ZGB_PATH/../../env/tools/Emulicious/Emulicious.jar

unset GTK_PATH

# uncomment to run Debug build
java -jar  $ZGB_PATH/../../env/tools/Emulicious/Emulicious.jar bin/gbc/rom_Debug.gbc
# uncomment to run Release build
#java -jar $ZGB_PATH/../../env/tools/Emulicious/Emulicious.jar bin/gbc/rom.gbc


