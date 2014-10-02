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

Defines = {
    URL_THUMBS_PATH: "thumbnails",
    IMAGES_PATH: "images",
};

TOOLS = {
    trace: function( log ){ //trace a message in the console if available
        if (window.console) {
            var date = new Date();
            console.log( date.getTime() + ": " + log );
        }
    },
    clone: function( obj ) {
        var cloned  = {};
        jQuery.extend( cloned, obj );
        return cloned;
    },
    debug: function( msg ) { //displays a debug message on the top of the page
        $("#DEBUG").append( msg )
                   .show();
    },
    
    
}

GOOGLEAPI = {
    evtLoaded: new CEvent(),
    initGoogleAPI: function() {
        //Read http://stackoverflow.com/questions/14184956/async-google-maps-api-v3-undefined-is-not-a-function
        this.evtLoaded.fire();
    }
    
}


/*
 * Pseudo plugin JQuery pour centrer un objet verticalement dans son parent
 * On peut passer un décalage en paramètre pour ajuster le calcul si l'objet n'est pas seul dans son parent
 * !! -> Nécessite un objet en position RELATIVE !!
 */
jQuery.fn.verticalCenter = function( offset ) {
  return this.each(function(  ){
     var obj = jQuery(this);
     var parent = obj.parent( );
     if( obj.is(':visible') && parent.is(':visible') ) { //Doesn't properly work if object or its parent are not visible
            var top;
            top =( parent.innerHeight( ) - obj.outerHeight( )  )/ 2 - offset;
            top = Math.max( top, 0 ); // No offset < 0! They lead to positionning bugs.
            obj.css("top",top);
        }
     else {
        TOOLS.trace( "Vertical center: object or its parent not visible!" );
     }
  });
};


/*
 * Fit a JQuery object in a given width
 */
jQuery.fn.fit = function( width ) {
  return this.each(function(  ){
    var obj = jQuery(this);
    var ratio = obj.width() / obj.height();
    obj.width( width )
        .height( width / ratio );
     
  });
};

//Never called as attr("src",...) are not ajax loadings
$(document).ajaxError( function()
{
    TOOLS.trace("Error ajax: " + $(this).attr("src") );                            
});


//Starting point
$(document).ready(function()
{
    g_properties.defines = Defines;
    
    HtmlStructure = 
    {
    $window : $(window),
    progressBar : {
        $screen : $("#screenProgress"),
        $box : $("#progressbarWrapper"),
        $bar : $("#progressbar")        
    },
    toolbar : {
        $buttonMap: $(".button_map"),
        $buttonIndex:  $(".boutonIndex")
    },
    map : {
        $screen : $("#screenMap"),        
    },
    index : {
        $screen : $("#screenIndex"),
        mosaic : {
            $wrapper :  $("#wrapper_mosaic"),
            $handle : $("#mosaic"),
            $title : $("#indexTitle")
            }
       },
    photo : {
        $screen : $("#screenPhoto"),
        $current : $("#currentSlide"),
        $frame : $("#cadrePhoto"),
        $wrapper : $("#wrapperAffichagePhoto"),
        $div : $("#divPhoto"),
        $caption : $(".photoCaption"),
        $navBars : $.merge( $('#photoLefter'), $('#photoRighter') ),
        buttons : {
                $previous : $("#boutonPrevious"),
                $next : $("#boutonNext"),
                $close :  $("#screenPhoto").find(".boutonIndex")
        }
       }
    };
    
    HtmlStructure.index.$screen.find(".boutonIndex").css("opacity","0.4")
                                                    .css("filter","alpha(opacity=40)");

    //Creating instances
    Mosaic = new CMosaic( g_properties, HtmlStructure );
    progressBar = new CProgressBar ( {$screen: $("#screenPogress"), $progessBar: HtmlStructure.progressBar.$bar, nbThumbnails: g_properties.photos.list.length} );
    Mosaic.getLoadingEvent().subscribe( progressBar.onLoad );
    Mosaic.getLoadedEvent().subscribe( function() { //When all thumbnails are loaded
          //Showing mosaic
          progressBar.hide();
          Mosaic.show();
          //Removing the progressBar
          Mosaic.getLoadingEvent().unsubscribe( progressBar.onLoad() );
          HtmlStructure.progressBar.$screen.remove(  );
          delete HtmlStructure.progressBar.$screen;
    } );
    
    var Display = new CDisplay( g_properties, HtmlStructure );    
    var UserHandler; //Managing user events
    
    // --- ENTERING THIS FUNCTION WHEN THE THUMBNAILS ARE LOADED
    Mosaic.getLoadedEvent().subscribe( function()
    {   
        //at this point, HtmlStructure must be *completed* !
        //Handling user' interractions
        UserHandler = new CUserInterractions( g_properties, HtmlStructure );
        UserHandler.start();
        Display.getPhotoDisplayedLoadedEvent().subscribe( function() { UserHandler.onPhotoDisplayedLoaded( this.id ); } );
        Display.getPhotoScreenEvent().subscribe( UserHandler.onPhotoScreen );
        Display.getIndexScreenEvent().subscribe( function() { Mosaic.show( this ); } );
        Display.getIndexScreenEvent().subscribe( UserHandler.onIndexScreen );
        Display.getDisableUISignal().subscribe( UserHandler.disablePreviousNext );
        Display.getEnableUISignal().subscribe( UserHandler.enablePreviousNext );
        Display.getScrollingEvent().subscribe( UserHandler.onScrolling );
        Display.getScrolledEvent().subscribe( UserHandler.onScrolled );
        
        //Subscribing to user events
        UserHandler.getWindowResizedEvent().subscribe( Mosaic.onResize );
        UserHandler.getThumbnailClickedEvent().subscribe( function() { Display.displayPhoto( parseInt(this.id) ); } ); //this, will be the object clicked
        //UserHandler.getClosePhotoEvent().subscribe( Display.hidePhoto );
        UserHandler.getPreviousPhotoEvent().subscribe( Display.onPrevious );        
        UserHandler.getNextPhotoEvent().subscribe( Display.onNext );
        
        //Display according to URL
        Display.displayCurrentUrl(); // --> STARTING POINT FOR THE USER <--
    });

    //loading google maps api
    GOOGLEAPI.evtLoaded.subscribe( Display.buttonMap.enable );
    var mapAPI = document.createElement("script");
    mapAPI.src = "http://maps.googleapis.com/maps/api/js?sensor=false&callback=GOOGLEAPI.initGoogleAPI";
    document.head.appendChild( mapAPI );

    //Building the mosaic / Loading the thumbnails
    progressBar.show();    
    HtmlStructure = Mosaic.buildHtml(); //loads the thumbnails and build the html

    
    

} );
