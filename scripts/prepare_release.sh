#!/bin/sh

if [ $# -eq 0 ]; then

echo "...Copying resource files in the release directory"

SCRIPTDIR="$( cd "$( dirname "$0" )" && pwd )"
DATADIR=$SCRIPTDIR/../deployment/common

#Creating the folder structure
mkdir --parents $DATADIR/data/ressources/css
mkdir --parents $DATADIR/data/ressources/php
mkdir --parents $DATADIR/data/ressources/images/smileys
mkdir --parents $DATADIR/data/ressources/javascript/jquery/plugins
mkdir --parents $DATADIR/data/ressources/javascript/tools
mkdir --parents $DATADIR/data/ressources/javascript/core

#copying static resources
cp $SCRIPTDIR/../web/index.html $DATADIR/data
cp $SCRIPTDIR/../web/ressources/php/*.php $DATADIR/data/ressources/php
cp $SCRIPTDIR/../web/ressources/images/*.* $DATADIR/data/ressources/images
cp $SCRIPTDIR/../web/ressources/images/smileys/*.* $DATADIR/data/ressources/images/smileys
cp $SCRIPTDIR/../web/ressources/css/*.css $DATADIR/data/ressources/css
cp $SCRIPTDIR/../web/ressources/javascript/jquery/*.js $DATADIR/data/ressources/javascript/jquery
cp $SCRIPTDIR/../web/ressources/javascript/jquery/plugins/*.js $DATADIR/data/ressources/javascript/jquery/plugins
cp $SCRIPTDIR/../web/ressources/javascript/tools/*.js $DATADIR/data/ressources/javascript/tools

#minimizing javascript files
$SCRIPTDIR/compress_javascript.sh $DATADIR/data/ressources/javascript

fi

