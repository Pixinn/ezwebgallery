 #!/bin/sh

if [ $# -eq 3 ]; then

echo " "
echo "###### Copying resource files in the release directory "
echo " "

SCRIPTDIR="$( cd "$( dirname "$0" )" && pwd )"

mkdir --parents $3

mkdir --parents $3/res/ressources/css
mkdir --parents $3/res/ressources/php
mkdir --parents $3/res/ressources/images/smileys
mkdir --parents $3/res/ressources/javascript/jquery/plugins
mkdir --parents $3/res/ressources/javascript/tools
mkdir --parents $3/res/ressources/javascript/core

cp $SCRIPTDIR/web/index.html $3/res
cp $SCRIPTDIR/web/ressources/php/*.php $3/res/ressources/php
cp $SCRIPTDIR/web/ressources/images/*.* $3/res/ressources/images
cp $SCRIPTDIR/web/ressources/images/smileys/*.* $3/res/ressources/images/smileys
cp $SCRIPTDIR/web/ressources/css/*.css $3/res/ressources/css
cp $SCRIPTDIR/web/ressources/javascript/jquery/*.js $3/res/ressources/javascript/jquery
cp $SCRIPTDIR/web/ressources/javascript/jquery/plugins/*.js $3/res/ressources/javascript/jquery/plugins
cp $SCRIPTDIR/web/ressources/javascript/tools/*.js $3/res/ressources/javascript/tools
cp $2/*-min.js $3/res/ressources/javascript/
cp $2/core/*-min.js $3/res/ressources/javascript/core

mkdir --parents $3/skins/
cp -R $SCRIPTDIR/skins/* $3/skins/

#Copying binary if it nor trying to copy it on itself
if [ -e $1 ];then
echo $1
    DIRNAME="$(dirname $1)"
    if [ $DIRNAME != $3 -a $DIRNAME/ != $3 ];then
    echo $3
         cp $1 $3
    fi
fi

else

echo " "
echo "###############################################################################"
echo "#"
echo "# Usage: create_release BINARY MINIMIZEDJS_FOLDER DESTINATION_FOLDER"
echo "# This script copies all the necessary files to the provided DESTINATION_FOLDER"
echo "#"
echo "# In order to create a release you need to set the folowing env variables"
echo "# + QDTIR: path to Qt"
echo "# + MAGICK_HOME: path to ImageMagick"
echo "#"
echo "# NOTE: you need to minimize some files uising $SCRIPTDIR/compress_javascript.h"
echo "# and placethem in $SCRIPTDIR/web/ressources/javascript/"
echo "# You can run compress_js.bat script for that purpose."
echo "#"
echo "###############################################################################"
echo " "


fi


