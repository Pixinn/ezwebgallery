/* 
 *  EZWebGallery:
 *  Copyright (C) 2011-2013 Christophe Meneboeuf <dev@ezwebgallery.org>
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



#include "CImagePicker.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>
#include <QApplication>

// --- Dfinition des membres statiques
QString CImagePicker::m_dirToBrowse = QDir::homePath (); //A mettre dans le cpp et non dans le .h : http://ubuntuforums.org/showthread.php?t=836043


CImagePicker::CImagePicker( QWidget* parent) : QLabel( parent ),
    NO_IMAGE( tr("no image") )
{
    //UI
    setText( NO_IMAGE );
    setMinimumSize( 64, 64 );
    setMaximumSize( 64, 64 );
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    setLineWidth( 2 );
    setAlignment( Qt::AlignCenter );
    setToolTip( tr("Right-click to remove the current image."));
    //Init
    connect(this, SIGNAL(imageSet()), this, SLOT(computeAverageColor()));
}

//Besoin de rimplmenter pour que ce driv de QLabel puisse se faire cliquer dessus comme un bouton
void CImagePicker::mousePressEvent(QMouseEvent *e)
{
    if( e->button() == Qt::LeftButton ){
        e->accept();
        chooseImage( );
        emit clicked( );
    }
	if( e->button() == Qt::RightButton ){
        e->accept();
        clearImage( );
		emit imageCleared();
        emit clicked( );
    }
}

//Fourni le rpertoire  parcourir lorsqu'on clique sur le widget
void CImagePicker::setDirToBrowse( const QString &dir )
{
        m_dirToBrowse = dir;
}


//Choisi une image
void CImagePicker::chooseImage( )
{
        //Ouverture d'une boite de dialogue pour choisir une image
        QString fileName = QFileDialog::getOpenFileName( this, tr("Choose an Image"),
                                                         m_dirToBrowse, tr("Image Files (*.png *.jpg)"));
        if( !fileName.isEmpty( ) ){
            QImage image;
            QApplication::setOverrideCursor ( Qt::WaitCursor ); //Affichage d'un sablier si c'est un peu long

            if( image.load( fileName ) )
            {
                m_imageSize = image.size();
                QFileInfo fileInfo( fileName );
                setDirToBrowse( fileInfo.absolutePath() );
                m_thumbnail = image.scaled( this->size(), //Taille du widget
                                            Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation);

                QPixmap imagePxMap =  QPixmap::fromImage ( m_thumbnail );
                this->setPixmap( imagePxMap );
                m_fileName = fileInfo.absoluteFilePath();
                emit imageSet();
            }
            else{
                clearImage( );
            }

            //Restauration du curseur
            QApplication::restoreOverrideCursor() ;
        }
}

bool CImagePicker::setImage( const QString &fileName )
{
    QImage image;
    QApplication::setOverrideCursor ( Qt::WaitCursor ); //Affichage d'un sablier si c'est un peu long

    if( image.load( fileName ) )
    {
        m_imageSize = image.size();
        m_thumbnail = image.scaled( this->size(), //Taille du widget
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation);

        QPixmap imagePxMap =  QPixmap::fromImage ( m_thumbnail );
        this->setPixmap( imagePxMap );
        m_fileName = fileName;
        emit imageSet();
        QApplication::restoreOverrideCursor ();
        return true;
    }
    else{
        QApplication::restoreOverrideCursor ();
        clearImage( );
        return false;
    }
}

//Retourne le fichier de l'image
QString CImagePicker::fileName()
{
    return m_fileName;
}

//Retourne la taille de l'image
QSize CImagePicker::imageSize()
{
    return m_imageSize;
}

//Efface l'image
void CImagePicker::clearImage( )
{
    clear( );
    setText( NO_IMAGE );
    m_fileName.clear();
    m_imageSize.setHeight( 0 );
    m_imageSize.setWidth( 0 );
    emit imageCleared();
}


void CImagePicker::computeAverageColor( )
{
    int nbPixels;
    int meanRed = 0;
    int meanGreen = 0;
    int meanBlue = 0;


    QColor tmpColor;
    for( int w = 0; w < m_thumbnail.width(); w++ )
    {
        for( int h = 0; h < m_thumbnail.height(); h++ )
        {
            tmpColor = QColor( m_thumbnail.pixel(w,h) );
            meanRed += tmpColor.red();
            meanGreen += tmpColor.green();
            meanBlue += tmpColor.blue();
        }
    }

    nbPixels = m_thumbnail.width() * m_thumbnail.height();
    meanRed = meanRed / nbPixels;
    meanGreen = meanGreen / nbPixels;
    meanBlue = meanBlue / nbPixels;

    m_averageColor.setRed( meanRed );
    m_averageColor.setGreen( meanGreen );
    m_averageColor.setBlue( meanBlue );

    emit averageColorComputed( m_averageColor.name() );

}
