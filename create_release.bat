cls

echo off

if [%2] == [] goto :error
if not [%3] == [] goto :error

:process

md %2

md %2\res
md %2\res\ressources
md %2\res\ressources\php
md %2\res\ressources\images
md %2\res\ressources\images\smileys
md %2\res\ressources\css
md %2\res\ressources\javascript\jquery
md %2\res\ressources\javascript\jquery\plugins
md %2\res\ressources\javascript\tools
md %2\res\ressources\javascript\core

copy ".\Web\index.html" %2\res
copy ".\Web\ressources\php\*.php" %2\res\ressources\php
copy ".\Web\ressources\images\*.*" %2\res\ressources\images
copy ".\Web\ressources\images\smileys\*.*" %2\res\ressources\images\smileys
copy ".\Web\ressources\css\*.css" %2\res\ressources\css
copy ".\Web\ressources\javascript\jquery\*.js" %2\res\ressources\javascript\jquery
copy ".\Web\ressources\javascript\jquery\plugins\*.js" %2\res\ressources\javascript\jquery\plugins
copy ".\Web\ressources\javascript\tools\*.js" %2\res\ressources\javascript\tools
copy ".\Web\ressources\javascript\min-js\*-min.js" %2\res\ressources\javascript\
copy ".\Web\ressources\javascript\min-js\core\*-min.js" %2\res\ressources\javascript\core

md %2\skins
xcopy /e .\Skins %2\skins\ 

copy %1\*.exe %2

copy %QTDIR%\bin\QtCore4.dll %2
copy %QTDIR%\bin\QtGui4.dll %2
copy %QTDIR%\bin\QtXml4.dll %2
md   %2\imageformats
copy %QTDIR%\plugins\imageformats\qgif4.dll %2\imageformats
copy %QTDIR%\plugins\imageformats\qico4.dll %2\imageformats
copy %QTDIR%\plugins\imageformats\qjpeg4.dll %2\imageformats
copy %QTDIR%\plugins\imageformats\qmng4.dll %2\imageformats
copy %QTDIR%\plugins\imageformats\qsvg4.dll %2\imageformats
copy %QTDIR%\plugins\imageformats\qtiff4.dll %2\imageformats

copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_bzlib_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_jpeg_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_lcms_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_magick_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_Magick++_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_png_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_tiff_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_ttf_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_wand_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_xlib_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_zlib_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_jpeg_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_png_.dll %2
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_tiff_.dll %2

cd %2
echo DONE!
goto :end

:error

echo.
echo ###############################################################################
echo #
echo # Usage create_realease BINARY_FOLDER DESTINATION_FOLDER
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

