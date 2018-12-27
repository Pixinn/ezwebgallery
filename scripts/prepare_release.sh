#!/bin/sh

if [ $# -eq 0 ]; then

echo "...Copying resource files in the release directory"

SCRIPTDIR="$( cd "$( dirname "$0" )" && pwd )"
DATADIR=$SCRIPTDIR/../deployment/common

#Creating the folder structure
mkdir --parents $DATADIR/data/resources/css
mkdir --parents $DATADIR/data/resources/css/opt
mkdir --parents $DATADIR/data/resources/php
mkdir --parents $DATADIR/data/resources/images/smileys
mkdir --parents $DATADIR/data/resources/javascript/jquery/plugins
mkdir --parents $DATADIR/data/resources/javascript/hammer
mkdir --parents $DATADIR/data/resources/javascript/modernizr
mkdir --parents $DATADIR/data/resources/javascript/screenfull
mkdir --parents $DATADIR/data/resources/javascript/tools

#copying static resources
cp $SCRIPTDIR/../web/index.html $DATADIR/data
cp $SCRIPTDIR/../web/resources/php/*.php $DATADIR/data/resources/php
cp $SCRIPTDIR/../web/resources/images/*.* $DATADIR/data/resources/images
cp $SCRIPTDIR/../web/resources/images/smileys/*.* $DATADIR/data/resources/images/smileys
cp $SCRIPTDIR/../web/resources/css/*.css $DATADIR/data/resources/css
cp $SCRIPTDIR/../web/resources/css/opt/*.css $DATADIR/data/resources/css/opt
cp $SCRIPTDIR/../web/resources/javascript/Toolbar.js $DATADIR/data/resources/javascript
cp $SCRIPTDIR/../web/resources/javascript/jquery/*.js $DATADIR/data/resources/javascript/jquery
cp $SCRIPTDIR/../web/resources/javascript/jquery/plugins/*.js $DATADIR/data/resources/javascript/jquery/plugins
cp $SCRIPTDIR/../web/resources/javascript/hammer/*.js $DATADIR/data/resources/javascript/hammer
cp $SCRIPTDIR/../web/resources/javascript/modernizr/*.js $DATADIR/data/resources/javascript/modernizr
cp $SCRIPTDIR/../web/resources/javascript/screenfull/*.js $DATADIR/data/resources/javascript/screenfull
cp $SCRIPTDIR/../web/resources/javascript/tools/*.js $DATADIR/data/resources/javascript/tools

#minimizing javascript files
$SCRIPTDIR/compress_javascript.sh $DATADIR/data/resources/javascript

fi

