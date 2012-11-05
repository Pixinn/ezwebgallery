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

Defines = {
    URL_THUMBS_PATH: "thumbnails",
    IMAGES_PATH: "images"
};

TOOLS = {
    trace: function( log ){
        if (window.console) {
            console.log( log );
        }
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
            top =( parent.height( ) - obj.outerHeight( )  )/ 2 - offset;
            top = Math.max( top, 0 ); // No offset < 0! They lead to positionning bugs.
            obj.css("top",top);
        }
     else {
        TOOLS.trace( "Vertical center: object or its parent not visible!" );
     }
  });
};


$(document).ready(function()
{
    g_properties.defines = Defines;

    HtmlStructure = 
    {
    progressBar : {
        $box : $("#progressbarWrapper"),
        $bar : $("#progressbar"),
    },
    index : {
        $screen : $("#screenIndex"),
        mosaic : {
            $container : $("#indexSliderContainer"),
            $scrollViewport : $(".thumbsWrapper"),
            $scrollContainer : $(".scrollContainer"),
            $tabsContainer : $("ul.indexNavigation")
            }
       },
    photo : {
        $screen : $("#screenPhoto"),
        $frame : $("#cadrePhoto"),
            $wrapper : $("#wrapperAffichagePhoto"),
            $div : $("#divPhoto"),
            $title : $("#photoTitle"),
        buttons : {
                $previous : $("#boutonPrevious"),
                $next : $("#boutonNext"),
                $close : $("#boutonIndex")
        }
       }
    };

    //Creating instances
    Mosaic = new CMosaic( g_properties, HtmlStructure );
    progressBar = new CProgressBar ( {$progessBar: HtmlStructure.progressBar.$bar, nbThumbnails: g_properties.photos.list.length} );
    Mosaic.getLoadingEvent().subscribe( progressBar.onLoad );
    Mosaic.getLoadedEvent().subscribe( function() { //When all thumbnails are loaded
          //Showing mosaic
          progressBar.hide();
          Mosaic.show();
          //Removing the progressBar
          Mosaic.getLoadingEvent().unsubscribe( progressBar.onLoad() );
          delete progressBar;
          HtmlStructure.progressBar.$box.remove(  );
          delete HtmlStructure.progressBar.$box;
    } );

    var Display = new CDisplay( g_properties, HtmlStructure );
    //User events
    var UserHandler;
    Mosaic.getLoadedEvent().subscribe( function()
    {   //at this point, HtmlStructure must be *completed* !
        //Handling user' interractions
        UserHandler = new CUserInterractions( g_properties, HtmlStructure );
        UserHandler.start();
        Display.getPhotoDisplayedLoadedEvent().subscribe( function() { UserHandler.onPhotoDisplayedLoaded( this.id ); } );
        Display.getPhotoScreenEvent().subscribe( UserHandler.onPhotoScreen );
        Display.getIndexScreenEvent().subscribe( UserHandler.onIndexScreen );
        
        //Subscribing to user events
        UserHandler.getWindowResizedEvent().subscribe( Mosaic.onResize );
        UserHandler.getThumbnailClickedEvent().subscribe( function() { Display.displayPhoto( parseInt(this.id) ); } ); //this will be the object clicked
        UserHandler.getClosePhotoEvent().subscribe( Display.hidePhoto );
        UserHandler.getPreviousPhotoEvent().subscribe( Display.onPrevious );
        UserHandler.getPreviousPhotoEvent().subscribe( function() { Mosaic.onPreviousNext(this); } );
        UserHandler.getNextPhotoEvent().subscribe( Display.onNext );
        UserHandler.getNextPhotoEvent().subscribe( function() { Mosaic.onPreviousNext(this); } );
        UserHandler.getPreviousIndexEvent().subscribe( Mosaic.onPreviousIndex );
        UserHandler.getNextIndexEvent().subscribe( Mosaic.onNextIndex );        
        
    });

    //Building the mosaic / Loading the thumbnails
    progressBar.show();
    HtmlStructure = Mosaic.buildHtml(); //loads the thumbnails and build the html


} );
