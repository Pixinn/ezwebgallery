﻿/* 
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


#include <QImage>
#include <QRect>
#include <QPainter>
#include <QPen>
#include <QFile>
#include <QFontMetrics>
#include <QBuffer>
#include <QByteArray>

#include <cstdlib>
#include <iostream>

#include "CPhoto.h"
#include "CTaggedString.h"
#include "CLogger.h"
#include "CWarning.h"
#include "CError.h"

using namespace std;
using namespace Magick;

//////////////////// DEFINITIONS ////////////////
#define QIMAGETOMAGICKFORMAT    "png"           /* Format pour convertion entre QImage et Image */

///////////////////////////////////////////////////////////////////
/////////////// CLASSE CMAGICK ///////////////////
///////////////////////////////////////////////////////////////////


/*******************************************************************
* CMagick( )
* ---------
* Constructeur par dfaut
********************************************************************/
CMagick::CMagick()
{

}

/*******************************************************************
* load( const QString & fileName )
* ---------
* Charge une image via le bom de fichier fourni
* Revoie true si pas de problme, false sinon
********************************************************************/
bool CMagick::load( const QString & fileName )
{
    bool f_success = true;
   
    //On n'ouvre l'image que si le fichier existe
    QFile imageFile( fileName );
    if( imageFile.exists() /*&& imageFile.isReadable()*/ ) //isReadable() retourne false par dfaut en NTFS
    {
        //--- Ouverture de l'image    
        string filenameUtf8 = string( fileName.toUtf8().constData() );
        try{
            this->read( filenameUtf8 );		
        }
        catch( Magick::Error &error ){
            CLogger::getInstance().log( PtrMessage( new CError(QObject::tr("Caught a Magick error: "), error.what() ) ) );
            m_errors << QString( error.what() );
            f_success = false;
        }
        catch( Magick::Warning &warning ){
            CLogger::getInstance().log( PtrMessage( new CWarning(QObject::tr("Caught a Magick warning: "), warning.what() ) ) );
            m_errors << QString( warning.what() );
        }
        catch( std::exception &error ) { // Process any other exceptions derived from standard C++ exception
            CLogger::getInstance().log( PtrMessage( new CError(QObject::tr("Caught a C++ STD exception: "), error.what() ) ) );
            m_errors << QString( error.what() );
            f_success = false;
        } 
    }
    else //File doesn't exist or cannot be read
    {
        m_errors << QObject::tr("File doesn't exist");
        CLogger::getInstance().log( PtrMessage(new CError(m_errors.last(), fileName)) );        
        f_success = false;
    }

    return f_success;
}


/*******************************************************************
* load( const QString & fileName )
* ---------
* Charge une image via le bom de fichier fourni
* Revoie true si pas de problme, false sinon
********************************************************************/
bool CMagick::save(  const QString & fileToWrite, const int quality )
{
    bool f_success;

    this->quality( (size_t)quality );

    string filenameUtf8( fileToWrite.toUtf8().constData() );
    try{
        f_success = true;
        this->write( filenameUtf8 );
    }
    catch( Magick::Error &error ){
        CLogger::getInstance().log( PtrMessage( new CError(QObject::tr("Caught a Magick error: "), error.what() ) ) );
        m_errors << QString( error.what() );
        f_success = false;
    }
    catch( Magick::Warning &warning ){
        CLogger::getInstance().log( PtrMessage( new CWarning(QObject::tr("Caught a Magick warning: "), warning.what() ) ) );
        m_errors << QString( warning.what() );
    }
    catch( std::exception &error ) { // Process any other exceptions derived from standard C++ exception
        CLogger::getInstance().log( PtrMessage( new CError(QObject::tr("Caught a C++ STD exception: "), error.what() ) ) );
        m_errors << QString( error.what() );
        f_success = false;
    } 


    return f_success;
}


/*******************************************************************
* error(  )
* ---------
* Retourne la dernire erreur survenue
********************************************************************/
QString CMagick::error()
{
    if( !m_errors.isEmpty() ){
        return m_errors.last();
    }
    else{
        return QString();
    }
}


/*******************************************************************
* errors(  )
* ---------
* Retourne la liste de toutes les erreurs survenues
********************************************************************/
QStringList CMagick::errors()
{
    return m_errors;
}



///////////////////////////////////////////////////////////////////
/////////////// CLASSE CWatermark ///////////////////
///////////////////////////////////////////////////////////////////

const int CWatermark::TEXTWIDTH = 1024;     //Filgranne textuel calcul  cette taille.
                                            //Si besoin de plus grans, il sera interpol.
const QString CWatermark::ERROR_UNINITIALIZED = QObject::tr("Watermark not initialized.");
const QString CWatermark::ERROR_STRINGCLEARED = QObject::tr("Watermark string is cleared.");


/*******************************************************************
* CWaterMark( )
* ---------
* Constructeur par dfaut
********************************************************************/
CWatermark::CWatermark(  )
{
    m_isValid = false;
}





bool CWatermark::fromQImage( const QImage &qimage )
{   
    bool f_converted;

    //Sauvegarde du QImage dans un buffer de donnes
    QByteArray qimageData;
    QBuffer qimageBuffer(&qimageData);
    qimageBuffer.open(QIODevice::WriteOnly);
    qimage.save(&qimageBuffer, QIMAGETOMAGICKFORMAT);
    
	//Lecture du buffer de donnes
    try{
        Blob newImageBlob(qimageData.data(),qimageData.size());
        this->read(newImageBlob);
        f_converted = true;
    }
    catch( Magick::Error &error ){
        f_converted = false;
        m_errors << QString( error.what() );
    }

    return f_converted;
}



/*******************************************************************
* setImage( const QString &watermarkImagePath )
* ---------
* Indique une image  utiliser. Fait passer le watermark en type IMAGE
* In : (QString) chemin vers l'image  utiliser
********************************************************************/
void CWatermark::setImage( const QString &watermarkImagePath )
{
    if ( load( watermarkImagePath ) ){
        m_isValid = true;
    }
    else{
        m_isValid = false;
        m_parameters.type = t_watermark::IMAGE;
    }
}

/*******************************************************************
* setTaggedString( const CTaggedString &inText, QFont inFont, QColor inColor )
* ---------
* Indique une CTaggedString  utiliser. Fait passer le watermark en type TEXT
* In : (CTaggedString) le texte du filigrane
* In : (QFont) la police  utiliser
* In : (QColor) la couleur du texte
********************************************************************/
void CWatermark::setTaggedString( CTaggedString inText, QFont inFont, QColor inColor )
{
    QImage image;
    QFont newFont =  inFont;
    qreal scaleFactor;
    const qreal startingPointSize = 20.0;
    QRect textBoundingRect;

    m_isValid = false;
    inText.setEmoticonsEnabled( false );

    if( inText.isEmpty() ){
        m_errors << ERROR_STRINGCLEARED;
        return;
    }

    //1ere itration "en aveugle"
    inFont.setPointSizeF( startingPointSize );
    QFontMetrics fontMetrics(inFont);

    //Mise  l'échelle de la police
    scaleFactor = (qreal)CWatermark::TEXTWIDTH / (qreal)fontMetrics.width( inText );
    newFont.setPointSizeF( (startingPointSize * scaleFactor) );
    fontMetrics = QFontMetrics( newFont );
    textBoundingRect = fontMetrics.boundingRect( inText.render() );

    //Création de l'image avec le texte  la bonne taille
    image = QImage( textBoundingRect.size(), QImage::Format_ARGB32 );
    image.fill( Qt::transparent );
    QPainter painter( &image );
    painter.setFont( newFont );
    painter.setPen( QPen( inColor ) );
    QRect rect( 0, 0, image.width(), image.height() );
    painter.drawText( rect, Qt::AlignCenter | Qt::AlignVCenter, inText.render() );

    //--- Rcupration par la classe drive de Magick::Image
    if( this->fromQImage( image ) ){
        m_isValid = true;
        m_parameters.type = t_watermark::TEXT;
    }
}



/*******************************************************************
* setOpacity( const unsigned int opacity )
* ---------
* Change l'opacit du watermark
* In : Opacit dsire
********************************************************************/
void CWatermark::setOpacity( const unsigned int opacity )
{
    PixelPacket* pixel;
    size_t nbCols;
    size_t nbRows;
    
    if( opacity != 100 )
    {
        try{
            this->modifyImage( );
            this->matte( MagickTrue );  //A appeler imprativement avant de toucher  l'opacit des pixels.
                                        //sinon, l'alpha channel rique d'tre ignor !
    
            //Rcupration des pixels de l'image
            nbCols = this->size().width();
            nbRows = this->size().height();
            pixel = this->getPixels( 0, 0, nbCols , nbRows ); //Rcupration d'une COPIE des pixels
        
            //Changement de l'opacit
            for( int col = 0; col < nbCols; col++ ) {
                for( int row = 0; row < nbRows; row++ ){
                    //NB: pixel->opacity reprsente en ralit la TRANSPARENCE. ie: 0 = opaque...
                    Quantum pixelOpacity;
                    Quantum newPixelOpacity;
                    pixelOpacity = QuantumRange - pixel->opacity;
                    newPixelOpacity = ( pixelOpacity * opacity ) / 100; //Un peu lourd comme calcul...
                    pixel->opacity = QuantumRange - newPixelOpacity;
                    pixel++;
                }
            }

            //Mise  jour de l'image
            this->syncPixels();
        }
        catch( Magick::Error &error ){
            m_errors << QString( error.what() );
        }
    }

}


/*******************************************************************
* isValid( )
* ---------
* Return : Valide ?
********************************************************************/
bool CWatermark::isValid( )
{
   return m_isValid; 
}



/*******************************************************************
* setParameters( )
* ---------
* Met  jour les paramtres
********************************************************************/
void CWatermark::setParameters( const t_watermark &parameters )
{
    m_parameters = parameters;
}


/*******************************************************************
* parameters( )
* ---------
* Return : paramtres
********************************************************************/
t_watermark CWatermark::parameters( )
{
    return m_parameters;
}






///////////////////////////////////////////////////////////////////
/////////////// CLASSE CPhoto ///////////////////
///////////////////////////////////////////////////////////////////


//--- Membres statiques
//For EXIF Tags, see http://www.imagemagick.org/script/escape.php and http://www.sno.phy.queensu.ca/~phil/exiftool/TagNames/EXIF.html
const QString CPhoto::EXIFcameraMaker("EXIF:Make");
const QString CPhoto::EXIFcameraModel("EXIF:Model");
const QString CPhoto::EXIFdateTimeOriginal("EXIF:DateTimeOriginal");
const QString CPhoto::EXIFfocalLength("EXIF:FocalLength");
const QString CPhoto::EXIFfNumber("EXIF:FNumber");
const QString CPhoto::EXIFexposureTime("EXIF:ExposureTime");
const QString CPhoto::EXIFexposureProgram("EXIF:ExposureProgram");
const QString CPhoto::EXIFflash("EXIF:Flash");
const QString CPhoto::EXIFisoSpeedRatings("EXIF:ISOSpeedRatings");

//For IPTC Code, see http://www.imagemagick.org/script/escape.php and http://www.sno.phy.queensu.ca/~phil/exiftool/TagNames/IPTC.html
const QString CPhoto::TAGname( "IPTC:2:05" );
const QString CPhoto::TAGheadline( "IPTC:2:105" );
const QString CPhoto::TAGcaption("IPTC:2:120");
const QString CPhoto::TAGcountry("IPTC:2:101");
const QString CPhoto::TAGcity("IPTC:2:90");
const QString CPhoto::TAGsublocation("IPTC:2:92");
const QString CPhoto::TAGkeywords("IPTC:2:25");
const QString CPhoto::TAGauthor("IPTC:2:80");
const QString CPhoto::TAGcredit("IPTC:2:110");
const QString CPhoto::TAGsource("IPTC:2:115");
const QString CPhoto::TAGcopyright("IPTC:2:116");
const QString CPhoto::TAGcontact("IPTC:2:118");


/*******************************************************************
* CPhoto( )
* ---------
* Constructeur par dfaut
********************************************************************/
CPhoto::CPhoto()
{

}


/*******************************************************************
* load( const QString & fileName  )
* ---------
* Charge une photo
* In : (QString) filename - nom de la photo
* Return : (bool) true sur succs, false si echec
********************************************************************/
bool CPhoto::load( const QString &fileName  )
{
    bool returnValue;

    if( CMagick::load( fileName ) ) {
        readExifTags( );
        returnValue = true;
    }
    else {
        returnValue = false;
    }

    return returnValue;
}



/*******************************************************************
* watermark(CWatermark watermarkImage, int position, int orientation, int targetRelativeSize, int opacity)
* ---------
* Applique un watermark  la photo
* In : (int)position - position du watermak, dfini par l'numration t_watermark::e_position
* In : (int)orientation - orientation du watermark, dfini par m'numration t_watermark::e_rotation
* In : (int)targetRelativeSize - taille relative du grand cot du watermark par rapport au grand cot de la photo. En %
* In : (int)opacity - opacit du watermark. 0 = transparent
* Return : true
********************************************************************/
bool CPhoto::watermark(CWatermark watermarkImage, int position, int orientation, int targetRelativeSize, int opacity)
{
    bool f_success;
    Geometry watermarkSize;
    size_t watermarkLength;
    size_t newWatermarkWidth;
    size_t newWatermarkHeight;
    QRect watermarkRectangle;
    Geometry photoSize;
    size_t photoLength;
    GravityType gravity;
    qreal scaleFactor;
    qreal realRelativeSize;
//  string filenameLocal8;
    double rotation;
    double meanLuma;
    
    //--- Opacit
    watermarkImage.setOpacity( opacity );

    //--- Rotation
    try{
        switch( orientation )
        {
        case t_watermark::ROTATION_CCW_90:
            rotation = -90.0;
            break;
        case t_watermark::ROTATION_CW_90:
            rotation = 90.0;       
            break;
        default:
            rotation = 0.0;
            break;
        }
        watermarkImage.rotate( rotation );
     
        //--- Resize
        watermarkSize = watermarkImage.size( );
        watermarkLength = max( watermarkSize.width(), watermarkSize.height() );
        photoSize = Image::size( );
        photoLength = max( photoSize.width(), photoSize.height() );
        realRelativeSize = (qreal)watermarkLength / (qreal)photoLength;
        scaleFactor = ( (qreal)targetRelativeSize / 100.0 ) / realRelativeSize; //Taille relative souhaite (%) / taille relative relle
        newWatermarkWidth = ( watermarkSize.width() * scaleFactor );
        newWatermarkHeight = ( watermarkSize.height() * scaleFactor );
        watermarkImage.zoom( Geometry( newWatermarkWidth, newWatermarkHeight ) ); //zoom ne garde les proportions et ne dforme pas


        //--- Composition et calcul du rectangle dans lequel sera appos le watremark
        switch( position )
        {
        case t_watermark::SOUTHEAST:
            gravity = SouthEastGravity;
            watermarkRectangle = QRect(static_cast<int>(photoSize.width() - newWatermarkWidth), static_cast<int>(photoSize.height() - newWatermarkHeight),
                static_cast<int>(newWatermarkWidth), static_cast<int>(newWatermarkHeight));
            break;
        case t_watermark::SOUTH:
            gravity = SouthGravity;
            watermarkRectangle = QRect(static_cast<int>(photoSize.width() / 2u - newWatermarkWidth / 2u), static_cast<int>(photoSize.height() - newWatermarkHeight),
                static_cast<int>(newWatermarkWidth), static_cast<int>(newWatermarkHeight));
            break;
        case t_watermark::SOUTHWEST:
            gravity = SouthWestGravity;
            watermarkRectangle = QRect(0u, static_cast<int>(photoSize.height() - newWatermarkHeight),
                static_cast<int>(newWatermarkWidth), static_cast<int>(newWatermarkHeight));
            break;
        case t_watermark::WEST:
            gravity = WestGravity;
            watermarkRectangle = QRect(0u, static_cast<int>(photoSize.height() / 2u - newWatermarkHeight / 2u),
                static_cast<int>(newWatermarkWidth), static_cast<int>(newWatermarkHeight));
            break;
        case t_watermark::NORTHWEST:
            gravity = NorthWestGravity;
             watermarkRectangle = QRect( 0u, 0u,
                 static_cast<int>(newWatermarkWidth), static_cast<int>(newWatermarkHeight));
            break;
        case t_watermark::NORTH:
            gravity = NorthGravity;
            watermarkRectangle = QRect(static_cast<int>(photoSize.width() / 2 - newWatermarkWidth / 2u), 0u,
                static_cast<int>(newWatermarkWidth), static_cast<int>(newWatermarkHeight));
            break;
        case t_watermark::NORTHEAST:
            gravity = NorthEastGravity;
            watermarkRectangle = QRect(static_cast<int>(photoSize.width() - newWatermarkWidth), 0u,
                static_cast<int>(newWatermarkWidth), static_cast<int>(newWatermarkHeight));
            break;
        case t_watermark::EAST:
            gravity = EastGravity;
            watermarkRectangle = QRect(static_cast<int>(photoSize.width() - newWatermarkWidth), static_cast<int>(photoSize.height() / 2u - newWatermarkHeight / 2u),
                static_cast<int>(newWatermarkWidth), static_cast<int>(newWatermarkHeight));
            break;
        default:
            gravity = CenterGravity;
            watermarkRectangle = QRect(static_cast<int>(photoSize.width() / 2u - newWatermarkWidth / 2u), static_cast<int>(photoSize.height() / 2u - newWatermarkHeight / 2u),
                static_cast<int>(newWatermarkWidth), static_cast<int>(newWatermarkHeight));
            break;
        }

        //Modification de la couleur si demand
        t_watermark watermarkParams = watermarkImage.parameters();
        if( watermarkParams.f_autocolor && watermarkParams.type == t_watermark::TEXT ){
            //Calcul de la luminosit
            meanLuma = this->meanLightness( watermarkRectangle );
            //Inversion des couleurs du texte (blanc->noir) si luma > 0.5
            if( meanLuma > 0.5 ){
                watermarkImage.negate( );
            }
        }

        this->composite( watermarkImage, gravity, OverCompositeOp );

        f_success = true;
    }
    catch( Magick::Error &error ){
            m_errors << QString( error.what() );
            f_success = false;
    }

    return f_success;    
}




/*******************************************************************
* zoom( const QSize &size, Qt::TransformationMode filter  )
* ---------
* surcharge d'Image::zoom Qt' style
* In : (QSize) size - taille de l'image rescale
* In : (Qt::TransformationMode) filter - qualit du filtrage pour rescaling.
*      Par dfaut, Qt::SmoothTransformation qui utilisera du lanczos
********************************************************************/
void CPhoto::zoom( const QSize &size, Qt::TransformationMode filter )
{
    if( filter != Qt::SmoothTransformation ){
        this->filterType( PointFilter );
    }
    else{
        this->filterType( LanczosFilter );
    }

    try{
        Image::resize( Geometry( size.width(), size.height() ) ); 
    }
    catch( Magick::Error &error ){
        m_errors << QString( error.what() );
    }
}

/*******************************************************************
* meanLightness(const QRect &rect)
* ---------
* retourne la luminosit moyenne dans une zone de la photo
* In : (QRect) rect - rectangle dans lequel la luminosit est calcule
*      NB : bords du rectangle inclus.
* Retour : luminosit moyenne (comprise entre 0.0 et 1.0) ou -1 si erreur
********************************************************************/
double CPhoto::meanLightness(const QRect &rect)
{
    const PixelPacket* pixels;
    ColorRGB rgb;
    int rectX, rectY;
    int rectWidth, rectHeight;
    int nbPixels;
    double accumulatedLuma = 0.0;
    double meanLuma;

    rectX = rect.left();
    rectY = rect.top();
    rectWidth = rect.width();
    rectHeight = rect.height();        

    //Si le rectangle dborde de la photo
    if(     (size_t)(rectY + rectHeight) > this->rows() || rectY < 0
        ||  (size_t)(rectX + rectWidth ) > this->columns() ||  rectX < 0 )
    {
        m_errors << QObject::tr("Wrong rectangle dimension.");
        return -1.0;
    }

    nbPixels = rectWidth * rectHeight;

    try{
            pixels = this->getConstPixels( rectX, rectY, rectWidth, rectHeight); //Rcupration des pixels
            //Acummulation des canaux
            double accRed = 0.0;
            double accGreen = 0.0;
            double accBlue = 0.0;
            for( int i = 0; i < nbPixels; i++ )
            {
                rgb = ( *pixels++ );
                accRed += rgb.red();
                accGreen += rgb.green();
                accBlue += rgb.blue(); 
            }
            accumulatedLuma += ( 0.299*accRed + 0.587*accGreen + +0.114*accBlue ); //http://en.wikipedia.org/wiki/Luminosity
            meanLuma = (accumulatedLuma / (double)nbPixels);
    }
    catch( Magick::Error &error ){
        m_errors << QString( error.what() );
        return -1.0;
    }

    return meanLuma;

}

/*******************************************************************
* orientationExif( )
* ---------
* Orientation automatique en fonction du contenu de l'EXIF
* voir : http://sylvana.net/jpegcrop/exif_orientation.html

    For convenience, here is what the letter F would look like if it were tagged correctly and displayed by a
    program that ignores the orientation tag (thus showing the stored image): 

    1        2       3      4         5            6           7          8

    888888  888888      88  88      8888888888  88                  88  8888888888
    88          88      88  88      88  88      88  88          88  88      88  88
    8888      8888    8888  8888    88          8888888888  8888888888          88
    88          88      88  88
    88          88  888888  888888

* Return : (bool) true si succs, false sinon

********************************************************************/
bool CPhoto::orientationExif( )
{
    bool f_success = true;
    string exifAttrib = this->attribute( "EXIF:Orientation" );
    int orientation = atoi( exifAttrib.c_str() );

    try{
        switch( orientation )
        {
        case 1:   //Bonne orientation
            break;
        case 2:
            this->flip();
            break;
        case 3:
            this->rotate( 180.0 );
            break;
        case 4:
            this->rotate( 180.0 );
            this->flip();
            break;
        case 5:
            this->rotate( 90.0 );
            this->flip( );
            break;
        case 6:
            this->rotate( 90.0 );
            break;
        case 7:
            this->rotate( -90.0 );
            this->flip();
            break;
        case 8:
            this->rotate( -90.0 );
            break;
        default:
            f_success = false;
            break;
        }
    }
    catch( Magick::Error &error ){
        m_errors << QString( error.what() );
        f_success= false;
    }

    return f_success;
}


/*******************************************************************
* removeMetadata( )
* ---------
* Enlve les mtadonnes d'une photo
********************************************************************/
void CPhoto::removeMetadata( )
{
    try{
        this->profile( "*", Blob() );
    }
    catch( Magick::Error &error ){
        m_errors << QString( error.what() );
    }
}


/*******************************************************************
* size( )
* ---------
* Return : (QSize) Taille de l'image
********************************************************************/
QSize CPhoto::size()
{
    return QSize(static_cast<int>(Image::size().width()), static_cast<int>(Image::size().height()));
}

/*******************************************************************
* qimage( )
* ---------
* Return : (QImage) Photo convertie en QImage
********************************************************************/
QImage CPhoto::qimage(  )
{   

    const PixelPacket* pixels;
    ColorRGB rgb;
    QImage newQImage = QImage(static_cast<int>(this->columns()), static_cast<int>(this->rows()), QImage::Format_RGB32);

    for( int y = 0; y < newQImage.height(); y++ ) //Pour chaque ligne
    {
        pixels = this->getConstPixels(0, y, newQImage.width(), 1); //Rcupration d'une ligne

        for (int x = 0; x < newQImage.width(); x++) //Pour chaque pixel de la ligne
        {
            rgb = ( *(pixels + x) );
            QColor qcolor = QColor( (int) (255 * rgb.red()), (int) (255 * rgb.green()), (int) (255 * rgb.blue()) );
            newQImage.setPixel( x, y, qcolor.rgb() );
         }
      }

    return newQImage;

}

/*******************************************************************
* readExifTags( )
* ---------
* Lit et interprte une slection de tags exif intressant
********************************************************************/
void CPhoto::readExifTags( )
{
    QString expProgStr;
    int exposureProgInt;
    int flash;

    //Lectures simples
    m_exifTags.insert( CTaggedString::TAGcameraMaker,     QString(attribute( EXIFcameraMaker.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGcameraModel,     QString( attribute( EXIFcameraModel.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGfocalLength,     rationalToDoubleStr( attribute( EXIFfocalLength.toLatin1().constData() ).c_str()) + QString("mm") );
    m_exifTags.insert( CTaggedString::TAGexposureTime,    rationalToDoubleStr( attribute( EXIFexposureTime.toLatin1().constData() ).c_str()) + QString("s") );
    m_exifTags.insert( CTaggedString::TAGfNumber,         QString("f/") + rationalToDoubleStr( attribute( EXIFfNumber.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGisoSpeedRatings, QString( attribute( EXIFisoSpeedRatings.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGname,          QString::fromUtf8( attribute( TAGname.toLatin1().constData() ).c_str() ) );
    m_exifTags.insert( CTaggedString::TAGheadline,      QString::fromUtf8( attribute( TAGheadline.toLatin1().constData() ).c_str() ) );
    m_exifTags.insert( CTaggedString::TAGcaption,        QString::fromUtf8( attribute( TAGcaption.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGcountry,        QString::fromUtf8(attribute( TAGcountry.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGcity,           QString::fromUtf8(attribute( TAGcity.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGsublocation,    QString::fromUtf8(attribute( TAGsublocation.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGkeywords,       QString::fromUtf8(attribute( TAGkeywords.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGauthor,         QString::fromUtf8(attribute( TAGauthor.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGcredit,         QString::fromUtf8(attribute( TAGcredit.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGsource,         QString::fromUtf8(attribute( TAGsource.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGcopyright,      QString::fromUtf8(attribute( TAGcopyright.toLatin1().constData() ).c_str()) );
    m_exifTags.insert( CTaggedString::TAGcontact,        QString::fromUtf8(attribute( TAGcontact.toLatin1().constData() ).c_str()) );

    //--- Interprtation
    //- Programme
    exposureProgInt = atoi( attribute( EXIFexposureProgram.toLatin1().constData() ).c_str() );
    switch( exposureProgInt )
    {
    case 0:
        expProgStr = QObject::tr("Not Defined");
        break;
    case 1:
        expProgStr = QObject::tr("Manual");
        break;
    case 2:
        expProgStr = QObject::tr("Program");
        break;
    case 3:
        expProgStr = QObject::tr("Aperture priority");
        break;
    case 4:
        expProgStr = QObject::tr("Shutter speed priority");
        break;
    case 5:
        expProgStr = QObject::tr("Creative");
        break;
    case 6:
        expProgStr = QObject::tr("Action");
        break;
    case 7:
        expProgStr = QObject::tr("Portrait");
        break;
    case 8:
        expProgStr = QObject::tr("Landscape");
        break;
    case 9:
        expProgStr = QObject::tr("Bulb");
        break;
    default:
        expProgStr = QObject::tr("Undefined");
        break;
    }
    m_exifTags.insert( CTaggedString::TAGexposureProgram, expProgStr);

    //- Flash
    flash = atoi( attribute( EXIFflash.toLatin1().constData() ).c_str() );
    if( flash & 0x1 ) { //Test LSB
        m_exifTags.insert( CTaggedString::TAGflash, QObject::tr("Fired") );
    }
    else {
        m_exifTags.insert( CTaggedString::TAGflash, QObject::tr("Not fired") );
    }

    //- Date
    QString dateTime( attribute( EXIFdateTimeOriginal.toLatin1().constData() ).c_str() );
    int split = dateTime.indexOf( QChar(' ') );
    QString date = dateTime.left( split );
    date.replace( QChar(':'), QChar('/') );
    m_exifTags.insert( CTaggedString::TAGdateOriginal, date );
    QString time = dateTime.right( dateTime.size() - split - 1 );
    time.replace( time.indexOf( QChar(':') ), 1, QChar('h') );
    m_exifTags.insert( CTaggedString::TAGtimeOriginal, time );
    split = dateTime.indexOf( QChar(':') );
    QString year = dateTime.left( split );
    m_exifTags.insert( CTaggedString::TAGyearOriginal, year );
}



/*******************************************************************
* exifTags( )
* ---------
* Return : (CExifTags) Slection de tags exifs intressants
********************************************************************/
QMap<QString,QString> CPhoto::exifTags()
{
    return m_exifTags;
}


/*******************************************************************
* rationalToDouble( const string &rational )
* ---------
* Converti un nombre rationel provenant d'un tag exif en double
* In:(string) nombre rationel convertir
* Return: (double)
********************************************************************/
QString CPhoto::rationalToDoubleStr( const string &inRational )
{
    double result = 0.0;    
    int numerator;
    int denominator;
    QString rational( inRational.c_str() );

    if( rational.contains("/") )
    {
        int indexOfSlash = rational.indexOf("/");
        numerator = rational.left( indexOfSlash ).toInt();
        denominator = rational.right( rational.length() - indexOfSlash - 1).toInt();
        result = (double)numerator / (double)denominator;
    }
    if( result < 1.0 && result != 0.0 ) {
        //normalisation
        denominator /= numerator;
        numerator = 1;        
        return QString::number( numerator ) + QString("/") + QString::number( denominator ) ;
    }
    else{          
        return QString::number( result, 'f', 2 );
    }
}
