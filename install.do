exec >&2

: ${INSTALL:=install}
: ${DESTDIR=NONE}
: ${PREFIX:=/usr}
: ${MANDIR:=$DESTDIR$PREFIX/share/man/man1}
: ${BINDIR:=$DESTDIR$PREFIX/bin}

if [ "$DESTDIR" == "NONE" ]; then
  echo "$0: fatal: set DESTDIR before trying to install."
  exit 99
fi

redo-ifchange all
echo "Installing to $DESTDIR$PREFIX"

# make dirs
${INSTALL} -m 755 -d "${MANDIR}" "${BINDIR}"

# Docs
${INSTALL} -m 0644 rtee.1.gz "${MANDIR}"

# Eexecutable
${INSTALL} -m 0755 rtee "${BINDIR}"
