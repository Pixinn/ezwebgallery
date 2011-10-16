/* 
 *  EZWebGallery:
 *  Copyright (C) 2011 The EZWebGallery Dev Team <dev@ezwebgallery.org>
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

	function HashTable( )
{
	//public:
	var m_length = 0;
	var m_items = new Array();
	
	for (var i = 0; i < arguments.length; i += 2) {
		if (typeof(arguments[i + 1]) != 'undefined') {
			m_items[arguments[i]] = arguments[i + 1];
			m_length++;
		}
	}
   
	this.removeItem = function(in_key)
	{
		var tmp_value;
		if (typeof(m_items[in_key]) != 'undefined') {
			m_length--;
			var tmp_value = m_items[in_key];
			delete m_items[in_key];
		}
	   
		return tmp_value;
	}
	
	this.addItem = function(in_key, in_value)
	{
		if (typeof(in_value) != 'undefined' /* in_value valide */ &&
		typeof(m_items[in_key]) == 'undefined' /* Item non déjà présent */) {
			m_length++;
			m_items[in_key] = in_value;
			return in_value;
		}
		else {
			return 'undefined';
		}	
	}

	this.getItem = function(in_key) {
		return m_items[in_key];
	}

	this.hasItem = function(in_key)
	{
		return typeof(m_items[in_key]) != 'undefined';
	}
	
	this.getLength = function( )
	{
		return m_length;
	}
}
/* Fin Classe HashTable */