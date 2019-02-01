/*
 *  EZWebGallery:
 *  Copyright (C) 2013 Christophe Meneboeuf <xtof@ezwebgallery.org>
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

//This class will compute the image to fetch according to two possible strategies:
// 0 - Get the biggest image fitting in the available space
// 1 - Get the image just larger than the available space


function CPhotoStrategy( p_properties )
{
    var that = this;
    this.properties = p_properties;
        
    this.getDppx = function( )
    {
        var dppx = 1;
        if( !( navigator.userAgent.match(/(Phone)|(Mobile)/i) && !navigator.userAgent.match(/(iPad)|(iPod)/i)) ) {
            dppx = window.devicePixelRatio || Math.round(window.screen.availWidth / document.documentElement.clientWidth) || 1; // http://stackoverflow.com/questions/16383503/window-devicepixelratio-does-not-work-in-ie-10-mobile
        }
        return dppx;
    }

    this.getImage = function( id )
    {         
        var photo = that.getImageStrategyFct( id );
        photo.id = id;
        TOOLS.trace( "CPhotoStrategy::getImage - " + "space: " + that.spaceRealPx.w  + " " +  that.spaceRealPx.h +  " / " + photo.url );
        return photo;
    }

    this.setSpace = function( space )
    {
        that.spaceRealPx.h = space.h * that.spaceRealPx.dppx;
        that.spaceRealPx.w = space.w * that.spaceRealPx.dppx;
    }
       
       
    //+++Two functions chosen by a strategy
    
    //Traversing all the available image size
    //Returning the first photo > biggest dimension
    this.getBiggerImage = function( numPhoto )
    {     
        //TOOLS.trace( "CPhotoStrategy::getBiggerImage");
        var f_OK = false;
        var sizes = that.properties.photos.list[ numPhoto - 1].sizes;
        var suitableSizes = new Object();
        //Extracting all suitable sizes
        //var maxDim = Math.max( that.spaceRealPx.w, that.spaceRealPx.h );
        $.each( sizes, function( key, size )
        {
            if(  Math.max(size.width,size.height) >= Math.max( that.spaceRealPx.w, that.spaceRealPx.h ) ) //suitable size found
            /* if (size.width >= maxDim || size.height >= maxDim ) */ 
            {
                suitableSizes[ key ] = size;
                f_OK = true;
            }
        } );
        
        //Finding the smallest photo in the suitable size set
        //same format so we can compare only one dimension
        var minWidth = Number.MAX_VALUE;
        var res = that.properties.photos.technical.largetSet;
        $.each( suitableSizes, function( key, size )
        {
            if(  size.width < minWidth )
            {
                minWidth = suitableSizes[ key ].width;
                res = key;
            }
        } );              

        return {    url: that.properties.defines.IMAGES_PATH  + "/" + res + "/" + that.properties.photos.list[ numPhoto - 1].filename,
                    size: { w: sizes[ res ].width,
                            h: sizes[ res ].height }
           };
    }

    
    //Return the first fitting in the available space
    this.getSmallerImage = function( numPhoto )
    {
        //TOOLS.trace("CPhotoStrategy::getSmallerImage");
        var f_OK = false;
        var sizes = that.properties.photos.list[ numPhoto - 1].sizes;
        var suitableSizes = new Object();
        //Extracting all suitable sizes
        $.each( sizes, function( key, size )
        {
            if( size.width <= that.spaceRealPx.w && size.height <= that.spaceRealPx.h ) //suitable size found
            {
                suitableSizes[ key ] = size;
                f_OK = true;
            }
        } );
        //Finding the biggest suitable size
        var maxWidth = 0;
        var res = that.properties.photos.technical.smallestSet;
        $.each( suitableSizes, function( key, size )
        {
            if(  size.width > maxWidth )
            {
                maxWidth =  suitableSizes[ key ].width;
                res = key;
            }
        } );

        return {    url: that.properties.defines.IMAGES_PATH + "/" + res + "/" + that.properties.photos.list[ numPhoto - 1].filename,
                    size: {    w: sizes[ res ].width,
                               h: sizes[ res ].height }
               };

    }
    
    // +++++ Strategy
    this.getImageStrategyStr = {        
        0: that.getBiggerImage,
        1: that.getSmallerImage,
    };
    
    //++++ Init
    //Set the correct algo to find the proper image via a strategy using  "technical.qualityStrategy" as a key
    this.getImageStrategyFct = this.getImageStrategyStr[ this.properties.photos.technical.qualityStrategy ];
    this.spaceRealPx = { h: 0, w: 0, dppx: this.getDppx() };  //Everywhere else, pixels are "CSS pixel". Here we work in "real pixels" to fetch the best photo available to match the real display
    
 };
