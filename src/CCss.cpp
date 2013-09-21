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



#include "CCss.h"

#define STYLESHEET_NAME_ATTR_NAME "name"
#define SELECTION_SELECTOR_ATTR_NAME  "selector"

////////////////////////////////////////
////////////// CCFssSheet ///////////////
////////////////////////////////////////

CCssSheet::CCssSheet( const QString name )
{ 
    m_name = name;
}

//Constructeur: a partir d'un QDomElement reprsentant la feuille de style
CCssSheet::CCssSheet( const QDomElement &domElement )
{ 
    //Nom
    m_name = domElement.attribute( STYLESHEET_NAME_ATTR_NAME, "UNDEFINED" );
    
    //On parcourt tous les sous-noeuds XML et on ajoute les slections  la feuille
    QDomNode node = domElement.firstChild();
    while( !node.isNull() )
    {
        //Seuls les lments nous intressent
        if( node.isElement() )
        {
            QDomElement element = node.toElement();
            //Si c'est une feuille on la converti en CSS
            if( element.tagName() == SELECTION_TAG_NAME ){
                CCssSelection subSelection = CCssSelection( element );
                this->addSelection( subSelection );
            }
        }
        node = node.nextSibling();
    }
}

bool CCssSheet::operator ==( const CCssSheet &ref )
{
    foreach( QString key, m_selections.keys() ){
        CCssSelection refSel = ref.m_selections.value( key );
        CCssSelection sel = m_selections.value( key );
        if( refSel != sel ){
            return false;
        }
    }
    return( m_name == ref.m_name );
}


bool CCssSheet::operator !=( const CCssSheet &ref )
{
    if( *this == ref ){
        return false;
    }
    else{
        return true;
    }
}


//Efface le contenu de la feuille
void CCssSheet::clear( )
{
    m_selections.clear();
}


//Rcupration d'une slection. Les slecteurs pass en paramtre sont une liste *ordonne*
//donnant le "chemin" jusqu' la slection
CCssSelection CCssSheet::selection( QStringList selectors )
{
    CCssSelection selection;

    if( !selectors.isEmpty() ){
        QString select = selectors.takeFirst();
        if( m_selections.contains( select ) ){
            selection = m_selections.value( select );
            if( !selectors.isEmpty() ){ //Il reste encore du "chemin"  parcourir
                selection = selection.subSelection( selectors );
            }
        }
    }
    return selection;
}

void CCssSheet::addSelection( CCssSelection &selection )
{
	m_selections.insert( selection.selector(), selection );
}


QMap<QString,CCssSelection> CCssSheet::selections( )
{
	return m_selections;
}

QString CCssSheet::name( )
{
    return m_name;
}

//Convertir en QDomElement
QDomElement CCssSheet::toDomElement( QDomDocument & doc )
{
    QDomElement element = doc.createElement( QString( STYLESHEET_TAG_NAME ) );
    element.setAttribute( QString(STYLESHEET_NAME_ATTR_NAME), m_name );
    //Ajout de tous les sous-lments issus des slecteurs
    foreach( QString selector, m_selections.keys() ){
        CCssSelection sel = m_selections.value( selector );
        QDomElement subElement = sel.toDomElement( doc );
        element.appendChild( subElement );
    }
    
    return element;
}

void CCssSheet::toStream( QTextStream & stream )
{
    foreach( QString selector, m_selections.keys() ){
        CCssSelection sel = m_selections.value( selector );
    	stream << sel.toString();
    }
}



//////////////////////////////////////////////
////////////// CCssSelection /////////////////
/////////////////////////////////////////////

//Constructeurs
CCssSelection::CCssSelection( )
{
    m_selector = QString("undefined");
}

CCssSelection::CCssSelection( const QString &selection )
{
	m_selector = selection;
}

//Constructeur: cr une slection  partir d'un QDomElement la reprsentant
CCssSelection::CCssSelection( const QDomElement &domElement )
{
    //Selector
    m_selector = domElement.attribute( SELECTION_SELECTOR_ATTR_NAME, "UNDEFINED" );

    //On parcourt tous les sous-noeuds XML.
    QDomNode node = domElement.firstChild();
    while( !node.isNull() )
    {
        //Seuls les lments nous intressent
        if( node.isElement() )
        {
            QDomElement element = node.toElement();
            //Si c'est un sous-slecteur, on le converti en CSS -> rcursivit
            if( element.tagName() == SELECTION_TAG_NAME ){
                CCssSelection subSelection = CCssSelection( element );
                this->addSubSelection( subSelection );
            }
            else //Sinon c'est une proprit
            {
                QDomNode propertie = element.firstChild();
                if( propertie.isText() ){
                    this->setProperty( element.tagName(), propertie.toText().data() );
                }
            }
        }
        node = node.nextSibling();
    }
    
}


bool CCssSelection::operator !=( const CCssSelection & ref)
{
    foreach( QString key, m_subSelections.keys() ){
        CCssSelection refSel = ref.m_subSelections.value( key );
        CCssSelection sel = m_subSelections.value( key );
        if( refSel != sel ){
            return true;
        }
    }

    return(     this->m_parentFullSelector != ref.m_parentFullSelector
            ||  this->m_properties != ref.m_properties
            ||  this->m_selector != ref.m_selector
          );
}

//Rcupre une sous-slection. Le "chemin complet" est pass via une liste *ordonne*
CCssSelection CCssSelection::subSelection( QStringList selectors )
{
    CCssSelection subSelection;

    if( !selectors.isEmpty() ){
        QString select = selectors.takeFirst();
        if( m_subSelections.contains( select ) ){
            subSelection = m_subSelections.value( select );
            if( !selectors.isEmpty() ){ //Il reste encore du "chemin"  parcourir
                subSelection = subSelection.subSelection( selectors );
            }
        }
    }

    return subSelection;
}

//Ajoute une "sous-slection" (ie: .selection #subselection)
void CCssSelection::addSubSelection( CCssSelection &subSelection )
{
    subSelection.setParentFullSelector( m_parentFullSelector + m_selector );
	m_subSelections.insert( subSelection.selector(), subSelection );        
}

//Rcupre la liste de sous-lments			
QMap<QString,CCssSelection> CCssSelection::selections( )
{
	return m_subSelections;
}

QString CCssSelection::selector( )
{
    return m_selector;
}

void CCssSelection::setSelector( const QString &selector)
{
    m_selector = selector;
}

void CCssSelection::setParentFullSelector( const QString &path )
{
    m_parentFullSelector = path;
    if( !m_parentFullSelector.isEmpty() ){ //Pour ne pas ajouter plein de tabulation...
        m_parentFullSelector = m_parentFullSelector + "\t";
    }
    //On propage le "path" vers toutes les sous-slections
    foreach( QString key, m_subSelections.keys() ){
        CCssSelection child = m_subSelections.value(key);
        child.setParentFullSelector( m_parentFullSelector + m_selector );
        m_subSelections.insert( key, child );
    }
}

//Ajoute une proprit et sa valeur associe  la la slection
void CCssSelection::setProperty( const QString &property, const QString &value )
{
	m_properties.insert( property, value );
}

//Renvoie la valeur d'une proprit
QString CCssSelection::property( const QString &property )
{
	return m_properties.value( property );
}

//Renvoie les proprits
QMap<QString,QString> CCssSelection::properties( )
{
	return m_properties;
}

/*****************************
* QString toDomElement( )
* ----------
* Convertir la slection en QDomElement XML valide
******************************/
QDomElement CCssSelection::toDomElement( QDomDocument & doc )
{
    QDomElement element = doc.createElement( SELECTION_TAG_NAME );
    //Attribus
    element.setAttribute( SELECTION_SELECTOR_ATTR_NAME, m_selector );
    //Proprits
    foreach( QString key, m_properties.keys() ){        
        QDomElement propertyElement = doc.createElement(key);
        propertyElement.appendChild( doc.createTextNode(  m_properties.value(key) ) );
        element.appendChild( propertyElement );
    }    
    //Sous-lments
    foreach( QString selection, m_subSelections.keys() ){
        CCssSelection sel = m_subSelections.value( selection );
        QDomElement subElement = sel.toDomElement( doc );
        element.appendChild( subElement );
    }
    
    return element;
}

/*****************************
* QString toString( )
* ----------
* Converti la slection en string CSS valide
******************************/
QString CCssSelection::toString( )
{
    QString ccsString;

    //Proprits de la slection
    ccsString = m_parentFullSelector + m_selector + " {\n";
    foreach( QString key, m_properties.keys() ){
        ccsString.append("\t" + key + ":\t" + m_properties.value(key) + ";\n");
    }
    ccsString.append("}\n");
    //Sous-slections (rcursivit)
    foreach( QString selection, m_subSelections.keys() ){
       CCssSelection sel = m_subSelections.value( selection );
       ccsString.append( sel.toString( ) );
    }
    ccsString.append("\n");

    return ccsString;
}

/*****************************
* QString isNull( )
* ----------
* Renvoie true si il n'y a pas de sous-slection ni de proprit
******************************/
bool CCssSelection::isNull()
{
    if( m_subSelections.size() == 0 && m_properties.size() == 0 ){
        return true;
    }
    else{
        return false;
    }
}
