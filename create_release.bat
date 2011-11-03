cls

echo off

if [%3] == [] goto :error
if not [%4] == [] goto :error

:process

md %3

md %3\res
md %3\res\ressources
md %3\res\ressources\php
md %3\res\ressources\images
md %3\res\ressources\images\smileys
md %3\res\ressources\css
md %3\res\ressources\javascript\jquery
md %3\res\ressources\javascript\jquery\plugins
md %3\res\ressources\javascript\tools
md %3\res\ressources\javascript\core

copy ".\Web\index.html" %3\res
copy ".\Web\ressources\php\*.php" %3\res\ressources\php
copy ".\Web\ressources\images\*.*" %3\res\ressources\images
copy ".\Web\ressources\images\smileys\*.*" %3\res\ressources\images\smileys
copy ".\Web\ressources\css\*.css" %3\res\ressources\css
copy ".\Web\ressources\javascript\jquery\*.js" %3\res\ressources\javascript\jquery
copy ".\Web\ressources\javascript\jquery\plugins\*.js" %3\res\ressources\javascript\jquery\plugins
copy ".\Web\ressources\javascript\tools\*.js" %3\res\ressources\javascript\tools
copy "%2\*-min.js" %3\res\ressources\javascript\
copy "%2\core\*-min.js" %3\res\ressources\javascript\core

md %3\skins
xcopy /e .\Skins %3\skins\ 

copy %QTDIR%\bin\QtCore4.dll %3
copy %QTDIR%\bin\QtGui4.dll %3
copy %QTDIR%\bin\QtXml4.dll %3
md   %3\imageformats
copy %QTDIR%\plugins\imageformats\qgif4.dll %3\imageformats
copy %QTDIR%\plugins\imageformats\qico4.dll %3\imageformats
copy %QTDIR%\plugins\imageformats\qjpeg4.dll %3\imageformats
copy %QTDIR%\plugins\imageformats\qmng4.dll %3\imageformats
copy %QTDIR%\plugins\imageformats\qsvg4.dll %3\imageformats
copy %QTDIR%\plugins\imageformats\qtiff4.dll %3\imageformats

copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_bzlib_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_jpeg_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_lcms_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_magick_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_Magick++_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_png_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_tiff_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_ttf_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_wand_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_xlib_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_zlib_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_jpeg_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_png_.dll %3
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_tiff_.dll %3

copy %1 %3

cd %3
echo DONE!
goto :end

:error

echo.
echo ###############################################################################
echo #
echo # Usage create_realease BINARY MINIMIZEDJS_FOLDER DESTINATION_FOLDER
echo #
echo # In order to create a release you need to set the folowing env variables
echo # + QDTIR: path to Qt
echo # + MAGICK_HOME: path to ImageMagick
echo #
echo # This script copies all the necessary files to the provided DESTINATION_FOLDER
echo #
echo # NOTE: you need to minimize some files located in .\Web\ressources\javascript\
echo # and place them in .\Web\ressources\javascript\
echo # You can run compress_js.bat script for that purpose.
echo #
echo ###############################################################################
echo.
echo on

:end
pause

