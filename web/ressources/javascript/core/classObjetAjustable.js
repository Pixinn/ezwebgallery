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


 /**************************
  * 		CLASSE
  * In : 	(JQuery) photo 	 : photo à ajuster
  * ATTENTION : les 4 paddings du div contenant la photo doivent être =  
  *
  *************************/
function PhotoAjustable( photo, maxWidth, maxHeight )
{
	//- PROPRIETES
	//private :
	var m_$photo = photo;
	var m_$photoDiv = photo.parent( ).parent( ); /* C'est vraiment de la merde !!! */
	var m_photoNativeHeight;
	var m_photoNativeWidth;
	var m_photoRatio;
	var m_photoMaxWidth;
	var m_photoMaxHeight;
	var m_divphotoHeight;
	var m_divphotoWidth;
	
	//-- Init
	/* Récupération des tailles "Natives" */
 	m_photoNativeHeight = m_$photo.height();
	m_photoNativeWidth = m_$photo.width();
	m_photoMaxWidth = maxWidth;
	m_photoMaxHeight = maxHeight;
	m_photoRatio = m_photoNativeWidth / m_photoNativeHeight;
	
    //-- Méthodes
	this.ojectResize = function( )
	{
			var newHeight = m_photoNativeHeight;
			var fResized = 0;
			/* Resize - en 2 passes
		 	* L'ordre est important !
		 	*/
				
			if( m_photoNativeWidth > m_divphotoWidth )
			{
				newHeight = m_divphotoWidth /m_photoRatio;
				m_$photo.width( m_divphotoWidth );
				m_$photo.height( newHeight );
				fResized = 1;
			}
			if( newHeight > m_divphotoHeight )
			{
				m_$photo.height( m_divphotoHeight );
				m_$photo.width(  m_divphotoHeight*m_photoRatio );

				fResized = 1;
			}
			if( fResized == 0)
			{
				m_$photo.height( m_photoNativeHeight );
				m_$photo.width(  m_photoNativeWidth );
			}
		
}

	/**
 	* @param {Object} nbPixelsOccupesEnHaut - Nombre de pixels occupes par autre chose à l'écran en haut
 	* @param {Object} nbPixelsOccupesEnBas	- Nombre de pixels occupes par autre chose à l'écran en bas
 	*/
	this.ajuster = function( nbPixelsOccupesEnHaut , nbPixelsOccupesEnBas )
	{	
			var photoHW;

			/* Calculer tailles max du div */
			photoHW = calculerTaillesMax( nbPixelsOccupesEnHaut,nbPixelsOccupesEnBas,m_$photoDiv, m_photoMaxWidth,m_photoMaxHeight );
			m_divphotoHeight = photoHW.h;
			m_divphotoWidth  = photoHW.w;
			
			/* - Resize - */
 			this.ojectResize( );
	
	}



} 
