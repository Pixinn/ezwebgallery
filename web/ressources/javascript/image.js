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


var g_watchedKeyDown = false; //Faudrait une classe pour gérer les évênement keyboard et pas de var globale...
							//Pis aller en attendant...

/**************************
* ButtonDeactivable
* ------------------
*/
function ButtonDeactivable( handle, onClickFct )
{
	var m_$handle = handle;
	var m_onclick = onClickFct;
	var m_f_enabled = false;	
	
	this.enable = function( ){
				m_$handle
					.css("cursor","pointer")					
					.removeClass("photoButtonDisabled")
					.addClass("photoButtonEnabled")
                    .click( m_onclick );								
				
                m_f_enabled = true;
	}
	
	this.disable = function( ){		
				m_$handle
                        .unbind( "click" )                  //Nécessaire : sinon l'evt "click" peut être triggé plusieurs fois pour un bouton activé!
						.css("cursor","not-allowed")		//Attention curseur non W3C -> n'est pas supporté par Opéra...
						.removeClass("photoButtonEnabled")
						.addClass("photoButtonDisabled");
						
						m_f_enabled = false;
	}
	

}

/****** CALLBACKS DES BOUTONS *******/

		/* Photo suivante */
	function onClickNext( )
	{
		var idOldIndexPage = g_idCurrentIndexPage;
		//Seulement si le bouton est activé
		if (	g_displayManager.fControlsDisabled === false
			&&	g_idCurrentPhoto < g_nbPhotos ) //Si on n'est pas sur la dernière photo
		{
			//Mise à jour des éléments de navigation
			g_idCurrentPhoto++;
			g_idCurrentIndexPage = Math.ceil( g_idCurrentPhoto/ (g_properties.index.mosaic.nbRows * g_properties.index.mosaic.nbCols) );
			g_idPhotoToLoadNext = g_idCurrentPhoto;
			//dbgTrace("Bouton NEXT - " + g_idCurrentPhoto + " oldIndex: " + idOldIndexPage + " index: " +  g_idCurrentIndexPage);
			//On change de page d'index si besoin
			if (g_idCurrentIndexPage !== idOldIndexPage) { //Visiblement cette fonction est éxécutée avant le href sinon ça ne marcherait pas
					g_$buttonsIndexNavigation.eq(g_idCurrentIndexPage - 1).click( );
			}

		  	for(var i=0; i<=g_properties.photos.technical.prefetchSize; i++){
				//Pour une réactivité maximale, on insère les photos en haut de la pile
		 		g_loadQueue.insert( g_idCurrentPhoto + i , i);
			}  		
					
			g_displayManager.photoToDisplay = g_idCurrentPhoto;	
			loadingManagement( "NEXT" );
		}
	}
	
	/* Photo précédante */
	function onClickPrevious(){
		var idOldIndexPage = g_idCurrentIndexPage;
		
		if (   g_displayManager.fControlsDisabled === false
			&& g_idCurrentPhoto > g_properties.photos.technical.first ) //Si on n'est pas sur la première photo
		{
			g_idCurrentPhoto--;
			g_idCurrentIndexPage = Math.ceil( g_idCurrentPhoto/ (g_properties.index.mosaic.nbRows * g_properties.index.mosaic.nbCols) );
			g_idPhotoToLoadNext = g_idCurrentPhoto;
			//dbgTrace("Bouton PREVIOUS - " + g_idCurrentPhoto + " oldIndex: " + idOldIndexPage + " index: " + g_idCurrentIndexPage);
			//On change de page d'index si besoin
			if (g_idCurrentIndexPage !== idOldIndexPage) { //Visiblement cette fonction est éxécutée avant le href sinon ça ne marcherait pas
				g_$buttonsIndexNavigation.eq(g_idCurrentIndexPage - 1).click();
			}
			for (var i = 0; i <= g_properties.photos.technical.prefetchSize; i++) {
				//Pour une réactivité maximale, on insère les photos en haut de la pile
				g_loadQueue.insert(g_idCurrentPhoto - i, i);
			}
			
			g_displayManager.photoToDisplay = g_idCurrentPhoto;
			loadingManagement("PREVIOUS");
		}
	}

	/* On cache la zone d'affichage des photos */
	function onClickReturnToIndex( )
	{
		//dbgTrace("Bouton INDEX - " + "index: " +  g_idCurrentIndexPage);
		g_displayManager.hideScreenPhoto( );		
	}


/* imageEvents( )
 * ----------------------
 *  Inscription des évênements concernant la fenêtre d'affichage des photos
 */
function imageEvents( )
{
		//Code des touches pour les evt clavier nous intéressant
		var KEY_ARROW_LEFT = 37;
		var KEY_ARROW_RIGHT = 39;
		var KEY_ESC = 27;

	  //Instanciation des boutons. Le callback est défini dans le constructeur
		g_buttonNextPhoto = new ButtonDeactivable( $(BUTTON_NEXTPHOTONAME), onClickNext );
		g_buttonPreviousPhoto = new ButtonDeactivable( $(BUTTON_PREVIOUSPHOTONAME), onClickPrevious );			
		$(BUTTON_RETURNTOINDEXNAME).click( onClickReturnToIndex );
		
		//Si clic droit non authorisé -> on surcharge l'evt
		if( !g_properties.photos.technical.rightClickEnabled )		{
			g_$divDisplayZoneName.bind( "contextmenu", function( e )
			{
				return false;
			});

		}

		//EVTs on appuie sur une touche
		$(window).keydown( function( evt )
		{			
			var f_watchedKeyCatched = false;
			if( g_watchedKeyDown === false 	//On ne gère qu'un keyevt (une touche pressée) à la fois
					&&  				 								//Cela a aussi pour avantage de ne pas appeler les fcts tant que les touches sont pressées
					g_displayManager.screenPhotoIsDisplayed() === true ) //Ne pas gérer les flèches quand on est sur l'écran d'index					
			{
				var keyPressed = evt.keyCode || evt.which;		
				switch( keyPressed )
				{
				//Passer d'une photo à la suivante / précédante lors de l'appuie sur une flèche
				case KEY_ARROW_LEFT:
					g_watchedKeyDown = true;
					onClickPrevious();
					f_watchedKeyCatched = true;
					break;
				case KEY_ARROW_RIGHT:
					g_watchedKeyDown = true;
					onClickNext();
					f_watchedKeyCatched = true;
					break;
				case KEY_ESC:
					g_watchedKeyDown = true;
					onClickReturnToIndex();
					f_watchedKeyCatched = true;
					break;				
				}
			}
			return !f_watchedKeyCatched;	//pour éviter de continuer à propager l'évent une fois qu'on l'a "utilisé".	
		});
		//EVT on relache une touche
		$(window).keyup( function( evt )
		{
			var keyPressed = evt.keyCode || evt.which;		
			switch( keyPressed )
			{
				case KEY_ARROW_LEFT:
				case KEY_ARROW_RIGHT:
				case KEY_ESC:
					g_watchedKeyDown = false;
					break;
			}
			/*return false*/
		});
}




/* loadingManagement( )
 * --------------------
 * Lancement du chargement d'une photo.
 * Definition du OnLoad -> 	+ Appel au DisplayManager (affichage si nécessaire)
 * 							+ Rappel de chargerPhoto( ) (fonction réentrante) si buffer non plein

 */
function loadingManagement( evt )
{
	
	var $handlePhoto; 				/* Objet JQuery pointant sur la photo à charger/afficher */
	var photoDivHW;					/* Dimensions souhaitees pour la photo */
	var photoTargetedSize;			/* Permet de récupérer la taille qui était prévu lorsqu'on a plannifié le chargement */
	var photoProperties;
	
	/* Init */	
	g_idPhotoToLoadNext = g_loadQueue.pop( );
	photoDivHW = g_displayManager.getMaxSize( );
	
	//Action uniquement si il y a une photo plannifiée pour chargement...
	//Si il n'y a plus de photo dans la file d'attente, on sort directementt de la fonction
	if( g_idPhotoToLoadNext != "undefined" &&  g_idPhotoToLoadNext >= 1 && g_idPhotoToLoadNext <= g_nbThumbnails )
	{
		//Photo à charger
		photoProperties =  choosePhotoToLoad( photoDivHW, g_idPhotoToLoadNext );
		//Récupération de la taille qui était prévue à l'origine
		photoTargetedSize = g_prefetchManager.getTargetedSize( g_idPhotoToLoadNext );
		
		// --- Actions spécifique suivant l'evt qui a appelé la fonction loadingManagement( ) --- //
		switch( evt ){
			//-- Il faut voir si on doit afficher le spinner
			case "NEXT":
			case "PREVIOUS":
			case "THUMBNAIL":
				if ( 		//La photo n'était pas déjà chargée
						 g_prefetchManager.isPhotoLoaded( g_idCurrentPhoto ) === false 
					 	|| // Ou la taille de la photo a changé
					 	(	photoTargetedSize !== "undefined"
					 		&&			//   || car sinon ça ne marche pas dans le cas de la photo récupérée en PHP
					 		( ( photoTargetedSize.h  < photoProperties.size.h || (photoTargetedSize.w) < photoProperties.size.w) )
					 	)
				   ) 
				{ 		// ==> Alors afficher le spinner
						g_displayManager.displayLoading( );
				}			
				break;
			//-- Il ne se passe rien de particulier si on est rentré par récusivité
			case "AUTO":
				break;
			default:
				break;	
		}
		//-----------------------------------//		
		
		
		/* Si la photo ne fait pas encore partie du cache => la charger */
		if (g_prefetchManager.hasPhoto( g_idPhotoToLoadNext ) === false)
		{ 
			photoURL = photoProperties.url;
			$handlePhoto = g_prefetchManager.addPhoto( g_idPhotoToLoadNext , photoProperties.size, g_displayManager.photoToDisplay );
			//dbgTrace( g_idPhotoToLoadNext + " (" + photoURL + ") "  + "not in cache" );							
			
			if ($handlePhoto != "undefined")
			{
				$handlePhoto
						/* callback OnLoad */
						.load(function()
						{	
							// La photo est bien présente dans la table
							var fDisplayed;			
							//dbgTrace( $(this).attr('src') + " loaded");
					    	/* Photo Chargée */
					    	g_prefetchManager.loadingFinished( this );
							/* Gestion de l'affichage */
							fDisplayed = g_displayManager.displayPhoto( $(this), g_prefetchManager); /* A besoin que loadingFinished ait été appelé précédemment */

							/* Préchargement de la photo suivante si il en reste à précharger */
							loadingManagement( "AUTO" ); /* <- Nouvelle entrée dans la fonction */
						})
						/* Gestion Erreur */
						.error( function()
						{
							$(this).attr('src','ressources/images/errorPhoto.gif');
							if(window.console){
									dbgTrace("Erreur de chargement photo #"+g_idPhotoToLoadNext);
							}							
						})
						/* Source de l'image */						
						.attr("src", src = photoURL);
			}
		}
		/* La photo fait déjà partie du cache, mais est-elle déjà vraiment chargée ? */
		else
		{	
			//dbgTrace( g_idPhotoToLoadNext + " in cache..." );
			/* Si la photo est déjà chargée  */
			if( g_prefetchManager.isPhotoLoaded( g_idPhotoToLoadNext ) == true )
			{ 	
				//dbgTrace("    ...already loaded");
				//On teste si la taille de la fenêtre a augmenté.
				// Oui -> Recharger l'image
				// Non -> Afficher (si besoin) l'image et programmer (si besoin) le chargement de la suivante
				if (photoTargetedSize !== "undefined" && //   || car sinon ça ne marche pas dans le cas de la photo récupérée en PHP
							( (photoTargetedSize.h ) < photoProperties.size.h || (photoTargetedSize.w) < photoProperties.size.w)
					  )
				{ 	
					//La taille de la fenêtre a augmenté => Rechargement de la photo. C'est la même photo : on la réinsère !
					//dbgTrace("Size modified -> reloading number " + g_idPhotoToLoadNext + " !" );
					g_prefetchManager.delPhoto( g_idPhotoToLoadNext );
					g_loadQueue.insert( g_idPhotoToLoadNext, 0 ); /* Insertion en 1ere position */
					loadingManagement( "AUTO" ); /* <- Point de récurssivité */
				}
				else
				{   // La taille de la fenêtre n'a pas augmenté => affichage de la photo et chargement de la suivante... si besoin !
					var fDisplayed;
					
					/* Gestion de l'affichage */
					$handlePhoto = g_prefetchManager.getPhoto( g_idPhotoToLoadNext );
					fDisplayed = g_displayManager.displayPhoto( $handlePhoto, g_prefetchManager );				

					/* Préchargement de la photo suivante si besoin */
					loadingManagement( "AUTO" ); /* <- Point de récursivité */

				}
			}
			/* Si Non => On ne fait rien : l'affichage se fera au moment du OnLoad */
		}
	}
	
	delete $handlePhoto;
	delete photoDivHW;
}

/* choosePhotoToLoad(photoDivHW, numPhoto )
 * ----------------------
 *  Récupération de l'URL de la photo en fonction de l'espace d'affichage dispo
 *  Cette URL peut être :
 *  + Un des répertoires contenant les photos à différentes tailles
 *  + le fichier getPhoto.php5 resizant les photos sur le serveur si l'espace d'affichage est trop petit
 *  In : photoDivHW - espace affichable
 *  In : numPhoto - numéro de la photo
 *  Ret: { URL de la photo , { w: width de la Photo , h: height de la photo } }
 */
function choosePhotoToLoad( photoDivHW, numPhoto )
{
	var photoURL = "undefined";
	var photoSize = {h:0,w:0};
	var resW;
	var resH;
	var f_OK = false;
	
	////////////////////////////////////
	//Deux méthodes différentes :
	//IE6 et 7 ne sont pas capable de redimensionner correctement une image --> on affichera TOUJOURS l'image immédiatement < à l'écran
	//Autres navigateurs : 
	//	Optimise Quality  --> on affichera l'image immédiatement < à l'écran
	//  Optimise Display Size  --> on prend la photo immédiatement > et le navigateur la redimensionnera
    // Cela fait donc 3 cas possibles
	////////////////////////////////////
	

  //Optimisation de la qualité ?
  if( g_properties.photos.technical.qualityStrategy === 0)
  {
      //On parcours les tailles d'images disponibles
      //On prend la première qui tient dans l'espace disponible
      resH = getSmallerImage( photoDivHW, numPhoto );
      
      //Si aucune photo ne convient, on prend la plus petite dispo et le navigateur resizera
      if( resH === -1 ){
          resH = g_nbRes;
      }
      
      //Récupération de la photo choisie
      var folderId = resH+1;
      photoSize.h = g_properties.photos.list[ numPhoto - 1].sizes[ resH ].height;
      photoSize.w = g_properties.photos.list[ numPhoto - 1].sizes[ resH ].width; //Oui c'est bien resH !!
      photoURL = IMAGES_PATH + folderId + "/"  + g_properties.photos.list[ numPhoto - 1].filename;
  }//Fin Optimisation de la qualité
  
  //Optimisation de l'espace ? -- Marche de façon médiocre avec ie6/ie7 qui ont du mal avec les redimensionnements
  else
  {
      //On récupère la plus petite image de taille > à l'espace afficache. Le navigateur resizera.
      resH = getBiggerImage( photoDivHW, numPhoto );
      //Si aucune photo n'a convenu, on prend la plus grande. Elle sera naturellement conservée à sa taille d'origine.
      if( resH === -1 ){
          resH = 1;
      }
      resW = resH;
      
      //Récupération de la photo choisie
      var folderId = resH+1;
      photoSize.h = g_properties.photos.list[ numPhoto - 1].sizes[ resH ].height;
      photoSize.w = g_properties.photos.list[ numPhoto - 1 ].sizes[ resW ].width;
      photoURL = IMAGES_PATH + folderId + "/"  + g_properties.photos.list[ numPhoto - 1].filename;
  }// Fin Optimisation de l'espace       
	
	
	
	return { url:photoURL , size:photoSize }; 
}

/* getBiggerImage( photoDivHW, numPhoto )
 * ----------------------
 * Trouve l'index, dans le tableau des résolutions, de la photo de résolution immédiatement suppérieure
 * à l'espace affichable.
 * In : photoDivHW - espace affichable
 * In : numPhoto - numéro de la photo
 * Ret: index de la photo ; -1 si non trouvé
 */
function getBiggerImage( photoDivHW, numPhoto )
{
	var resW = g_nbRes-1;
	var resH = g_nbRes-1;
	var f_OK = false;
    
    //On parcourt toutes les tailles de photos de la plus petite à la plus grande
	//Celle qui conviendra sera la première avec une dimension trop grande pour être affichée			
	while( resW >= 0 && resH >= 0 && f_OK === false )
	{
			//Une dimension est-elle supérieure à l'espace affichable ?
			if( g_properties.photos.list[ numPhoto - 1].sizes[ resW ].width >= photoDivHW.w 
					||
					g_properties.photos.list[ numPhoto - 1].sizes[ resH ].height >= photoDivHW.h  )
			{
				f_OK = true;
			}
			else{ //On passe à la photo de taille supérieure
				resW--;
				resH--;
			}
	}//Fin while
    
    if( f_OK === true ){ //Résultat concluant
        return resH; //<<-- Implique que resH = resW...
    }
    else{
        return -1;
    }
}

/* getSmallerImage( photoDivHW, numPhoto )
 * ----------------------
 * Trouve l'index, dans le tableau des résolutions, de la photo de résolution immédiatement suppérieure
 * à l'espace affichable.
 * In : photoDivHW - espace affichable
 * In : numPhoto - numéro de la photo
 * Ret: index de la photo ; -1 si non trouvé
 */
function getSmallerImage( photoDivHW, numPhoto )
{
	var resW = 0;
	var resH = 0;
    var f_OK = false;
	
	//On parcours les tailles d'images disponibles
    //On prend la première qui tient dans l'espace disponible
	while( resW <= g_nbRes && resH <= g_nbRes && f_OK === false )
	{
		//Recherche du width immédiatement inférieur à celui disponible
		if( g_properties.photos.list[ numPhoto - 1].sizes[ resW ].width <= photoDivHW.w )
		{
			//Puis Recherche du height immédiatement inférieur à celui disponible
			resH = resW;
			while( resH <= g_nbRes && f_OK === false )
			{
					//On a trouvé la bonne combinaison !
					if( g_properties.photos.list[ numPhoto - 1 ].sizes[ resH ].height <= photoDivHW.h ){
						f_OK = true;
					}
					else{
						resH++;
					}
			}//Fin while
		}
		else{
			resW++;
		}
	}//Fin while
    
    if( f_OK === true ){ //Résultat concluant
        return resH; //<<-- Implique que resH = resW...
    }
    else{
        return -1;
    }
}
