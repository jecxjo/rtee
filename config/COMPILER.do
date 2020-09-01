# Check LLVM
if [ "x$(which clang)" != "x" ]
then
  echo "int main() { return 0; }" | clang -o /dev/null -xc -
  if [ $? -eq 0 ];
  then
    echo $(which clang) >$3
    redo-always
    redo-stamp <$3
    exit 0
  fi
fi

# Check gcc
if [ "x$(which gcc)" != "x" ]
  echo "void main() {}" | gcc -o /dev/null -xc -
  if [ $? -eq 0 ];
  then
    echo $(which gcc) >$3
    redo-always
    redo-stamp <$3
    exit 0
  fi
fi

# Check cc
if [ "x$(which cc)" != "x" ]
  echo "void main() {}" | cc -o /dev/null -xc -
  if [ $? -eq 0 ];
  then
    echo $(which cc) >$3
    redo-always
    redo-stamp <$3
    exit 0
  fi
fi

echo "$0: Fatal: cannot find c compiler" >&2
exit 99
