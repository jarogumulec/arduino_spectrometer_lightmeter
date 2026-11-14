#!/bin/sh
# Prevede vsechny ROM z esp32_nes_handheld/roms/*.nes na src/roms_data.h
# (const pole ve flash). Spoustet pred kazdym buildem po zmene sady her.
# Vystup je v .gitignore (autorsky chranena data).
set -eu
HERE=$(cd "$(dirname "$0")/.." && pwd)
ROMS=${1:-"$HERE/../../roms"}
OUT="$HERE/src/roms_data.h"
TMP="$OUT.tmp"

{
  echo "// Generovano tools/make_roms.sh, needitovat."
  echo "#pragma once"
  echo "#include <stdint.h>"
  echo "struct EmbeddedRom { const char* name; const uint8_t* data; uint32_t size; };"
} > "$TMP"

i=0
names=""
for f in "$ROMS"/*.nes; do
  [ -e "$f" ] || continue
  base=$(basename "$f" .nes | sed 's/"/ /g')
  size=$(wc -c < "$f" | tr -d ' ')
  echo "// $base, $size B" >> "$TMP"
  echo "alignas(4) static const uint8_t rom_$i[] = {" >> "$TMP"
  xxd -i < "$f" >> "$TMP"
  echo "};" >> "$TMP"
  names="$names  { \"$base\", rom_$i, $size },
"
  i=$((i + 1))
done

{
  echo "static const EmbeddedRom EMBEDDED_ROMS[] = {"
  printf "%s" "$names"
  [ "$i" -eq 0 ] && echo "  { nullptr, nullptr, 0 },"
  echo "};"
  echo "static constexpr int EMBEDDED_ROM_COUNT = $i;"
} >> "$TMP"

mv "$TMP" "$OUT"
echo "$i ROM -> $OUT"
