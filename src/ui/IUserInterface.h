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

#include "CGalleryGenerator.h"
#include "IMessage.h"
#include "CMessage.h"


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
   
// !!! Fonctions  implmenter comme SLOTS !!!
// C'est impossible dans cette interface, car il faudrait hriter de QObject. Hors il est interdit
// d'hériter deux fois indirectement de QObject. Ce qui est embtant pour les UI qui doivent ventuellement elles mme hriter d'un QWidget
public:
   //public slots
   virtual void onLogMsg( const IMessage & ) = 0; //Message to Log
   virtual void onProgressBar( int completion, QString color, QString message, int timeout = 0 ) = 0; //Affiche un % d'avancement sur la progressBar et un message assi dans la statusbar
   virtual void onGalleryGenerationFinished( QList<CPhotoProperties>  ) = 0; //Fin nominale de la gnration de la galerie
   virtual void onForceStoppedFinished( QStringList ) = 0; //Fin prmature de la gnration de la galerie
   virtual void error( CMessage ) = 0;          //An error occured
   virtual void warning( CMessage ) = 0;        //A warning occured
   virtual void information( CMessage ) = 0;    //Display an iformative message

};


#endif // IUserInterface_H
