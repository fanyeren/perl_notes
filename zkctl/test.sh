#!/bin/bash
./zkctl -s cluster repair

for input in test/*.in; do
    sh $input > ${input%.in}.real
    diff ${input%.in}.real ${input%.in}.out &> /dev/null
    if [ "$?" == "0" ]; then
        echo -e "[\e[00;32mSucceed\e[00m] for $input"
        rm ${input%.in}.real
    else
        echo -e "[\e[00;31mFailed\e[00m] for $input"
    fi
done
