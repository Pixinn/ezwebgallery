 #!/bin/sh

if [ $# -eq 0 ]; then

echo " "
echo "###############################################################################"
echo "#"
echo "# Usage: create_realease BINARY_FOLDER DESTINATION_FOLDER"
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


mkdir --parents $2

mkdir --parents $2/res/ressources/css
mkdir --parents $2/res/ressources/php
mkdir --parents $2/res/ressources/images/smileys
mkdir --parents $2/res/ressources/javascript/jquery/plugins
mkdir --parents $2/res/ressources/javascript/tools
mkdir --parents $2/res/ressources/javascript/core

cp ./Web/index.html $2/res
cp ./Web/ressources/php/*.php $2/res/ressources/php
cp ./Web/ressources/images/*.* $2/res/ressources/images
cp ./Web/ressources/images/smileys/*.* $2/res/ressources/images/smileys
cp ./Web/ressources/css/*.css $2/res/ressources/css
cp ./Web/ressources/javascript/jquery/*.js $2/res/ressources/javascript/jquery
cp ./Web/ressources/javascript/jquery/plugins/*.js $2/res/ressources/javascript/jquery/plugins
cp ./Web/ressources/javascript/tools/*.js $2/res/ressources/javascript/tools
cp ./Web/ressources/javascript/min-js/*-min.js $2/res/ressources/javascript/
cp ./Web/ressources/javascript/min-js/core/*-min.js $2/res/ressources/javascript/core

cp -R --parents ./Skins/* $2/ 
mv $2/Skins $2/skins

cp $1/GalleryDesigner $2

cd $2
echo "DONE!"


fi


