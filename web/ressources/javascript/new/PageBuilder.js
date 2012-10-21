
//"Highlight" l'onglet de navigation correspondant à l'index  affiché 
// voir : http://jqueryfordesigners.com/coda-slider-effect/
function highlightNav() {
    $("ul.indexNavigation  a").each( function(){
		$(this).parent().removeClass("navTabSelected"); // enlever la classe de TOUS les onglets
	} );
   $(this).parent().addClass("navTabSelected");  						   
}


/* Appelé à la fin d'un scrolling par le "onAfter" défini dans "scrolloptions" 
 * voir : http://jqueryfordesigners.com/coda-slider-effect/
 */
function triggedAfterScroll(data) {
  // within the .navigation element, find the A element
  // whose href ends with ID ($= is ends with)
  var el = $("ul.indexNavigation").find('a[href$="' + data.id + '"]').get(0);
  // we're passing the actual element, and not the jQuery instance.
  highlightNav.call(el);
}





function CPageBuilder( p_properties, p_htmlStructure ) {
    
    // ++++ Attributes ++++ //
    //General
    var self = this;
    this.m_properties = p_properties;
    this.m_htmlStructure = p_htmlStructure;
    // Mosaic
    this.m_mosaicSizeMargin = 100; //60 is necessary for vertical (tab height + logo). 100 is for horizontal comfort.
    this.m_mosaicNbThumbnails = self.m_properties.photos.list.length;
    this.m_mosaicNbThumbsByPanels = (self.m_properties.index.mosaic.nbRows * self.m_properties.index.mosaic.nbCols);
    this.m_mosaicNbPanels = Math.ceil( self.m_mosaicNbThumbnails / self.m_mosaicNbThumbsByPanels );
    this.m_nbThumbnailLoaded = 0;
    this.m_ThumbnailSize = 0;
    //div naming
    // ++++ Methods ++++ //
      
    //Building the thumbnail mosaic
    this.buildMosaic = function( ) 
    {  
        var divSlidingPanelName = "slidingPanel";
        var divThumbBoxName = "thumbBox";
        
        //Generating panels and tabs
        for( var numIndex = 1; numIndex <= self.m_mosaicNbPanels; numIndex++ ) {
            self.m_htmlStructure.index.mosaic.$scrollContainer.append( '<div class="' + divSlidingPanelName + '" id="' + numIndex + '"></div>' );
        }
        var $pannels = $("." + divSlidingPanelName);
        self.m_htmlStructure.index.mosaic.$pannels = $pannels;
        
        //Generating the tabs
        for( var numIndex = 1; numIndex <= self.m_mosaicNbPanels; numIndex++ ) {
            self.m_htmlStructure.index.mosaic.$tabsContainer.append('<li class=\"tab\"><a href="#' + numIndex + '" id="' + numIndex + '">' + numIndex + '</a></li>');
        }
        
        //Generating the thumbnail containers        
        var thumbNr = 1;
        for (var pannel = 0; pannel < $pannels.length; pannel++)
        {
            var i = 1;
            while( ( i <= self.m_mosaicNbThumbsByPanels ) && ( thumbNr <= self.m_mosaicNbThumbnails ) ) {
                $pannels.eq(pannel).append('<div class="'+ divThumbBoxName +'" id="' + thumbNr + '"></div>');	
                thumbNr++;
                i++;
            }       
        }
        var $thumbBoxes = $("." + divThumbBoxName);
        self.m_htmlStructure.index.mosaic.$thumbBoxes = $thumbBoxes;
        
        //Loading the thumbnails        
        this.thumbnailsSet = this.computeThumbnailsSet();
        self.m_ThumbnailSize = self.m_properties.index.mosaic.sizes[ this.thumbnailsSet ]
        $thumbBoxes.each( function( index )
        {
            var thumbName = self.m_properties.photos.list[ index ].filename;
            var thumbnail = new Image();
            $(this).append( thumbnail );
            $(thumbnail).load( function( )	{	// callback OnLoad
                            if( ++self.m_nbThumbnailLoaded == self.m_mosaicNbThumbnails ) {
                                self.onThumbnailsLoaded( );
                            }
                            //$(DIV_PROGRESSBARNAME).reportprogress( g_nbThumbFullyLoaded, g_nbThumbnails );                            
                        })
                        .error( function()  {
                            TOOLS.debug("Error Loadind a thumbnail: " + ++self.m_nbThumbnailLoaded);                            
                        })
                        // Source de l'image : A METTRE EN DERNIER
                        .attr("src", src = self.m_properties.defines.URL_THUMBS_PATH+self.thumbnailsSet+'/'+thumbName );
                        
        } );
        
        
        //Returning the updated html structure
        return self.m_htmlStructure;
    };
    
    
    //When all thumbnails are loaded
    this.onThumbnailsLoaded = function( )
    {               
        //resizing the mosaic according to the thumb set chosen
        var $thumbsWrapper = $(".thumbsWrapper");
        var $scrollingLinks = $(".indexNavigation a");
        var $thumbBoxes = self.m_htmlStructure.index.mosaic.$thumbBoxes;
        var $thumbnails = $thumbBoxes.find( 'img' );
        var thumbBorderWidth = $thumbnails.first().css("border-top-width");
        thumbBorderWidth = thumbBorderWidth.replace("px","");
        $thumbBoxes.width( self.m_ThumbnailSize + 2*thumbBorderWidth )
                   .height( self.m_ThumbnailSize + 2*thumbBorderWidth);
        var mosaicWidth = self.m_properties.index.mosaic.nbCols * $thumbBoxes.outerWidth();
        var mosaicHeight = self.m_properties.index.mosaic.nbRows * $thumbBoxes.outerHeight();
        
        var $thumbsWrapperContainer = $(".thumbsWrapperContainer");
        var thumbsWrapperContainerBorder = $thumbsWrapperContainer.css("border-left-width")
        thumbsWrapperContainerBorder = thumbsWrapperContainerBorder.replace( "px","" );
        $("#indexSliderContainer").width( mosaicWidth + 2*thumbsWrapperContainerBorder );
        $thumbsWrapperContainer.width( mosaicWidth )
                               .height( mosaicHeight );
        $thumbsWrapper.width( mosaicWidth )
                      .height( mosaicHeight );
        self.m_htmlStructure.index.mosaic.$scrollContainer.find(".slidingPanel").width( mosaicWidth )
        .height( mosaicHeight );
        
        //showing the mosaic
        self.m_htmlStructure.index.mosaic.$container.show()
                                                    .verticalCenter(0);
        $thumbnails.verticalCenter(0);
        
        //binding the scrolling to the mosaic
        var scrollOptions = {
          target: $thumbsWrapper, // the element that has the overflow
                                  // can be a selector which will be relative to the target
          margin: true,								
          items: self.m_htmlStructure.index.mosaic.$pannels, // Que scrolle t'on ?
          navigation: "a", // En cliquant sur quoi ?
          axis: 'xy',    // allow the scroll effect to run both directions
          duration: 500, // duration of the sliding effect
          easing: 'swing', //Effet de "freinage"
          onAfter: triggedAfterScroll // our final callback
        };

         //Tous les liens de la page permettent de faire scroller
        $.localScroll( scrollOptions );
    };
        
    
    // +++ internal tools +++
    //Compute the appropriate thumbnail set
    this.computeThumbnailsSet = function( )
    {
        var fSetFound = false;
        var thumbSet = self.m_properties.index.mosaic.defaultSet;    //if no suitable set is found
        var availableWidth = self.m_htmlStructure.index.$screen.width() - self.m_properties.index.mosaic.unavailable.horizontal - self.m_mosaicSizeMargin/2;
        var availableHeight = self.m_htmlStructure.index.$screen.height() - self.m_properties.index.mosaic.unavailable.vertical - self.m_mosaicSizeMargin;
        
        $.each( self.m_properties.index.mosaic.sizes, function( key, size ) //iterating on the object using jQuery
        {
            if (   !fSetFound
                && (availableWidth > size * self.m_properties.index.mosaic.nbCols)
                && (availableHeight > size * self.m_properties.index.mosaic.nbRows) )
            {
                thumbSet = key;
                fSetFound = true;
            }
        } );//each
        
        return thumbSet;
    };
};
