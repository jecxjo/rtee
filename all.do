redo-ifchange rtee config/MANPAGES

read MANPAGES <config/MANPAGES
if [ $MANPAGES -eq 1 ]
then
  redo-ifchange rtee.1.gz
fi
