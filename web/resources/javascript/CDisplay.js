﻿/*
 *  EZWebGallery:
 *  Copyright (C) 2013-2014 Christophe Meneboeuf <xtof@ezwebgallery.org>
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

function CDisplay( p_properties, p_htmlStructure, user_interactions)
{
    var that = this;
    this.html = p_htmlStructure;
    this.html.photo.spinnerSrc = "resources/images/ajax-loader.gif";

    this.properties = p_properties;
    this.carrousel = new CCarrousel( p_properties, this.html.photo );
    this.navBars = new CNavBars( p_htmlStructure );
    this.url = new CUrl( p_properties.photos.list );
    this.idCurrentPhoto = -1;
    this.photoScreenEvent = new CEvent();
    this.indexScreenEvent = new CEvent();
    this.disableUISignal = new CEvent();
    this.enableUISignal = new CEvent();
    this.displayedPhotoLoadedEvent = new CEvent();
    this.availableSpace = {h: 0, w:0};

    $(".display_optional").hide(); //Hiding all optional items

    this.deck = new CDeck();
    this.toolbar = Toolbar( p_htmlStructure, user_interactions);

    that.deck.moveOnTop( that.toolbar.screenIndex );
    that.toolbar.screenPhoto.eventOnHiding.subscribe( function() { that.hidePhoto(); } );

    $(window).resize( function() {
        //TOOLS.trace( "CDisplay::resize - " +  "Evt resize");
        that.setSpace( that.computeAvailableSpace() );
        if( that.html.photo.$screen.is(":visible") === true )
        {
            that.fitPhoto( that.carrousel.getCurrentPhoto() );
            that.carrousel.centerViewport(); //We need to reposition the viewport to adapt to the new slides' size
        }
    } );


    // ++++ Methods ++++ //

    this.displayCurrentUrl = function()
    {
        TOOLS.trace( "CDisplay::displayCurrentUrl");
        that.url.display( that.displayPhoto );
    }

    this.displayPhoto = function( id )
    {
        TOOLS.trace( "CDisplay::displayPhoto - " + "id: " + id);
        that.idCurrentPhoto = id;
        that.toolbar.screenPhoto.display();
        that.html.photo.buttons.$next.verticalCenter( that.computeToolbarTopHeigth()/2 );
        that.html.photo.buttons.$previous.css("top",  that.html.photo.buttons.$next.css("top") ); //no v center on previous to correct an ie8 bug

        that.setSpace( that.computeAvailableSpace() );
        that.load( id, that.carrousel.load );

        that.photoScreenEvent.fire();
    }

    //back to index
    this.hidePhoto = function( )
    {
        TOOLS.trace( "CDisplay::hidePhoto");
        //that.url.clearHash();
        var $thumbBox =  that.html.index.mosaic.$thumbBoxes.eq( that.idCurrentPhoto - 1 );
        that.indexScreenEvent.fire( $thumbBox );
    }


    //+++ EVENTS

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
        //that.url.setHash( --that.idCurrentPhoto );
        that.idCurrentPhoto--;
        that.load( that.idCurrentPhoto, that.carrousel.previous );
    }

    this.onNext = function()
    {
        //that.url.setHash( ++that.idCurrentPhoto );
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
        TOOLS.trace( "CDisplay::setSpace - " + "space: " + space);
        var mySpace = {};
        mySpace.h = space.h - that.computeToolbarTopHeigth();
        mySpace.w = space.w;
        that.availableSpace = mySpace;
        that.carrousel.setSpace( mySpace );
    }

    this.load = function( id, loadFct ) //loadFct( id ) : function used to fetch the photo
    {
        TOOLS.trace( "CDisplay::load - " + "id: " + id);
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
        var ie6BugCorrection = 0/*6*/;
        var wastedPixelTop = 0/* 33*/;
        var wastedPixelBottom = 0/*33*/;
        var widthWasted = 0;

        //Computing width used by other divs
        //NOTE : only sibling divs are considerd
        //This allows to correct an IE6 bug.
        //for(var i = 0; i < that.html.photo.$wrapper.siblings().length; i++){
            //      widthWasted += that.html.photo.$wrapper.siblings().eq(i).outerWidth( );
        //}

        //non available space
        var frameBorderSize = parseInt( that.html.photo.$frame.css("padding-top").replace("px","") );
        var heightCaption = 2*$(".photoCaption").css("padding-bottom").replace("px","") + $(".photoCaption").innerHeight();
        var height = that.html.photo.$wrapper.innerHeight() - 2*frameBorderSize - 2*that.properties.photos.technical.decoration.padding - heightCaption - wastedPixelTop;
        var width =  that.html.photo.$wrapper.innerWidth( ) - widthWasted - 2*frameBorderSize - 2*that.properties.photos.technical.decoration.padding - ie6BugCorrection;

        return { h: height, w: width };
    }

    this.fitPhoto = function( photo )
    {
        //TOOLS.trace( "CDisplay::fitPhoto - " + "photo: " + photo);
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

        var toolbarHeight_2 = that.computeToolbarTopHeigth() / 2; //<<-- IT SHOULDN'T BE NECESSARY TO OFFSET THE VERTICAL CENTERING!!!
        //resizing (capped) and vertical centering the photo
        photo.resize( newPhotoSz );
        that.html.photo.$div.width( photo.size.w + 2*that.properties.photos.technical.decoration.padding )
                            .height( photo.size.h + 2*that.properties.photos.technical.decoration.padding );
        that.html.photo.$frame.width( that.html.photo.$div.outerWidth() )
                                        .height( that.html.photo.$div.outerHeight() + that.html.photo.$caption.outerHeight() )
                                        .css("position","relative")
                                        .verticalCenter( toolbarHeight_2 );

        photo.verticalCenter( 0 );  //inside its frame

    }

    //Computes the height of the toolbar on the top.
    //0 if vertical toolbar
    this.computeToolbarTopHeigth = function()
    {
        //Toolbar height
        var toolbarHeight = $(".toolbar").filter(":visible").outerHeight();
        if( toolbarHeight > $(document).height() / 2 ) { //if vertical
            toolbarHeight = 0;
        }
        //TOOLS.trace("CDisplay::computeToolbarTopHeigth - " + "Toolbar up space: " + toolbarHeight +"px");
        return toolbarHeight;
    }

}
