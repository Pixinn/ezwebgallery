/* 
 *  EZWebGallery:
 *  Copyright (C) 2013 Christophe Meneboeuf <xtof@ezwebgallery.org>
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


//scrolloptions:
//--------------
//$viewport   :   the element that has the overflow and the hidden scrollbar                
//$navigation :   where do we click to scroll ? Items must have a target attribute with a selector describing the target to sroll to.
//duration :      duration of the sliding effect
//easing:         Easing effect
//onAfter:        final callbox (source,target)

CScroller = function( scrolloptions )
{
    this.options = scrolloptions;
    var that = this;
    
    //binding click event to scrolling
    this.options.$navigation.css("cursor", "pointer")
                            .click( function() {
        var target = $(this).attr("target");
                                            var $target = that.options.$viewport.find( target );
                                            that.scrollTo($(this),$target);
    } );
    
    CScroller.prototype.scrollTo = function( $clicked, $target ) {
        that.options.$viewport.animate({scrollLeft: $target.position().left}, that.options.duration,that.options.easing, function() { that.options.onAfter( $clicked, $target ); }  );
    };
    
    CScroller.prototype.scrollToPageNr = function( clickedNr, pageNr )
    {
        var $src = that.options.$navigation.eq( clickedNr - 1 );        
        var target = that.options.$navigation.eq( pageNr - 1 ).attr("target");
        var $target = that.options.$viewport.find( target )
        that.scrollTo( $src, $target );
    }
};
