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
md %DATADIR%\data\resources\javascript\screenfull
md %DATADIR%\data\resources\javascript\tools

REM --- MINIMZING JS
call compress_javascript.bat %DATADIR%\data\resources\javascript

echo off

REM --- COPYING WEB FILES TO DATA
copy "..\web\index.html" %DATADIR%\data
copy "..\web\resources\php\*.php" %DATADIR%\data\resources\php
copy "..\web\resources\images\*.*" %DATADIR%\data\resources\images
copy "..\web\resources\images\smileys\*.*" %DATADIR%\data\resources\images\smileys
copy "..\web\resources\css\*.css" %DATADIR%\data\resources\css
copy "..\web\resources\css\opt\*.css" %DATADIR%\data\resources\css\opt
copy "..\web\resources\javascript\jquery\*.js" %DATADIR%\data\resources\javascript\jquery
copy "..\web\resources\javascript\jquery\plugins\*.js" %DATADIR%\data\resources\javascript\jquery\plugins
copy "..\web\resources\javascript\hammer\*.js" %DATADIR%\data\resources\javascript\hammer
copy "..\web\resources\javascript\modernizr\*.js" %DATADIR%\data\resources\javascript\modernizr
copy "..\web\resources\javascript\screenfull\*.js" %DATADIR%\data\resources\javascript\screenfull
copy "..\web\resources\javascript\tools\*.js" %DATADIR%\data\resources\javascript\tools


REM --- COPYING DATA AND SKINS TO TEMP DIR
md %TEMPDIR%
md %TEMPDIR%\data
xcopy /e %DATADIR%\data %TEMPDIR%\data\
md %TEMPDIR%\skins
xcopy /e ..\skins %TEMPDIR%\skins\

REM --- COPYING DLL TO TEMP DIR
%QTDIR%\bin\windeployqt.exe --release --dir %TEMPDIR% --no-compiler-runtime %2\EZWebGallery.exe

copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_bzlib_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_freetype_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_glib_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_jpeg_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_lcms_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_libxml_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_lqr_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_magick_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_Magick"++"_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_png_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_tiff_.dll %TEMPDIR%
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_wand_.dll %TEMPDIR%
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
echo # ! DO NOT RUN UNDER POWERSHELL !
echo #
echo ###############################################################################
echo.
echo on

:end
pause

