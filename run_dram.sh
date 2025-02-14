#!/bin/bash

# Check if directory is provided
if [ -z "$1" ]
then
    echo "No directory provided. Usage: ./run_converter.sh <directory>"
    exit 1
fi

# Iterate over all files starting with 'output' in the provided directory
for file in "$1"/output*.csv
do
    # Check if file is a regular file and not a directory
    if [ -f "$file" ]
    then
        echo "Processing $file"
        # Run the python script on the file
        ./build/bin/drampower_cli $file ddr4.json > $file.out
    fi
done  