#!/bin/sh

cd src
make
cd ..
java -jar $ZGB_HOME/ZGB/tools/Emulicious/Emulicious.jar bin/gbc/zap-game.gbc