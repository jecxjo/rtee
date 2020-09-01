redo-ifchange $2.c config/COMPILER config.h
read COMPILER <config/COMPILER
${COMPILER} -MD -MF $2.d -o $3 $2.c
read DEPS <$2.d
redo-ifchange ${DEPS#*:}
${COMPILER} -c -o $3 -I. $2.c

