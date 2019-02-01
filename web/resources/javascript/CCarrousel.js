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
    this.html.$next  = $("#nextSlide");
    this.photoDisplayedLoadedEvent = new CEvent();
    this.scrollingEvent = new CEvent();
    this.scrolledEvent = new CEvent();

    this.loader = new CPhotoLoader( p_properties, this.html );
    this.frameFactory = new CFrameFactory( p_properties.photos.list );

    this.prevPhoto = -1;
    this.currPhoto = -1;
    this.nextPhoto = -1;

    //In: CPhoto
    this.displayPhoto = function( photo, $slide )
    {
        TOOLS.trace( "CCarrousel::displayPhoto - " + "photo: " + photo + "$slide: " + $slide);
        var $div = $slide.find( that.frameFactory.getStrPhoto() );
        $div.find('img').remove();
        var image = photo.getImage();
        $image = $( image );

        //If not the spinner, add the border
        if( photo.isLoaded() == true ) {
            $image.removeClass()
                       .addClass( that.html.photoClass );
        }


        $div.append( image );

        $div.width( $image.outerWidth() )
              .height( $image.outerHeight() );
         $div.parent().width( $div.outerWidth() )
                          .height( $div.outerHeight() + $div.siblings().outerHeight() ) //photo + title + caption
                          .verticalCenter( 0 );

        // TOOLS.trace( "CCarrousel::displayPhoto - " + "parent: " + $( that.frameFactory.getStrFrame() ).width() +  " - " + $( that.frameFactory.getStrFrame() ).parent().height() );
        // TOOLS.trace( "CCarrousel::displayPhoto - " + "div size: " + $div.width() +  " - " + $div.height() );
        // TOOLS.trace( "CCarrousel::displayPhoto - " + "img size: " + $image.width() +  " - " + $image.height() );
    }

    //Centers the viewport on the current slide
    this.centerViewport = function()
    {
        TOOLS.trace( "CCarrousel::centerViewport");
        that.html.$viewport.scrollLeft( that.html.$current.position().left );

    }

    this.previous = function( newid )
    {
        TOOLS.trace( "CCarrousel::previous" + "new id: " + newid);
        that.setCurrentPhoto( newid );

        var photo = that.loader.load( that.currPhoto, that.loader.PREVIOUS );
        that.currentPhoto = photo;
        that.frameFactory.frame( that.html.$previous, newid-1 );
        that.displayPhoto( photo, that.html.$previous );

        that.scroll( that.html.$previous );

        TOOLS.trace( "CCarrousel::previous");

        return photo;
    }

    this.load = function( id )
    {
        TOOLS.trace( "CCarrousel::load - " + "id: " + id);
        that.setCurrentPhoto( id );
        var photo = that.loader.load( id, that.loader.NEXT );
        that.displayPhoto( photo, that.html.$current );
        that.frameFactory.caption( that.html.$caption, id-1 );

        that.html.$viewport.scrollLeft( that.html.$current.position().left ); //centering on current photo
        that.currentPhoto = photo;

        return photo;
    }

    this.next = function( newid )
    {
        TOOLS.trace( "CCarrousel::next - " + "new id: " + newid);
        that.setCurrentPhoto( newid );

        var photo = that.loader.load( that.currPhoto, that.loader.NEXT );
        that.currentPhoto = photo;
        that.frameFactory.frame( that.html.$next, newid-1 );
        that.displayPhoto( photo, that.html.$next );

        that.scroll( that.html.$next );

        return photo;
    }


    this.getCurrentPhoto = function()
    {
        TOOLS.trace( "CCarrousel::getCurrentPhoto")
        return that.currentPhoto;
    }

    //updates the avalaible space
    this.setSpace = function( space )
    {
        TOOLS.trace( "CCarrousel::setSpace - " + "space: " - space);
        that.loader.setSpace( space );
    }

    //+++ PRIVATE
    this.updateHandles = function()
    {
        TOOLS.trace( "CCarrousel::updateHandles");
        that.html.$deprecated = $(".deprecated");
        that.html.$previous  = $("#previousSlide");
        that.html.$current  = $("#currentSlide");
        that.html.$next  = $("#nextSlide");
        that.html.$frame = that.html.$current.find( that.frameFactory.getStrFrame() );
        that.html.$div = that.html.$frame.find( that.frameFactory.getStrPhoto() );
        that.html.$caption = that.html.$frame.find( that.frameFactory.getStrCaption() );
        that.html.$slides = $(".slide");
    }

    this.setCurrentPhoto = function( id )
    {
        TOOLS.trace( "CCarrousel::setCurrentPhoto - " + "id: " + id);
        that.currPhoto = id;
        that.prevPhoto = id - 1;
        that.nextPhoto = id + 1;
    }


    this.scroll = function( $target )
    {
        TOOLS.trace( "CCarrousel::scroll - " + "$target: " + $target);
        that.scrollingEvent.fire();

        // updating slides attributes and handles.
        // "deprecated" will be erased from the hmt structure after the scrolling
        that.html.$frame.addClass("deprecated");
        if( $target == that.html.$previous ) { //backward scrolling
            //$("#slidesContainer").animate({left: "+="+that.html.$current.outerWidth()+"px"}, "fast", "swing", function() {that.onScrolled("BACKWARD");}  );                         $("#slidesContainer").animate({left: "+="+that.html.$current.outerWidth()+"px"}, "fast", "swing", function() {
            $("#slidesContainer").animate({left: "+="+that.html.$current.outerWidth()+"px"}, "fast", "swing", function() {    
                window.setTimeout( function() { // better to wait a bit
                                 that.onScrolled("BACKWARD");
                             }
                             , 20 );
            }  );
            that.html.$next.addClass("deprecated");
            that.html.$current.attr("id","nextSlide");
            that.html.$previous.attr("id","currentSlide");
        }
        else { //forward scrolling
            //$("#slidesContainer").animate({left: "-="+that.html.$current.outerWidth()+"px"}, "fast", "swing", function() {that.onScrolled("FORWARD");}  );
            $("#slidesContainer").animate({left: "-="+that.html.$current.outerWidth()+"px"}, "fast", "swing", function() {    
                window.setTimeout( function() { // better to wait a bit
                                 that.onScrolled("FORWARD");
                             }
                             , 20 );
            }  );
            that.html.$previous.addClass("deprecated");
            that.html.$current.attr("id","previousSlide");
            that.html.$next.attr("id","currentSlide");
        }

        that.updateHandles();
    }

    //+++ EVENTS

    this.onPhotoLoaded = function(  )
    {    
        TOOLS.trace( "CCarrousel::onPhotoLoaded");
        //typeof this: CPhoto
        if( this.id == that.currPhoto )  {
            that.displayPhoto( this, that.html.$current );
            that.photoDisplayedLoadedEvent.fire( this );
        }
    }
    this.loader.getPhotoLoadedEvent().subscribe( that.onPhotoLoaded );

    this.onScrolled = function( direction )
    {
        TOOLS.trace( "CCarrousel::onScrolled - " + "erase $deprecated: " + that.html.$deprecated);
        //updating page's structure
        that.html.$deprecated.remove(); //removing the deprecated structures changes the slide under the viewport
        if( direction == "FORWARD" ) {
            $("#slidesContainer").css("left", "0px");
            that.html.$current.parent().append( "<div id=\"nextSlide\" class=\"slide\"></div>" );
        }
        else {
            that.html.$current.parent().prepend( "<div id=\"previousSlide\" class=\"slide\"></div>" );
            $("#slidesContainer").css("left", "0px");
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
