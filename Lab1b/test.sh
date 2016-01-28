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
    --verbose \
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
    rm *temp
    rm 0ans
    exit
fi

if diff 2temp 0ans >/dev/null ; then
    echo "Passed second test."
else
    echo "Failed second test."
    rm *temp
    rm 0ans
    exit
fi

echo "Passed all tests."
rm *temp
rm 0ans



# Round 2
# Error checking
touch 0temp
touch 1temp
touch 2temp
touch 3temp
touch 4temp
touch 5temp
touch 6temp
touch 7temp
touch 8temp
touch 9temp


printf "./simpsh: option '--rdonly' requires an argument\n" > 0temp

./simpsh \
    --rdonly 2> 1temp

if diff 0temp 1temp >/dev/null ; then
    echo "Passed first test: recognizes missing arguments"
else
    echo "Failed first test: doesn't recognize missing arguments"
    rm *temp;
    exit
fi

printf "./simpsh: unrecognized option '--option'\n" > 2temp

./simpsh \
    --option 2> 3temp

if diff 2temp 3temp >/dev/null ; then
    echo "Passed second test: detects unrecognized options"
else
    echo "Failed second test: doesn't detect unrecognized options"
    rm *temp;
    exit
fi


printf "Invalid argument for file.\n" > 4temp
printf "Invalid argument for file.\n" > 6temp

./simpsh \
    --rdonly --wronly 2> 5temp

./simpsh \
    --wronly --rdonly 2> 7temp

if diff 4temp 5temp >/dev/null ; then
    echo "Passed third test: detects invalid arguments to rdonly wronly"
else
    echo "Failed third test: doesn't detect invalid arguments to rdonly wronly"
    rm *temp;
    exit
fi

printf "invalid file descriptor 12\n" > 8temp

./simpsh \
    --command 12 12 12 cat 2> 9temp

if diff 8temp 9temp >/dev/null ; then
    echo "Passed fourth test: detects invalid file descriptors"
else
    echo "Failed fourth test: doesn't detect invalid file descriptors"
    rm *temp;
    exit
fi

echo "Passed all tests."
rm *temp
