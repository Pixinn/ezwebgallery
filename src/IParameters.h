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



#ifndef IPARAMETERS_H
#define IPARAMETERS_H

#include <QDomDocument>
#include <QWidget>


/*****************************
 * IParameters
 * ----------------------
 * Classe abstraite dont hritent toutes les classes reprsentant des paramtres 
 * li  une UI et pouvant ouvrir/sauver dans un doc XML
 ******************************/
template<class UI>
class IParameters
{
public:
    virtual void fromUi( void ) = 0;                        //Rcupre les donnes provenant de l'UI
    virtual void fromDomDocument( QDomDocument & ) = 0;     //Rcupre les donnes provenant d'un QDomDocument
    virtual void toUi( void ) = 0;                          //Rempli l'UI
    virtual QDomDocument toDomDocument( void ) = 0;         //Gnre un QDomDocument
    virtual void setUi( UI* ) = 0;                          //Connecte avec l'UI
    virtual unsigned int version( void ) = 0;               //Retourne la version d'EZWG ayant créé le IParameters
private:
    virtual QDomDocument convertFromOldVersion(  const QDomDocument &, const int ) = 0; //Converti un ancien document dans le format actuel
};

#endif // IPARAMETERS_H
