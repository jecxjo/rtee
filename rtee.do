DEPS=rtee.o
redo-ifchange $DEPS
gcc -o $3 $DEPS
