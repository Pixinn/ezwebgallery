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


function CUserInterractions( htmlstructure )
{
    var that = this;
    this.html = htmlstructure;
    
    this.windowResizedEvent = new CEvent();
    this.thumbnailClickedEvent = new CEvent();
    this.closePhotoEvent = new CEvent();
    this.previousPhotoEvent = new CEvent();
    this.nextPhotoEvent = new CEvent();
    
    this.buttonEnabledClass = "photoButtonEnabled";
    this.buttonDisabledClass = "photoButtonDisabled";
    this.previousPhotoId = -1;
    this.currentPhotoId = 1;
    this.nextPhotoId = 2;
    
    this.start = function()
    {
        $(window).resize( function() { that.onWindowResized(); } );        
    
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
        that.html.photo.buttons.$previous.removeClass()
                                                        .addClass( that.buttonEnabledClass )
                                                        .one( "click", that.onPreviousPhoto );
        that.html.photo.buttons.$next.removeClass()
                                                   .addClass( that.buttonEnabledClass )
                                                   .one( "click", that.onNextPhoto );
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
    
    this.onWindowResized = function() {
        that.windowResizedEvent.fire();
    }
    
    this.onClosePhoto = function() {
        that.closePhotoEvent.fire();
    }
    
    this.onPhotoLoaded = function( photoId )
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
   
    
}
