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
    
    this.NEXT = 1;
    this.PREVIOUS = -1;

    this.properties = p_properties;
    this.html = p_html;

    this.$spinner = $(new Image());
    this.$spinner.attr( "src", this.html.spinnerSrc );
    this.storage = new CStorage( p_properties.photos.technical.cacheSize );
    this.prefetchingQueue = new CFifo();
    this.space = { h: 0, w: 0 };

    this.photoLoadedEvent = new CEvent();


    //Fetches the propoer photo
    this.fetch = function( id )
    {
        TOOLS.trace("Fetching " + id );
        var photoToLoad = that.computeUrl( id );
        var photo = that.storage.addPhoto( photoToLoad );
        if( photo.isLoaded() == false ) { //loading the photo if it has not already been loaded
            photo.load( photoToLoad.url,  function( photoLoaded ) {
                                                            that.photoLoadedEvent.fire( photoLoaded );
                                                            if( that.prefetchingQueue.size() > 0 ) { //Prefetching the photos in the queue if any
                                                                that.fetch( that.prefetchingQueue.pop() ); //Loading next photo
                                                            }
                                                        } );
        }
        else { //if already loaded the next photo still has to be prefetched
            if( that.prefetchingQueue.size() > 0 ) { //Prefetching the photos in the queue if any
                that.fetch( that.prefetchingQueue.pop() ); //Loading next photo
            }
        }
        return photo;
    }
    
    //Loads the proper photo
    this.load = function( id, direction )
    {      
        //inserting the next photo to prefetch to the queue
        for( var i = that.properties.photos.technical.prefetchSize; i > 0; i-- ) {
            var toPrefetch = id + ( direction * i );
            if( toPrefetch <= that.properties.photos.list.length && toPrefetch > 0 ) {
                that.prefetchingQueue.pushFirst( toPrefetch );
            }
        }
        return that.fetch( id );
    }

    //Events
    this.getPhotoLoadedEvent = function( )
    {
        return that.photoLoadedEvent;
    }

    
    
    //Computing the proper url to load
    this.computeUrl = function( id )
    {
        // Strategy: best quality ?
        var photo;
        if( this.properties.photos.technical.qualityStrategy === 0) {
            photo = this.getSmallerImage( id );
        }
        //Optimisation de l'espace ? -- Marche de façon médiocre avec ie6/ie7 qui ont du mal avec les redimensionnements
        else {
            //On récupère la plus petite image de taille > à l'espace afficache. Le navigateur resizera.
            photo = this.getBiggerImage( id );
        }// Fin Optimisation de l'espace

        TOOLS.trace( "space: " + that.space.w  + " " +  that.space.h +  " / " + photo.url );
        photo.id = id;
        return photo;
    }

    
    //Traversing all the available image size
    //Return the first fitting
    this.getBiggerImage = function( numPhoto )
    {
        var f_OK = false;
        var sizes = this.properties.photos.list[ numPhoto - 1].sizes;
        var suitableSizes = new Object();
        //Extracting all suitable sizes
        $.each( sizes, function( key, size )
        {
            if( size.width >= that.space.w || size.height >= that.space.h ) //suitable size found
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
    this.getSmallerImage = function( numPhoto )
    {
        var f_OK = false;
        var sizes = this.properties.photos.list[ numPhoto - 1].sizes;
        var suitableSizes = new Object();
        //Extracting all suitable sizes
        $.each( sizes, function( key, size )
        {
            if( size.width <= that.space.w && size.height <= that.space.h ) //suitable size found
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

}
