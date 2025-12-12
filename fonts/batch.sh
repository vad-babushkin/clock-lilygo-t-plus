#!/bin/bash

filename=$(basename -- "$1")
extension="${filename##*.}"
filenm="${filename%.*}"

echo "$filename"
echo "$extension"
echo "$filenm"

python3 fontconvert.py --compress "$filenm"15B 15 $1 > "$filenm"15b.h
python3 fontconvert.py --compress "$filenm"24B 24 $1 > "$filenm"24b.h
python3 fontconvert.py --compress "$filenm"48B 48 $1 > "$filenm"48b.h
python3 fontconvert.py --compress "$filenm"72B 72 $1 > "$filenm"72b.h
python3 fontconvert.py --compress "$filenm"88B 86 $1 > "$filenm"88b.h