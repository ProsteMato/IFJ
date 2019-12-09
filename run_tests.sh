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
TOTAL_SEMTHREE=0
TOTAL_SEMFOUR=0
TOTAL_SEMFIVE=0
TOTAL_SEMSIX=0
TOTAL_ZERO=0
TOTAL_OK=0
ERR_LEX=0
ERR_SYN=0
ERR_SEMTHREE=0
ERR_SEMFOUR=0
ERR_SEMFIVE=0
ERR_SEMSIX=0
ERR_ZERO=0
ERR_OK=0

function print_ok() {
  TEXT=$1
  if [ "$USE_COLOR" -eq 1 ] ; then
    printf "[${GREEN}OK${NC}] $TEXT \n"
  else
    printf  "[OK] $TEXT \n"
  fi
}

function print_err() {
  TEXT=$1
  if [ "$USE_COLOR" -eq 1 ] ; then
    printf "[${RED}FAILED${NC}] $TEXT \n"
  else
    printf "[FAILED] $TEXT \n"
  fi
}


for f in ./tests/lex_err/*.lex_err
do 
     echo "-------------------------------------------"
    ((TOTAL_CNT++))
    ((TOTAL_LEX++))
    timeout 2s ${APP} <$f >/dev/null
    RETURN_CODE=$?
    if [ $RETURN_CODE -eq 1 ]  
    then 
        print_ok "Test File: $f"
    else 
        print_err "Test File: $f \nError: Should be lex error. Return Code: $RETURN_CODE"
        ((ERROR_CNT++))
        ((ERR_LEX++))
    fi
done

for f in ./tests/syn_err/*.syn_err
do 
     echo "-------------------------------------------"
    ((TOTAL_CNT++))
    ((TOTAL_SYN++))
    timeout 2s ${APP} <$f >/dev/null 2>/dev/null 
    RETURN_CODE=$?
    if [ $RETURN_CODE -eq 2 ]  
    then 
        print_ok "Test File: $f"
    else 
        print_err "Test File: $f \nError: Should be syntax error. Return Code: $RETURN_CODE"
        ((ERROR_CNT++))
        ((ERR_SYN++))
    fi
done

for f in ./tests/sem_err/*.sem_err3
do 
     echo "-------------------------------------------"
    ((TOTAL_CNT++))
    ((TOTAL_SEMTHREE++))
    timeout 2s ${APP} <$f >/dev/null
    RETURN_CODE=$?
    if [ $RETURN_CODE -eq 3 ]  
    then 
        print_ok "Test File: $f"
    else
        print_err "Test File: $f \nError: Should be sem 3 error. Return Code: $RETURN_CODE"
        ((ERROR_CNT++))
        ((ERR_SEMTHREE++))
    fi
done

for f in ./tests/sem_err/*.sem_err4
do 
     echo "-------------------------------------------"
    ((TOTAL_CNT++))
    ((TOTAL_SEMFOUR++))
    timeout 2s ${APP} <$f >/dev/null
    RETURN_CODE=$?
    if [ $RETURN_CODE -eq 4 ]  
    then 
        print_ok "Test File: $f"
    else
        print_err "Test File: $f \nError: Should be sem 4 error. Return Code: $RETURN_CODE"
        ((ERROR_CNT++))
        ((ERR_SEMFOUR++))
    fi
done

for f in ./tests/sem_err/*.sem_err5
do 
     echo "-------------------------------------------"
    ((TOTAL_CNT++))
    ((TOTAL_SEMFIVE++))
    timeout 2s ${APP} <$f >/dev/null
    RETURN_CODE=$?
    if [ $RETURN_CODE -eq 5 ]  
    then 
        print_ok "Test File: $f"
    else
        print_err "Test File: $f \nError: Should be sem 5 error. Return Code: $RETURN_CODE"
        ((ERROR_CNT++))
        ((ERR_SEMFIVE++))
    fi
done

for f in ./tests/sem_err/*.sem_err6
do 
     echo "-------------------------------------------"
    ((TOTAL_CNT++))
    ((TOTAL_SEMSIX++))
    timeout 2s ${APP} <$f >/dev/null
    RETURN_CODE=$?
    if [ $RETURN_CODE -eq 6 ]  
    then 
        print_ok "Test File: $f"
    else 
        print_err "Test File: $f \nError: Should be sem 6 error. Return Code: $RETURN_CODE"
        ((ERROR_CNT++))
        ((ERR_SEMSIX++))
   fi
done

for f in ./tests/run_time_error/*.zero
do 
     echo "-------------------------------------------"
   ((TOTAL_CNT++))
    ((TOTAL_ZERO++))
    timeout 2s ${APP} <$f > $f.output
    /pub/courses/ifj/ic19int/linux/ic19int $f.output
    RETURN_CODE=$?
    if [ $RETURN_CODE -eq 9 ]  
    then 
        print_ok "Test File: $f"
    else 
        print_err "Test File: $f \nError: Should be div with zero error. Return Code: $RETURN_CODE"
        ((ERROR_CNT++))
        ((ERR_ZERO++))
    fi
done

for f in ./tests/gen/*.ok
do 
    echo "-------------------------------------------"
    ((TOTAL_CNT++))
    ((TOTAL_OK++))
    timeout 2s ${APP} < $f > $f.output
    RETURN_CODE=$?
    if [ $RETURN_CODE -eq 0 ]
    then
        print_ok "Test File: $f Error code: ok"
        /pub/courses/ifj/ic19int/linux/ic19int <$f.in $f.output > $f.output_interpret
        output=$(diff $f.output_interpret $f.expected_output 2>&1)
        if [ "$output" = "" ]
        then
            print_ok "Interpreted correctly"
        else
            print_err "Test File: $f \nError: Bad Interpretation"
            printf "OUTPUT FROM DIFF: \n $output \n"
            ((ERROR_CNT++))
            ((ERR_OK++))
        fi
    else 
        print_err "Test File: $f \nError: Should be without error. Return Code: $RETURN_CODE"
        ((ERROR_CNT++))
        ((ERR_OK++))
    fi
done

echo "Test lex count: $TOTAL_LEX, Error lex count: $ERR_LEX"
echo "Test syntax count: $TOTAL_SYN, Error syntax count: $ERR_SYN"
echo "Test sem3 count: $TOTAL_SEMTHREE, Error sem3 count: $ERR_SEMTHREE"
echo "Test sem4 count: $TOTAL_SEMFOUR, Error sem4 count: $ERR_SEMFOUR"
echo "Test sem5 count: $TOTAL_SEMFIVE, Error sem5 count: $ERR_SEMFIVE"
echo "Test sem6 count: $TOTAL_SEMSIX, Error sem6 count: $ERR_SEMSIX"
echo "Test zero count: $TOTAL_ZERO, Error zero count: $ERR_ZERO"
echo "Test OK count: $TOTAL_OK, Error OK count: $ERR_OK"
echo "Test count: $TOTAL_CNT, Error count: $ERROR_CNT"
