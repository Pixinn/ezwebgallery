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

/*****************************************************************************
 * 							PREREQUIS
 *****************************************************************************
 * JAVASCRIPT :
 * ------------
 *  FRAMEWORK
 * 		+ JQuery version >= 1.3.2
 *  CORE CLASSES
 * 		+ classTablePhoto.js	 -	Cache contenant les photos et les infos nécessaires
 * 				-> requiert classHashTable.js
 * 		+ classDisplayManager.js - Gère l'affichage et la mise à l'echelle
 * 				-> requiert classObjetAjustable.js
 *  FONCTIONS APPELEES
 * 		+ images.js				 - Gère tout ce qui a trait au chargement et à l'affichage de la photo
 * 				-> utilise classTablePhoto et classDisplayManager
 * 				-> défini les évenements associés à la photo. ie les boutons de navigation
 * 		+ index.js				 - Gère l'interactivite (scrolling) et la navigation sur les vignettes (page d'index)
 * 		+ common.js				 - Instancie la galerie, génère le html et appèle les fct pour inscire les evts
 *****************************************************************************/

/********* DEFINES ************/
//--- Urls 
var URL_BASE = "ressources/php/getPhoto.php?photoURL="; //<= RELEASE
var URL_THUMBS_PATH = "thumbnails/"; /* chemin vers les vignettes. En relatif et avec le / final ! */
var IMAGES_PATH = "images";
var IMAGEFILEEXTENSION = ".jpg";
//-- CSS & DOM --//
//-- Barre de chargement
var DIV_PROGRESSBARWRAPPER = "#progressbarWrapper";
var DIV_PROGRESSBARNAME = "#progressbar";
//-- Ecran Photos
var DIV_SCREENPHOTO = "#screenPhoto";
var DIV_PHOTONAME = "#cadrePhoto";
var CLASS_LEGEND = 	"photoCaption"; //Pas de # ou de . !!
var DIV_DISPLAYZONENAME = "#wrapperAffichagePhoto";
var CLASS_MAINPHOTO = "mainPhoto"; /* classe appliquée à la photo affichée */
var BUTTON_NEXTPHOTONAME = "#boutonNext";
var BUTTON_PREVIOUSPHOTONAME = "#boutonPrevious";
var BUTTON_RETURNTOINDEXNAME = "#boutonIndex";
//-- Loading
var CLASS_SPINNER = "Spinner"; //Pas de # ou de . !!
var AJAX_TIMEOUT_VALUE = 30000;
//-- Ecran Index
var DIV_SCREENINDEX = "#screenIndex";
var DIV_INDEXDISPLAYZONE = "#wrapperAffichageIndex";
var DIV_NAVIGATIONINDEX_CONTAINER = "ul.indexNavigation";
var CLASSNAVIGATIONTABSELECTED = "navTabSelected"; /* Onglet de navigation sélectionné */
var DIV_INDEX_CONTAINER = "#indexSliderContainer";
var DIV_THUMBSWRAPPER = ".thumbsWrapper"; 	/* Besoin de ce wrapper sinon petit bug : taille des borders non prises en compte */
var DIV_SCROLLCONTAINER = ".scrollContainer";/* Contient les panneaux glissant */
var DIV_SLIDINGPANEL = ".slidingPanel";			/* Panneau glissant */
var DIV_THUMBCONTAINER = ".thumbBox";				/* Thumbnails */
var ID_INDEXPREFIX = "index";	/* Utilisé lors de la génération automatique pour l'id des panneaux coulissant */
//var THUMBFILEPREFIX = "tn_"; /*Préfixe pour le nom des fichiers de vignettes */
//--- Commun



/*****************************************/

/* *********** VAR GLOBALES *********** */
//objets
var g_loadQueue;
var g_prefetchManager;
var g_displayManager;
//navigation
var g_idCurrentPhoto;					/* photo à afficher */
var g_idPhotoToLoadNext;				/* On essaie d'avoir en permanence g_idCurrentPhoto + g_properties.photos.technical.prefetchSize dans le cache */
var g_idCurrentIndexPage;				/* Page de vignettes courantes */
var g_properties;                       //Gallery properties
//photos
var g_nbRes;						/* Nombre de resolutions */
var g_nbPhotos;						/* Nombre de photos */
//affichage
var g_pixelsOccupesEnHaut;			/* NB de pixels utilises pour autre chose que la photo */
var g_pixelsOccupesEnBas;				/* NB de pixels utilises pour autre chose que la photo */
//index
var g_listeThumbnails;					/* Tableaux contenant le nom des thumbs associé à leur numéro */
var g_nbThumbFullyLoaded;				/* Nombre de vignettes effectivement chargées (on doir attendre le chargement de toutes les vignettes */
var g_nbThumbnails;
//controles et boutons
var g_buttonNextPhoto;
var g_buttonPreviousPhoto;
var g_buttonReturnToIndex;


// *** Objets JQuery instanciés une fois pour plus de rapidité
//Pour scroller l'index avec les vignettes
var g_$divScrollContainer;
var g_$divSlidingPanels;
//CSS: div et conteneur
//Fenêtre Photo
var g_$screenPhoto;	
var	g_$divCadrePhotoName;
var	g_$divDisplayZoneName;
//var	g_$divSpinner;			// A VIRER !!!
//-- Index
var g_$screenIndex;
var g_$buttonsIndexNavigation;
var	g_$divIndexContainer;
var	g_$divThumbsWrapper; 
var g_$divThumbBox;
var g_$divScrollContainer;
var	g_$divSlidingPanel;
var g_thumbSize;

var g_idWaitForThumbs;		//handle vers la fonction d'attente des vignettes
/*****************************************/	


function ajaxError(request, errorType, errorThrown) {
	if (window.console) {
		console.log("Ajax error: " + errorType);
	}
} 

 /* *********** OnReady *********** */
$(document).ready(function()
{		

	/******* Barre de chargement ******/
	/* Pour une raison inconnue le "width" est mal calculé à ce moment... */
	/* Donc pas de .widowsCenter() possible. pas de show( ) non plus ??? */
	/* Ces bugs n'affectent pas ie7 ??? */
	$(DIV_PROGRESSBARWRAPPER).css("display","block")
							 .css("top",( $(window).height( ) - $(DIV_PROGRESSBARWRAPPER).outerHeight( ) )/ 2)
							 .css("left",( $(window).width( ) - 266)/2 );
							 
	
	//Configuration Ajax
	$.ajaxSetup( {
   		timeout: AJAX_TIMEOUT_VALUE,
		cache: false,
		error: ajaxError
 	} );
	//Catcher les erreurs
	$(document).ajaxError(function(event, request, settings)
	{ 	//Seulement sur les browsers qui supportent...
    	if (window.console && window.console.error) {
        console.error( "Error requesting page:" + settings.url );
    	}	
	});
    
    $.getJSON("ressources/parameters.json",
        function(data){
            g_properties = data;
            initGallery( );	 //Next: gallery initialization
        }
    );
		
	
});	


/*************** Fonctions pour préparation de la page *********************/


function initGallery( )
{
	 /******* STEP 1 : Génération de l'index ******************/
	/* ---- Init Index ----*/
	// => Inclusion de l'index des vignettes dans le HTML
	
	// 1 - Les "paneaux" contenant les vignettes et les onglets associés
	g_nbThumbFullyLoaded = 0;
	g_nbThumbnails = 0;
	g_nbRes = 0;
	var nbPanneaux;
	var numIndex;
	var numThumbnails;
    var nbThumbsByPanel = g_properties.index.mosaic.nbRows * g_properties.index.mosaic.nbCols;
	g_listeThumbnails = new Array;

    g_nbThumbnails = g_properties.photos.list.length;
    g_nbRes = g_properties.photos.list[0].sizes.length; //at least one photo in the list
    
	g_nbPhotos = g_nbThumbnails;
	nbPanneaux = Math.ceil( g_nbThumbnails / nbThumbsByPanel );
   
	
	/* Récupération et mise en forme de la liste des thumbnails */
	/* création de g_listeThumbnails[nbPanneaux][NBTHUMBSBYPAGE]  */
    for( var i = 0 ; i <  g_properties.photos.list.length; i++ ) {
        var photo = g_properties.photos.list[ i ];
        g_listeThumbnails[ photo.filename ] =  i + 1 ;
    }
	
	
	/* Génération des panneaux et des onglets */
	for ( i = 0; i < nbPanneaux ; i++){
		numIndex = i+1;
		//Panneaux
		$(DIV_SCROLLCONTAINER).append( '<div class="' +DIV_SLIDINGPANEL.substr(1,DIV_SLIDINGPANEL.length-1)+ '" id="' +ID_INDEXPREFIX+numIndex+ '"></div>' );
		//Onglets... seulement si y'en a plusieurs...
		if (nbPanneaux > 1) {
			$(DIV_NAVIGATIONINDEX_CONTAINER).append('<li><a href="#' + ID_INDEXPREFIX + numIndex + '" id="' + numIndex + '">' + numIndex + '</a></li>');
		}
	}
	// L'onglet #1 est sélectionné au démarrage :
	$(DIV_NAVIGATIONINDEX_CONTAINER).find("#"+1)
									.parent().addClass( CLASSNAVIGATIONTABSELECTED ); 
	
	// 2 - Les vignettes
    // a. Computing wich thumbnail set to use
    var f_setFound = false;
    var thumbSet = g_properties.index.mosaic.defaultSet; //if no suitable set is found
    g_thumbSize = g_properties.index.mosaic.sizes[ thumbSet ];
    var margin = 100; //60 is necessary for vertical (tab height + logo). 100 is for horizontal comfort.
    var availableWidth = $("#screenIndex").width() - g_properties.index.mosaic.unavailable.horizontal - margin/2;
    var availableHeight = $("#screenIndex").height() - g_properties.index.mosaic.unavailable.vertical - margin;
    $.each( g_properties.index.mosaic.sizes, function( key, size ) //iterating on the object using jQuery
    {
            if (   !f_setFound
                && (availableWidth > size * g_properties.index.mosaic.nbCols)
                && (availableHeight > size * g_properties.index.mosaic.nbRows) )
            {
                thumbSet = key;
                g_thumbSize = size;
                f_setFound = true;
            }
    } );
    
    // b. Thumbnail containers
	var $panneaux = $(DIV_SLIDINGPANEL);
	var indexPanneau;
    var numThumbnails = 1;
	for (indexPanneau = 0; indexPanneau < $panneaux.length; indexPanneau++) {
        var i = 1;
        while( ( i <= nbThumbsByPanel ) && ( numThumbnails <= g_nbThumbnails ) ) {
            $(DIV_SLIDINGPANEL).eq(indexPanneau).append('<div class="'+DIV_THUMBCONTAINER.substr(1,DIV_THUMBCONTAINER.length-1)+'" id="'+numThumbnails+'"></div>');	
            numThumbnails++;
            i++;
        }
    }
      
    // c. loading thumbnails
    $(DIV_THUMBCONTAINER).each( function( index )
    {
        var thumbName =g_properties.photos.list[ index ].filename
        var thumbnail = new Image();
        $(this).append( thumbnail );
        $(thumbnail).load( function( )	{	// callback OnLoad
                        g_nbThumbFullyLoaded++;               
                        $(DIV_PROGRESSBARNAME).reportprogress( g_nbThumbFullyLoaded, g_nbThumbnails );                            
					})
					.error( function()  {
						dbgTrace("common.js / Erreur de chargement thumbnail "+g_nbThumbFullyLoaded++);                            
					})
					// Source de l'image : A METTRE EN DERNIER
					.attr("src", src = URL_THUMBS_PATH+thumbSet+'/'+thumbName );
    } );
  
	
	/******* STEP 2 : Attente de fin de chargement des vignettes ***********/
	/******* Initialisation des objets en // pour gagner du temps **********/
	g_idWaitForThumbs = window.setInterval( initGallery_step2_waitForThumbs, 500 ); //Appel à intervalles réguliers jusqu'à ce que toutes les vignettes soient chargées
	
	
	//-- Objets JQuery --//
	//-- Fenêtre Photo
	g_$screenPhoto = $(DIV_SCREENPHOTO);
	g_$divCadrePhotoName = $(DIV_PHOTONAME);
	g_$divDisplayZoneName = $(DIV_DISPLAYZONENAME);

	//-- Index
		//Variables pour scrolling
	g_$divScrollContainer = $(DIV_SCROLLCONTAINER);
	g_$divSlidingPanels = $(DIV_SLIDINGPANEL);
	g_$divThumbsWrapper = $(DIV_THUMBSWRAPPER).css( "overflow" , "hidden" );
	 //autres
	g_$screenIndex = $(DIV_SCREENINDEX);    
  	g_$buttonsIndexNavigation =  $(DIV_NAVIGATIONINDEX_CONTAINER + " a"); /* Onglet */
	g_$divIndexContainer = $(DIV_INDEX_CONTAINER);
 	g_$divThumbBox = $(DIV_THUMBCONTAINER);
 	
 	// ---- Init et instanciation des autres objets ---- //
	g_idCurrentPhoto = g_properties.photos.technical.first;
	g_idCurrentIndexPage = g_properties.index.mosaic.first;
	g_pixelsOccupesEnHaut = 33;
	g_pixelsOccupesEnBas =  33;

	g_displayManager = new DisplayManager( 									 g_$screenPhoto,
																			 g_$divCadrePhotoName,
																			 $(BUTTON_NEXTPHOTONAME),
																			 $(BUTTON_PREVIOUSPHOTONAME),
																			 $(BUTTON_RETURNTOINDEXNAME),
																			 CLASS_MAINPHOTO,
																			 CLASS_LEGEND,
																			 CLASS_SPINNER,
																			 0,
																			 g_pixelsOccupesEnHaut,
																			 g_pixelsOccupesEnBas,
																			 g_properties.photos.technical.maxSize.width,
																			 g_properties.photos.technical.maxSize.height );
																			 
	g_prefetchManager = new TablePhotos( g_properties.photos.technical.cacheSize );
	g_loadQueue = new Fifo( );

}


function initGallery_step2_waitForThumbs( )
{
	if (g_nbThumbFullyLoaded >= g_nbThumbnails) {
		clearInterval(g_idWaitForThumbs); //Arrêt de cette fonction
		initGallery_step3( );
	}
}


function initGallery_step3( )
{	
    //resizing the mosaic according to the thumb set chosen
    var thumbBorderWidth = $(".thumbBox img").first().css("border-top-width");
    thumbBorderWidth = thumbBorderWidth.substr( 0, thumbBorderWidth.indexOf("px",0) );
    $(".thumbBox").width( g_thumbSize + 2*thumbBorderWidth )
                  .height( g_thumbSize + 2*thumbBorderWidth);
    var mosaicWidth = g_properties.index.mosaic.nbCols * $(".thumbBox").outerWidth();
    var mosaicHeight = g_properties.index.mosaic.nbRows * $(".thumbBox").outerHeight();
    var $thumbsWrapperContainer = $(".thumbsWrapperContainer");
    var thumbsWrapperContainerBorder = $thumbsWrapperContainer.css("border-left-width")
    thumbsWrapperContainerBorder = thumbsWrapperContainerBorder.substr( 0, thumbsWrapperContainerBorder.indexOf("px",0) );
    $("#indexSliderContainer").width( mosaicWidth + 2*thumbsWrapperContainerBorder );
    $thumbsWrapperContainer.width( mosaicWidth )
                           .height( mosaicHeight );
    $(".thumbsWrapper").width( mosaicWidth )
                       .height( mosaicHeight );
    $(".scrollContainer").find(".slidingPanel").width( mosaicWidth )
                                                .height( mosaicHeight );

    // ########## DEBUG ##########
    /*alert( "indexSliderContainer: " + $("#indexSliderContainer").eq(0).width() + " - " + $("#indexSliderContainer").eq(0).height() + "\n" +
           "thumbsWrapperContainer: " +  $thumbsWrapperContainer.width() + " - " + $thumbsWrapperContainer.height() + "\n" +
           "thumbsWrapper: " + $(".thumbsWrapper").eq(0).width() + " - " + $(".thumbsWrapper").eq(0).height() + "\n" +
           "scrollContainer div sliding: " + $(".scrollContainer").find(".slidingPanel").eq(0).width() + " - " + $(".scrollContainer div.slidingPanel").eq(0).height() ); */
    // ##########################
                                          
	//Cacher la barre de loading
	$(DIV_PROGRESSBARWRAPPER).hide( )
	
	// ---- Positionnement ----
	//Index
	g_$divIndexContainer.show( )
					    .verticalCenter( 0 );
	$(DIV_THUMBCONTAINER+" img").verticalCenter( 0 );
	
	
	/* --------- Evenements  ----------- */
	//Tout ce qui concerne l'index et ses vignettes
	indexEvents( ); 		// <- Défini dans index.js
	//Tout ce qui concerne la fenêtre d'affichage des photos
	imageEvents( );			// <- Défini dans image.js
	/* Evenement resize fenêtre */
	/* => Concerne l'index et la fenêtre d'affichage des photos */
	$(window).resize( function( ){ onWindowResize( ); } );
	/* ----------------------------------- */
		
		
	/**** SCROLLING de l'Index ***
	 * Inspiré de : http://jqueryfordesigners.com/coda-slider-effect/
	 */
	var fHorizontal = true;
	
	// float the panels left if we're going horizontal
	if (fHorizontal === true) {
	  g_$divSlidingPanels.css({
    	'float' : 'left',
	    'position' : 'relative' // IE fix to ensure overflow is hidden
	  }); 
	  // calculate a new width for the container (so it holds all panels)
	  g_$divScrollContainer.css('width', g_$divSlidingPanels[0].offsetWidth * g_$divSlidingPanels.length);
	}


	var scrollOptions = {
	  target: g_$divThumbsWrapper, // the element that has the overflow
	  				   			// can be a selector which will be relative to the target
	  margin:true,								
	  items: g_$divSlidingPanels, // Que scrolle t'on ?
	  navigation: '.indexNavigation a', // En cliquant sur quoi ?
	  axis: 'xy',    // allow the scroll effect to run both directions
	  duration: 500, // duration of the sliding effect
	  easing: 'swing', //Effet de "freinage"
	  onAfter: triggedAfterScroll // our final callback

	};

	 //Tous les liens de la page permettent de faire scroller
	$.localScroll( scrollOptions );
		
	//Highlighter le bon onglet si l'adresse de la page pointe sur un index précisé (ie #index...)
	if (window.location.hash) {
  		triggedAfterScroll({ id : window.location.hash.substr(1) }); /* On appelle manuellement la fonction */
	}
	else { //Si pas de sharp dans la barre d'adresse, simuler un appuis sur le 1er onglet
		  g_$buttonsIndexNavigation.eq(0).click( );
	}
		
}//Fin Step3


//"Highlight" l'onglet de navigation correspondant à l'index affiché 
// voir : http://jqueryfordesigners.com/coda-slider-effect/
function highlightNav() {
	g_$buttonsIndexNavigation.each( function(){
		$(this).parent().removeClass(CLASSNAVIGATIONTABSELECTED); // enlever la classe de TOUS les onglets
	} );
   $(this).parent().addClass(CLASSNAVIGATIONTABSELECTED);  						   
}

/* Appelé à la fin d'un scrolling par le "onAfter" défini dans "scrolloptions" 
 * voir : http://jqueryfordesigners.com/coda-slider-effect/
 */
function triggedAfterScroll(data) {
  // within the .navigation element, find the A element
  // whose href ends with ID ($= is ends with)
  var el = $(DIV_NAVIGATIONINDEX_CONTAINER).find('a[href$="' + data.id + '"]').get(0);
  // we're passing the actual element, and not the jQuery instance.
  highlightNav.call(el);
}


/* Appelé lorsqu'une vignette est chargée => permet de les compter */
function onThumbLoaded( id )
{
	g_nbThumbFullyLoaded++;
	$(DIV_PROGRESSBARNAME).reportprogress( g_nbThumbFullyLoaded, g_nbThumbnails );
}

/*************** TYPEDEF *************/
function s_Taille( )
{
	var h;
	var w;
}



/* *********** FONCTIONS Génériques *********** */


/* onWindowResize( )
 * -----------------
 * L'espace d'affichage vient de changer
 */
function onWindowResize( )
{
	//Index
	g_$divIndexContainer.verticalCenter( 0 );
	//Photos
	//Seulement si la zone des photos est affichée..
	if (g_$divDisplayZoneName.is(':visible') === true) {
		//Ajustement de la photo et de la zone d'affichage associée
		var JqPhoto = g_prefetchManager.getPhoto(g_idCurrentPhoto);
		g_displayManager.adjustPhoto(JqPhoto);
		$(BUTTON_PREVIOUSPHOTONAME).verticalCenter( 0 );
		$(BUTTON_NEXTPHOTONAME).verticalCenter( 0 );
	}
}

/*
 * Pseudo plugin JQuery pour centrer un objet verticalement dans son parent 
 * On peut passer un décalage en paramètre pour ajuster le calcul si l'objet n'est pas seul dans son parent
 * !! -> Nécessite un objet en position RELATIVE !!
 */
jQuery.fn.verticalCenter = function( decalage) {
  return this.each(function(  ){
	   	var top;
		top =( jQuery(this).parent( ).height( ) - jQuery(this).outerHeight( ) - 2*decalage )/ 2;
		top = Math.max( top, 0 ); /* Pas de décalage < 0 ! Sinon on a des bugs de positionnement */
		jQuery(this).css("top",top);
  });
};

/*
 * Pseudo plugin JQuery pour centrer un objet horizontalement dans son parent 
 */
jQuery.fn.horizontalCenter = function() {
  return this.each(function(){
   		var left;
			left =( jQuery(this).parent( ).innerWidth( ) - jQuery(this).outerWidth( ) )/ 2;
			jQuery(this).css("margin-left",left);
  });
};

/*
 * Pseudo plugin JQuery pour centrer un objet dans la fenêtre 
 * Doit être en position:absolute
 */
jQuery.fn.windowCenter = function(){
  return this.each(function(){
   	var left;
		var top;
		left =( jQuery(window).width( ) - jQuery(this).outerWidth( ) )/ 2;
		top =( jQuery(window).height( ) - jQuery(this).outerHeight( ) )/ 2;
		jQuery(this).css("left",left)
		       		.css("top" ,top);
  });
};


/* calculerTaillesMax( ... )
 * ------------------------
 * Calcul la taille max disponible pour afficher la photo
 * In : (jQuery) cadrePhoto - le cadre contenant la photo + le titre et la légende
 * In : maxWidth - width max de la photo
 * In : maxHeight - height max de la photo
 */
function calculerTaillesMax( nbPixelsOccupesEnHaut, nbPixelsOccupesEnBas, cadrePhoto, maxWidth, maxHeight)
{
	
	/*** ATTENTION : METHODE DE CALCUL OU STRUCTURE DU HTML A REVOIR ****/
	/* Ne marche pas si header et footer au même niveau que g_$divDisplayZoneName : ils seront comptés comme siblings */
	/* La structure de la page doit alors êtree:
	 * <header> DIVs OPTIONNELS </header>
			<vertCentral>
				<left>TOUT CE QUI EST A GAUCHE DE LA PHOTO</left>
					<centralZone (ie:g_$divDisplayZoneName)> PHOTO + TITRE + LEGENDE + etc </centralZone>
				<right>TOUT CE QUI EST A DROITE DE LA PHOTO</right>
			</vertCentral>
		<footer> DIVs OPTIONNELS </footer>
	  quitte à ce que certaines zone aient une taille nulle (ou 1px si impossible) */


		var CORRECTIF_BUG_IE6 = 6; /* IE6 ne peut pas calculer au pixel prêt: décalage de 3px de chaque coté, le Three Pixel Jog"! */
		var photoBorderWidth;
		var divTaille = new s_Taille( );
		var widthWasted = 0;
		var i;
		
		/* On calcule la largeur occupé par d'autres div. 
		 * ATTENTION : seuls les divs du même niveau hiérarchique sont contabilisés! 
		 * Cela permet de corriger un bug sous IE6 -> voir plus bas.
		 */		
		for(i = 0; i < g_$divDisplayZoneName.siblings().length; i++){
				widthWasted += g_$divDisplayZoneName.siblings().eq(i).outerWidth( );
		}
	
		
		//Espace non dispo pour la photo
		divCadrePhotoBorderSize = cadrePhoto.css("padding-top");
		divCadrePhotoBorderSize = divCadrePhotoBorderSize.substr( 0, divCadrePhotoBorderSize.indexOf("px",0) );
		/* NB: On ne peut pas utiliser g_$divDisplayZoneName.innerWidth( ), car bug sous IE6 :( */
		/* Du coup il faut que:	+ g_$divDisplayZoneName.siblings()::margin = 0px
														+ g_$divDisplayZoneName::border=0 et ::margin=0 */
        divTaille.h = g_$divDisplayZoneName.innerHeight( ) - 2*divCadrePhotoBorderSize - 2*g_properties.photos.technical.decoration.padding - nbPixelsOccupesEnHaut - nbPixelsOccupesEnBas;
		/*divTaille.w = g_$divDisplayZoneName.width( ) - divCadrePhotoBorderSizeW - CORRECTIF_BUG_IE6; <-- NE MARCHE PAS SOUS IE6 !!!*/ 
		divTaille.w = $(window).width( ) - widthWasted - 2*divCadrePhotoBorderSize - 2*g_properties.photos.technical.decoration.padding - CORRECTIF_BUG_IE6;
		
		if( divTaille.h > maxHeight ){
			divTaille.h = 	maxHeight;}
		if( divTaille.w > maxWidth ){
			divTaille.w = 	maxWidth;}
				
		return divTaille;
}

/* dbgTrace( log )
 * ------------------------
 * Trace dans la console si dispo
 * In: log
 */
function dbgTrace( log ){
	if (window.console) {
		console.log( log );
	}
}
