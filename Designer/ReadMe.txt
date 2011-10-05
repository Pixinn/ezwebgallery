
Read the folowing instructions in oder to build EZWebGallery more easily.

***************
Prerequisites *
***************

* Qt : https://qt.nokia.com/downloads/
* ImageMagick : http://www.imagemagick.org/script/download.php
    + If you want to build ImageMagick from source to enhance performance, use the following options : --disable-static --with-quantum-depth=8 --without-perl
    + Windows only : ImageMagick cannot be built using Visual Studio Express.

    
*************************
Common to all platforms *
*************************

NOTE: in order to run EZWebGallery, the "res" and "skins" directories need to be placed in the binary folder. Thus, YOU HAVE TO manually copy them before testing your builds. Read the "ReadMe.txt" file of the parent folder for further details about generating these folders using the two provided scripts.

* The project use "svnrev" to be aware of the *global* revion number. You have to add the Rev Date and Id to the svn:keywords property of each newly added file. svnrev is used as the first pre-building step.
  Add the following lines at the beginning of each file :
  
        /*
		* This header is mandatory on all files of the project in order to track the global svn revision usin svnrev
		* $Id: ReadMe.txt 130 2011-09-25 22:24:43Z xtof $
		*/
			
* If you want to see and use the custom widgets in Qt Designer:
    1 - Build the widgets with the project widgets/EzwWidgets.pro
	2 - Copy the resulting shared libs in the proper directory. Usualy [QTDIR]/plugins/designer/
        ex in Linux : sudo cp libPluginName.so /usr/share/qt4/plugins/designer/libPluginName.so


*********
Linux   *
*********

* You have to install the dev version of Qt and ImageMagick with these packages :
    libqt4-dev
    libmagick++-dev

* You'll have to give "execute permission" to ressources/linux/svnrev

* If your editor has trouble displaying the accentuated characters, know that te files are encoded using the Windows-1252 (CP-1252) character encoding.


*********
MacOSX *
*********

* You'll have to give "execute permission" to ressources/linux/svnrev

* If your editor has trouble displaying the accentuated characters, know that te files are encoded using the Windows-1252 (CP-1252) character encoding.



*********
Windows *
*********

* Its easier to build EZWebGallery with VC2008 and VC2010 than QtCreator.

* define the folling environment variables :
    + QMAKESPEC = win32-msvc2008 (if building using Visual Studio 2008)
    + QTDIR = [folder where you installed Qt]
	+ MAGICK_HOME = [folder where you installed ImageMagick]
    
* Some dlls from Qt and ImageMagick are mandatory and must be placed in the .exe directory or in the PATH.
    + DEBUG:
        CORE_DB_bzlib_.dll
        CORE_DB_jpeg_.dll
        CORE_DB_lcms_.dll
        CORE_DB_Magick++_.dll
        CORE_DB_magick_.dll
        CORE_DB_png_.dll
        CORE_DB_tiff_.dll
        CORE_DB_ttf_.dll
        CORE_DB_wand_.dll
        CORE_DB_xlib_.dll
        CORE_DB_zlib_.dll
        IM_MOD_DB_jpeg_.dll
        IM_MOD_DB_png_.dll
        IM_MOD_DB_tiff_.dll
        QtCored4.dll
        QtGuid4.dll
        QtXmld4.dll
        imageformats\qgifd4.dll
        imageformats\qicod4.dll
        imageformats\qjpegd4.dll
        imageformats\qmngd4.dll
        imageformats\qsvgd4.dll
        imageformats\qtiffd4.dll
    + RELEASE:   
        CORE_RL_bzlib_.dll
        CORE_RL_jpeg_.dll
        CORE_RL_lcms_.dll
        CORE_RL_Magick++_.dll
        CORE_RL_magick_.dll
        CORE_RL_png_.dll
        CORE_RL_tiff_.dll
        CORE_RL_ttf_.dll
        CORE_RL_wand_.dll
        CORE_RL_xlib_.dll
        CORE_RL_zlib_.dll
        IM_MOD_RL_jpeg_.dll
        IM_MOD_RL_png_.dll
        IM_MOD_RL_tiff_.dll
        QtCore4.dll
        QtGui4.dll
        QtXml4.dll
        imageformats\qgif4.dll
        imageformats\qico4.dll
        imageformats\qjpeg4.dll
        imageformats\qmng4.dll
        imageformats\qsvg4.dll
        imageformats\qtiff4.dll        
    
    + NOTE: These dll must be provided when distributing EZWebGallery. Use "create_release.bat" to generate a proper distribution including those libraries.    

* If you wish to compile Qt yourself, you can use the folowing options :
configure -opensource -sse2 -no-webkit -dsp no-opengl -debug-and-release -mp

*Check that the "additionnal Include directories" of the project are set as below:
.\;.\global;.\ui;.\widgets;$(MAGICK_HOME)\;$(MAGICK_HOME)\Magick++\lib;%(AdditionalIncludeDirectories);$(QTDIR)\lib;$(QTDIR)\mkspecs\win32-msvc2010;$(QTDIR)\include\;$(QTDIR)\include\Qt;$(QTDIR)\include\QtCore;$(QTDIR)\include\QtGui;$(QTDIR)\include\QtXml

*Check that the Additional Library Directories" of the project are set as below: $(QTDIR)\lib;%(AdditionalLibraryDirectories);$(MAGICK_HOME)\VisualMagick\lib;

