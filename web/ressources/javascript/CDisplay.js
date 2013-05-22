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

function CDisplay( p_properties, p_htmlStructure )
{
    var that = this;
    this.html = p_htmlStructure;
    this.html.photo.spinnerSrc = "ressources/images/ajax-loader.gif";

    this.properties = p_properties;
    this.carrousel = new CCarrousel( p_properties, this.html.photo );
    this.idCurrentPhoto = -1;
    this.previousEvent = new CEvent();
    this.nextEvent = new CEvent();
    this.photoScreenEvent = new CEvent();
    this.indexScreenEvent = new CEvent();
    this.disableUISignal = new CEvent();
    this.enableUISignal = new CEvent();
    this.displayedPhotoLoadedEvent = new CEvent();

    this.availableSpace = {h: 0, w:0};

    $(window).resize( function() {
        that.setSpace( that.computeAvailableSpace() );
        if( that.html.photo.$screen.is(":visible") === true )
        {
            that.fitPhoto( that.carrousel.getCurrentPhoto() );
            that.html.photo.buttons.$previous.verticalCenter(0);
            that.html.photo.buttons.$next.verticalCenter(0);
            that.carrousel.centerViewport(); //We need to reposition the viewport to adapt to the new slides' size
        }
    } );


    // ++++ Methods ++++ //

    this.displayPhoto = function( id )
    {
        that.idCurrentPhoto = id;
        that.html.photo.$screen.fadeIn(); //photoScreen is located above the index screen: no need to hide it
        that.html.photo.buttons.$next.verticalCenter(0);
        that.html.photo.buttons.$previous.css("top",  that.html.photo.buttons.$next.css("top") ); //no v center on previous to correct an ie8 bug
        
        that.setSpace( that.computeAvailableSpace() );
        that.load( id, that.carrousel.load );
        
        that.photoScreenEvent.fire();
    }


    this.hidePhoto = function( ) {
        that.html.photo.$screen.fadeOut('fast');
        that.indexScreenEvent.fire();
    }
    

    //+++ EVENTS

    this.getPreviousEvent = function()
    {
        return that.previousEvent;
    }

    this.getNextEvent = function()
    {
        return that.nextEvent;
    }

    this.getPhotoDisplayedLoadedEvent = function()
    {
        return that.displayedPhotoLoadedEvent;
    }
    
    this.getPhotoScreenEvent = function()
    {
        return that.photoScreenEvent;
    }
    
    this.getIndexScreenEvent = function()
    {
        return that.indexScreenEvent
    } 
    
    this.getDisableUISignal = function()
    {
        return that.disableUISignal;
    }
    
    this.getEnableUISignal = function()
    {
        return that.enableUISignal;
    }
    
    this.getScrollingEvent = function()
    {
        return that.carrousel.getScrollingEvent();
    }
    
    this.getScrolledEvent = function()
    {
        return that.carrousel.getScrolledEvent();
    }


    this.onPrevious = function()
    {
        that.idCurrentPhoto--;
        that.load( that.idCurrentPhoto, that.carrousel.previous );
    }

    this.onNext = function()
    {
        that.idCurrentPhoto++;
        that.load( that.idCurrentPhoto, that.carrousel.next );
    }

    this.onPhotoDisplayedLoaded = function()
    {
        that.displayedPhotoLoadedEvent.fire( this );
        that.fitPhoto( this );        
    }
    that.carrousel.getPhotoDisplayedLoadedEvent().subscribe( this.onPhotoDisplayedLoaded );
    
    //+++ Private
    
    this.setSpace = function( space )
    {
        that.availableSpace = space;
        that.carrousel.setSpace( space );
    }
    
    this.load = function( id, loadFct ) //loadFct( id ) : function used to fetch the photo
    {
        that.disableUISignal.fire();
        var photo = loadFct( id ); //must be placed *after* the fadein or the dimensions will be incorrectly computed
        if( photo.isLoaded() == true ) {
            that.displayedPhotoLoadedEvent.fire( photo );            
        }
        that.fitPhoto( photo );
    }
    
    
    //+++ Computing the maximal available size on screen
    this.computeAvailableSpace = function( )
    {
        var ie6BugCorrection = 6;
        var wastedPixelTop = 33;
        var wastedPixelBottom = 33;
        var widthWasted = 0;

        //Computing width used by other divs
        //NOTE : only sibling divs are considerd
        //This allows to correct an IE6 bug.
        for(var i = 0; i < that.html.photo.$wrapper.siblings().length; i++){
            widthWasted += that.html.photo.$wrapper.siblings().eq(i).outerWidth( );
    }
        //non available space
        var frameBorderSize = parseInt( that.html.photo.$frame.css("padding-top").replace("px","") );
        //NB: On ne peut pas utiliser g_$divDisplayZoneName.innerWidth( ), car bug sous IE6 :(
        //Du coup il faut que:	+ g_$divDisplayZoneName.siblings()::margin = 0px
        //+ g_$divDisplayZoneName::border=0 et ::margin=0
        var height = that.html.photo.$wrapper.innerHeight() - 2*frameBorderSize - 2*that.properties.photos.technical.decoration.padding - wastedPixelTop;
        var width = $(window).width( ) - widthWasted - 2*frameBorderSize - 2*that.properties.photos.technical.decoration.padding - ie6BugCorrection;

        if( height > that.properties.photos.technical.maxSize.height ) {
            height = that.properties.photos.technical.maxSize.height;
        }
        if( width > that.properties.photos.technical.maxSize.width ) {
            width = that.properties.photos.technical.maxSize.width;
        }

        return { h: height, w: width };
    }

    this.fitPhoto = function( photo )
    {
        var ratio = photo.nativeSize.h / photo.nativeSize.w;
        var newPhotoSz = {};
        
        //Finding the biggest rectangle fitting in the available space
        if( ratio > 1 ) { //vertical photo
            newPhotoSz.h = that.availableSpace.h;
            newPhotoSz.w = Math.round( newPhotoSz.h / ratio );
            if( newPhotoSz.w  > that.availableSpace.w ) {
                newPhotoSz.w = that.availableSpace.w;
                newPhotoSz.h = Math.round( newPhotoSz.w * ratio );
            }
        }
        else {//horizontal photo
            newPhotoSz.w = that.availableSpace.w;
            newPhotoSz.h = Math.round( newPhotoSz.w * ratio );
            if( newPhotoSz.h  > that.availableSpace.h ) {
                newPhotoSz.h = that.availableSpace.h;
                newPhotoSz.w = Math.round( newPhotoSz.h / ratio );
            }        
        }
        
        //resizing (capped)
        photo.resize( newPhotoSz );
    
        that.html.photo.$div.width( photo.size.w + 2*that.properties.photos.technical.decoration.padding )
                            .height( photo.size.h + 2*that.properties.photos.technical.decoration.padding );
        that.html.photo.$frame.width( that.html.photo.$div.outerWidth() )
                                        .height( that.html.photo.$div.outerHeight() + that.html.photo.$title.outerHeight() +that.html.photo.$caption.outerHeight() )
                                        .css("position","relative")
                                        .verticalCenter( 0 );   

        photo.verticalCenter( 0 );       
    }

}
