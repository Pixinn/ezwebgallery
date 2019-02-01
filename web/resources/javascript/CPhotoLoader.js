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

function CPhotoLoader( p_properties, p_html )
{
    var that = this;
    
    this.NEXT = 1;
    this.PREVIOUS = -1;

    this.properties = p_properties;
    this.html = p_html;

    this.strategy = new  CPhotoStrategy( p_properties );
    this.$spinner = $(new Image());
    this.$spinner.attr( "src", this.html.spinnerSrc );
    this.storage = new CStorage( p_properties.photos.technical.cacheSize );
    this.prefetchingQueue = new CFifo();

    this.photoLoadedEvent = new CEvent();


    //Fetches the proper photo
    this.fetch = function( id )
    {
        TOOLS.trace("CPhotoLoader::fetch - " + "Fetching " + id );
        var photoToLoad = that.strategy.getImage( id );
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
                that.fetch( that.prefetchingQueue.pop() ); //Loading next photo: recursive call
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

     this.setSpace = function( space )
     {
        that.strategy.setSpace( space );
     }
    
    //Events
    this.getPhotoLoadedEvent = function( )
    {
        return that.photoLoadedEvent;
    }

}
