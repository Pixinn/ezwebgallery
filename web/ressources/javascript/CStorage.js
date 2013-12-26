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

function CStorage( nbItems )
{
    var that = this;
    
    this.nbItems = nbItems;
    this.storage = new CHashTable(); // url : { image, size, isLoaded }
    this.ages = new Array(); // allows to sort the stored photo by age. Newest first (index = 0 )
    
    for( var i = 0; i < this.nbItems; i++) {
	 		this.ages[i] = -1;
	 }
    
    
    // addPhoto
	// --------
	// Instanciate an image, add it to the storage and returns an handle
	// In  : photoToLoad : { url, size:{w,h} }
	// Out : handle on the photo
	this.addPhoto = function ( photoToLoad )
	{
        if( that.storage.hasItem( photoToLoad.url ) == false ) //won't load if photo is already in store
        {
            that.removeOldest();
            that.storage.addItem(  photoToLoad.url, new CPhoto( photoToLoad.id, photoToLoad.size ) )
            that.updateAges( photoToLoad.url );            
        }
        TOOLS.trace("Cache free space: " + (that.nbItems - that.storage.getLength()) );
        return that.storage.getItem( photoToLoad.url );
    }
    
    
     // removeOldest
	 //--------
	 //If the storage is full, remove the oldest photo	 
    this.removeOldest = function()
    {
        if( that.storage.getLength() >= that.nbItems ) {
            var toBeRemoved = that.ages[ that.nbItems - 1 ];
            that.storage.removeItem( toBeRemoved ); //no need to update ages array
            TOOLS.trace("removing " + toBeRemoved );
        }
    }
    
    
     // updateAges
	 //--------
	 //Add a newest photo and update all ages accordingly
     //In: newest url
     this.updateAges = function( url )
     {
        //Aging the existing urls
        for( i = that.nbItems - 1; i > 0; i--  ) {
            that.ages[ i ] = that.ages[ i - 1 ];				
        }
        //Adding the newest url
        that.ages[ 0 ] = url;
     }
     
     this.photoLoaded = function( url )
     {        
        that.storage.getItem( url ).isLoaded = true;
     }
}