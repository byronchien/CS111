#!/bin/bash
# Test cases

# Normal behavior
touch 0temp
touch 1temp
touch 2temp
touch 3temp
touch 4temp
touch 5temp
touch 0ans

printf "the\nquick\nbrown\nfox\njumps\nover\nthe\nlazy\ndog" > 0temp
printf "THE\nQUICK\nBROWN\nFOX\nJUMPS\nOVER\nTHE\nLAZY\nDOG" > 1temp

./simpsh \
    --rdonly 0temp \
    --rdonly 1temp \
    --wronly 2temp \
    --wronly 3temp \
    --wronly 4temp \
    --wronly 5temp \
    --command 1 4 5 tr A-Z a-z \
    --command 0 2 3 sort

printf "brown\ndog\nfox\njumps\nlazy\nover\nquick\nthe\nthe\n" > 0ans

if diff 4temp 0temp >/dev/null ; then
    echo "Passed first test."
else
    echo "Failed first test."
    exit
fi

if diff 2temp 0ans >/dev/null ; then
    echo "Passed second test."
else
    echo "Failed second test."
    exit
fi

echo "Passed all tests."
rm *temp
rm 0ans
