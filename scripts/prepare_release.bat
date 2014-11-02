cls

echo off

if [%2] == [] goto :error
if not [%3] == [] goto :error

:process

set DATADIR=..\deployment\common
set TEMPDIR=..\..\distribution\windows

REM --- CREATING DATA FOLDERS
md %DATADIR%\data
md %DATADIR%\data\resources
md %DATADIR%\data\resources\php
md %DATADIR%\data\resources\images
md %DATADIR%\data\resources\images\smileys
md %DATADIR%\data\resources\css
md %DATADIR%\data\resources\css\opt
md %DATADIR%\data\resources\javascript\jquery
md %DATADIR%\data\resources\javascript\jquery\plugins
md %DATADIR%\data\resources\javascript\hammer
md %DATADIR%\data\resources\javascript\modernizr
md %DATADIR%\data\resources\javascript\tools

REM --- MINIMZING JS
call compress_javascript.bat %DATADIR%\data\resources\javascript

echo off

REM --- COPYING WEB FILES TO DATA
copy "..\Web\index.html" %DATADIR%\data
copy "..\Web\resources\php\*.php" %DATADIR%\data\resources\php
copy "..\Web\resources\images\*.*" %DATADIR%\data\resources\images
copy "..\Web\resources\images\smileys\*.*" %DATADIR%\data\resources\images\smileys
copy "..\Web\resources\css\*.css" %DATADIR%\data\resources\css
copy "..\Web\resources\css\opt\*.css" %DATADIR%\data\resources\css\opt
copy "..\Web\resources\javascript\jquery\*.js" %DATADIR%\data\resources\javascript\jquery
copy "..\Web\resources\javascript\jquery\plugins\*.js" %DATADIR%\data\resources\javascript\jquery\plugins
copy "..\Web\resources\javascript\hammer\*.js" %DATADIR%\data\resources\javascript\hammer
copy "..\Web\resources\javascript\modernizr\*.js" %DATADIR%\data\resources\javascript\modernizr
copy "..\Web\resources\javascript\tools\*.js" %DATADIR%\data\resources\javascript\tools


REM --- COPYING DATA AND SKINS TO TEMP DIR
md %TEMPDIR%
md %TEMPDIR%\data
xcopy /e %DATADIR%\data %TEMPDIR%\data\
md %TEMPDIR%\skins
xcopy /e ..\Skins %TEMPDIR%\skins\

REM --- COPYING DLL TO TEMP DIR
copy %QTDIR%\bin\QtCore4.dll %TEMPDIR%
copy %QTDIR%\bin\QtGui4.dll %TEMPDIR%
copy %QTDIR%\bin\QtXml4.dll %TEMPDIR%
md   %TEMPDIR%\imageformats
copy %QTDIR%\plugins\imageformats\qgif4.dll %TEMPDIR%\imageformats
copy %QTDIR%\plugins\imageformats\qico4.dll %TEMPDIR%\imageformats
copy %QTDIR%\plugins\imageformats\qjpeg4.dll %TEMPDIR%\imageformats
copy %QTDIR%\plugins\imageformats\qmng4.dll %TEMPDIR%\imageformats
copy %QTDIR%\plugins\imageformats\qsvg4.dll %TEMPDIR%\imageformats
copy %QTDIR%\plugins\imageformats\qtiff4.dll %TEMPDIR%\imageformats

copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_bzlib_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_jpeg_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_lcms_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_magick_.dll %TEMPDIR%
copy "%MAGICK_HOME%\VisualMagick\bin\CORE_RL_Magick++_.dll" %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_png_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_tiff_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_ttf_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_wand_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_xlib_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_zlib_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_jpeg_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_png_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_tiff_.dll %TEMPDIR%

REM --- COPYING THE BINARY FILE
copy %2 %TEMPDIR%

REM --- EXPORTING VERSION NUMBERS TO INSTALLER GENERATOR
SET /P FULLTXT=<..\src\builddate.h
SET BUILD_DATE=%FULLTXT:~20,10%
echo BUILD_DATE: %BUILD_DATE%
echo !define BUILD_DATE "%BUILD_DATE%" > ..\deployment\windows\version.nsh
echo !define VERSION "%1" >> ..\deployment\windows\version.nsh


echo DONE!
goto :end

:error

echo.
echo ###############################################################################
echo #
echo # Usage prepare_realease VERSION_NUMBER(x.x) PATH_TO_BINARY
echo #
echo # In order to create a release you need to set the folowing env variables
echo # + QDTIR: path to Qt
echo # + MAGICK_HOME: path to ImageMagick
echo #
echo # This script copies all the necessary files to ..\..\distribution\windows
echo #
echo ###############################################################################
echo.
echo on

:end
pause

