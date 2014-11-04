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

function CMap()
{
    var that = this;
    this.isDisplayed = false;
    
    this.display = function()
    {
        if( that.isDisplayed === false )
        {
            var mapOptions = {
                    center: { lat: 9.6257333, lng: -84.010995 }, //San Jose
                    zoom: 8
                };
            var map = new google.maps.Map( document.getElementById('wrapper_map_costarica'),
                                           mapOptions );
                
            var kmlLayer = new google.maps.KmlLayer( { url: "http://voyages.pixinn.net/costarica/maps/costarica.kml" } );
            kmlLayer.setMap( map );
            
            that.isDisplayed = true;
        }
    
    }
}

