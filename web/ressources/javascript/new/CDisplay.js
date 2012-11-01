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

function CDisplay( p_properties, p_htmlStructure )
{
    var that = this;
    this.htmlStructure = p_htmlStructure;
    //this.userHandler = p_userHandler;
   
    this.properties = p_properties;
    this.carrousel = new CCarrousel( p_properties );
    this.idCurrentPhoto = -1;
    this.previousEvent = new CEvent();
    this.nextEvent = new CEvent();

    // ++++ Methods ++++ //

    this.displayPhoto = function( id ) {
        that.idCurrentPhoto = id;
        that.htmlStructure.photo.$screen.fadeIn(); //photoScreen is located above the index screen: no need to hide it
        that.htmlStructure.photo.buttons.$previous.verticalCenter(0);
        that.htmlStructure.photo.buttons.$next.verticalCenter(0);
        that.carrousel.load( id ); //must be placed *after* the fadein or the dimensions will be incorrectly computed
    }


    this.hidePhoto = function( ) {
        that.htmlStructure.photo.$screen.fadeOut('fast');
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
    
    this.getPhotoLoadedEvent = function()
    {
        return that.carrousel.getPhotoLoadedEvent();
    }

    /*this.onPhotoLoaded = function()
    {
        that.enableButtons();
    }
    this.carrousel.getPhotoLoadedEvent().subscribe( this.onPhotoLoaded );*/

    this.onPrevious = function()
    {
        if( that.idCurrentPhoto > 1 ) { 
            --that.idCurrentPhoto;
            //that.userHandler.disablePreviousNext();
            that.carrousel.load( that.idCurrentPhoto );
            //that.previousEvent.fire( that.idCurrentPhoto );
        }

    }
    
    this.onNext = function()
    {
        if( that.idCurrentPhoto < that.properties.photos.list.length ) { 
            that.idCurrentPhoto++;
           //that.userHandler.disablePreviousNext();
            that.carrousel.load( that.idCurrentPhoto );
            //that.nextEvent.fire( that.idCurrentPhoto );
        }
    }

    // +++ INTERNALS
    /*this.disableButtons = function( )
    {
        that.htmlStructure.photo.buttons.$previous.removeClass();
        that.htmlStructure.photo.buttons.$next.removeClass();
        that.htmlStructure.photo.buttons.$previous.addClass( that.buttonDisabledClass )
                                                                     .unbind( "click" );
        that.htmlStructure.photo.buttons.$next.addClass( that.buttonDisabledClass )
                                                                     .unbind( "click" );
    }

    this.enableButtons = function()
    {
        that.htmlStructure.photo.buttons.$previous.removeClass();
        that.htmlStructure.photo.buttons.$next.removeClass();
        that.htmlStructure.photo.buttons.$previous.addClass( that.buttonEnabledClass )
                                                                     .one( "click", that.onPrevious );
            that.htmlStructure.photo.buttons.$next.addClass( that.buttonEnabledClass )
                                                                     .one( "click", that.onNext );
    }*/
}
