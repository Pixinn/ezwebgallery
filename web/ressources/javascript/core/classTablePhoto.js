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

/*-------- EVOLUTIONS ---------*
 * + Ne pas se baser sur le num�ro de la photo mais sur son URL.
 *				=> Prendrait directement en compte le changement de taille (puisque qu'inclu dans l'url)
 ------------------------------*/			


/* Classe TablePhotos
 * ------------------
 * In : LMax, nombre max de photos stock�es.
 */
function TablePhotos( LMax )
{    
	  /* Membres */
	  //Private:
 	 var m_lengthMax = LMax;											// Nombre max de photo en cache
	 var m_tablePhotos = new HashTable( );					//Contient les photos proprement dites : clef : Numero - Photo
	 var m_tableTargetedSizes = new HashTable( );	//Contient la taille d�sir�e des photos
	 var m_tableIsLoaded = new HashTable( );				//Relie les photos � leur flag isLoaded : clef : Numero  - isLoaded. Si la clef est pr�sente dans la table -> la photo est bien charg�e !
	 var m_tableAnciennete = new Array( )					//Table d'anciennete des photos. Index 0 -> lengthMax-1 : du plus r�cent au plus vieux.
	 
	 for( i = 0; i < m_lengthMax ; i++){		//Init de l'historique
	 		m_tableAnciennete[i] = -1;			//0 photo la plus r�cente, lengthMax-1, photo la plus vieille
	 }
	 
	 /* M�thodes */
	
	/* addPhoto
	 * --------
	 * Instencie une image, l'ajoute dans la table et retoune un handle
	 * In  : numero -  numero de la photo a ajouter (clef)
	 * In  : {h,w}targetsize - taille désirée de la photo
	 * In  : idPhotoDisplayed - num�ro de la photo affich�e en ce moment
	 * Out : (Objet JQuery) Handle sur la Photo si succes, "undefined" sinon
	 */
	this.addPhoto = function ( numero  , targetsize , idPhotoDisplayed )
	{
			
		//N'agir que si la photo n'�tait pas d�j� stock�e
		if (m_tablePhotos.hasItem( numero ) == false)
		{	
			//Il ne reste plus de place dans la table -> Supprimer d'abord la photo la plus ancienne
			if (m_tablePhotos.getLength( ) >= m_lengthMax  )
			{
				var numPhotoASupprimer = m_tableAnciennete[m_lengthMax - 1];
				var codeRetour1;
				var codeRetour2;
				
				//dbgTrace( "num "+numPhotoASupprimer );				
				
				//Cas particulier � g�rer : il ne faut surtout pas supprim�e la photo affich�e
				if( numPhotoASupprimer === idPhotoDisplayed){
					numPhotoASupprimer = m_tableAnciennete[m_lengthMax - 2];				 //Changement de photo � supprimer
					m_tableAnciennete[m_lengthMax - 2] = m_tableAnciennete[m_lengthMax - 1]; //Modification de la table d'anciennet�
					m_tableAnciennete[m_lengthMax - 1] = -1;
				}
				var handle = m_tablePhotos.getItem( numPhotoASupprimer );
				
				codeRetour1 = m_tableIsLoaded.removeItem( trimFileName( PHOTOFILEPREFIXE , handle.src ) );
				codeRetour2 = m_tablePhotos.removeItem( numPhotoASupprimer );
				codeRetour3 = m_tableTargetedSizes.removeItem( numPhotoASupprimer );
				
				//Echec
				if( codeRetour1 === 'undefined' || codeRetour1 === 'undefined' || codeRetour3 === 'undefined' )
				{		
					//Annulation du chargement
					/* Si on rentre dans ce code il risque de se passer des choses bizarres... */
					// delete photo; <- ???
					return "undefined";
				}
				
			}
			// Instanciation et Ajout de la nouvelle photo dans la hashtable:
			if( m_tablePhotos.addItem(numero, new Image( ) ) !== 'undefined' ){
								
				//Faire "vieillir" les photos de la table
				var i = m_lengthMax - 2;
				while(i >= 0){
					var j = i + 1;		
				 	m_tableAnciennete[j] = m_tableAnciennete[i];
					i--;				
	 			}
				//Ajout de la nouvelle photo dans l'historique
				m_tableAnciennete[0] = numero;			
				//Sauvegarde de la taille d�sir�e
				m_tableTargetedSizes.addItem(numero, targetsize)
				//Handle sur la photo (Objet Jquery) retourn�
				return $(m_tablePhotos.getItem( numero ));
			}
			else{
					/* Si on rentre dans ce code il risque de se passer des choses bizarres... */
					// delete photo; <- ???
					return "undefined";
			}
		}
		
		//Echec
		return "undefined";	
		
	}
	//Fin fonction addPhoto
	
	/* delPhoto
	 * --------
	 * supprime une photo de la table
	 * In  : numero -  numero de la photo a supprimer (clef)
	 */
	this.delPhoto = function( numero )
	{
		var i = 0;
		var j;
		var handle = m_tablePhotos.getItem( numero );
		
		//D�caler toutes la table d'anciennet� pour boucher le "trou"
		while( m_tableAnciennete[i] !== numero ){ //Retrouver la place de la photo dans l'historique
			i++;
		}
		while( i <= m_lengthMax - 2) //D�calage
		{
			j = i + 1;
			m_tableAnciennete[i] = m_tableAnciennete[j];
			i++;
		}
		m_tableAnciennete[m_lengthMax - 1] = -1; //dernier item
		
		m_tableIsLoaded.removeItem( trimFileName( PHOTOFILEPREFIXE , handle.src ) );
		m_tablePhotos.removeItem( numero );
		m_tableTargetedSizes.removeItem( numero );
	}
		
	
	/* hasPhoto
	 * --------
	 * Renvoie un flag pour d�terminer si la photo est pr�sente dans la table
	 * In : numero de la photo
	 * Out : True or False
	 */
	this.hasPhoto = function ( numero ){
		return m_tablePhotos.hasItem( numero );
	}
	
	/* isPhotoLoaded
	 * -------------
	 * Renvoie un flag pour d�terminer si la photo est charg�e
	 * In : numero de la photo
	 * Out : True or False
	 */
	this.isPhotoLoaded = function ( numero ){
		var handle = m_tablePhotos.getItem( numero );
		if ( handle !== undefined )
		{
			if( m_tableIsLoaded.getItem( trimFileName( PHOTOFILEPREFIXE , handle.src ) ) == true ){
				return true;	
			}
			
		}
		return false;			

	}
	
	/* loadingFinished
	 * ---------------
	 * Est appel� quand le chargement d'une photo est achev�
	 * In : numero de la photo
	 */
	this.loadingFinished = function( handle ){
			m_tableIsLoaded.addItem( trimFileName( PHOTOFILEPREFIXE , handle.src ) , true);
	}
		
	
	/* getPhoto( numero )
	 * --------
	 * Retourne un handle (objet JQuery) sur la photo demand�e
	 * In : numero de la photo
	 * Out : (Jquery) handle sur Photo
	 */
	this.getPhoto = function ( numero ){
		var handle = $(m_tablePhotos.getItem( numero ));
		if ( this.isPhotoLoaded( numero ) ){
			return $(m_tablePhotos.getItem( numero ));
		}
		else{
			return 'undefined';
		}
	}
	  
	/* getTargetedSize
	 * ---------------
	 * Retourne la taille qui �tait d�sir�e lorsqu'on a ajout� la photo � la table
	 * In : num�ro de la photo
	 * Out : {h,w} taille d�sir�e de la photo
	 */  
	  this.getTargetedSize = function( numero ){
	  	 return	m_tableTargetedSizes.getItem( numero );
	  }
}
/* Fin Classe TablePhotos */
		


