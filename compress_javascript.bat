echo off
cls
echo.
echo ###############################################################################
echo #
echo # This script compress the necessary javascript files.
echo # The ouput will be placed in the DESTDIR folder.
echo #
echo # You need to install the YUI Compressor and set the YUI_COMPRESSOR environment
echo # variable to indicate the path to the .jar file.
echo # You need YUI compressor 2.4.6 or later.
echo # See http://developer.yahoo.com/yui/compressor/ for details about YUI
echo #
echo ###############################################################################
echo.

if [%1] == [] goto :error
if not [%2] == [] goto :error

:process

SET JS_PATH=web\ressources\javascript

md %1\min-js\core

echo on

java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %1\common-min.js  %JS_PATH%\common.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %1\image-min.js  %JS_PATH%\image.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %1\index-min.js  %JS_PATH%\index.js
                                            
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %1\core\classDisplayManager-min.js  %JS_PATH%\core\classDisplayManager.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %1\core\classFifo-min.js  %JS_PATH%\core\classFifo.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %1\core\classHashTable-min.js  %JS_PATH%\core\classHashTable.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %1\core\classObjetAjustable-min.js  %JS_PATH%\core\classObjetAjustable.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %1\core\classTablePhoto-min.js  %JS_PATH%\core\classTablePhoto.js

pause

goto :end

:error
echo.
echo # Usage: compress_javascript DESTDIR
echo.

:end
