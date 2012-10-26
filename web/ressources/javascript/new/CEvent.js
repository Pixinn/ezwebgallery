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

function CEvent( )
{
    this.subscribers = new Array();
    
    CEvent.prototype.subscribe = function( subscriber )
    {
        this.subscribers.push( subscriber );
    };
    
    CEvent.prototype.unsubscribe = function( subscriber )
    {
        for(var i in this.subscribers) {
            if( this.subscribers[i] == subscriber) {
                this.subscribers.splice(i,1);
                break;
             }
        }
    }
    
    CEvent.prototype.fire = function( evt )
    {   //call each subscibed functions
        for( var i = 0; i < this.subscribers.length; i++ ) {
             //about call(): http://odetocode.com/blogs/scott/archive/2007/07/05/function-apply-and-function-call-in-javascript.aspx
            this.subscribers[i].call( evt );
        }
    }
    
}
