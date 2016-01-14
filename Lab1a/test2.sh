#!/bin/bash
# Test cases

# Errors
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


printf "invalid argument to --rdonly" > 4temp
printf "invalid argument to --wronly" > 6temp

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
