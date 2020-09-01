redo-ifchange COMPILER
read COMPILER <COMPILER

case $(basename ${COMPILER}) in
  gcc)
    echo ${COMPILER} >$3
    ;;
  cc)
    which ld >$3
    ;;
  clang)
    echo ${COMPILER} >$3
    ;;
  *)
    echo "$0: Fatal: Cannot find linker" >&2
    exit 99
    ;;
esac

redo-always
redo-stamp <$3
