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


/* indexEvents( )
 * ----------------------
 *  Inscription des évênements concernant l'index et ses vignettes
 */
function indexEvents( )
{
	
	/* Evenements onclick */
	
	//Onglets de navigation dans l'index
	g_$buttonsIndexNavigation.click( function ( )
	{
		g_idCurrentIndexPage = $(this).attr("id");
	});
	
	//Vignettes
	$(DIV_THUMBCONTAINER+" img").click( function( )
	{	
		/* Mise a jour de la photo courante */
		var key_ThumbFileURL = $(this).attr("src");
		//var key_ThumbFileURL = thumbURL.substr(URL_THUMBS_PATH.length , thumbURL.length - URL_THUMBS_PATH.length);		
        var cutPosition = key_ThumbFileURL.lastIndexOf("/") + 1;
        var key_ThumbFileURL = key_ThumbFileURL.substr( cutPosition, key_ThumbFileURL.length - cutPosition );		
		g_idCurrentPhoto = g_listeThumbnails[ key_ThumbFileURL ];
			
		//----- Affichage zone photo -----//
	  	g_displayManager.displayScreenPhoto(  );	
		//-------------------------------//
		
		/* Chargement et affichage de la photo */
		g_displayManager.photoToDisplay = g_idCurrentPhoto;
		g_idPhotoToLoadNext = g_idCurrentPhoto;
		for( var i=0; i<=NBPHOTOSAPRECHARGER; i++){
				//Pour une réactivité maximale, on insère les photos en haut de la pile
		 		g_loadQueue.insert( g_idCurrentPhoto + i , i);
		}
		//dbgTrace("VIGNETTE -> "+" index: " + g_idCurrentIndexPage + " img: " + g_idCurrentPhoto);
		loadingManagement( "AUTO" );
		
	}	);
}
