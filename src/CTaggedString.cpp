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


#include "CTaggedString.h"
#include "CPhoto.h"
#include "GlobalDefinitions.h"
#include "CPlatform.h"

///////////////////////////////////////////////////////////////////
/////////////////////// CLASSE CExifTags //////////////////////////
///////////////////////////////////////////////////////////////////


//--- Membres statiques
//Exif
const QString CTaggedString::TAGcameraMaker("[EXIF:CameraMaker]");
const QString CTaggedString::TAGcameraModel("[EXIF:CameraModel]");
const QString CTaggedString::TAGtimeOriginal("[EXIF:Time]");
const QString CTaggedString::TAGdateOriginal("[EXIF:Date]");
const QString CTaggedString::TAGyearOriginal("[EXIF:Year]");
const QString CTaggedString::TAGfocalLength("[EXIF:FocalLength]");
const QString CTaggedString::TAGfNumber("[EXIF:FNumber]");
const QString CTaggedString::TAGexposureTime("[EXIF:ExposureTime]");
const QString CTaggedString::TAGexposureProgram("[EXIF:ExposureProgram]");
const QString CTaggedString::TAGflash("[EXIF:Flash]");
const QString CTaggedString::TAGisoSpeedRatings("[EXIF:Iso]");
//Iptc
const QString CTaggedString::TAGcaption("[IPTC:Caption]");
const QString CTaggedString::TAGcountry("[IPTC:Country]");
const QString CTaggedString::TAGcity("[IPTC:City]");
const QString CTaggedString::TAGsublocation("[IPTC:Sublocation]");
const QString CTaggedString::TAGkeywords("[IPTC:Keywords]");
const QString CTaggedString::TAGauthor("[IPTC:Author]");
const QString CTaggedString::TAGcredit("[IPTC:Credit]");
const QString CTaggedString::TAGsource("[IPTC:Source]");
const QString CTaggedString::TAGcopyright("[IPTC:Copyright]");
const QString CTaggedString::TAGcontact("[IPTC:Contact]");
//Fichier
const QString CTaggedString::TAGfileBaseName("[FILE:BaseName]");
const QString CTaggedString::TAGfileName("[FILE:Name]");
//Misc
const QString CTaggedString::TAGphotoNumber("[MISC:PhotoNumber]");
//Smileys - source : http://en.wikipedia.org/wiki/List_of_emoticons
const QString CTaggedString::EMOTICONsmile(":)");
const QString CTaggedString::EMOTICONlaugh(":D");
const QString CTaggedString::EMOTICONtongue(":P");
const QString CTaggedString::EMOTICONsad(":(");
const QString CTaggedString::EMOTICONcrying(":'(");
const QString CTaggedString::EMOTICONskeptical(":/");
const QString CTaggedString::EMOTICONkiss(":*");
const QString CTaggedString::EMOTICONembarassed(":X");
const QString CTaggedString::EMOTICONevil(">:)");
const QString CTaggedString::EMOTICONlove("<3");
const QString CTaggedString::EMOTICONsurprise(":O");
const QString CTaggedString::EMOTICONwink(";)");
const QString CTaggedString::EMOTICONbad("(n)");
const QString CTaggedString::EMOTICONgood("(y)");

/*******************************************************************
* CTaggedString( )
* ---------
* Constructeur par dfaut
********************************************************************/
CTaggedString::CTaggedString( )
    : QString( )
{
    init();
}


/*******************************************************************
* CTaggedString( const QString &other )
* ---------
* Constructeur par recopie d'un QString
* In: (QString) string  recopier
********************************************************************/
CTaggedString::CTaggedString( const QString &other )
    : QString( other )
{
    init();
}


/*******************************************************************
* CTaggedString( const CTaggedString &other )
* ---------
* Constructeur par recopie
* In: (CTaggedString) CTaggedString  recopier
********************************************************************/
CTaggedString::CTaggedString( const CTaggedString &other )
    : QString( )
{
    //Le texte
    const QChar* p_char = other.data( );
    clear();
    while (!p_char->isNull()) {
         append( p_char->unicode() );
         p_char++;
     }
    //Les autres champs
	this->m_tags = other.m_tags;
	this->m_fileInfo = other.m_fileInfo;
	this->m_id = other.m_id;
    this->m_f_emoticonsEnabled = other.m_f_emoticonsEnabled;
}

/*******************************************************************
* CTaggedString( const CTaggedString &other )
* ---------
* Initialisation des membres de la classe
********************************************************************/
void CTaggedString::init()
{
    m_id = -1;
    m_f_emoticonsEnabled = true;
}


/*******************************************************************
* operator =(const CTaggedString &)
* ---------
* Surcharge de l'oprateur par dfaut
********************************************************************/
CTaggedString & CTaggedString::operator=(const CTaggedString &other )
{   
	//Le texte
    const QChar* p_char = other.data( );
    clear();
    while (!p_char->isNull()) {
         append( p_char->unicode() );
         p_char++;
     }
	//Les autres champs
	this->m_tags = other.m_tags;
	this->m_fileInfo = other.m_fileInfo;
	this->m_id = other.m_id;
    this->m_f_emoticonsEnabled = other.m_f_emoticonsEnabled;

    return *this;
}

/*******************************************************************
* void setString( const QString & );           
* ---------
* Fourni le texte de la string tagge
* In : (QMap<QString,QString>) tag exifs
********************************************************************/
void CTaggedString::setString( const QString &string )
{   
    this->clear();
    if( !string.isEmpty() )
    {
        const QChar* data = string.constData();
        QChar charac = *data++;
        //Remplacement des caractres par ceux du QString    
        while( charac != QChar('\0') ){
            this->append( charac );
            charac = *data++;
        }
    }
}


/*******************************************************************
* setExifTags( const QMap<QString,QString> & )
* ---------
* Indique les <tags Exifs / metadata>  utiliser
* In : (QMap<QString,QString>) tag exifs
********************************************************************/
void CTaggedString::setExifTags( const QMap<QString,QString> & exifTags )
{   
    m_tags = exifTags;
}

/*******************************************************************
* setFileInfo( const QQFileInfo & )
* ---------
* Indique le fileinfo  utiliser
* In : (QFileInfo) file info
********************************************************************/
void CTaggedString::setFileInfo( const QFileInfo & fileInfo )
{
    m_fileInfo = fileInfo;
}


/*******************************************************************
* setId( int )
* ---------
* Indique l'id de la photo
* In : (int) id
********************************************************************/
void CTaggedString::setId( int id )
{
    m_id = id;
}


/*******************************************************************
* render( int preview = NOPREVIEW )
* ---------
* Calcule la chane de caractre avec tous les tags remplacs par les donnes correspondantes
* In :(int) preview : indique si il s'agit d'un render pour une preview ou non (NOPREVIOU par dfaut)
* return : le QString rsultant
********************************************************************/
QString CTaggedString::render( int preview ) const
{
    QString rendered = *this;
    
    //Exif
    foreach( QString tag, m_tags.keys() ) {
        rendered = rendered.replace( tag, m_tags.value( tag ) );
    }
    //Fichier
    rendered = rendered.replace( TAGfileBaseName, m_fileInfo.baseName() );
    rendered = rendered.replace( TAGfileName, m_fileInfo.fileName() );    
    //Misc
    rendered = rendered.replace( TAGphotoNumber, QString::number( m_id ) );

    //Prise en compte des smileys
    if( m_f_emoticonsEnabled )
    {
        QString imgOpening;
        QString imgClosing( "'/>" );
        //Le chemin vers les smileys est diffrent si il s'agit d'une preview ou non
        if( preview == PREVIEW ){
            imgOpening = QString( "<img src=':/smileys/" );
        }
        else{
            imgOpening = QString( "<img src='" ) + QString( RESEMOTICONSPATH );
        }
        rendered = rendered.replace( EMOTICONskeptical, imgOpening + QString("skeptical.gif") + imgClosing, Qt::CaseInsensitive ); //skeptical doit tre en PREMIER (sinon pb en mode previw car "c:/" ...
        rendered = rendered.replace( EMOTICONevil, imgOpening + QString("evil.gif") + imgClosing, Qt::CaseInsensitive ); //evil DOIT tre avant smile !
        rendered = rendered.replace( EMOTICONsmile, imgOpening + QString("smile.gif") + imgClosing, Qt::CaseInsensitive );
        rendered = rendered.replace( EMOTICONlaugh, imgOpening + QString("laugh.gif") + imgClosing, Qt::CaseInsensitive );
        rendered = rendered.replace( EMOTICONtongue, imgOpening + QString("tongue.gif") + imgClosing, Qt::CaseInsensitive );
        rendered = rendered.replace( EMOTICONsad, imgOpening + QString("sad.gif") + imgClosing, Qt::CaseInsensitive );
        rendered = rendered.replace( EMOTICONcrying, imgOpening + QString("crying.gif") + imgClosing, Qt::CaseInsensitive );        
        rendered = rendered.replace( EMOTICONkiss, imgOpening + QString("kiss.gif") + imgClosing, Qt::CaseInsensitive );
        rendered = rendered.replace( EMOTICONembarassed, imgOpening + QString("embarassed.gif") + imgClosing, Qt::CaseInsensitive );        
        rendered = rendered.replace( EMOTICONlove, imgOpening + QString("love.gif") + imgClosing, Qt::CaseInsensitive );
        rendered = rendered.replace( EMOTICONsurprise, imgOpening + QString("surprise.gif") + imgClosing, Qt::CaseInsensitive );
        rendered = rendered.replace( EMOTICONwink, imgOpening + QString("wink.gif") + imgClosing, Qt::CaseInsensitive );
        rendered = rendered.replace( EMOTICONbad, imgOpening + QString("bad.gif") + imgClosing, Qt::CaseInsensitive );
        rendered = rendered.replace( EMOTICONgood, imgOpening + QString("good.gif") + imgClosing, Qt::CaseInsensitive );
    }

    return rendered;
}


/*******************************************************************
* setEmoticonsEnabled( bool enabled )
* ---------
* Active/Dsactive la prise en compte des smileys en fonction du boolen pass en paramtre
* In : (bool) activation des smileys
********************************************************************/
void CTaggedString::setEmoticonsEnabled( bool enabled )
{
    m_f_emoticonsEnabled = enabled;
}

