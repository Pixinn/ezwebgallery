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

    // When all thumbnails are loaded
    // Resizes the mosaic and its elements
    this.onThumbnailsLoaded = function( )
    {
        that.htmlStructure.index.mosaic.$thumbnails = that.htmlStructure.index.mosaic.$thumbBoxes.find( 'img' );

        var $thumbBoxes = that.htmlStructure.index.mosaic.$thumbBoxes;
        var $thumbnails = that.htmlStructure.index.mosaic.$thumbnails;
        that.htmlStructure.index.mosaic.$handle.show(); //show necessary for the correct sizes
        that.htmlStructure.index.mosaic.$handle.width("95%");;
        $thumbBoxes.css("box-sizing", "border-box");
        var width = (1 / that.properties.index.mosaic.nbCols)*100;
        $thumbBoxes.css("width", width + "%");
        that.ratioThumbboxeBorders = $thumbBoxes.eq(0).css("border-left-width").replace("px", "") / that.htmlStructure.index.mosaic.$handle.width();
        that.ratioThumbnailBorders = $thumbnails.eq(0).css("border-left-width").replace("px", "") / that.htmlStructure.index.mosaic.$handle.width();

        that.resize();
    };

    //showing the mosaic
    this.show = function( $thumbDisplayed )
    {
        that.htmlStructure.index.mosaic.$thumbnails.verticalCenter(0);
        //centering on $thumbDisplayed
        if(typeof $thumbDisplayed != "undefined") {
            that.center( $thumbDisplayed );
        }
    }

    this.hide = function()
    {
        that.htmlStructure.index.mosaic.$handle.hide();
    }


    this.center = function( $thumbDisplayed )
    {
        var $firstThumbBox = that.htmlStructure.index.mosaic.$thumbBoxes.eq(0);
        var scrollAmount = $thumbDisplayed.offset().top - $firstThumbBox.offset().top;
        that.htmlStructure.index.mosaic.$wrapper.scrollTop( scrollAmount );
        TOOLS.trace( "scrollAmount: " + scrollAmount );
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
    // Find the set that give a size immediately greater than the available space
    this.computeThumbnailsSet = function( )
    {
        var thumbSet = that.properties.index.mosaic.defaultSet;    //if no suitable set is found
        var availableWidth = that.htmlStructure.index.mosaic.$wrapper.innerWidth();

        var diffMin = Number.MAX_VALUE;
        $.each( that.properties.index.mosaic.sizes, function( key, size )
        {
            var diff = availableWidth - that.properties.index.mosaic.nbCols * (size + 2*that.properties.index.mosaic.decoration.borderBox + 2*that.properties.index.mosaic.decoration.borderPhoto);
            if( diff > 0 && diff < diffMin ) {
                diffMin = diff;
                thumbSet = key;
            }
        });

        return thumbSet;
    };


    // Resize the mosaic and all its elements to fit the display
    // This is a *SLOW* function -> call it with care
    this.resize = function()
    {
        var $thumbBoxes = that.htmlStructure.index.mosaic.$thumbBoxes;
        var $thumbnails = that.htmlStructure.index.mosaic.$thumbnails;

        // Squaring the boxes
        width = $thumbBoxes.eq(0).width();
        $thumbBoxes.css("height", width + "px");
        // Making the thumbnails fit
        $thumbnails.css("max-width", "100%");
        $thumbnails.css("max-height", "100%");

        // Resizing the borders
        var sizeBorder = that.ratioThumbboxeBorders * that.htmlStructure.index.mosaic.$handle.width();
        $thumbBoxes.css("border-left-width", sizeBorder + "px")
                   .css("border-top-width", sizeBorder + "px")
                   .css("border-right-width", sizeBorder + "px")
                   .css("border-bottom-width", sizeBorder + "px");
       sizeBorder = that.ratioThumbnailBorders * that.htmlStructure.index.mosaic.$handle.width();
       $thumbnails.css("border-left-width", sizeBorder + "px")
                  .css("border-top-width", sizeBorder + "px")
                  .css("border-right-width", sizeBorder + "px")
                  .css("border-bottom-width", sizeBorder + "px");

        var nbLines = Math.ceil(that.mosaicNbThumbnails / that.properties.index.mosaic.nbCols);
        var heightThumbs = width * nbLines;
        // Adjusting Thumbboxes' sizes - Max height
        var idxThumbs = 0;
        var idxBoxes = 0;
        var nbThumbsLeft = that.mosaicNbThumbnails;
        var heightMosaic = 0;
        for(var i = 0; i < nbLines; ++i) {
            var heightMax = 0;
            var nbThumbs = Math.min(that.properties.index.mosaic.nbCols, nbThumbsLeft);
            //Find max height of the line
            for(var j = 0; j < nbThumbs; ++j) {
                var height = $thumbnails.eq(idxThumbs++).height();
                if( heightMax < height) {
                    heightMax = height;
                }
                --nbThumbsLeft;
            }
            for(var j = 0; j < nbThumbs; ++j) {
                $thumbBoxes.eq(idxBoxes++).css("height", heightMax + "px");
            }
            heightMosaic += heightMax;
        }
        // Adjusting Thumbboxes' sizes - Mean height
        // var nbLines = Math.ceil(that.mosaicNbThumbnails / that.properties.index.mosaic.nbCols);
        // var idx = 0;
        // var idxLine = 0;
        // var nbThumbsLeft = that.mosaicNbThumbnails;
        // for(var i = 0; i < nbLines; ++i) {
        //     var heights = 0;
        //     var idxLine = idx;
        //     var nbThumbs = Math.min(that.properties.index.mosaic.nbCols, nbThumbsLeft);
        //     // Find max height of the line
        //     for( var j = 0; j < nbThumbs; ++j) {
        //         var $thumb = $thumbnails.eq(idx++);
        //         heights +=  $thumb.height();
        //         --nbThumbsLeft;
        //     }
        //     var heightMean = heights / nbThumbs;
        //     // All heights of the line ) max height
        //     for( var j = 0; j < nbThumbs; ++j) {
        //         $thumbBoxes.eq(idxLine++).css("height", heightMean + "px");
        //     }
        //     // Update the total height of the thumbboxes
        //     heightThumbs += heightMean;
        // }
        $thumbnails.verticalCenter(0);
        // Masaic height
        heightMosaic += that.htmlStructure.index.mosaic.$title.outerHeight(true) + 50;
        that.htmlStructure.index.mosaic.$handle.height( heightMosaic );
    }

        // ++++ Attributes ++++ //
    //General
    var that = this;
    this.properties = p_properties;
    this.htmlStructure = p_htmlStructure;
    // Mosaic
    this.mosaicNbThumbnails = that.properties.photos.list.length;
    this.nbThumbnailLoaded = 0;
    // Elements size ratios
    this.ratioThumbboxeBorders = 0;
    this.ratioThumbnailBorders = 0;
    //Observing the loaded thumbnails
    this.oneThumbLoadedEvent = new CEvent();
    this.oneThumbLoadedEvent.subscribe( this.onThumbnailLoaded );
    this.allThumbsLoadedEvent = new CEvent();
    this.allThumbsLoadedEvent.subscribe( this.onThumbnailsLoaded );

};
