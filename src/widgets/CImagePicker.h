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


#ifndef CImagePicker_H
#define CImagePicker_H

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QImage>
#include <QDir>
#include <QSize>
//Important pour pouvoir manipuler les events:
#include <QMouseEvent>

class CImagePicker : public QLabel
{
    Q_OBJECT

public:
    CImagePicker( QWidget* parent = 0);
    bool setImage( const QString & ); //Donne le chemin de l'image
    QString fileName( ); //Retourne le nom du fichier de l'image
    static void setDirToBrowse( const QString &dir );
    QSize imageSize( );                 //Renvoie la taille de l'image;
    void clearImage( );
public slots:
    void computeAverageColor( );
protected:
    void mousePressEvent(QMouseEvent *);	
private:
    void chooseImage( );    
signals:
    void clicked( );
    void imageSet( );           //Emit when image is set
    void imageCleared( );       //Emit when image is cleared
    void averageColorComputed( QString );
	
private:
    QString m_fileName;
    const QString NO_IMAGE;//Texte  afficher quand pas d'image

    static QString m_dirToBrowse;//Rpertoire  ouvrir lorsqu'on clique sur le widget
                                 //static car pratique de partager entre toutes les instances !
    QColor m_averageColor;       //Couleur moyenne de l'image choisie;
    QImage m_thumbnail;          //Vignette de la taille du widget
    QSize m_imageSize;           //Taille de l'image
    
};

#endif
