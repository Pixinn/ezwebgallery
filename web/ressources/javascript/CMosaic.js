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



function CMosaic( p_properties, p_htmlStructure ) 
{    
   
    // ++++ Methods ++++ //
      
    //Building the thumbnail mosaic
    this.buildHtml = function( ) 
    {  
        var divThumbBoxName = "thumbBox";
        that.htmlStructure.index.mosaic.$handle.hide();
        
        //Generating the thumbnail containers        
        for( var thumbNr = 1; thumbNr <= that.mosaicNbThumbnails; thumbNr++ ) {
            that.htmlStructure.index.mosaic.$handle.append('<div class="'+ divThumbBoxName +'" id="' + thumbNr + '"></div>');
        }
        var $thumbBoxes = $("." + divThumbBoxName);
        that.htmlStructure.index.mosaic.$thumbBoxes = $thumbBoxes;
        
        //Loading the thumbnails        
        that.thumbnailsSet =that.computeThumbnailsSet();
        that.ThumbnailSize = that.properties.index.mosaic.sizes[that.thumbnailsSet ]
        $thumbBoxes.each( function( index )
        {
            var thumbName = that.properties.photos.list[ index ].filename;
            var thumbnail = new Image();
            $(this).append( thumbnail );
            $(thumbnail).load( function( )	{	// callback OnLoad
                            that.oneThumbLoadedEvent.fire( this );   // this : thumbnail::Image
                        })
                        .attr("id",this.id)
                        .attr("src", src = that.properties.defines.URL_THUMBS_PATH+'/'+that.thumbnailsSet+'/'+thumbName );  // src will load the Image: MUST BE THE LAST ATTRIBUTE TO SET!!
                        
        } );
        
        return that.htmlStructure;
    };
    
    
    //when a single thumbnail was loaded
    this.onThumbnailLoaded = function( )
    {        
        if( ++that.nbThumbnailLoaded == that.mosaicNbThumbnails ) {
            that.allThumbsLoadedEvent.fire( );
        }
    }
    
    //When all thumbnails are loaded
    this.onThumbnailsLoaded = function( )
    {               
        //resizing the mosaic according to the thumb set chosen
        var $thumbBoxes = that.htmlStructure.index.mosaic.$thumbBoxes;
        var $thumbnails = $thumbBoxes.find( 'img' );
        var thumbBorderWidth = $thumbnails.first().css("border-top-width");
        thumbBorderWidth = thumbBorderWidth.replace("px","");
        $thumbBoxes.width( that.ThumbnailSize + 2*thumbBorderWidth )
                   .height( that.ThumbnailSize + 2*thumbBorderWidth);
        var mosaicWidth = that.properties.index.mosaic.nbCols * $thumbBoxes.outerWidth();
        that.htmlStructure.index.mosaic.$handle.show(); //show necessary for that.htmlStructure.index.mosaic.$title.outerHeight()
        var mosaicHeight = Math.ceil(that.mosaicNbThumbnails / that.properties.index.mosaic.nbCols) * $thumbBoxes.outerHeight() +  that.htmlStructure.index.mosaic.$title.outerHeight(true);
        //updating structure
        that.htmlStructure.index.mosaic.$thumbnails = $thumbnails;
        //displaying mosaic
        that.htmlStructure.index.mosaic.$handle.width( mosaicWidth )
                                                  .height( mosaicHeight );        
    };
    
    //showing the mosaic
    this.show = function()
    {
        that.htmlStructure.index.mosaic.$thumbnails.verticalCenter(0);
    }
    
    this.hide = function()
    {
        that.htmlStructure.index.mosaic.$handle.hide();
    }
    
    
    //On window resize
    this.onResize = function()
    {
      // that.htmlStructure.index.mosaic.$container.verticalCenter( 0 );
    }
    
    //On previous
    this.onPreviousNext = function( evt )
    {
        var $firstThumbBox = that.htmlStructure.index.mosaic.$thumbBoxes.eq(0);
        var $thumbBox =  that.htmlStructure.index.mosaic.$thumbBoxes.eq( evt.id - 1 );
        $("body").scrollTop( $thumbBox.offset().top - $firstThumbBox.offset().top );
    }
   
    
    //Returns the loading Event
    this.getLoadingEvent = function() {
        return that.oneThumbLoadedEvent;
    }
    
    
        //Returns the loaded Event
    this.getLoadedEvent = function() {
        return that.allThumbsLoadedEvent;
    }
        
    
    // +++ internal tools +++
    //Compute the appropriate thumbnail set
    this.computeThumbnailsSet = function( )
    {
        var fSetFound = false;
        var thumbSet = that.properties.index.mosaic.defaultSet;    //if no suitable set is found
        var availableWidth = that.htmlStructure.$window.innerWidth() - that.properties.index.mosaic.unavailable.horizontal - that.mosaicSizeMargin/2; //$window.innerWidth(): "visual viewport" width: http://www.quirksmode.org/mobile/viewports2.html
        var availableHeight = that.htmlStructure.$window.innerHeight() - that.properties.index.mosaic.unavailable.vertical - that.mosaicSizeMargin;
        
        $.each( that.properties.index.mosaic.sizes, function( key, size ) //iterating on the object using jQuery
        {
            if (   !fSetFound
                && (availableWidth > size * that.properties.index.mosaic.nbCols)
                /*&& (availableHeight > size * that.properties.index.mosaic.nbRows)*/ )
            {
                thumbSet = key;
                fSetFound = true;
            }
        } );//each
        
        return thumbSet;
    };
    
    
        // ++++ Attributes ++++ //
    //General
    var that = this;
    this.properties = p_properties;
    this.htmlStructure = p_htmlStructure;
    // Mosaic
    this.scroller; //instanciated in start()
    this.mosaicSizeMargin = 100; //60 is necessary for vertical (tab height + logo). 100 is for horizontal comfort.
    this.mosaicNbThumbnails = that.properties.photos.list.length;
    this.mosaicNbThumbsByPanels = (that.properties.index.mosaic.nbRows * that.properties.index.mosaic.nbCols);
    this.mosaicNbPanels = Math.ceil( that.mosaicNbThumbnails / that.mosaicNbThumbsByPanels );
    this.nbThumbnailLoaded = 0;
    this.ThumbnailSize = 0;
    this.currentPageNr = 1;
    //Observing the loaded thumbnails
    this.oneThumbLoadedEvent = new CEvent();
    this.oneThumbLoadedEvent.subscribe( this.onThumbnailLoaded );
    this.allThumbsLoadedEvent = new CEvent();
    this.allThumbsLoadedEvent.subscribe( this.onThumbnailsLoaded );
        
};
