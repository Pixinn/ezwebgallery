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

function CCarrousel( p_properties )
{
    var that = this;
    this.properties = p_properties;
    this.html = {
        $photoWrapper : $("#wrapperAffichagePhoto"),
        $photoFrame : $("#cadrePhoto"),
        $photoDiv : $("#divPhoto"),
        $photoTitle : $("#photoTitle"),
        photoClass : "mainPhoto",
        loaderSrc : "ajax-loader.gif"
    };
    this.loader = new CPhotoLoader( p_properties, this.html );

    this.displayPhoto = function( photo )
    {
        //If not the spinner, add the border
        if( photo.$image.attr("src").indexOf( that.html.loaderSrc ) == -1 ) {
            photo.$image.removeClass()
                        .addClass( that.html.photoClass );
        }
        that.html.$photoDiv.empty()
                            .append( photo.$image )
                            .width( photo.size.w + 2*that.properties.photos.technical.decoration.padding )
                            .height( photo.size.h + 2*that.properties.photos.technical.decoration.padding );
        that.html.$photoFrame.width( that.html.$photoDiv.outerWidth() )
                            .height( that.html.$photoDiv.outerHeight() + that.html.$photoTitle.height())
                            .verticalCenter( 0 );
        that.html.$photoDiv.find('img').css("position","relative")
                                       .verticalCenter( 0 );
    }


    this.load = function( id )
    {
        that.displayPhoto( that.loader.load( id ) );
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
    }

    this.getPhotoLoadedEvent = function( )
    {
        return this.loader.getPhotoLoadedEvent();
    }

    this.loader.getPhotoLoadedEvent().subscribe( that.onPhotoLoaded );
}
