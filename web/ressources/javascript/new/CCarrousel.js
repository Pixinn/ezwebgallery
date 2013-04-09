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

function CCarrousel( p_properties, p_html )
{
    var that = this;
    this.properties = p_properties;
    this.html = p_html;
    this.html.photoClass = "mainPhoto";
    this.html.$viewport = $("#photoViewport");
    this.html.$slides = $(".slide");
    this.html.$deprecated = "NULL";
    this.html.$previous  = $("#previousSlide");
    this.html.$current  = $("#currentSlide");
    this.html.$next  = $("#nextSlide");
    this.photoDisplayedLoadedEvent = new CEvent();
    this.scrollingEvent = new CEvent();
    this.scrolledEvent = new CEvent();
    
    this.loader = new CPhotoLoader( p_properties, this.html );
    
    this.scrolledAmount = 0;
    this.prevPhoto = -1;
    this.currPhoto = -1;
    this.nextPhoto = -1; 

    //In: CPhoto
    this.displayPhoto = function( photo, $slide )
    {   
        var $div = $slide.find("#divPhoto");
        $div.find('img').remove();
        var image = photo.getImage();
        $image = $( image );
        
        if( photo.isLoaded() == true ) { //If not the spinner, add the border
            $image.removeClass()
                       .addClass( that.html.photoClass );
        }
        $div.append( image );                
        
        $div.width( $image.outerWidth() )
              .height( $image.outerHeight() );
         $div.parent().width( $div.outerWidth() )
                          .height( $div.outerHeight() + $div.siblings().outerHeight() ) //photo + title + caption
                          .verticalCenter( 0 );
                          
        TOOLS.trace( "parent: " + $("#cadrePhoto").width() +  " - " + $("#cadrePhoto").parent().height() );
        TOOLS.trace( "div: " + $div.width() +  " - " + $div.height() );
        TOOLS.trace( "img: " + $image.width() +  " - " + $image.height() );
    }
    
    //Centers the viewport on the current slide
    this.centerViewport = function()
    {
        that.html.$viewport.scrollLeft( that.html.$current.position().left );
    }

    this.previous = function( newid )
    {
        that.setCurrentPhoto( newid );
                       
        var photo = that.loader.load( that.currPhoto, that.loader.PREVIOUS );
        that.currentPhoto = photo;
        that.insertPhotoFrame( that.html.$previous );
        that.displayPhoto( photo, that.html.$previous );
        
        that.scroll( that.html.$previous );
        
        return photo;
    }

    this.load = function( id )
    {
        that.setCurrentPhoto( id );   
        var photo = that.loader.load( id, that.loader.NEXT );        
        that.displayPhoto( photo, that.html.$current );
        
        that.html.$viewport.scrollLeft( that.html.$current.position().left ); //centering on current photo
        that.currentPhoto = photo;
        
        return photo;
    }

    this.next = function( newid )
    {
        that.setCurrentPhoto( newid );
                       
        var photo = that.loader.load( that.currPhoto, that.loader.NEXT );
        that.currentPhoto = photo;
        that.insertPhotoFrame( that.html.$next );
        that.displayPhoto( photo, that.html.$next );
        
        that.scroll( that.html.$next );
        
        return photo;
    }

    
    this.getCurrentPhoto = function()
    {
        return that.currentPhoto;
    }
    
    //updates the avalaible space
    this.setSpace = function( space )
    {
        that.loader.space = space;
    }

    //+++ PRIVATE
    this.updateHandles = function()
    {
        that.html.$deprecated = $(".deprecated");
        that.html.$previous  = $("#previousSlide"); 
        that.html.$current  = $("#currentSlide");
        that.html.$next  = $("#nextSlide");
        that.html.$frame = that.html.$current.find("#cadrePhoto");
        that.html.$div = that.html.$frame.find("#divPhoto");
        that.html.$title = that.html.$frame.find("#photoTitle");
        that.html.buttons.$close = that.html.$title.find("#boutonIndex");    
        that.html.$slides = $(".slide");
    }
    
    this.setCurrentPhoto = function( id )
    {
        that.currPhoto = id;        
        that.prevPhoto = id - 1;
        that.nextPhoto = id + 1;
    }
    
    this.scroll = function( $target )
    {
        that.scrollingEvent.fire();
        that.scrolledAmount = $target.position().left;        
        
        //updating slides attributes and handles.
        //"deprecated" will be erased from the hmt structure after the scrolling
        that.html.$frame.addClass("deprecated");
        if( $target == that.html.$previous ) { //backward scrolling
            that.html.$viewport.animate({scrollLeft: that.scrolledAmount}, "fast", "swing", function() {that.onScrolled("BACKWARD");}  );
            that.html.$next.addClass("deprecated");        
            that.html.$current.attr("id","nextSlide");
            that.html.$previous.attr("id","currentSlide");        
        }
        else { //forward scrolling
            that.html.$viewport.animate({scrollLeft: that.scrolledAmount}, "fast", "swing", function() {that.onScrolled("FORWARD");}  );
            that.html.$previous.addClass("deprecated");        
            that.html.$current.attr("id","previousSlide");
            that.html.$next.attr("id","currentSlide");   
        }

        that.updateHandles();
    }
    
    this.insertPhotoFrame = function( $slide )
    {
        $slide.append( "<div id=\"cadrePhoto\"><div id=\"photoTitle\"><img id=\"boutonIndex\" src=\"ressources/images/close.gif\"></div><div id=\"divPhoto\"></div></div>" );
    }
    
    //+++ EVENTS

    this.onPhotoLoaded = function(  )
    {    //typeof this: CPhoto
        if( this.id == that.currPhoto )  { 
            that.displayPhoto( this, that.html.$current ); 
            that.photoDisplayedLoadedEvent.fire( this );
        }
    }
    this.loader.getPhotoLoadedEvent().subscribe( that.onPhotoLoaded );
    
    this.onScrolled = function( direction )
    {
        //updating page's structure
        that.html.$deprecated.remove(); //removing the deprecated structures changes the slide under the viewport                   
        if( direction == "FORWARD" ) {
            that.html.$viewport.scrollLeft( that.html.$current.outerWidth() ); //quickly recenter the viewport on the new current slide 
            that.html.$current.parent().append( "<div id=\"nextSlide\" class=\"slide\"></div>" );                  
        }
        else {
            that.html.$current.parent().prepend( "<div id=\"previousSlide\" class=\"slide\"></div>" );
            that.html.$viewport.scrollLeft( that.html.$current.outerWidth() ); //quickly recenter the viewport on the new current slide 
        }

        that.updateHandles();
        that.scrolledEvent.fire();
    }

    //--- EVENTS
    
    this.getPhotoDisplayedLoadedEvent = function( )
    {
        return that.photoDisplayedLoadedEvent;
    }
    
    this.getScrollingEvent = function()
    {
        return that.scrollingEvent;
    }
    
    this.getScrolledEvent = function() 
    {
        return that.scrolledEvent;
    }

}
