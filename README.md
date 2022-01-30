
Read the following for a better understanding on how to generate a release of EZWebGallery.

## UBUNTU or DEBIAN:

install the required packages:
```bash
    sudo apt-get install libmagick++-6-headers libmagick++-6.q16-dev qtbase5-dev libqt5opengl5-dev libqt5webkit5-dev 
```
build the sources then install ezwebgallery:
```bash
    sudo qmake && make install
```

AFTERWARDS, you can build a functional DEBUG ezwebgallery.

## WINDOWS:

Python3 and Java are required!
*QTDIR* and *MAGICK_HOME* environment variable set to Qt and ImageMagick directories. You may have to build ImageMagick from sources.<br/>
The latets release links against *ImageMagick-6.9.2-5* and *Qt-5.9.9*

1. Build a release target using the Visual Studio project located in src.
2. Run prepare_release.bat from the "script" subfolder.<br/>
   A proper build of EZWebGallery will be generated in ../distribution/windows
3. Optionnaly you can build a full installer, based on [NSIS](https://nsis.sourceforge.io/Download), with deployment/windows/Setup_EZWebGallery.nsi 


### IMPORTANT NOTE
In order to run EZWebGallery, the directories "data" and "skins"
have to be placed alongside the binary. Thus, if you want to debug a binary,
YOU HAVE TO manually copy them into the debug directory.



