echo off

echo.
echo ###############################################################################
echo #
echo # This script compress the necessary javascript files.
echo # The ouput will be placed in the DESTDIR folder.
echo #
echo ###############################################################################
echo.

if [%1] == [] goto :error
if not [%2] == [] goto :error

echo %1\ezwg-min.js

:process

rem script dir: %~dp0 ; read: http:\\ss64.com\nt\syntax-args.html
SET JS_PATH=..\web\resources\javascript

md %1

echo on

java -jar ..\tools\closure\closure-compiler-v20181210.jar --js %JS_PATH%\C*.js %JS_PATH%\start.js --create_source_map %1\ezwg-min.js.map --source_map_format=V3  --js_output_file %1\ezwg-min.js


echo off
goto :end
 
:error
echo.
echo # Usage: compress_javascript DESTDIR
echo.

:end
