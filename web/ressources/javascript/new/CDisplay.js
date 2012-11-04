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
    this.html = p_htmlStructure;
    this.html.photo.spinnerSrc = "ressources/images/ajax-loader.gif";

    this.properties = p_properties;
    this.carrousel = new CCarrousel( p_properties, this.html.photo );
    this.idCurrentPhoto = -1;
    this.previousEvent = new CEvent();
    this.nextEvent = new CEvent();

    this.availableSpace = {h: 0, w:0};
    this.photoNativeSz = {h:0, w:0};
    this.photoCurrentSz = {h:0, w:0};

    $(window).resize( function() {
        that.availableSpace = that.computeAvailableSpace();
        if( that.html.photo.$screen.is(":visible") === true ) {
            that.fitPhoto( );
            that.html.photo.buttons.$previous.verticalCenter(0);
            that.html.photo.buttons.$next.verticalCenter(0);
        }
    } );


    // ++++ Methods ++++ //

    this.displayPhoto = function( id )
    {
        that.idCurrentPhoto = id;
        that.html.photo.$screen.fadeIn(); //photoScreen is located above the index screen: no need to hide it
        that.html.photo.buttons.$next.verticalCenter(0);
        that.html.photo.buttons.$previous.css("top",  that.html.photo.buttons.$next.css("top") ); //no v center on previous to correct an ie8 bug

        that.availableSpace = that.computeAvailableSpace();
        that.photoNativeSz = that.carrousel.load( id, that.availableSpace ); //must be placed *after* the fadein or the dimensions will be incorrectly computed
        that.photoCurrentSz = {h:0, w:0};
        that.fitPhoto( );
    }


    this.hidePhoto = function( ) {
        that.html.photo.$screen.fadeOut('fast');
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

    this.getPhotoDisplayedLoadedEvent = function()
    {
        return that.carrousel.getPhotoDisplayedLoadedEvent();
    }


    this.onPrevious = function()
    {
        that.idCurrentPhoto--;
        that.photoNativeSz = that.carrousel.load( that.idCurrentPhoto, that.availableSpace );
        that.photoCurrentSz = {h:0, w:0};
        that.fitPhoto( );
    }

    this.onNext = function()
    {
        that.idCurrentPhoto++;
        that.photoNativeSz = that.carrousel.load( that.idCurrentPhoto, that.availableSpace );
        that.photoCurrentSz = {h:0, w:0};
        that.fitPhoto( );
    }

    this.onPhotoDisplayedLoaded = function()
    {
        that.photoNativeSz = {w: this.size.w, h: this.size.h};
        that.photoCurrentSz = {w:0, h:0};
        that.fitPhoto();
    }
    that.carrousel.getPhotoDisplayedLoadedEvent().subscribe( this.onPhotoDisplayedLoaded );

    //+++ Private
    //+++ Computing the maximal available size on screen

    this.computeAvailableSpace = function( )
    {
        var ie6BugCorrection = 6;
        var wastedPixelTop = 33;
        var wastedPixelBottom = 33;
        var widthWasted = 0;

        //Computing width used by other divs
        //NOTE : only sibling divs are considerd
        //This allows to correct an IE6 bug.
        for(var i = 0; i < that.html.photo.$wrapper.siblings().length; i++){
            widthWasted += that.html.photo.$wrapper.siblings().eq(i).outerWidth( );
    }
        //non available space
        var frameBorderSize = parseInt( that.html.photo.$frame.css("padding-top").replace("px","") );
        //NB: On ne peut pas utiliser g_$divDisplayZoneName.innerWidth( ), car bug sous IE6 :(
        //Du coup il faut que:	+ g_$divDisplayZoneName.siblings()::margin = 0px
        //+ g_$divDisplayZoneName::border=0 et ::margin=0
        var height = that.html.photo.$wrapper.innerHeight() - 2*frameBorderSize - 2*that.properties.photos.technical.decoration.padding - wastedPixelTop;
        var width = $(window).width( ) - widthWasted - 2*frameBorderSize - 2*that.properties.photos.technical.decoration.padding - ie6BugCorrection;

        if( height > that.properties.photos.technical.maxSize.height ) {
            height = that.properties.photos.technical.maxSize.height;
        }
        if( width > that.properties.photos.technical.maxSize.width ) {
            width = that.properties.photos.technical.maxSize.width;
        }

        return { h: height, w: width };
    }

    this.fitPhoto = function( )
    {
        //if photo is too big, compute a new size to fit. In 2 steps.
        var ratio = that.photoNativeSz.h / that.photoNativeSz.w;
        var photoSz = {h: that.photoNativeSz.h, w: that.photoNativeSz.w}; //copying: otherwise, photoSz would be a reference!
        if( that.photoNativeSz.h > that.availableSpace.h ) {
            photoSz.h = that.availableSpace.h;
            photoSz.w = photoSz.h / ratio;
        }
        if( photoSz.w > that.availableSpace.w ) {
            photoSz.w = that.availableSpace.w;
            photoSz.h = photoSz.w * ratio;
        }
        //adapting the photo and its containers
        if( photoSz.h != that.photoCurrentSz.h || photoSz.w != that.photoCurrentSz.w )
        {
            $img = that.html.photo.$div.find('img')
            //resize the picture itself only if it's not the spinner
            if( $img.attr("src").indexOf( that.html.photo.spinnerSrc ) == -1 ) {
                $img.width( photoSz.w )
                    .height( photoSz.h );
            }
            that.photoCurrentSz = {h:photoSz.h, w:photoSz.w};
            that.html.photo.$div.width( photoSz.w + 2*that.properties.photos.technical.decoration.padding )
                                .height( photoSz.h + 2*that.properties.photos.technical.decoration.padding );
            that.html.photo.$frame.width( that.html.photo.$div.outerWidth() )
                                  .height( that.html.photo.$div.outerHeight() + that.html.photo.$title.height());
             $img.css("position","relative")
                 .verticalCenter( 0 );
        }
        //always center the frame
        that.html.photo.$frame.verticalCenter( 0 );
    }

}
