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



#ifndef CPHOTO_H
#define CPHOTO_H

#include <QRunnable> //Ncessaire sinon warnings des classes de Magick++ ???
#include <QIODevice>
#include <QString>
#include <QFont>
#include <QColor>
#include <QSize>

#include <Magick++.h>

#include <cstring>

#include "CError.h"
#include "CProjectParameters.h"
#include "CTaggedString.h"


 /*****************************
 * CMagick
 * ----------------------
 * Classe de base pour encapsuler ImageMagick et interragir un peu plus  la manire de QImage
 *****************************/
class CMagick : public Magick::Image
{
public:
    CMagick(  );
    CMagick( const QString & filename );   //Cration  partir d'un fichier
    bool load( const QString & fileName ); //Chargement d'une image
    bool save( const QString & fileName, const int quality = -1); //Sauvegarde d'une image
    QString error( );                      //Retourne la dernire erreur survenue
    QStringList errors( );                 //Retourne la liste de toutes les erreurs survenues

protected:
    QStringList m_errors;
};

/*****************************
 * CWatermark
 * ----------------------
 * Image de Magick++ ne semble pas de setter correctement l'opacit.
 * L'opacit de la couche Alpha prend directement la valeur passe en paramtre, au lieu de devenir plus faible.
 * Seuls les pixels opaques sont modifis correctement...
 * De plus, dans le rfrentiel ImageMagick opacity veut dire "transparence". ie: opacity = 0 donne... 100% opaque.
 * Je prfre que opacity = opacit.
 * --> Surcharge de la fonction opacity() pour correction
 *****************************/
 
class CWatermark :  public CMagick
{
    public: 
        static const QString ERROR_UNINITIALIZED;        
        static const QString ERROR_STRINGCLEARED;        

    public:
        CWatermark( );
        void setImage( const QString &watermarkImagePath );
        void setTaggedString( CTaggedString inText, QFont inFont, QColor inColor );
        void setOpacity( const unsigned int opacity );         //Rgler l'opacit
        bool isValid( );
        void setParameters( const t_watermark &parameters );
        t_watermark parameters( );

	private:
		bool fromQImage( const QImage& qimage );

    private:
        static const int TEXTWIDTH;                         //Largeur du filigrane textuel calcul.

        bool m_isValid;
        t_watermark m_parameters;
};


 /*****************************
 * CPhoto
 * ----------------------
 * Image tendue afin de pouvoir facilement tre watermarke
 ******************************/
 
class CPhoto : public CMagick
{
    public:
        CPhoto( );
        bool load( const QString & fileName ); //Chargement d'une photo
        bool watermark( CWatermark watermarkImage,   //Copie car sera modifi
                        int position, int orientation,
                        int relativeSize, int opacity );
        void zoom( const QSize &size, Qt::TransformationMode filter = Qt::SmoothTransformation );             //Surchage d'Image::zoom Qt style
        double meanLightness( const QRect &rect );     //Cacule la luminosit moyenne dans un rectangle dfini
        bool orientationExif( );                    //Orientation automatique en fonction de l'exif
        void removeMetadata( );                    //Enlve les mtadonnes d'une photo
        QSize size( );                              //Renvoie la taille de l'image
        QImage qimage( );                           //Conversion en QImage. Assez lent !        
        QMap<QString,QString> exifTags( );          //Slection de tags exifs intressants

    private:
        void readExifTags( );
        QString rationalToDoubleStr( const std::string &rational );  //Converti un tag exif "rationel" en double

    private:
        QMap<QString,QString> m_exifTags;

        //Membres privs statiques
    private:
        static const QString EXIFcameraMaker;
        static const QString EXIFcameraModel;
        static const QString EXIFcopyright;
        static const QString EXIFdateTimeOriginal;
        static const QString EXIFfocalLength;
        static const QString EXIFfNumber;
        static const QString EXIFexposureTime;
        static const QString EXIFexposureProgram;
        static const QString EXIFflash;
        static const QString EXIFisoSpeedRatings;
};



#endif // CPHOTO_H
