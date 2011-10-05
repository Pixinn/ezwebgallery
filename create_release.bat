cls

echo off

if [%1] == [] goto :error
if not [%2] == [] goto :error

:process

md %1

md %1\res
md %1\res\ressources
md %1\res\ressources\php
md %1\res\ressources\images
md %1\res\ressources\images\smileys
md %1\res\ressources\css
md %1\res\ressources\javascript\jquery
md %1\res\ressources\javascript\jquery\plugins
md %1\res\ressources\javascript\tools
md %1\res\ressources\javascript\core

copy ".\Web\index.html" %1\res
copy ".\Web\ressources\php\*.php" %1\res\ressources\php
copy ".\Web\ressources\images\*.*" %1\res\ressources\images
copy ".\Web\ressources\images\smileys\*.*" %1\res\ressources\images\smileys
copy ".\Web\ressources\css\*.css" %1\res\ressources\css
copy ".\Web\ressources\javascript\jquery\*.js" %1\res\ressources\javascript\jquery
copy ".\Web\ressources\javascript\jquery\plugins\*.js" %1\res\ressources\javascript\jquery\plugins
copy ".\Web\ressources\javascript\tools\*.js" %1\res\ressources\javascript\tools
copy ".\Web\ressources\javascript\min-js\*-min.js" %1\res\ressources\javascript\
copy ".\Web\ressources\javascript\min-js\core\*-min.js" %1\res\ressources\javascript\core

md %1\skins
xcopy /e .\Skins %1\skins\ 

copy .\Designer\release\bin\*.exe %1

copy %QTDIR%\bin\QtCore4.dll %1
copy %QTDIR%\bin\QtGui4.dll %1
copy %QTDIR%\bin\QtXml4.dll %1
md %1\imageformats
copy %QTDIR%\plugins\imageformats\qgif4.dll %1\imageformats
copy %QTDIR%\plugins\imageformats\qico4.dll %1\imageformats
copy %QTDIR%\plugins\imageformats\qjpeg4.dll %1\imageformats
copy %QTDIR%\plugins\imageformats\qmng4.dll %1\imageformats
copy %QTDIR%\plugins\imageformats\qsvg4.dll %1\imageformats
copy %QTDIR%\plugins\imageformats\qtiff4.dll %1\imageformats

copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_bzlib_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_jpeg_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_lcms_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_magick_.dll %1
copy "%MAGICK_HOME%\VisualMagick\bin\CORE_RL_Magick++_.dll" %1
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_png_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_tiff_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_ttf_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_wand_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_xlib_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\CORE_RL_zlib_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_jpeg_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_png_.dll %1
copy %MAGICK_HOME%\VisualMagick\bin\IM_MOD_RL_tiff_.dll %1

cd %1
echo DONE!
goto :end

:error

echo.
echo ###############################################################################
echo #
echo # Usage create_realease DESTINATION_FOLDER
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

