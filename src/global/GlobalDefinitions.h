/* 
 *  EZWebGallery:
 *  Copyright (C) 2011 Christophe Meneboeuf <christophe@xtof.info>
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
#define SETTINGS_EMBEDGAID          "Embed a default GA tracking ID"
#define SETTINGS_GAID               "Default GA ID"
#define SETTINGS_ENABLEPREVIEW      "Enable Preview"
#define SETTINGS_AFTERGENERATION    "Action after producing the gallery"


/*** GUI ***/
#define XMLSESSIONEXTENSION     ".gallery"
#define XMLLASTSESSIONFILE      "lastSession"
#define SKINSESSIONEXTENSION    ".skin"
#define SKINLASTSESSIONFILE     "lastSkin"


/*** Galerie ***/
#define GALLERYTHUMBFILENAME    "galleryThumbnail.jpg"
#define PHOTOPREFIXE            "Photo_"
#define THUMBPREFIXE            "tn_"
#define THUMBSPATH              "thumbnails"
#define PHOTOSPATH              "images"
#define RESOLUTIONPATH          "res"
#define RESPATH		            "resources"
#define CSSPATH		            "resources/css"
#define RESIMGPATH              "resources/images"
#define RESEMOTICONSPATH        "resources/images/smileys/"
#define JSPATH                  "resources/javascript"
#define IMAGEFILEEXTENSION      ".jpg"


/**** skins ****/
#define DEFAULTSKINNAME    "default"
#define SKINRESDIR         "resources"
#define SKINRESIMGDIR      "images"
#define SKINCSSFILENAME    "skin.css"                             /*Fichier css qu'index.html doit inclure*/


#endif // GLOBALDEFINITIONS_H
