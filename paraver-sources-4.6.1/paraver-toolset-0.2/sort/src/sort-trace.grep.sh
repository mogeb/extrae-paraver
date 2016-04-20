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

# get trace body
echo "Dividing ${TRACENAME}.prv trace in header and body sections..."
grep "^#" ${TRACENAME}.prv > ${TRACENAME}.tmp
head -n 1 ${TRACENAME}.tmp > ${TRACENAME}.header
tail -n +2 ${TRACENAME}.tmp > ${TRACENAME}.comments 

grep "^c" ${TRACENAME}.prv > ${TRACENAME}.communicators
grep "^C" ${TRACENAME}.prv >> ${TRACENAME}.communicators
grep "^i" ${TRACENAME}.prv >> ${TRACENAME}.communicators
grep "^I" ${TRACENAME}.prv >> ${TRACENAME}.communicators

grep -v "^#" ${TRACENAME}.prv > ${TRACENAME}.tmp
grep -v "^c" ${TRACENAME}.tmp > ${TRACENAME}.tmp1
rm -rf ${TRACENAME}.tmp
grep -v "^C" ${TRACENAME}.tmp1 > ${TRACENAME}.tmp2
rm -rf ${TRACENAME}.tmp1
grep -v "^i" ${TRACENAME}.tmp2 > ${TRACENAME}.tmp3
rm -rf ${TRACENAME}.tmp2
grep -v "^I" ${TRACENAME}.tmp3 > ${TRACENAME}.body
rm -rf ${TRACENAME}.tmp3
echo "...Done!"


# sort by time
echo "Sorting the body of the trace..."
sort -n -s --field-separator=':' --key=6,6 ${TRACENAME}.body > ${TRACENAME}.body.sorted
rm -rf ${TRACENAME}.body
echo "...Done!"

# rebuild trace
echo "Writing ${TRACENAME}.sorted.prv trace..."
cat ${TRACENAME}.header ${TRACENAME}.communicators ${TRACENAME}.comments ${TRACENAME}.body.sorted > ${TRACENAME}.sorted.prv
echo "...Done!"

# remove temporal files
echo "Deleting temporal files..."
rm -rf ${TRACENAME}.header
rm -rf ${TRACENAME}.communicators
rm -rf ${TRACENAME}.comments
rm -rf ${TRACENAME}.body.sorted
echo "...Done!"

# gzip again if it was
if [ $COMPRESSED = 1 ]; then
  echo "Compressing ${TRACENAME}.sorted.prv..."
  gzip --best ${TRACENAME}.sorted.prv
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


