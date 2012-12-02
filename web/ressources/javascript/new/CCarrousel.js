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

    //In: CPhoto
    this.displayPhoto = function( photo )
    {
        that.html.$div.find('img').remove();
        var image = photo.getImage();
        //If not the spinner, add the border
        if( photo.isLoaded() == true ) {
            $( image ).removeClass()
                        .addClass( that.html.photoClass );
        }
        that.html.$div.append( image  );
        this.currentPhoto = photo;
    }


    this.load = function( id, space )
    {
        var photo = that.loader.load( id, space );
        that.displayPhoto( photo );
        return photo;
    }

    this.next = function()
    {

    }

    this.previous = function()
    {

    }

    this.getCurrentPhoto = function()
    {
        return that.currentPhoto;
    }

    //+++ EVENTS

    this.onPhotoLoaded = function( )
    {
       that.displayPhoto( this ); //this: CPhoto
       that.photoDisplayedLoaded.fire( this );
    }
    this.loader.getPhotoLoadedEvent().subscribe( that.onPhotoLoaded );

    
    this.getPhotoDisplayedLoadedEvent = function( )
    {
        return this.photoDisplayedLoaded;
    }

}
