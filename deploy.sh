 #!/bin/sh

echo ""
echo "###############################################################################"
echo "#"
echo "# Usage: deploy BINARY_FOLDER DESTINATION_FOLDER"
echo "#"
echo "# This script build the project, compress the js files and copies the resulting"
echo "# files into the provided destination folder."
echo "#"
echo "# The package yui-compressor is required to compress the javascript files."
echo "#"
echo "# The binary folder is defined in the .pro file."
echo "###############################################################################"
echo ""

if [ $# -eq 0 ]; then

echo "Usage: deploy BINARY_FOLDER DESTINATION_FOLDER."

else

qmake ./Designer/GalleryDesigner.pro
make -f Makefile.Release
./compress_javascript.sh
./create_release.sh $1 $2

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
