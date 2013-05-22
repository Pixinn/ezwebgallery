echo off

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

rem script dir: %~dp0 ; read: http://ss64.com/nt/syntax-args.html
SET JS_PATH=%~dp0..\web\ressources\javascript

md %1

echo on
copy /b %JS_PATH%\CEvent.js+%JS_PATH%\CHashTable.js+%JS_PATH%\CPhoto.js+%JS_PATH%\CFifo.js+%JS_PATH%\CStorage.js+%JS_PATH%\CUserInterractions.js+%JS_PATH%\CFrameFactory.js+%JS_PATH%\CCarrousel.js+%JS_PATH%\CPhotoLoader.js+%JS_PATH%\CDisplay.js+%JS_PATH%\CScroller.js+%JS_PATH%\CProgressBar.js+%JS_PATH%\CMosaic.js+%JS_PATH%\start.js ezwg.js
java -jar  %YUI_COMPRESSOR% --charset UTF-8 -o %1\ezwg-min.js ezwg.js

echo off
del ezwg.js
goto :end
 
:error
echo.
echo # Usage: compress_javascript DESTDIR
echo.

:end
