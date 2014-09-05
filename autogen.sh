#! /bin/sh
set -e -v
aclocal
autoheader
automake --add-missing --copy
autoconf


