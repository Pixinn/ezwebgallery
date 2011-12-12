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


#ifndef IUserInterface_H
#define IUserInterface_H

#include <QMainWindow>
#include <QString>

#include "CPhotoPropertiesExtended.h"
#include "CGalleryGenerator.h"


/*******************************
 * IUserInterface
 * ----------------------
 * Classe d'interface : abstraction de l'interface graphique
 * pour le gnrateur de photo.
 ******************************/

class IUserInterface/*: public QMainWindow *///Pour pouvoir proposer des slots, doit hriter de manire directe ou indirecte de Q_Object
                                           //Et on ne peut pas faire d'hritage multiple avec plusieurs classe drivant de Q_Object
                                           //On prend donc la classe d'ui la plus gnraliste
{
//  Q_OBJECT
   
public /*slots*/:
// !!! Fonctions  implmenter comme SLOTS !!!
// C'est impossible dans cette interface, car il faudrait hriter de QObject. Hors il est interdit
// d'hriter deux fois indirectement de QObject. Ce qui est embtant pour les UI qui doivent ventuellement elles mme hriter d'un QWidget
public:
   virtual void onLogMsg( QString ) = 0; //Rception d'un message de DEBUG
   virtual void onProgressBar( int completion, QString color, QString message, int timeout = 0 ) = 0; //Affiche un % d'avancement sur la progressBar et un message assi dans la statusbar
   virtual void onGalleryGenerationFinished( QList<CPhotoPropertiesExtended>  ) = 0; //Fin nominale de la gnration de la galerie
   virtual void onForceStoppedFinished( QStringList ) = 0; //Fin prmature de la gnration de la galerie

public:
//    IUserInterface( QWidget *parent);
//    IUserInterface( );
//    virtual ~IUserInterface() = 0;    //Destructeur virtuel obligatoire
//    virtual void setGenerator( CGalleryGenerator* ) = 0; //Indique le gnrateur de galerie

};


#endif // IUserInterface_H
