################################################################################
#   Author: Shiv Bhushan Tripathi                                       ########
#   Date: 18th August 2021                                              ########
################################################################################


#!/bin/bash

test () {

    # Execute the given ops pattern.
    RESULT=`echo "$($2)"`
    # If produced result and expected result is equal then test is passed.
    if [[ $RESULT -eq $3 ]] 
    then
        echo "TEST $1 PASSED"
    else
        echo "TEST $1 FAILED"
    fi

}


# Cleanup old executable 
[ -f root ] && rm root
[ -f double ] && rm double
[ -f square ] && rm square

# Compile
gcc -o root root.c -lm
gcc -o double double.c -lm
gcc -o square square.c -lm

# Tests
test 1 "./root 5" 2
test 2 "./double square 2" 16
test 3 "./root square 4" 4
