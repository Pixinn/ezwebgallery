/* 
 *  EZWebGallery:
 *  Copyright (C) 2011 Christophe Meneboeuf <dev@ezwebgallery.org>
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



#ifndef GLOBALDEFINITIONS_H
#define GLOBALDEFINITIONS_H

#include <QString>
#include <QDateTime>
#include <QMap>

/*** SETTINGS ***/
#define SETTINGS_RECENTPROJECTS     "Recent Projects List"
#define SETTINGS_RECENTSKINS        "Recent Skins List"
#define SETTINGS_LANGUAGE           "Prefered Language"
#define SETTINGS_OPENMOSTRECENTPJT  "Open Most Recent Project"
#define SETTINGS_OPENGENERATEDGAL   "Open Generated Gallery"

/*** GUI ***/
//#define RELEASE             "15"
#define XMLSESSIONEXTENSION ".gallery"
#define XMLLASTSESSIONFILE  "lastSession"
#define SKINSESSIONEXTENSION ".skin"
#define SKINLASTSESSIONFILE  "lastSkin"


/*** Galerie ***/
#define GALLERYTHUMBFILENAME "galleryThumbnail.jpg"
#define PHOTOPREFIXE    "Photo_"
#define THUMBPREFIXE    "tn"
#define THUMBSPATH      "thumbnails"
#define PHOTOSPATH      "images"
#define RESOLUTIONPATH  "res"
#define RESPATH		"ressources"
#define CSSPATH		"ressources/css"
#define RESIMGPATH      "ressources/images"
#define RESEMOTICONSPATH  "ressources/images/smileys/"
#define CSSINDEXSKINFILENAME "index-skin.css"
#define JSPATH          "ressources/javascript"
#define JSCONFFILENAME  "galleryConfiguration.js"
#define JSPRESFILENAME  "galleryPresentation.js"

/**** skins ****/
#define SKINPATH           "skins"                                  /*Path depuis le rpertoire de l'appli*/
#define DEFAULTSKINNAME    "default"
#define SKINRESDIR         "ressources"
#define SKINRESIMGDIR      "images"
#define SKINCSSFILENAME    "skin.css"                             /*Fichier css qu'index.html doit inclure*/

/*** Fichiers  copier ***/
#define RESSOURCES     "res"


#endif // GLOBALDEFINITIONS_H
