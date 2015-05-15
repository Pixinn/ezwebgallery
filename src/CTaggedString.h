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


#ifndef CTAGGEDSTRING_H
#define CTAGGEDSTRING_H

#include <QString>
#include <QMap>
#include <QFileInfo>

 /*****************************
 * CTags
 * ----------------------
 * QString comprenant un ou plusieur tag  remplacer
 ******************************/
class CTaggedString : public QString
{
    //enumrations publiques
public:
    enum e_previewMode{
        PREVIEW = 1,
        NOPREVIEW = 2
    };

        //Membres statiques publiques
public:     
        //Exif
        static const QString TAGcameraMaker;
        static const QString TAGcameraModel;
        static const QString TAGtimeOriginal;
        static const QString TAGdateOriginal;
        static const QString TAGyearOriginal;
        static const QString TAGfocalLength;
        static const QString TAGfNumber;
        static const QString TAGexposureTime;
        static const QString TAGexposureProgram;
        static const QString TAGflash;
        static const QString TAGisoSpeedRatings;
        //Iptc
        static const QString TAGname;
        static const QString TAGheadline;
        static const QString TAGcaption;
        static const QString TAGcountry;
        static const QString TAGcity;
        static const QString TAGsublocation;
        static const QString TAGkeywords;
        static const QString TAGauthor;
        static const QString TAGcredit;
        static const QString TAGsource;
        static const QString TAGcopyright;
        static const QString TAGcontact;
        //Fichier
        static const QString TAGfileBaseName;
        static const QString TAGfileName;
        //Id
        static const QString TAGphotoNumber;
        //Smileys - Leur prise en compte est activable/dsactivable
        static const QString EMOTICONsmile;
        static const QString EMOTICONlaugh;
        static const QString EMOTICONtongue;
        static const QString EMOTICONsad;
        static const QString EMOTICONcrying;
        static const QString EMOTICONskeptical;
        static const QString EMOTICONkiss;
        static const QString EMOTICONembarassed;
        static const QString EMOTICONevil;
        static const QString EMOTICONlove;
        static const QString EMOTICONsurprise;
        static const QString EMOTICONwink;
        static const QString EMOTICONbad;
        static const QString EMOTICONgood;

        //fonctions publiques
public:
       CTaggedString( );
	   CTaggedString( const CTaggedString & );            //Constructeur par recopie
       CTaggedString( const QString & );                  //Constructeur  partir d'un QString
	   CTaggedString & operator=( const CTaggedString & );//Surcharge de l'oprateut d'assignation
       void setString( const QString & );                 //Fourni le texte
       void setExifTags( const QMap<QString,QString> & ); //Indique les <tags Exifs / metadata>  utiliser
       void setFileInfo( const QFileInfo & );             //Indique les infos fichier  utiliser
       void setId( int id );                              //Indique un id entier
       QString render( int preview = NOPREVIEW ) const;                            //Calcule le QString rsultant du remplacement des tags
       void setEmoticonsEnabled( bool enabled );            //Active/Dsactive la prise en compte des smileys en fonction du boolen

       //fonction prives
private:
        void init();            //Initialisation des membres

	   //METTRE A JOUR operator= en cas de modification !
private:
        QMap<QString,QString> m_tags;
        QFileInfo m_fileInfo;
        int m_id;
        bool m_f_emoticonsEnabled; // = true par dfaut
};



#endif
