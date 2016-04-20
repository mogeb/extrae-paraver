#!/bin/bash

echo

if [ $# -ne 1 ]; then
  echo "Usage: $0 <trace>"
  echo "       Sorts trace by time, writing a new \"trace.sorted.prv\" file."
  echo "       Stable sort is applied."
  echo "       The sorted trace is compressed if the original was."
  echo
  echo "Parameters:"
  echo "       <trace>   Paraver tracefile. Can be the \".prv\" or the \".prv.gz\" file."

  exit
fi

if [ ! -f $1 ]; then
  echo "Error: trace $1 not found!"
  exit
fi

# Get tracename
TRACENAME=$(echo "$1" | sed 's/\.[^\.]*$//')
EXTENSION=$(echo "$1" | sed 's/^.*\.//')
COMPRESSED=0

# Is gzipped?
if [[ ${EXTENSION} = "gz" ]]; then
  echo "Decompressing $1 trace..."
  gunzip $1
#  unpigz -p 2 $1
  TRACENAME=$(echo "${TRACENAME}" | sed 's/\.[^\.]*$//')
  COMPRESSED=1
  echo "...Done!"
fi

BODY=${TRACENAME}.body
HEADER=${TRACENAME}.header
BODY_SORTED=${TRACENAME}.body.sorted
TRACE_SORTED=${TRACENAME}.sorted.prv

# get trace body
echo "Dividing ${TRACENAME}.prv trace in header and body sections..."
#cat ${TRACENAME}.prv | awk '$0 ~/^1/ || $0 ~/^2/ || $0 ~/^3/ { print > "'$BODY'" ;} $0 ~/^#/ || $0 ~/^c/ || $0 ~/^C/ { print > "'$HEADER'" ; }'
awk '$0 ~/^[123]/ { print > "'$BODY'" ;} $0 ~/^[#cCiI]/ { print > "'$HEADER'" ; }' ${TRACENAME}.prv
echo "...Done!"

# sort by time
echo "Sorting the body of the trace..."
sort -n -s --field-separator=':' --key=6,6 ${BODY} > ${BODY_SORTED}
rm -rf ${BODY}
echo "...Done!"

# rebuild trace
echo "Writing ${TRACENAME}.sorted.prv trace..."
cat ${HEADER} ${BODY_SORTED} > ${TRACE_SORTED}
echo "...Done!"

# remove temporal files
echo "Deleting temporal files..."

rm -rf ${HEADER}
rm -rf ${BODY_SORTED} 

echo "...Done!"

# gzip again if it was
if [ $COMPRESSED = 1 ]; then
  echo "Compressing ${TRACE_SORTED}..."
  gzip --best ${TRACE_SORTED}
  echo "...Done!"
fi

# copy pcf and row
if [ -f ${TRACENAME}.pcf ]; then
  echo "Copying ${TRACENAME}.pcf file as ${TRACENAME}.sorted.pcf..."
  cp ${TRACENAME}.pcf ${TRACENAME}.sorted.pcf
  echo "...Done!"
fi

if [ -f ${TRACENAME}.row ]; then
  echo "Copying ${TRACENAME}.row file as ${TRACENAME}.sorted.row..."
  cp ${TRACENAME}.row ${TRACENAME}.sorted.row
  echo "...Done!"
fi


echo

