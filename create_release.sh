 #!/bin/sh

if [ $# -eq 0 ]; then

echo " "
echo "###############################################################################"
echo "#"
echo "# Usage: create_realease DESTINATION_FOLDER"
echo "# This script copies all the necessary files to the provided DESTINATION_FOLDER"
echo "#"
echo "# In order to create a release you need to set the folowing env variables"
echo "# + QDTIR: path to Qt"
echo "# + MAGICK_HOME: path to ImageMagick"
echo "#"
echo "# NOTE: you need to minimize some files located in ./Web/ressources/javascript/"
echo "# and placethem in ./Web/ressources/javascript/"
echo "# You can run compress_js.bat script for that purpose."
echo "#"
echo "###############################################################################"
echo " "

else

BUILDTARGET="./Designer/release/bin/GalleryDesigner"

mkdir --parents $1

mkdir --parents $1/res/ressources/css
mkdir --parents $1/res/ressources/php
mkdir --parents $1/res/ressources/images/smileys
mkdir --parents $1/res/ressources/javascript/jquery/plugins
mkdir --parents $1/res/ressources/javascript/tools
mkdir --parents $1/res/ressources/javascript/core

cp ./Web/index.html $1/res
cp ./Web/ressources/php/*.php $1/res/ressources/php
cp ./Web/ressources/images/*.* $1/res/ressources/images
cp ./Web/ressources/images/smileys/*.* $1/res/ressources/images/smileys
cp ./Web/ressources/css/*.css $1/res/ressources/css
cp ./Web/ressources/javascript/jquery/*.js $1/res/ressources/javascript/jquery
cp ./Web/ressources/javascript/jquery/plugins/*.js $1/res/ressources/javascript/jquery/plugins
cp ./Web/ressources/javascript/tools/*.js $1/res/ressources/javascript/tools
cp ./Web/ressources/javascript/min-js/*-min.js $1/res/ressources/javascript/
cp ./Web/ressources/javascript/min-js/core/*-min.js $1/res/ressources/javascript/core

cp -R --parents ./Skins/* $1/ 
mv $1/Skins $1/skins

cp $BUILDTARGET $1

cd $1
echo "DONE!"


fi


