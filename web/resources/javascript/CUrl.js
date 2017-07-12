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

///////////////////////////////////
// This class manages URLs :
// setHash : changes the URL without reloading
// display : display the photo corresponding to the anchor if it exists
///////////////////////////////////

function CUrl( p_photoList )
{
    var that = this;
    this.photoStr = "Photo";
    this.photoList = p_photoList;
    
    this.setHash = function( id )
    {
        window.location.hash = that.photoStr + id;
    }
    
    this.clearHash = function()
    {
        window.location.hash = "";
    }
    
    this.display = function( fctDisplayPhoto )
    {
        var url = window.location.hash;
        var photoId = parseInt( url.substring(url.indexOf(that.photoStr)+that.photoStr.length) );
        if( isNumber(photoId) && photoId <= that.photoList.length ) { //Valid: display photo
            fctDisplayPhoto( photoId );
        }
        else { //invladid, clear hash
            //that.clearHash();
        }
    }
    
    //+++ PRIVATE$
    //returns true if n is a valid integer
    function isNumber(n) {
        return !isNaN(parseInt(n)) && isFinite(n);
    }

};
  