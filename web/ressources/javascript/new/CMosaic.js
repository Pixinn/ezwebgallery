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
        for( var numIndex = 1; numIndex <= that.m_mosaicNbPanels; numIndex++ ) {
            that.m_htmlStructure.index.mosaic.$scrollContainer.append( '<div class="' + divSlidingPanelName + '" id=\"index' + numIndex + '\"></div>' );
        }
        var $pannels = $("." + divSlidingPanelName);
                
        //Generating the tabs
        for( var numIndex = 1; numIndex <= that.m_mosaicNbPanels; numIndex++ ) {
            that.m_htmlStructure.index.mosaic.$tabsContainer.append('<li class=\"tab\" target="#index' + numIndex + '\">'+ numIndex + '</li>');
        }
        that.m_htmlStructure.index.mosaic.$tabs = that.m_htmlStructure.index.mosaic.$tabsContainer.find('li');
        
        //Generating the thumbnail containers        
        var thumbNr = 1;
        for (var pannel = 0; pannel < $pannels.length; pannel++)
        {
            var i = 1;
            while( ( i <= that.m_mosaicNbThumbsByPanels ) && ( thumbNr <= that.m_mosaicNbThumbnails ) ) {
                $pannels.eq(pannel).append('<div class="'+ divThumbBoxName +'" id="' + thumbNr + '"></div>');	
                thumbNr++;
                i++;
            }       
        }
        var $thumbBoxes = $("." + divThumbBoxName);
        that.m_htmlStructure.index.mosaic.$thumbBoxes = $thumbBoxes;
        
        //Loading the thumbnails        
        this.thumbnailsSet = this.computeThumbnailsSet();
        that.m_ThumbnailSize = that.m_properties.index.mosaic.sizes[ this.thumbnailsSet ]
        $thumbBoxes.each( function( index )
        {
            var thumbName = that.m_properties.photos.list[ index ].filename;
            var thumbnail = new Image();
            $(this).append( thumbnail );
            $(thumbnail).load( function( )	{	// callback OnLoad
                            that.m_oneThumbLoadedEvent.fire( "thumbnailLoaded" );                     
                        })
                        .error( function()  {
                            TOOLS.trace("Error Loadind a thumbnail: " + ++that.m_nbThumbnailLoaded);                            
                        })
                        .attr("id", this.id)
                        // Source de l'image : A METTRE EN DERNIER
                        .attr("src", src = that.m_properties.defines.URL_THUMBS_PATH+that.thumbnailsSet+'/'+thumbName );
                        
        } );
        
        
        //Returning the updated html structure
        that.m_htmlStructure.index.mosaic.$pannels = $pannels;
        return that.m_htmlStructure;
    };
    
    //when a single thumbnail was loaded
    this.onThumbnailLoaded = function( )
    {        
        if( ++that.m_nbThumbnailLoaded == that.m_mosaicNbThumbnails ) {
            that.m_allThumbsLoadedEvent.fire( );
        }
    }
    
    //When all thumbnails are loaded
    this.onThumbnailsLoaded = function( )
    {               
        //resizing the mosaic according to the thumb set chosen
        var $scrollViewport = that.m_htmlStructure.index.mosaic.$scrollViewport;
        var $thumbBoxes = that.m_htmlStructure.index.mosaic.$thumbBoxes;
        var $thumbnails = $thumbBoxes.find( 'img' );
        var thumbBorderWidth = $thumbnails.first().css("border-top-width");
        thumbBorderWidth = thumbBorderWidth.replace("px","");
        $thumbBoxes.width( that.m_ThumbnailSize + 2*thumbBorderWidth )
                   .height( that.m_ThumbnailSize + 2*thumbBorderWidth);
        var mosaicWidth = that.m_properties.index.mosaic.nbCols * $thumbBoxes.outerWidth();
        var mosaicHeight = that.m_properties.index.mosaic.nbRows * $thumbBoxes.outerHeight();
        
        $scrollViewport.width( mosaicWidth )
                      .height( mosaicHeight );
        that.m_htmlStructure.index.mosaic.$pannels.width( mosaicWidth )
                                                  .height( mosaicHeight );        
        
        that.m_htmlStructure.index.mosaic.$container.width( $scrollViewport.outerWidth() );
        that.m_htmlStructure.index.mosaic.$thumbnails = $thumbnails;
        
        that.show(); //Must be visible to set up the scrolling :/
        
        // --- scrolling
        var $pannels = that.m_htmlStructure.index.mosaic.$pannels;
        var $scrollcontainer = that.m_htmlStructure.index.mosaic.$scrollContainer;
        $pannels.css({  'float' : 'left',
                               'position' : 'relative' // IE fix to ensure overflow is hidden
        }); 
        // calculate a new width for the container (so it holds all panels)
        $scrollcontainer.css('width', $pannels[0].offsetWidth * $pannels.length);
        $scrollViewport.scrollLeft( 0 );
        
        //binding the scrolling to the mosaic
        var scrollOptions = {
          $viewport: $scrollViewport,
          $navigation: that.m_htmlStructure.index.mosaic.$tabs,
          duration: 500, 
          easing: 'swing',
          onAfter: that.highlightNav
        };

         this.scroller = new CScroller( scrollOptions );
         
         that.hide();
                 
        //highlights the first navigation tab
        that.highlightNav ( that.m_htmlStructure.index.mosaic.$tabs.eq(0) );
    };
    
    //showing the mosaic
    this.show = function()
    {
        that.m_htmlStructure.index.mosaic.$container.show()
                                                                          .verticalCenter(0);
        that.m_htmlStructure.index.mosaic.$thumbBoxes.find('img').verticalCenter(0);
    }
    
    this.hide = function()
    {
        that.m_htmlStructure.index.mosaic.$container.hide();
    }
    
    
    //On window resize
    this.onResize = function()
    {
        that.m_htmlStructure.index.mosaic.$container.verticalCenter( 0 );
    }
    
    //Returns the loading Event
    this.getLoadingEvent = function() {
        return that.m_oneThumbLoadedEvent;
    }
    
    
        //Returns the loaded Event
    this.getLoadedEvent = function() {
        return that.m_allThumbsLoadedEvent;
    }
        
    
    // +++ internal tools +++
    //Compute the appropriate thumbnail set
    this.computeThumbnailsSet = function( )
    {
        var fSetFound = false;
        var thumbSet = that.m_properties.index.mosaic.defaultSet;    //if no suitable set is found
        var availableWidth = that.m_htmlStructure.index.$screen.width() - that.m_properties.index.mosaic.unavailable.horizontal - that.m_mosaicSizeMargin/2;
        var availableHeight = that.m_htmlStructure.index.$screen.height() - that.m_properties.index.mosaic.unavailable.vertical - that.m_mosaicSizeMargin;
        
        $.each( that.m_properties.index.mosaic.sizes, function( key, size ) //iterating on the object using jQuery
        {
            if (   !fSetFound
                && (availableWidth > size * that.m_properties.index.mosaic.nbCols)
                && (availableHeight > size * that.m_properties.index.mosaic.nbRows) )
            {
                thumbSet = key;
                fSetFound = true;
            }
        } );//each
        
        return thumbSet;
    };
    
    //Highlights the selected navigation tab
    this.highlightNav = function ( $toHighlight )
    {  
        $(".navTabSelected").removeClass("navTabSelected");
        $toHighlight.addClass("navTabSelected");
    }
    
        // ++++ Attributes ++++ //
    //General
    var that = this;
    this.m_properties = p_properties;
    this.m_htmlStructure = p_htmlStructure;
    // Mosaic
    this.m_mosaicSizeMargin = 100; //60 is necessary for vertical (tab height + logo). 100 is for horizontal comfort.
    this.m_mosaicNbThumbnails = that.m_properties.photos.list.length;
    this.m_mosaicNbThumbsByPanels = (that.m_properties.index.mosaic.nbRows * that.m_properties.index.mosaic.nbCols);
    this.m_mosaicNbPanels = Math.ceil( that.m_mosaicNbThumbnails / that.m_mosaicNbThumbsByPanels );
    this.m_nbThumbnailLoaded = 0;
    this.m_ThumbnailSize = 0;
    //Observing the loaded thumbnails
    this.m_oneThumbLoadedEvent = new CEvent();
    this.m_oneThumbLoadedEvent.subscribe( this.onThumbnailLoaded );
    this.m_allThumbsLoadedEvent = new CEvent();
    this.m_allThumbsLoadedEvent.subscribe( this.onThumbnailsLoaded );
        
};
