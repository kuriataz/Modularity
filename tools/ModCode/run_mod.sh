#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <filename>"
  exit 1
fi

base="$1"
mainfile="${base}.txt"

if [ ! -f "$mainfile" ]; then
  echo "Main file '$mainfile' does not exist."
  exit 1
fi

for pfile in ${base}P*.txt; do
  if [ -f "$pfile" ]; then
    echo "$pfile:"
    ./mod -f "$mainfile" -p "$pfile"
  fi
done
