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



function CMosaic( p_properties, p_htmlStructure ) 
{    
   
    // ++++ Methods ++++ //
      
    //Building the thumbnail mosaic
    this.buildHtml = function( ) 
    {  
        var divSlidingPanelName = "slidingPanel";
        var divThumbBoxName = "thumbBox";
        
        //Generating panels and tabs
        for( var numIndex = 1; numIndex <= that.mosaicNbPanels; numIndex++ ) {
            that.htmlStructure.index.mosaic.$scrollContainer.append( '<div class="' + divSlidingPanelName + '" id=\"index' + numIndex + '\"></div>' );
        }
        var $pannels = $("." + divSlidingPanelName);
                
        //Generating the tabs
        for( var numIndex = 1; numIndex <= that.mosaicNbPanels; numIndex++ ) {
            that.htmlStructure.index.mosaic.$tabsContainer.append('<li class=\"tab\" id=\"' + numIndex + '\" target="#index' + numIndex + '\">'+ numIndex + '</li>');
        }
        that.htmlStructure.index.mosaic.$tabs = that.htmlStructure.index.mosaic.$tabsContainer.find('li');
        
        //Generating the thumbnail containers        
        var thumbNr = 1;
        for (var pannel = 0; pannel < $pannels.length; pannel++)
        {
            var i = 1;
            while( ( i <= that.mosaicNbThumbsByPanels ) && ( thumbNr <= that.mosaicNbThumbnails ) ) {
                $pannels.eq(pannel).append('<div class="'+ divThumbBoxName +'" id="' + thumbNr + '"></div>');	
                thumbNr++;
                i++;
            }       
        }
        var $thumbBoxes = $("." + divThumbBoxName);
        that.htmlStructure.index.mosaic.$thumbBoxes = $thumbBoxes;
        
        //Loading the thumbnails        
        this.thumbnailsSet = this.computeThumbnailsSet();
        that.ThumbnailSize = that.properties.index.mosaic.sizes[ this.thumbnailsSet ]
        $thumbBoxes.each( function( index )
        {
            var thumbName = that.properties.photos.list[ index ].filename;
            var thumbnail = new Image();
            $(this).append( thumbnail );
            $(thumbnail).load( function( )	{	// callback OnLoad
                            that.oneThumbLoadedEvent.fire( "thumbnailLoaded" );                     
                        })
                        .error( function()  {
                            TOOLS.trace("Error Loadind a thumbnail: " + ++that.nbThumbnailLoaded);                            
                        })
                        .attr("id", this.id)
                        // Source de l'image : A METTRE EN DERNIER
                        .attr("src", src = that.properties.defines.URL_THUMBS_PATH+'/'+that.thumbnailsSet+'/'+thumbName );
                        
        } );
        
        
        //Returning the updated html structure
        that.htmlStructure.index.mosaic.$pannels = $pannels;
        return that.htmlStructure;
    };
    
    
    this.onPreviousIndex = function()
    {
        if( that.currentPageNr > 1 ) {
            that.currentPageNr--;
            that.scroller.scrollToPageNr( that.currentPageNr, that.currentPageNr );
        }
    }
    
    this.onNextIndex = function()
    {
        if( that.currentPageNr < that.mosaicNbPanels ) {
            that.currentPageNr++;
            that.scroller.scrollToPageNr( that.currentPageNr, that.currentPageNr );
        }
    }
    
    
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
        var $scrollViewport = that.htmlStructure.index.mosaic.$scrollViewport;
        var $thumbBoxes = that.htmlStructure.index.mosaic.$thumbBoxes;
        var $thumbnails = $thumbBoxes.find( 'img' );
        var thumbBorderWidth = $thumbnails.first().css("border-top-width");
        thumbBorderWidth = thumbBorderWidth.replace("px","");
        $thumbBoxes.width( that.ThumbnailSize + 2*thumbBorderWidth )
                   .height( that.ThumbnailSize + 2*thumbBorderWidth);
        var mosaicWidth = that.properties.index.mosaic.nbCols * $thumbBoxes.outerWidth();
        var mosaicHeight = that.properties.index.mosaic.nbRows * $thumbBoxes.outerHeight();
        
        $scrollViewport.width( mosaicWidth )
                      .height( mosaicHeight );
        that.htmlStructure.index.mosaic.$pannels.width( mosaicWidth )
                                                  .height( mosaicHeight );        
        
        that.htmlStructure.index.mosaic.$container.width( $scrollViewport.outerWidth() );
        that.htmlStructure.index.mosaic.$thumbnails = $thumbnails;
        
        that.show(); //Must be visible to set up the scrolling :/
        
        // --- scrolling
        var $pannels = that.htmlStructure.index.mosaic.$pannels;
        var $scrollcontainer = that.htmlStructure.index.mosaic.$scrollContainer;
        $pannels.css({  'float' : 'left',
                               'position' : 'relative' // IE fix to ensure overflow is hidden
        }); 
        // calculate a new width for the container (so it holds all panels)
        $scrollcontainer.css('width', $pannels[0].offsetWidth * $pannels.length);
        $scrollViewport.scrollLeft( 0 );
        
        //binding the scrolling to the mosaic
        var scrollOptions = {
          $viewport: $scrollViewport,
          $navigation: that.htmlStructure.index.mosaic.$tabs,
          duration: 500, 
          easing: 'swing',
          onAfter: that.highlightNav
        };

         that.scroller = new CScroller( scrollOptions );
         
         that.hide();
                 
        //highlights the first navigation tab
        that.highlightNav ( that.htmlStructure.index.mosaic.$tabs.eq(0) );
    };
    
    //showing the mosaic
    this.show = function()
    {
        that.htmlStructure.index.mosaic.$container.show()
                                                                          .verticalCenter(0);
        that.htmlStructure.index.mosaic.$thumbBoxes.find('img').verticalCenter(0);
    }
    
    this.hide = function()
    {
        that.htmlStructure.index.mosaic.$container.hide();
    }
    
    
    //On window resize
    this.onResize = function()
    {
        that.htmlStructure.index.mosaic.$container.verticalCenter( 0 );
    }
    
    //On previous
    this.onPreviousNext = function( evt )
    {
        var newPage = Math.ceil( evt.id / (that.properties.index.mosaic.nbRows * that.properties.index.mosaic.nbCols) );
        if( newPage != that.currentPageNr ) {
            TOOLS.trace( "Mosaic - new page: " + newPage );
            that.scroller.scrollToPageNr( newPage, newPage );
        }
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
        var availableWidth = that.htmlStructure.index.$screen.width() - that.properties.index.mosaic.unavailable.horizontal - that.mosaicSizeMargin/2;
        var availableHeight = that.htmlStructure.index.$screen.height() - that.properties.index.mosaic.unavailable.vertical - that.mosaicSizeMargin;
        
        $.each( that.properties.index.mosaic.sizes, function( key, size ) //iterating on the object using jQuery
        {
            if (   !fSetFound
                && (availableWidth > size * that.properties.index.mosaic.nbCols)
                && (availableHeight > size * that.properties.index.mosaic.nbRows) )
            {
                thumbSet = key;
                fSetFound = true;
            }
        } );//each
        
        return thumbSet;
    };
    
    //Highlights the selected navigation tab and update the current page number
    this.highlightNav = function ( $toHighlight )
    {  
        $(".navTabSelected").removeClass("navTabSelected");
        $toHighlight.addClass("navTabSelected");
        that.currentPageNr = parseInt( $toHighlight.attr("id") );
    }
    
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
