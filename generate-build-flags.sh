#!/bin/bash

# Read .env file and output as -D flags
cat .env | while read line; do
  if [[ $line == ";"* ]] || [[ $line == "#"* ]]; then
    continue
  fi
  echo "'-D $line'"
done
