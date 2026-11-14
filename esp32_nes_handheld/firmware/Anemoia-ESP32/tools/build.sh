#!/bin/sh
# Build a nahrani forku Anemoie. Oddelena instalace arduino-cli
# (~/Library/Arduino15-nes: ESP32 core 3.2.1 s platform.txt Anemoie,
# vlastni kopie TFT_eSPI s User_Setup.h z tohoto adresare), globalni
# instalace se nemeni. Pouziti: tools/build.sh [port]   (bez portu jen build)
set -eu
HERE=$(cd "$(dirname "$0")/.." && pwd)
CFG="$HOME/Library/Arduino15-nes/arduino-cli.yaml"
FQBN="esp32:esp32:esp32:PartitionScheme=huge_app"
BUILD="${BUILD_DIR:-$HOME/Library/Arduino15-nes/build/Anemoia-ESP32}"

"$HERE/tools/make_roms.sh"
cp "$HERE/User_Setup.h" "$HOME/Library/Arduino15-nes/user/libraries/TFT_eSPI/User_Setup.h"
arduino-cli --config-file "$CFG" compile --fqbn "$FQBN" --build-path "$BUILD" "$HERE"
if [ $# -ge 1 ]; then
  arduino-cli --config-file "$CFG" upload --fqbn "$FQBN" -p "$1" --input-dir "$BUILD" "$HERE"
fi
