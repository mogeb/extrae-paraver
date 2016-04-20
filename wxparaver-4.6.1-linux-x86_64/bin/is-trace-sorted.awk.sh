#!/bin/bash
echo

if [ $# -ne 1 ]; then
  echo "Usage: $0 <trace>"
  echo "       Checks if the trace is time sorted."
  echo
  echo "Parameters:"
  echo "       <trace>   Paraver tracefile. Can be the \".prv\" or the \".prv.gz\" file."

  exit
fi

if [ ! -f $1 ]; then
  echo "Error: trace $1 not found!"
  exit
fi

ORDERED=1

# Get tracename
TRACENAME=$(echo "$1" | sed 's/\.[^\.]*$//')
EXTENSION=$(echo "$1" | sed 's/^.*\.//')

# Is gzipped?
if [[ ${EXTENSION} = "gz" ]]; then
  echo "Decompressing $1 trace..."
#  unpigz -p 2 $1 
  gunzip $1
  TRACENAME=$(echo "${TRACENAME}" | sed 's/\.[^\.]*$//')
  echo "...Done!"
fi

BODY=${TRACENAME}.body

# get trace body
echo "Obtaining body section of ${TRACENAME}.prv trace..."
#awk '$0 ~/^1/ || $0 ~/^2/ || $0 ~/^3/ { print > "'$BODY'" ;}' ${TRACENAME}.prv
awk '$0 ~/^[123]/ { print > "'$BODY'" ;}' ${TRACENAME}.prv
echo "...Done!"

# check if body is sorted by time
echo "Checking the order of the body of the trace..."
sort -c -n -s --field-separator=':' --key=6,6 ${BODY}

if [ $? = 1 ]; then
  ORDERED=0
fi
echo "...Done!"

# remove temporal files
echo "Deleting temporal files..."
rm -rf ${BODY}
echo "...Done!"

echo

if [ $ORDERED = 1 ]; then
  echo "TRACE $1 IS ORDERED."
else
  echo "TRACE $1 IS DISORDERED!!!"
fi
  

