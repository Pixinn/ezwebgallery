 #!/bin/sh

###############################################################################
#
# Usage: deploy BINARY_FOLDER DESTINATION_FOLDER
#
# This script build the project, compress the js files and copies the resulting
# files into the provided destination folder.
#
# The package yui-compressor is required to compress the javascript files.
#
# The binary folder is defined in the .pro file.
###############################################################################

echo "DEPRECATED : make will deploy in the build/release folder."

if [ $# -eq 0 ]; then

echo "Usage: deploy BINARY DESTINATION_FOLDER."

else

TEMPDIR=.min-js
qmake ./src/EZWebGallery.pro
make -f Makefile.Release
./compress_javascript.sh $TEMPDIR
./create_release.sh $1 $TEMPDIR $2

echo ""
echo "###############################################################################"
echo "#"
echo "# Remember that EZWebGallery needs the following packages to run properly:"
echo "#"
echo "#     * libqtcore4 (version 4.6+)"
echo "#     * libqtgui4 (version 4.6+)"
echo "#     * libqt4-xml (version 4.6+)"
echo "#     * libmagick++"
echo "#"
echo "###############################################################################"
echo ""

fi
