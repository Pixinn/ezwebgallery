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

function CDisplay( p_htmlStructure )
{
    var that = this;
    this.htmlStructure = p_htmlStructure;
    this.carrousel = new CCarrousel();
    
    // ++++ Methods ++++ //
    
    this.displayPhoto = function( id ) {        
        this.carrousel.displayPhoto( id );
        that.htmlStructure.photo.$screen.fadeIn(); //photoScreen is located above the index screen: no need to hide it
        that.htmlStructure.photo.buttons.$previous.verticalCenter(0);
        that.htmlStructure.photo.buttons.$next.verticalCenter(0);
    }
    
        
    this.hidePhoto = function( ) {
        that.htmlStructure.photo.$screen.fadeOut('fast');
    }
}
