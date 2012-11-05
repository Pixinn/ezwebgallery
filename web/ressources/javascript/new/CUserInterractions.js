﻿/* 
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


function CUserInterractions( p_properties, htmlstructure )
{
    var that = this;
    this.html = htmlstructure;
    this.properties = p_properties;
    
    this.windowResizedEvent = new CEvent();
    this.thumbnailClickedEvent = new CEvent();
    this.closePhotoEvent = new CEvent();
    this.previousPhotoEvent = new CEvent();
    this.nextPhotoEvent = new CEvent();
    this.previousIndexEvent = new CEvent();
    this.nextIndexEvent = new CEvent();
    
    this.buttonEnabledClass = "photoButtonEnabled";
    this.buttonDisabledClass = "photoButtonDisabled";
    this.previousPhotoId = -1;
    this.currentPhotoId = 1;
    this.nextPhotoId = 2;
    
    var KEY_ARROW_LEFT = 37;
    var KEY_ARROW_RIGHT = 39;
    var KEY_ESC = 27;
    this.watchedKeyDown = false;
    
    this.start = function()
    {
        $(window).resize( function() { that.onWindowResized(); } )
                      .keydown( function(evt) { that.onKeyboardIndexSrc(evt); } );        
    
        that.html.index.mosaic.$thumbnails.click( function() {
            TOOLS.trace( "thumbnail #" + this.id + " clicked." );
            that.disablePreviousNext();
            that.thumbnailClickedEvent.fire( this );
        });
        
        that.html.photo.buttons.$close.click( function() { that.onClosePhoto(); } );        
        
        
    };
    
    this.disablePreviousNext = function()
    {
        that.html.photo.buttons.$previous.removeClass()
                                         .addClass( that.buttonDisabledClass )
                                         .unbind( "click" );
        that.html.photo.buttons.$next.removeClass()
                                     .addClass( that.buttonDisabledClass )
                                     .unbind( "click" );
    }
    
    
    this.enablePreviousNext = function()
    {
        if( that.previousPhotoId >= 1 ) {
            that.html.photo.buttons.$previous.removeClass()
                                              .addClass( that.buttonEnabledClass )
                                              .one( "click", that.onPreviousPhoto );
        }
        if( that.nextPhotoId <= that.properties.photos.list.length ) {
            that.html.photo.buttons.$next.removeClass()
                                          .addClass( that.buttonEnabledClass )
                                          .one( "click", that.onNextPhoto );
        }
    }
    
    this.getWindowResizedEvent = function() {
        return that.windowResizedEvent;
    }
    
    this.getThumbnailClickedEvent = function() {
        return that.thumbnailClickedEvent;
    }
    
    this.getClosePhotoEvent = function() {
        return that.closePhotoEvent;
    }
    
    this.getPreviousPhotoEvent = function() {
        return that.previousPhotoEvent;
    }
    
    this.getNextPhotoEvent = function() {
        return that.nextPhotoEvent;
    }
    
     this.getPreviousIndexEvent = function() {
        return  that.previousIndexEvent;
    }
    
    this.getNextIndexEvent = function() {
        return  that.nextIndexEvent;
    }
    
    this.onWindowResized = function() {
        that.windowResizedEvent.fire();
    }
    
    this.onClosePhoto = function() {
        that.closePhotoEvent.fire();
    }
    
    this.onPhotoDisplayedLoaded = function( photoId )
    {
        that.previousPhotoId = photoId - 1;
        that.nextPhotoId = photoId + 1;
        that.enablePreviousNext();
    }
    
    this.onPreviousPhoto = function() {
        that.disablePreviousNext();
        that.previousPhotoEvent.fire( {id: that.previousPhotoId} );
    }
    
    this.onNextPhoto = function() {
        that.disablePreviousNext();
        that.nextPhotoEvent.fire( {id: that.nextPhotoId} );
    }
    
    this.onIndexScreen  = function()
    {
        $(window).unbind("keydown")
                      .keydown( function( evt) { that.onKeyboardIndexSrc(evt); } );
    }
    
    this.onPhotoScreen  = function()
    {
        $(window).unbind("keydown")
                      .keydown( function( evt) { that.onKeyboardPhotoScr(evt); } );
    }
    
    this.onKeyboardIndexSrc = function( evt )
    {
        if( that.watchedKeyDown == false )
        {
            var keyPressed = evt.keyCode || evt.which;
            switch( keyPressed )
            {
                case KEY_ARROW_LEFT:
                    that.watchedKeyDown = true;
                    that.previousIndexEvent.fire();
                    that.watchedKeyDown = false;
                    //f_watchedKeyCatched = true;
                    break;
                case KEY_ARROW_RIGHT:
                    that.watchedKeyDown = true;
                    that.nextIndexEvent.fire();
                    that.watchedKeyDown = false;
                    //f_watchedKeyCatched = true;
                    break;
                }
        }
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
                    that.onClosePhoto();
                    that.watchedKeyDown = false;
                    break;				
            }
        }
    }
    
}
