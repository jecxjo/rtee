if [ "x$(which man)" != "x" ]
then
  echo 1 >$3
else
  echo 0 >$3
fi

redo-always
redo-stamp <$3
