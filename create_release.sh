 #!/bin/sh

if [ $# -eq 0 ]; then

echo " "
echo "###############################################################################"
echo "#"
echo "# Usage: create_release BINARY_FOLDER DESTINATION_FOLDER"
echo "# This script copies all the necessary files to the provided DESTINATION_FOLDER"
echo "#"
echo "# In order to create a release you need to set the folowing env variables"
echo "# + QDTIR: path to Qt"
echo "# + MAGICK_HOME: path to ImageMagick"
echo "#"
echo "# NOTE: you need to minimize some files located in ./web/ressources/javascript/"
echo "# and placethem in ./web/ressources/javascript/"
echo "# You can run compress_js.bat script for that purpose."
echo "#"
echo "###############################################################################"
echo " "

else

export BINARY_FILENAME='EZWebGallery'

mkdir --parents $2

mkdir --parents $2/res/ressources/css
mkdir --parents $2/res/ressources/php
mkdir --parents $2/res/ressources/images/smileys
mkdir --parents $2/res/ressources/javascript/jquery/plugins
mkdir --parents $2/res/ressources/javascript/tools
mkdir --parents $2/res/ressources/javascript/core

cp ./web/index.html $2/res
cp ./web/ressources/php/*.php $2/res/ressources/php
cp ./web/ressources/images/*.* $2/res/ressources/images
cp ./web/ressources/images/smileys/*.* $2/res/ressources/images/smileys
cp ./web/ressources/css/*.css $2/res/ressources/css
cp ./web/ressources/javascript/jquery/*.js $2/res/ressources/javascript/jquery
cp ./web/ressources/javascript/jquery/plugins/*.js $2/res/ressources/javascript/jquery/plugins
cp ./web/ressources/javascript/tools/*.js $2/res/ressources/javascript/tools
cp ./web/ressources/javascript/min-js/*-min.js $2/res/ressources/javascript/
cp ./web/ressources/javascript/min-js/core/*-min.js $2/res/ressources/javascript/core

cp -R --parents ./skins/* $2/ 

cp $1/$BINARY_FILENAME $2

cd $2
echo "DONE!"


fi


