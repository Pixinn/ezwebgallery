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

mkdir -p $DESTDIR/core

yui-compressor --charset UTF-8 -o $DESTDIR/common-min.js  $JS_PATH/common.js
yui-compressor --charset UTF-8 -o $DESTDIR/image-min.js   $JS_PATH/image.js
yui-compressor --charset UTF-8 -o $DESTDIR/index-min.js   $JS_PATH/index.js
                                
yui-compressor --charset UTF-8 -o $DESTDIR/core/classDisplayManager-min.js    $JS_PATH/core/classDisplayManager.js
yui-compressor --charset UTF-8 -o $DESTDIR/core/classFifo-min.js              $JS_PATH/core/classFifo.js
yui-compressor --charset UTF-8 -o $DESTDIR/core/classHashTable-min.js         $JS_PATH/core/classHashTable.js
yui-compressor --charset UTF-8 -o $DESTDIR/core/classObjetAjustable-min.js    $JS_PATH/core/classObjetAjustable.js
yui-compressor --charset UTF-8 -o $DESTDIR/core/classTablePhoto-min.js        $JS_PATH/core/classTablePhoto.js

else

echo "# Usage: compress_javascript.sh DESTDIR"

fi
