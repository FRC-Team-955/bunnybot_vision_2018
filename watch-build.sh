cd build/
#when-changed -1r ../src/ sh -c 'make && ./Vision'
when-changed -1r ../src/ make
