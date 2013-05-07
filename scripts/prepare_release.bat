cls

echo off

if [%1] == [] goto :error
if not [%2] == [] goto :error

:process

set DATADIR=..\deployment\common
set TEMPDIR=..\..\distribution\windows

REM --- CREATING DATA FOLDERS
md %DATADIR%\data
md %DATADIR%\data\ressources
md %DATADIR%\data\ressources\php
md %DATADIR%\data\ressources\images
md %DATADIR%\data\ressources\images\smileys
md %DATADIR%\data\ressources\css
md %DATADIR%\data\ressources\javascript\jquery
md %DATADIR%\data\ressources\javascript\jquery\plugins
md %DATADIR%\data\ressources\javascript\tools

REM --- MINIMZING JS
call compress_javascript.bat %DATADIR%\data\ressources\javascript

echo off

REM --- COPYING WEB FILES TO DATA
copy "..\Web\index.html" %DATADIR%\data
copy "..\Web\ressources\php\*.php" %DATADIR%\data\ressources\php
copy "..\Web\ressources\images\*.*" %DATADIR%\data\ressources\images
copy "..\Web\ressources\images\smileys\*.*" %DATADIR%\data\ressources\images\smileys
copy "..\Web\ressources\css\*.css" %DATADIR%\data\ressources\css
copy "..\Web\ressources\javascript\jquery\*.js" %DATADIR%\data\ressources\javascript\jquery
copy "..\Web\ressources\javascript\jquery\plugins\*.js" %DATADIR%\data\ressources\javascript\jquery\plugins
copy "..\Web\ressources\javascript\tools\*.js" %DATADIR%\data\ressources\javascript\tools


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
echo on
copy %1 %TEMPDIR%

cd %TEMPDIR%
echo DONE!
goto :end

:error

echo.
echo ###############################################################################
echo #
echo # Usage create_realease BINARY
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

