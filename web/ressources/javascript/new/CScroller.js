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
    var self = this;
    
    //binding click event to scrolling
    this.options.$navigation.css("cursor", "pointer")
                            .click( function() {
        var target = $(this).attr("target");
        var $target = self.options.$viewport.find( target );
        self.scrollTo($(this),$target);
    } );
    
    CScroller.prototype.scrollTo = function( $source, $target ) {
        scrolloptions.$viewport.animate({scrollLeft: $target.position().left}, self.options.duration,self.options.easing, function() { self.options.onAfter( $source, $target ); }  );
    };
};
