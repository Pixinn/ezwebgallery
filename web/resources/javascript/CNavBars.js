/*
 *  EZWebGallery:
 *  Copyright (C) 2014 Christophe Meneboeuf <xtof@ezwebgallery.org>
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

function CNavBars( p_htmlStructure )
{
    //init
    var that = this;
    this.html = p_htmlStructure;
    this.html.photo.buttons.$nav = $.merge( $(".photoButtonEnabled"), $(".photoButtonDisabled") );
    
    var timer = setTimeout(function() {
                    that.hide(); },
               250);
               
    $("#photoViewport").mousemove(function() {
        clearTimeout(that.timer);
        that.show();
        that.timer = setTimeout(function() {
            that.hide();
        }, 250); 
    });
    
    that.html.photo.buttons.$nav.mouseenter( function() {
        clearTimeout(that.timer);
    } );
               
    //show / hide
    this.show = function( ) 
    {
        that.html.photo.$navBars.fadeIn(10)
                                .css("z-index","50"); /*goes foreground*/
        that.html.photo.buttons.$nav.verticalCenter(0);
    }

    this.hide = function( )
    {
        that.html.photo.$navBars.stop() //stops fadeIn if any
                                .fadeOut(1000, function() {
                                    $(this).css("z-index","-10"); } /*back to back*/
                                );
    }
    
}
