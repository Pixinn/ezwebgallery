/* 
 *  EZWebGallery:
 *  Copyright (C) 2011-2012 The EZWebGallery Dev Team <dev@ezwebgallery.org>
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
 
    
/* Classe CFifo
 * ------------------
 */
 function CFifo( )
{ 
	var that = this;
    
	//Private:
	this.m_stack = new Array( );
	this.m_length = 0;
		
	// push( pushed )
	//--------------
    //Push an element to the back of the fifo 
	this.push = function( pushed )
	{
        that.m_stack[that.m_length] = pushed;
        that.m_length++;
	}
	
    
    //pushFirst( pushed  )
    //--------------
    // push an element at the first position
	this.pushFirst = function( pushed )
	{
        for (var j = that.m_length; j > 0; j--) {
            that.m_stack[j] = that.m_stack[j-1];
        }
        that.m_stack[0] = pushed;
        that.m_length++;
	}
	
	
	// pop( )
	// --------------
	// Reads the most ancient element and removes it from the stack
	this.pop = function ( )
	{
		var element;
	
		if( that.m_length > 0)
		{
			element = that.m_stack.shift();              
			that.m_length--;
		}
		else{
			element = "undefined";
		}
		
		return element;
	}
    
    	// size( )
	// --------------
	// Returns the size of the stack
    this.size = function()
    {
        return that.m_length;
    }
    	
}
