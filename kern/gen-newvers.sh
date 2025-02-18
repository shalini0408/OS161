#!/bin/sh
#
# newvers.sh - increment build number in the compile directory (a build directory)
#              and emit vers.c.
#              The build number is kept in the file "version".
#
# Usage: newvers.sh CONFIGNAME

if [ ! -f gen-autoconf.sh ]; then
    #
    # If there's no file gen-autoconf.sh, we are in the wrong place.
    #
    echo "$0: Not in a build directory"
    exit 1
fi

if [ "x$1" = x ]; then
    echo "Usage: %0 CONFIGNAME"
    exit 1
fi

CONFIG="$1"

#
# Get and increment the version number
#

VERS=`cat compile/version 2>/dev/null || echo 0`
VERS=`expr $VERS + 1`
echo "$VERS" > compile/version

#
# Write vers.c
#

echo '/* This file is automatically generated. Edits will be lost.*/' > compile/vers.c
echo "const int buildversion = $VERS;" >> compile/vers.c
echo 'const char buildconfig[] = "'"$CONFIG"'";' >> compile/vers.c

#
# Announce it in the hopes that it'll still be visible when the build
# finishes.
#
echo "*** This is $CONFIG build "'#'"$VERS ***"
