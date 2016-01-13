./simpsh \
    --rdonly ay \
    --rdonly by \
    --wronly cy \
    --wronly dy \
    --wronly ey \
    --wronly fy \
    --command 0 2 3 cat \
    --command 1 4 5 tr A-Z a-z \
    --command 0 3 3 sort
