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
    this.prevPhoto = -1;
    this.currPhoto = -1;
    this.nextPhoto = -1;

    //In: CPhoto
    this.displayPhoto = function( photo )
    {   
        that.html.$div.find('img').remove();
        var image = photo.getImage();
        
        if( photo.isLoaded() == true ) { //If not the spinner, add the border
            $( image ).removeClass()
                        .addClass( that.html.photoClass );
        }
        that.html.$div.append( image  );
        this.currentPhoto = photo;
    }


    this.load = function( id )
    {
        that.setCurrentPhoto( id );   
        var photo = that.loader.load( id, that.loader.NEXT );
        that.displayPhoto( photo );
        return photo;
    }

    this.next = function( newid )
    {
        that.setCurrentPhoto( newid );
        //+TBC
        var photo = that.loader.load( that.currPhoto, that.loader.NEXT );
        that.displayPhoto( photo );
        return photo;
        //-TBC
    }

    this.previous = function( newid )
    {
        that.setCurrentPhoto( newid );
        //+TBC
        var photo = that.loader.load( that.currPhoto, that.loader.PREVIOUS );
        that.displayPhoto( photo );
        return photo;
        //-TBC
    }
    
    this.getCurrentPhoto = function()
    {
        return that.currentPhoto;
    }
    
    //updates the avalaible space
    this.setSpace = function( space )
    {
        that.loader.space = space;
    }

    //+++ PRIVATE
    this.setCurrentPhoto = function( id )
    {
        that.currPhoto = id;        
        that.prevPhoto = id - 1;
        that.nextPhoto = id + 1;
    }
    
    //+++ EVENTS

    this.onPhotoLoaded = function(  )
    {
        if( this.id == that.currPhoto )  { //this: CPhoto 
            that.displayPhoto( this ); 
            that.photoDisplayedLoaded.fire( this );
        }
    }
    this.loader.getPhotoLoadedEvent().subscribe( that.onPhotoLoaded );

    
    this.getPhotoDisplayedLoadedEvent = function( )
    {
        return this.photoDisplayedLoaded;
    }

}
