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




function CPhoto( id, targetedSize )
{
    var that = this;
   
    this.nativeSize = targetedSize;
    this.size  = TOOLS.clone( targetedSize );
    this.id = id;
    
    this.f_loaded = false;
    this.spinner = new Image();
    this.spinner.src = "ressources/images/ajax-loader.gif";
    this.image = new Image();
        
    CPhoto.prototype.load = function( url, callback )
    {
        if( this.f_loaded == false ) {
            $(this.image).load( function() {  //Using "that" for "this" is the loaded Image
                                    TOOLS.trace( this.src + " loaded." );
                                    that.f_loaded = true;
                                    callback( that );
                                } )
                               .attr( "src", url );
            
        }
    }
    
    CPhoto.prototype.resize = function( size )
    {   
        var newSize = TOOLS.clone( this.nativeSize );
        if( size.w < this.nativeSize.w && size.h < this.nativeSize.h ) { //NEVER UPSCALE! : capping the resize
            newSize = size;
        }
        if( this.f_loaded == true ) { 
                $(this.image).width( newSize.w )
                                   .height( newSize.h );
        }
        else { //never upsize the spinner and keep its aspect ratio
                 //don't update this.size
            /* TBD */
        }
        this.size  = newSize;
            
    }   
    
    CPhoto.prototype.verticalCenter = function( shift )
    {
        $( this.getImage() ).css("position","relative")
                                   .verticalCenter( shift );
    }
    
    CPhoto.prototype.getImage = function()
    {
        if( this.f_loaded == true ) { return this.image; }
        else                               { return this.spinner; } 
    }
    
    CPhoto.prototype.isLoaded = function()
    {
        return this.f_loaded;
    }
    
}
