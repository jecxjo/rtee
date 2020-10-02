DEPS=$(ls CHANGELOG.md LICENSE README.md *.do *.c)
redo-ifchange ${DEPS}

git archive --format=tgz --prefix=rtee/ HEAD >$3
