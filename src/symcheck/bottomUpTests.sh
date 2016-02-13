#!/bin/bash

make clean; make

./symtab < bottomUpTestFiles/tooFewArgs.c
echo ====================================
echo NEXT TEST CASE
echo ====================================
./symtab < bottomUpTestFiles/tooManyArgs.c
echo ====================================
echo NEXT TEST CASE
echo ====================================
./symtab < bottomUpTestFiles/wrongArgTypes.c
echo ====================================
echo NEXT TEST CASE
echo ====================================
./symtab < bottomUpTestFiles/recordVarTypes.c
echo ====================================
echo NEXT TEST CASE
echo ====================================
./symtab < bottomUpTestFiles/functionNotDeclared.c
echo ====================================
echo NEXT TEST CASE
echo ====================================
./symtab < bottomUpTestFiles/wrongReturn.c
echo ====================================
echo NEXT TEST CASE
echo ====================================
./symtab < bottomUpTestFiles/rightReturn.c
echo ====================================
echo NEXT TEST CASE
echo ====================================
./symtab < bottomUpTestFiles/implicitReturn.c
