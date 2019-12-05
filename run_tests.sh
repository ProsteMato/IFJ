#!/bin/bash

APP=./cifj19
TEST_DIR=./lex_tests
RED='\033[1;31m'
GREEN='\033[1;32m'
NC='\033[0m' # No Color
USE_COLOR=1
TOTAL_CNT=0
ERROR_CNT=0
TOTAL_LEX=0
TOTAL_SYN=0
TOTAL_SEM3_=0
TOTAL_SEM4_=0
TOTAL_SEM5_=0
TOTAL_SEM6_=0
TOTAL_ZERO=0
TOTAL_OK=0
ERR_LEX=0
ERR_SYN=0
ERR_SEM3_=0
ERR_SEM4_=0
ERR_SEM5_=0
ERR_SEM6_=0
ERR_ZERO=0
ERR_OK=0

function print_ok() {
  TEXT=$1
  if [ "$USE_COLOR" -eq 1 ] ; then
    echo -e "[${GREEN}OK${NC}] $TEXT"
  else
    echo -e "[OK] $TEXT"
  fi
}

function print_err() {
  TEXT=$1
  if [ "$USE_COLOR" -eq 1 ] ; then
    echo -e "[${RED}FAILED${NC}] $TEXT"
  else
    echo -e "[FAILED] $TEXT"
  fi
}


for f in ./tests/lex_err*
do 
    ((TOTAL_CNT++))
    ((TOTAL_LEX++))
    dos2unix $f
    ${APP} <$f
    RETURN_CODE=$?
    echo $f
    if [ $RETURN_CODE -eq 1 ]  
    then 
        print_ok "Lex err returned."
    else 
        echo $RETURN_CODE
        print_err "Should be lex error."
        ((ERROR_CNT++))
        ((ERR_LEX++))
    fi
done

for f in ./tests/syn_err*
do 
    ((TOTAL_CNT++))
    ((TOTAL_SYN++))
    dos2unix $f
    ${APP} <$f
    RETURN_CODE=$?
    echo $f
    if [ $RETURN_CODE -eq 2 ]  
    then 
        print_ok "Syntax err returned."
    else 
        echo $RETURN_CODE
        print_err "Should be syntax error."
        ((ERROR_CNT++))
        ((ERR_SYN++))
    fi
done

for f in ./tests/sem_err3_*
do 
    ((TOTAL_CNT++))
    ((TOTAL_SEM3_++))
    dos2unix $f
    ${APP} <$f
    RETURN_CODE=$?
    echo $f
    if [ $RETURN_CODE -eq 3 ]  
    then 
        print_ok "Sematic 3 err returned."
    else 
        echo $RETURN_CODE
        print_err "Should be sem 3 error."
        ((ERROR_CNT++))
        ((ERR_SEM3_++))
    fi
done

for f in ./tests/sem_err4_*
do 
    ((TOTAL_CNT++))
    ((TOTAL_SEM4_++))
    dos2unix $f
    ${APP} <$f
    RETURN_CODE=$?
    echo $f
    if [ $RETURN_CODE -eq 4 ]  
    then 
        print_ok "Sematic 4 err returned."
    else 
        echo $RETURN_CODE
        print_err "Should be sem 4 error."
        ((ERROR_CNT++))
        ((ERR_SEM4_++))
    fi
done

for f in ./tests/sem_err5_*
do 
    ((TOTAL_CNT++))
    ((TOTAL_SEM5_++))
    dos2unix $f
    ${APP} <$f
    RETURN_CODE=$?
    echo $f
    if [ $RETURN_CODE -eq 5 ]  
    then 
        print_ok "Sematic 5 err returned."
    else 
        echo $RETURN_CODE
        print_err "Should be sem 5 error."
        ((ERROR_CNT++))
        ((ERR_SEM5_++))
    fi
done

for f in ./tests/sem_err6_*
do 
    ((TOTAL_CNT++))
    ((TOTAL_SEM6_++))
    dos2unix $f
    ${APP} <$f
    RETURN_CODE=$?
    echo $f
    if [ $RETURN_CODE -eq 6 ]  
    then 
        print_ok "Sematic 6 err returned."
    else 
        echo $RETURN_CODE
        print_err "Should be sem 6 error."
        ((ERROR_CNT++))
        ((ERR_SEM6_++))
    fi
done

for f in ./tests/zero*
do 
    ((TOTAL_CNT++))
    ((TOTAL_ZERO++))
    dos2unix $f
    ${APP} <$f
    RETURN_CODE=$?
    echo $f
    if [ $RETURN_CODE -eq 9 ]  
    then 
        print_ok "div with zero  err returned."
    else 
        echo $RETURN_CODE
        print_err "Should be div with zero error."
        ((ERROR_CNT++))
        ((ERR_ZERO++))
    fi
done

for f in ./tests/OK*
do 
    ((TOTAL_CNT++))
    ((TOTAL_OK++))
    dos2unix $f
    ${APP} <$f
    RETURN_CODE=$?
    echo $f
    if [ $RETURN_CODE -eq 0 ]  
    then 
        print_ok "No err returned."
    else 
        echo $RETURN_CODE
        print_err "Should be without error."
        ((ERROR_CNT++))
        ((ERR_OK++))
    fi
done

echo "Test lex count: $TOTAL_LEX, Error lex count: $ERROR_LEX"
echo "Test syntax count: $TOTAL_SYN, Error syntax count: $ERROR_SYN"
echo "Test sem3 count: $TOTAL_SEM3_, Error sem3 count: $ERROR_SEM3_"
echo "Test sem4 count: $TOTAL_SEM4_, Error sem4 count: $ERROR_SEM4_"
echo "Test sem5 count: $TOTAL_SEM5_, Error sem5 count: $ERROR_SEM5_"
echo "Test sem6 count: $TOTAL_SEM6_, Error sem6 count: $ERROR_SEM6_"
echo "Test zero count: $TOTAL_ZERO, Error zero count: $ERROR_ZERO"
echo "Test OK count: $TOTAL_OK, Error OK count: $ERROR_OK"
echo "Test count: $TOTAL_CNT, Error count: $ERROR_CNT"
