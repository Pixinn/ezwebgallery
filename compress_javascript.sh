 #!/bin/sh

echo ""
echo "###############################################################################"
echo "#"
echo "# This script compress the necessary javascript files."
echo "# The ouput will be placed in the Web/ressources/javascript/min-js/ folder."  
echo "#"
echo "# You need to install the YUI Compressor."
echo "# On Ubuntu, you can download the package \"yui-compressor\"."
echo "# See http://developer.yahoo.com/yui/compressor/ for details about YUI"
echo "#"
echo "###############################################################################"
echo ""


export JS_PATH='./web/ressources/javascript'

mkdir $JS_PATH/min-js/
mkdir $JS_PATH/min-js/core


yui-compressor --charset UTF-8 -o $JS_PATH/min-js/common-min.js  $JS_PATH/common.js
yui-compressor --charset UTF-8 -o $JS_PATH/min-js/image-min.js   $JS_PATH/image.js
yui-compressor --charset UTF-8 -o $JS_PATH/min-js/index-min.js   $JS_PATH/index.js
                                
yui-compressor --charset UTF-8 -o $JS_PATH/min-js/core/classDisplayManager-min.js    $JS_PATH/core/classDisplayManager.js
yui-compressor --charset UTF-8 -o $JS_PATH/min-js/core/classFifo-min.js              $JS_PATH/core/classFifo.js
yui-compressor --charset UTF-8 -o $JS_PATH/min-js/core/classHashTable-min.js         $JS_PATH/core/classHashTable.js
yui-compressor --charset UTF-8 -o $JS_PATH/min-js/core/classObjetAjustable-min.js    $JS_PATH/core/classObjetAjustable.js
yui-compressor --charset UTF-8 -o $JS_PATH/min-js/core/classTablePhoto-min.js        $JS_PATH/core/classTablePhoto.js

echo "DONE."
