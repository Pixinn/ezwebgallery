#!/bin/sh


###############################################################################
#
# This script compress the necessary javascript files.
# The ouput will be placed in the DESTDIR folder.
#
# You need to install the YUI Compressor.
# On Ubuntu, you can download the package \"yui-compressor\".
# See http://developer.yahoo.com/yui/compressor/ for details about YUI
#
###############################################################################

if [ $# -eq 1 ]; then

echo "...Minifying javascript files"
SCRIPTDIR="$( cd "$( dirname "$0" )" && pwd )"
JS_PATH=$SCRIPTDIR/../web/ressources/javascript  
DESTDIR=$1

mkdir -p $DESTDIR
cat $JS_PATH/CEvent.js $JS_PATH/CHashTable.js $JS_PATH/CPhoto.js $JS_PATH/CFifo.js $JS_PATH/CStorage.js $JS_PATH/CUserInterractions.js $JS_PATH/CCarrousel.js $JS_PATH/CPhotoLoader.js $JS_PATH/CDisplay.js $JS_PATH/CScroller.js  $JS_PATH/CProgressBar.js $JS_PATH/CMosaic.js $JS_PATH/start.js > /tmp/ezwg.js
yui-compressor --charset UTF-8 -o $DESTDIR/ezwg-min.js /tmp/ezwg.js

else

echo "# Usage: compress_javascript.sh DESTDIR"

fi
