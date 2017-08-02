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

	function CHashTable( )
{
	var that = this;

	this.len = 0;
	this.items = new Array();

	for (var i = 0; i < arguments.length; i += 2) {
		if (typeof(arguments[i + 1]) != 'undefined') {
			that.items[arguments[i]] = arguments[i + 1];
			that.len++;
		}
	}

	this.removeItem = function(in_key)
	{
		if (typeof(that.items[in_key]) != 'undefined') {
			that.len--;
			that.items.splice(in_key, 1); //removes the element from the table, does not destroy it! A job for the GC
		}
	}

	this.addItem = function(in_key, in_value)
	{
		if (  typeof(in_value) != 'undefined'  && typeof(that.items[in_key]) == 'undefined' /* Item non déjà présent */)  {
			that.len++;
			that.items[in_key] = in_value;
			return in_value;
		}
		else {
			return 'undefined';
		}
	}

	this.getItem = function(in_key) {
		return that.items[in_key];
	}

	this.hasItem = function(in_key)
	{
		return typeof(that.items[in_key]) != 'undefined';
	}

	this.getLength = function( )
	{
		return that.len;
	}
}
