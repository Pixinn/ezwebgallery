﻿/*
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


function CUserInterractions( p_properties, htmlstructure )
{
    var that = this;
    this.html = htmlstructure;
    this.properties = p_properties;

    this.windowResizedEvent = new CEvent();
    this.thumbnailClickedEvent = new CEvent();
    this.closePhotoEvent = new CEvent();
    this.previousPhotoEvent = new CEvent();
    this.previousPhotoEvent = new CEvent();
    this.nextPhotoEvent = new CEvent();

    this.buttonEnabledClass = "photoButtonEnabled";
    this.buttonDisabledClass = "photoButtonDisabled";
    this.previousPhotoId = -1;
    this.currentPhotoId = 1;
    this.nextPhotoId = 2;

    this.fCurrentlyScrolling = false;
    this.fEnablePreviousNextRequired = false;

    var KEY_ARROW_LEFT = 37;
    var KEY_ARROW_RIGHT = 39;
    var KEY_ESC = 27;
    this.watchedKeyDown = false;

    this.start = function()
    {
        $(window).resize( function() { that.onWindowResized(); } );

        that.enableThumbnailClick();
        that.disablePreviousNext();

        //intercept rightclick
        if( !that.properties.photos.technical.rightClickEnabled )		{
			that.html.photo.$screen.bind( "contextmenu", function( e ) { return false; });
		}
    };

    this.disablePreviousNext = function()
    {
         that.disablePrevious();
         that.disableNext();
    }


    this.enablePreviousNext = function()
    {
        if(  !that.fCurrentlyScrolling )  //Enabling buttons while scrolling is error prone
        {
            TOOLS.trace("CUserInterractions::enablePreviousNext - " + "not scrolling -> immediate");
            that.enablePrevious();
            that.enableNext();
            that.fEnablePreviousNextRequired = false;
            //These options may have to be call after every slides
            //This is the case as prev/next are disabled during scrolling
            var hammertime = that.html.photo.$current.data("hammer");
            hammertime.get('swipe').set({ direction: Hammer.DIRECTION_HORIZONTAL });
            hammertime.get('doubletap').set({ enable: false });
            hammertime.get('pan').set({ enable: false });
            hammertime.get('press').set({ enable: false });
        }
        else {
            TOOLS.trace("CUserInterractions::enablePreviousNext - " + "scrolling -> defered");
            that.fEnablePreviousNextRequired = true;
        }
    }

    this.enablePrevious = function()
    {
        if( that.previousPhotoId >= 1 )
        {
            TOOLS.trace("CUserInterractions::enablePrevious - " + "enable previous " + that.previousPhotoId);
            //Button
            that.html.photo.buttons.$previous.removeClass()
                                              .addClass( that.buttonEnabledClass )
                                              .one( "click", that.onPreviousPhoto );
            //Touch
            that.html.photo.$current.hammer().bind( "swiperight", function(event) {  that.onPreviousPhoto();/*preventDefault();*/  } );
        }
    }

    this.disablePrevious = function()
    {
        TOOLS.trace("CUserInterractions::disablePrevious - " + "disable previous");
        that.html.photo.buttons.$previous.removeClass()
                                         .addClass( that.buttonDisabledClass )
                                         .unbind( "click" );
        that.html.photo.$current.hammer().unbind( "swiperight" );
    }

    this.enableNext = function()
    {
        if( that.nextPhotoId <= that.properties.photos.list.length )
        {
            TOOLS.trace("CUserInterractions::enableNext - " + "enable next " + that.nextPhotoId);
            //Button
            that.html.photo.buttons.$next.removeClass()
                                          .addClass( that.buttonEnabledClass )
                                          .one( "click", that.onNextPhoto );
            //Touch
            that.html.photo.$current.hammer().bind( "swipeleft", function(event) {  that.onNextPhoto();/*preventDefault();*/  } );
        }
    }

      this.disableNext = function()
      {
            TOOLS.trace("CUserInterractions::disableNext - " + "disable next");
            that.html.photo.buttons.$next.removeClass()
                                         .addClass( that.buttonDisabledClass )
                                         .unbind( "click" );
            that.html.photo.$current.hammer().unbind( "swipeleft" );
       }

    this.enableThumbnailClick = function()
    {
        that.html.index.mosaic.$thumbnails.click( function() {
            TOOLS.trace( "CUserInterractions::enableThumbnailClick - " + "thumbnail #" + this.id + " clicked." );
            that.thumbnailClickedEvent.fire( this );
        });
    }

    this.getWindowResizedEvent = function() {
        return that.windowResizedEvent;
    }

    this.getThumbnailClickedEvent = function() {
        return that.thumbnailClickedEvent;
    }

    /*this.getClosePhotoEvent = function() {
        return that.closePhotoEvent;
    }*/

    this.getPreviousPhotoEvent = function() {
        return that.previousPhotoEvent;
    }

    this.getNextPhotoEvent = function() {
        return that.nextPhotoEvent;
    }

    this.onWindowResized = function() {
        that.windowResizedEvent.fire();
    }


    this.onPhotoDisplayedLoaded = function( photoId )
    {
        that.previousPhotoId = photoId - 1;
        that.nextPhotoId = photoId + 1;
        that.currentPhotoId  = photoId;
        window.setTimeout( function() { // better to wait a bit
            that.enablePreviousNext();
        }
        , 20 );
    }

    this.onPreviousPhoto = function() {
        that.disablePrevious();
        that.previousPhotoEvent.fire( {id: that.previousPhotoId} );
    }

    this.onNextPhoto = function() {
        that.disableNext();
        that.nextPhotoEvent.fire( {id: that.nextPhotoId} );
    }

    this.onScrolling = function() {
        that.fCurrentlyScrolling = true;
    }

    this.onScrolled = function() {
        TOOLS.trace("CUserInterractions::onScrolled - " + that.fEnablePreviousNextRequired);
        that.fCurrentlyScrolling = false;
        if( that.fEnablePreviousNextRequired ) {
            that.fEnablePreviousNextRequired = false;
            that.enablePreviousNext();
            // window.setTimeout( function() { // better to wait a bit
            //     that.enablePreviousNext();
            // }
            // , 20 );
        }
    }

    this.onIndexScreen  = function()
    {
        $(window).unbind("keydown");
        that.html.photo.$screen.css("z-index", 1);
        that.html.index.$screen.css("z-index", 10 );
        that.enableThumbnailClick();
    }

    this.onPhotoScreen  = function()
    {
        $(window).unbind("keydown")
                 .keydown( function( evt) { that.onKeyboardPhotoScr(evt); } );
        that.html.index.mosaic.$thumbnails.unbind("click");
        that.html.index.$screen.css("z-index", 1);
        that.html.photo.$screen.css("z-index", 10 );
    }

    this.onKeyboardPhotoScr = function( evt )
    {
        if( that.watchedKeyDown == false )
        {
            var keyPressed = evt.keyCode || evt.which;
            switch( keyPressed )
            {
                //Passer d'une photo à la suivante / précédante lors de l'appuie sur une flèche
                case KEY_ARROW_LEFT:
                    that.watchedKeyDown = true;
                    that.html.photo.buttons.$previous.click(); //using click event to be subject to enable / disable
                    that.watchedKeyDown = false;
                    break;
                case KEY_ARROW_RIGHT:
                    that.watchedKeyDown = true;
                    that.html.photo.buttons.$next.click(); //using click event to be subject to enable / disable
                    that.watchedKeyDown = false;
                    break;
                case KEY_ESC:
                    that.watchedKeyDown = true;
                    that.html.toolbar.$buttonIndex.click();  //using click event to be subject to enable / disable
                    that.watchedKeyDown = false;
                    break;
            }
        }
    }

}
