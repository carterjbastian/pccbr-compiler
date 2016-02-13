#!/bin/bash
TPATH="./testfiles/"
OPATH="./outfiles/"

cases="arrayTest controlFlow danglingElse declaredErrors errTest expressions
        forLoop functions generalTest loops scopeTest textbook"

count=`echo $cases | wc -w`
# build in parse mode
make clean; make parse

# Generate the Abstract Syntax Trees for each file
for c in $cases; do
  echo "Building Abstract Syntax Tree for Test Case $c"
  ./parse < "$TPATH$c.c" > "$OPATH$c.ast" 2>&1
done

# build in symbol table node
make clean; make

# Generate the symbol table for each file 
for c in $cases; do
  echo "Building Symbol Graph for Test Case $c"
  ./symtab < "$TPATH$c.c" > "$OPATH$c.sym" 2>&1
done

echo "Testing complete - tested with $count test cases."
