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


/* ----------------------------------
 * Constructeur : DisplayManager( divDisplayZone, div, divL, photoBorderS, nbPixelsOccupesEnHaut, nbPixelsOccupesEnBas,  maxWidth, maxHeight  )
 * ----------------------------------
 * In  : (JQuery) divScreenPhoto - div contenant tout "l'écran" photo
 * In  : (Jquery) divDisplayZone	- wrapper de la photo à centrer dans la fenêtre(ie photo + captione ou autre)
 * In  : (JQuery) div  				- le div où sera affichée la photo
 * In  : (JQuery) imgSpin			- image contenant le spinner de loading
 * In  : (JQuery) boutonNext		
 * In  : (JQuery) boutonPrev 
 * In  : (String) classPhoto  - nom de la classe appliquée à la photo affichée
 * In  : photoBorderS					- taille de la bordure de la photot
 * In  : nbPixelsOccupesEnHaut		- espace occupé dans la fenêtre, hors photo
 * In  : nbPixelsOccupesEnBas		- espace occupé dans la fenêtre, hors photo
 * In  : maxWidth - Width Max de la photo
 * In  : maxHeight - Height max de la photo
 */
function DisplayManager( divScreenPhoto,
												 div,
												 boutonNext,
												 boutonPrev,
												 boutonIndex,
												 classPhoto,
												 classCaption,
												 classSpinner,
												 photoBorderS,
												 nbPixelsOccupesEnHaut,
												 nbPixelsOccupesEnBas,
												 maxWidth,
												 maxHeight )
{    
	/* Membres */
	//private:
	var m_divScreenPhoto = divScreenPhoto;
	var m_$divCadrePhoto = div;
	var m_classSpinner = classSpinner;
	var m_selectorSpinner = "."+m_classSpinner;
	var m_$spinner = $(m_selectorSpinner);
	var m_$boutonNext = boutonNext;
	var m_$boutonPrevious = boutonPrev;
	var m_$boutonIndex = boutonIndex;
	var m_nbPixelsOccupesEnHaut = nbPixelsOccupesEnHaut;
	var m_nbPixelsOccupesEnBas  = nbPixelsOccupesEnBas;
	var m_photosMaxWidth = maxWidth;
	var m_photosMaxHeight = maxHeight;
	var m_classPhoto = classPhoto;
	var m_classCaption = classCaption;
	var m_selectorPhoto = "."+classPhoto;
	var m_selectorCaption = "."+classCaption;
	var m_photoNowDisplayed = "undefined"; /* Objet photo ajustable */	
	var m_f_screenPhotoIsDisplayed = false; //Est-ce que c'est l'écran photo qui est actuellement affiché ?	
	
	var m_$divPhoto = $("#divPhoto");
			
	//public:		
	this.photoToDisplay = -1;	
	this.fControlsDisabled = true;
	
/* disableControls( ) 
 * -----------------
 * Désactive les controles
 */
this.disableControls = function( )
{
	
	g_buttonNextPhoto.disable();	
	g_buttonPreviousPhoto.disable();
	this.fControlsDisabled = true;
}

/* enableControls( )
 * ----------------
 * Réactive les controles
 */
this.enableControls = function( )
{
	if (g_idCurrentPhoto !== g_nbPhotos) {
		g_buttonNextPhoto.enable();
	}
	if (g_idCurrentPhoto !== 1) {
		g_buttonPreviousPhoto.enable();
	}
	this.fControlsDisabled = false;
}


	
	 /* displayScreenPhoto(  )
	 * ------------------
	 * Afficher l'écran photo (appelé depuis l'écran d'index)
	 */
	 this.displayScreenPhoto = function( )
	{
			m_divScreenPhoto.fadeIn( "slow" );
			this.displayLoading( ); //A faire après l'affichage de divScreenPhoto
			m_f_screenPhotoIsDisplayed = true;
			//Boutons
			m_$boutonNext.verticalCenter( 0 );
			m_$boutonPrevious.verticalCenter( 0 );
	} 


	 /* hideScreenPhoto(  )
	 * ------------------
	 * Cacher l'écran photo (retour à l'écran d'index)
	 */
	 this.hideScreenPhoto = function( )
	 {
	 		m_divScreenPhoto.fadeOut( "fast" );
	 		m_f_screenPhotoIsDisplayed = false;
	 }
	 
	 
	
	/* displayLoading(  )
	 * ------------------
	 * Afficher le spinner lors du chargement d'une photo
	 */ 
	this.displayLoading = function( )
	{
		var oldX = Math.max(m_$divPhoto.width(), 480);
		var oldY = Math.max(m_$divPhoto.height(), 480);
		
		//m_boutonIndexSize = m_$boutonIndex.height();
		
		m_photoNowDisplayed = "undefined";
		
		//----- Effacement Légende et photo  ------//		
		$(m_selectorPhoto).remove();
		$(m_selectorCaption).remove();
		
		//Désactivation des boutons
		this.disableControls();
		
		//--------------------------------------//
		
		// ---------- Affichage Spinner ---------//		
		//Affichage du spinner - l'ordre importe
    m_$divPhoto.append(m_$spinner)
		                .css("margin", "auto")
		                .css("cursor", "wait")
		                .height(oldY)
		                .width(oldX);
		m_$divCadrePhoto.verticalCenter( 0 );		                
    		                
		
		m_$spinner.css("margin", "auto") //Positionnement Horizontal
					    .verticalCenter(0); //Positionnement vertical.
		//--------------------------------------//					 				 				 					
													 
	}
	
	
	/*	-----------------
	 *  displayPhoto( photo , prefetchManager )
	 *  -----------------
	 *  Affiche la photo passée en paramètre
	 *  In  : photo - (JQuery) Image à afficher
	 *  In  : prefectchManager - (classe TablePhoto)
	 *  Out : true si ok, false si nok
	 */
	this.displayPhoto = function(photo, prefetchManager){
		/* Est-ce que la photo passée en paramètre est la photo dont on a demandé l'affichage ?? */
		var fRightPhoto = false;
		var hPhotoToDisplay = prefetchManager.getPhoto(this.photoToDisplay);
		var $photo = $(photo);
				
		
		/* Si les conditions sont réunies, affichage */
		if (	hPhotoToDisplay !== "undefined"
				&&
				photo.attr('src') === hPhotoToDisplay.attr( 'src' )
				&&
				m_divScreenPhoto.is(':visible')  /* N'affiche pas la photo si la zone est cachée. IE si on navigue dans l'index */
			)
		{
			//dbgTrace("displaymanager.js / Displaying ->" + photo.attr('src') );
			this.disableControls( );
			
			//--------- Préparation -----------//
			//On enlève le spinner ET la photo, comme ça on est sûr d'enlever ce qui était affiché
			$(m_selectorSpinner).remove( );					//On enlève le spinner du html
			$(m_selectorPhoto).remove( );					//On enlève la photo précédante
			$(m_selectorCaption).remove( );					//On enlève la légende
									
			$photo	.hide( )	          									//On cache pour ne pas voir la préparation
					.addClass( m_classPhoto )								//Ajout dynamique de la classe -> Peut maintenant être désignée par m_selectorPhoto
					.css(	"cursor", "pointer" )							//Montre qu'on peut cliquer dessus
					.bind( "click", onClickReturnToIndex ); //Cliquer sur l'image fait retourner à l'index
					
			//Inclusion de la photo dans le html			
			m_$divPhoto.append( $photo )
                       .css("max-height", $photo.outerHeight() + "px" )
                       .css("height", $photo.outerHeight() + "px" );
						    /* .css( "cursor", "default" ); 
						     .css("height", "100%")	
						     .css("width", "100%");*/
			//--------------------------------//			 
			
			
			//--------- Affichage -----------//		
			//Réactivation des boutons
			this.enableControls( );
									
			//Affichage photo
			$(photo).fadeIn("fast") //FadeIn photo
							.css("display", "inline"); //Les <IMG> sont des inlines. Permet le centrage horizontal
			//Ajout légende			
			m_$divCadrePhoto.append(  //Ajout de la légende <- A faire APRES l'ajout de la photo !
						                    "<div class='"+m_classCaption+"'>"+g_properties.photos.list[ g_idCurrentPhoto - 1].caption+"</div>"
						                 );

			//Resize & Centrage
			//		-> Important : placer apres le fadeIn pour que la taille des div soit correcte et que ça marche...
			m_photoNowDisplayed = new PhotoAjustable( $(photo), m_photosMaxWidth, m_photosMaxHeight );
			this.adjustPhoto($(photo)); //<- A faire APRES l'ajout de la légende !
			//--------------------------------//			 
			
			return true; 	/* La photo a bien été affichée */
		}
		
		else
		{
			return false; /* La photo n'a pas été affichée */
		}
	}
	

	
	/* --------------------
	 * adjustPhoto(  )
	 * --------------------
	 * Ajuste la photo à la taille de la zone d'affichage et la centre
	 * In : (Image)  photo 				- Photo à ajuster
	 */
	this.adjustPhoto = function( photo )
	{	
		/* Indispensable de mettre auto avant de redimensioner en px sinon bug sous ie6 */
		m_$divCadrePhoto.css("width", "auto");
		m_$divPhoto.css("width", "auto");
						 		
		if (m_photoNowDisplayed != "undefined") { //Pour éviter une erreur si resize avant fin de chargement...					
			//Ajuster taille la taille de la photo
			m_photoNowDisplayed.ajuster( m_nbPixelsOccupesEnHaut, m_nbPixelsOccupesEnBas );
	 		//Ajuster ce qu'il y a autour
	 		var newWidth = photo.outerWidth( ); 	//outerWidth pour prendre en compte les borders
 			m_$divCadrePhoto.css("margin", "auto")
 					     				.verticalCenter( 0 )
										.width( newWidth );
									//	.children().width( newWidth ) <- Pose problème car tous les éléments doivent avoir la même largeur !
            m_$divPhoto.css("height", photo.outerHeight() + "px" );	
			} 

	}
	
	/* ---------------
	 * getMaxSize(  )
	 * ---------------
	 * Calcule la taille max souhaitée pour la photo à afficher
	 * Return : (s_Taille)				- Taille max de la photo
	 */
	this.getMaxSize = function( )
	{		/* Fonction définie dans classObjetAjustable.js */
			var taille = calculerTaillesMax( nbPixelsOccupesEnHaut, nbPixelsOccupesEnBas, m_$divCadrePhoto, m_photosMaxWidth, m_photosMaxHeight );
			return taille;
	}

		/* ---------------
	 * isScreenPhotoDisplayed(  )
	 * ---------------
	 *  retourne vrai si c'est l'écran photo qui est actuellement affiché
	 */
	this.screenPhotoIsDisplayed = function( )
	{
		 return m_f_screenPhotoIsDisplayed;
	}
	
}
	