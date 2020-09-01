DEPS=rtee.o
redo-ifchange $DEPS config/LINKER
read LINKER <config/LINKER
${LINKER} -o $3 $DEPS
