#!/bin/sh


###############################################################################
#
# This script compress the necessary javascript files.
# The ouput will be placed in the DESTDIR folder.
#
# JAVA > 1.4 is required
#
###############################################################################

if [ $# -eq 1 ]; then

echo "...Minifying javascript files"
SCRIPTDIR="$( cd "$( dirname "$0" )" && pwd )"
JS_PATH=$SCRIPTDIR/../web/resources/javascript  
DESTDIR=$1

mkdir -p $DESTDIR
java -jar ../tools/closure/closure-compiler-v20181210.jar --js $JS_PATH/C*.js $JS_PATH/start.js --create_source_map $DESTDIR/ezwg.js.map --source_map_format=V3  --js_output_file $DESTDIR/ezwg-min.js
echo "//# sourceMappingURL=ezwg-min.js.map" >> $DESTDIR/ezwg-min.js

else

echo "# Usage: compress_javascript.sh DESTDIR"

fi
