if [ -e "$1.in" ]
then
  redo-ifchange "$1.in" config/apply.sh
  sh config/apply.sh <$1.in >$3
else
  echo "$0: Fatal: don't know how to build '$1'" >&2
  exit 99
fi
