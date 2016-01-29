#!/bin/bash
# Test cases

# Round 1: read, write, commands
echo ""
echo "Starting round 1 of testing."

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

rm *temp
rm 0ans

echo ""

# Round 2: read, write error checking
echo "Starting round 2 of testing."

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

rm *temp

echo ""

# Round 3: oflags
echo "Starting round 3 of testing."

touch 1temp
touch 2temp
touch 3temp
touch 4temp
touch 5temp
touch 6temp

printf "the\nquick\nbrown\nfox\njumps\nover\nthe\nlazy\ndog" > 1temp
printf "the\nquick\nbrown\nfox\njumps\nover\nthe\nlazy\ndog" > 2temp
printf "test! " > 3temp
printf "test! test! " > 5temp
printf "test! test! test! " > 6temp

./simpsh \
    --verbose \
    --creat --rdwr 0temp \
    --trunc --nofollow --sync --rdonly 1temp \
    --nonblock --append --rsync --rdonly 2temp \
    --append --rdwr 3temp \
    --append --wronly 4temp \
    --rdonly 5temp \
    --command 2 0 4 cat \
    --command 5 3 4 cat \
    --command 1 1 4 cat

if diff 0temp 2temp >/dev/null ; then
    echo "Passed first test."
else
    echo "Failed first test."
    rm *temp
    exit
fi

if diff 3temp 6temp >/dev/null ; then
    echo "Passed second test."
else
    echo "Failed second test."
    rm *temp
    exit
fi

if diff 1temp 4temp >/dev/null ; then
    echo "Passed third test."
else
    echo "Failed third test."
    rm *temp
    exit
fi

rm *temp
echo ""

# Round 4: pipes
echo "Starting round 4 of testing."

touch 0temp
touch 5temp
touch 6temp # stderr
touch 7temp

printf "the\nquick\nbrown\nfox\njumps\n" > 0temp
printf "over\nthe\nlazy\ndog\n" > 7temp

./simpsh \
    --rdonly 0temp \
    --pipe \
    --pipe \
    --creat --trunc --wronly 5temp \
    --creat --append --wronly 6temp \
    --command 3 5 6 tr A-Z a-z \
    --command 0 2 6 sort \
    --command 1 4 6 cat 7temp -

rm *temp

echo ""

# Round 5: signals
echo "Starting round 5 of testing."

touch 0temp
touch 1temp
touch 2temp
touch 3temp
touch 4temp

fprint "This better not be in 1temp!!" > 0temp

./simpsh \
    --verbose \
    --rdonly 0temp \
    --wronly 1temp \
    --wronly 2temp \
    --catch 11 \
    --abort \
    --command cat 0 2 - \
    --default 11 \
    --ignore 11 \
    --abort \
    --command cat 0 3 - \
    --default 11 \
    --abort \
    --command cat 0 1 -


if diff 0temp 2temp >/dev/null ; then
    echo "Passed first test."
else
    echo "Failed first test."
    rm *temp
    exit
fi
	  
if diff 0temp 3temp >/dev/null ; then
    echo "Passed second test."
else
    echo "Failed second test."
    rm *temp
    exit
fi

if diff 1temp 4temp >/dev/null ; then
    echo "Passed third test."
else
    echo "Failed third test."
    rm *temp
    exit
fi
rm *temp

echo ""

echo "Passed all tests!"
