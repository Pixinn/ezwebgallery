cls
echo off
echo.
echo ###############################################################################
echo #
echo # This script compress the necessary javascript files.
echo # The ouput will be placed in the Web/ressources/javascript/min-js/ folder.
echo #
echo # You need to install the YUI Compressor and set the YUI_COMPRESSOR environment
echo # variable to indicate the path to the .jar file.
echo # You need YUI compressor 2.4.6 or later.
echo # See http://developer.yahoo.com/yui/compressor/ for details about YUI
echo #
echo ###############################################################################
echo.

SET JS_PATH=Web\ressources\javascript

md %JS_PATH%\min-js\core

echo on

java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %JS_PATH%\min-js\common-min.js  %JS_PATH%\common.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %JS_PATH%\min-js\image-min.js  %JS_PATH%\image.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %JS_PATH%\min-js\index-min.js  %JS_PATH%\index.js
                                            
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %JS_PATH%\min-js\core\classDisplayManager-min.js  %JS_PATH%\core\classDisplayManager.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %JS_PATH%\min-js\core\classFifo-min.js  %JS_PATH%\core\classFifo.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %JS_PATH%\min-js\core\classHashTable-min.js  %JS_PATH%\core\classHashTable.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %JS_PATH%\min-js\core\classObjetAjustable-min.js  %JS_PATH%\core\classObjetAjustable.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %JS_PATH%\min-js\core\classTablePhoto-min.js  %JS_PATH%\core\classTablePhoto.js

pause
