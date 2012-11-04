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

function CCarrousel( p_properties, p_html )
{
    var that = this;
    this.properties = p_properties;
    this.html = p_html;
    this.html.photoClass = "mainPhoto";
    this.photoDisplayedLoaded = new CEvent();
    
    this.loader = new CPhotoLoader( p_properties, this.html );

    this.displayPhoto = function( photo )
    {
        //If not the spinner, add the border
        if( photo.$image.attr("src").indexOf( that.html.spinnerSrc ) == -1 ) {
            photo.$image.removeClass()
                        .addClass( that.html.photoClass );
        }
        that.html.$div.empty()
                            .append( photo.$image );
    }


    this.load = function( id, space )
    {
        var photo = that.loader.load( id, space );
        that.displayPhoto( photo );
        return photo.size;
    }

    this.next = function()
    {

    }

    this.previous = function()
    {

    }

    //+++ EVENTS

    this.onPhotoLoaded = function( )
    {
       that.displayPhoto( this );
       that.photoDisplayedLoaded.fire( this );
    }

    
    this.getPhotoDisplayedLoadedEvent = function( )
    {
        return this.photoDisplayedLoaded;
    }

    this.loader.getPhotoLoadedEvent().subscribe( that.onPhotoLoaded );
}
