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



#ifndef CCSS_H
#define CCSS_H
 
#include <QString>
#include <QTextStream>
#include <QMap>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>

#define STYLESHEET_TAG_NAME "StyleSheet"
#define SELECTION_TAG_NAME  "Selection"

/****************************
* CLASS CCssSelection
*****************************/
class CCssSelection
{    
public:
    CCssSelection( );
    CCssSelection( const QString &selection );      //Cr une slection  partir d'un slecteur textuel
    CCssSelection( const QDomElement &domElement ); //Cr une slection  partir d'un QDomElement la reprsentant
    bool operator!=( const CCssSelection & );
    CCssSelection subSelection( QStringList selectors );    //Rcupre une sous-slection
    void addSubSelection( CCssSelection &subSelection ); 		//Ajoute une "sous-slection" (ie: .selection #subselection)
    QMap<QString,CCssSelection> selections( ); 				//Rcupre la liste de sous-slections
    QString selector( );                                //Renvoie le slecteur
    void setProperty( const QString &property, const QString &value ); 	//Ajoute une proprit et sa valeur associe
    QString property( const QString &property );  			//Renvoie la valeur d'une proprit
    QMap<QString,QString> properties( );                                //Renvoie toutes les proprits
    QString toString( );						//Retourne une chaine de caractres compatible css des proprits de la slection
    QDomElement toDomElement( QDomDocument & );         //Converti la slection en QDomElement
    bool isNull();                                  //Renvoie true si il n'y a pas de sous-slection ni de proprit
protected:
    void setSelector( const QString &);                  //Met  jour le slecteur
    void setParentFullSelector( const QString &);        //Met  jour le chemin complet vers le parent si il y en a

private:
    QString m_selector;
    QString m_parentFullSelector;
    QMap<QString,CCssSelection> m_subSelections;    //selector ; selection
    QMap<QString,QString> m_properties;             //Name ; Value
};



/****************************
* CLASS CCssSheet
*****************************/
class CCssSheet
{
public:
    CCssSheet( QString name = QString("unnamed") );
    CCssSheet( const QDomElement &domElement ); //Cr une feuille de style  partir d'un QDomElement la reprsentant
    bool operator==( const CCssSheet& );
    bool operator!=( const CCssSheet& );
    void clear( );                              //Efface le contenu de la feuille
    CCssSelection selection( QStringList selectors ) const; //Rcupre une slection de la feuille de style
    void addSelection( CCssSelection &selection );
    QMap<QString,CCssSelection> selections( );
    QString name( );
    void toStream( QTextStream & stream );
    QDomElement toDomElement( QDomDocument & ) const;         //Converti la feuille de style en QDomElement
    
private:
    QString m_name;
    QMap<QString,CCssSelection> m_selections; //selector ; selection
};

#endif // CCSS_H
