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
    
    this.start = function()
    {
        $(window).resize( function() { that.onWindowResized(); } );        
    
        that.html.index.mosaic.$thumbnails.click( function() {
            TOOLS.trace( "thumbnail #" + this.id + " clicked." );
            that.thumbnailClickedEvent.fire( this );
        });
        
        that.html.photo.buttons.$close.click( function() { that.onClosePhoto(); } );        
    };
    
    this.getWindowResizedEvent = function() {
        return that.windowResizedEvent;
    }
    
    this.getThumbnailClickedEvent = function() {
        return that.thumbnailClickedEvent;
    }
    
    this.getClosePhotoEvent = function() {
        return that.closePhotoEvent;
    }
    
    this.onWindowResized = function() {
        that.windowResizedEvent.fire();
    }
    
    this.onClosePhoto = function() {
        that.closePhotoEvent.fire();
    }
}
