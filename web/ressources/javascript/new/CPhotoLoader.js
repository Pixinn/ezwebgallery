/*
 *  EZWebGallery:
 *  Copyright (C) 2012 The EZWebGallery Dev Team <dev@ezwebgallery.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

function CPhotoLoader( p_properties, p_html )
{
    var that = this;

    this.properties = p_properties;
    this.html = p_html;

    this.$spinner = $(new Image());
    this.$spinner.attr( "src", "ressources/images/ajax-loader.gif");
    this.photoBuffer = new Array();

    this.photoLoadedEvent = new CEvent();

    //Loads the proper photo
    this.load = function( p_id )
    {
        var id = p_id;
        var photoToLoad = that.computeUrl( id );
        photo = new Image();
        $(photo).load( function() {
                that.photoLoadedEvent.fire( {
                    $image: $(this),
                    id: id,
                    size : { w: this.width, h: this.height }
                   } )
                } )
                .error( function() {
                    TOOLS.trace("ERROR loading " + $(this).attr("src"));
                })
                .attr("src", photoToLoad.url);

                return { $image: that.$spinner, size: photoToLoad.size };
    }


    //Events
    this.getPhotoLoadedEvent = function( )
    {
        return that.photoLoadedEvent;
    }

    this.onPhotoLoaded = function( )
    {
        TOOLS.trace( this.id + " " + this.$image.attr("src") + " loaded.");
    }

    //Computing the proper url to load
    this.computeUrl = function( id )
    {
        var ie6BugCorrection = 6;
        var wastedPixelTop = 33;
        var wastedPixelBottom = 33;
        var widthWasted = 0;

        //+++ Computing the maximal available size

        //Computing width used by other divs
        //NOTE : only sibling divs are considerd
        //This allows to correct an IE6 bug.
        for(var i = 0; i < this.html.$photoWrapper.siblings().length; i++){
            widthWasted += this.html.$photoWrapper.siblings().eq(i).outerWidth( );
        }
        //non available space
        var frameBorderSize = parseInt( this.html.$photoFrame.css("padding-top").replace("px","") );
        //NB: On ne peut pas utiliser g_$divDisplayZoneName.innerWidth( ), car bug sous IE6 :( */
        //Du coup il faut que:	+ g_$divDisplayZoneName.siblings()::margin = 0px
        //+ g_$divDisplayZoneName::border=0 et ::margin=0
        var height = this.html.$photoWrapper.innerHeight() - 2*frameBorderSize - 2*this.properties.photos.technical.decoration.padding - wastedPixelTop;
        var width = $(window).width( ) - widthWasted - 2*frameBorderSize - 2*this.properties.photos.technical.decoration.padding - ie6BugCorrection;

        if( height > this.properties.photos.technical.maxSize.height ) {
            height = this.properties.photos.technical.maxSize.height;
        }
        if( width > this.properties.photos.technical.maxSize.width ) {
            width = this.properties.photos.technical.maxSize.width;
        }

        // Strategy: best quality ?
        var photo;
        if( this.properties.photos.technical.qualityStrategy === 0)
        {
            photo = this.getSmallerImage( {h:height,w:width} , id );
        }//Fin Optimisation de la qualité

        //Optimisation de l'espace ? -- Marche de façon médiocre avec ie6/ie7 qui ont du mal avec les redimensionnements
        else
        {
            //On récupère la plus petite image de taille > à l'espace afficache. Le navigateur resizera.
            photo = this.getBiggerImage( {h:height,w:width} , id );
        }// Fin Optimisation de l'espace


        return photo;
    }

    //Traversing all the available image size
    //Return the first fitting
    this.getBiggerImage = function( photoDivHW, numPhoto )
    {
        var f_OK = false;
        var sizes = this.properties.photos.list[ numPhoto - 1].sizes;
        var suitableSizes = new Object();
        //Extracting all suitable sizes
        $.each( sizes, function( key, size )
        {
            if( size.width >= photoDivHW.w || size.height >= photoDivHW.h ) //suitable size found
            {
                suitableSizes[ key ] = size;
                f_OK = true;
            }
        } );
        //Finding the smallest suitable size
        //same format so we can compare only one dimension
        var minWidth = Number.MAX_VALUE;
        var res = this.properties.photos.technical.largestSet;
        $.each( suitableSizes, function( key, size )
        {
            if(  size.width < minWidth )
            {
                minWidth = suitableSizes[ key ].width;
                res = key;
            }
        } );

        return {    url: this.properties.defines.IMAGES_PATH  + "/" + res + "/" + this.properties.photos.list[ numPhoto - 1].filename,
                    size: { w: sizes[ res ].width,
                            h: sizes[ res ].height }
           };
    }

    //Returning the first photo > available space
    this.getSmallerImage = function( photoDivHW, numPhoto )
    {
        var f_OK = false;
        var sizes = this.properties.photos.list[ numPhoto - 1].sizes;
        var suitableSizes = new Object();
        //Extracting all suitable sizes
        $.each( sizes, function( key, size )
        {
            if( size.width <= photoDivHW.w && size.height <= photoDivHW.h ) //suitable size found
            {
                suitableSizes[ key ] = size;
                f_OK = true;
            }
        } );
        //Finding the biggest suitable size
        var maxWidth = 0;
        var res = this.properties.photos.technical.smallestSet;
        $.each( suitableSizes, function( key, size )
        {
            if(  size.width > maxWidth )
            {
                maxWidth =  suitableSizes[ key ].width;
                res = key;
            }
        } );

        return {    url: this.properties.defines.IMAGES_PATH + "/" + res + "/" + this.properties.photos.list[ numPhoto - 1].filename,
                    size: {    w: sizes[ res ].width,
                               h: sizes[ res ].height }
               };

    }


    this.photoLoadedEvent.subscribe( that.onPhotoLoaded );
}
