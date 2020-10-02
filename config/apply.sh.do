DEPS="COMPILER LINKER OS"
redo-ifchange ${DEPS}

read COMPILER_PATH <COMPILER
read LINKER_PATH <LINKER
read OS<OS

COMPILER=$(basename $COMPILER_PATH)
LINKER=$(basename $LINKER_PATH)

cat >>$3<<EOF
#!/bin/sh
sed -e "s/%COMPILER%/$COMPILER/g" \
    -e "s/%LINKER%/$LINKER/g" \
    -e "s/%OS%/$OS/g"
EOF

redo-always
redo-stamp <$3
