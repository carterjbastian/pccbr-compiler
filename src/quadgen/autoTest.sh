#!/bin/bash
TPATH="./testfiles/"
OPATH="./outfiles/"

cases="arrayTest controlFlow danglingElse declaredErrors errTest expressions
        forLoop functions generalTest loops scopeTest textbook simpleAdd
        simpleBool simpleAssignment simpleComplement simpleMod simpleNegate
        simpleSub"

count=`echo $cases | wc -w`
# build in parse mode
make clean; make

# Generate the Abstract Syntax Trees for each file
for c in $cases; do
  echo "Building Quad Code for Test Case $c"
  ./quadgen < "$TPATH$c.c" > "$OPATH$c.quad" 2>&1
done

echo "Testing complete - tested with $count test cases."
