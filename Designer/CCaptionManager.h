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

/*
* This header is mandatory on all files of the project in order to track the global svn revision usin svnrev
* $Id: CCaptionManager.h 133 2011-10-04 20:05:28Z xtof $
*/

#ifndef CCaptionManager_H
#define CCaptionManager_H




#include <QLabel>
#include <QListView>
#include <QMap>

#include "CCaption.h"

/*****************************
 * CCaptionManager
 * ----------------------
 * Classe grant les lgendes
 ******************************/


class CCaptionManager : public QObject
{
    Q_OBJECT

    public:
        CCaptionManager( );
        CCaptionManager( const CCaptionManager & );
        CCaptionManager operator=(const CCaptionManager &);
        void setPhotoList( QListView* photoList ); //Set la liste de photos et reinitialise l'objet
        QString photo( );                          //Retourne le nom de la photo affiche
        QList<CCaption> captionList( );          //Retourne la liste de lgendes sous forme de QStringList
        QMap<QString,CCaption> captionMap( );           //Retourne la liste de lgendes sous forme de QMap<QString,QString>
        void setCaptionMap( QMap<QString,CCaption> &);   //Set la liste de lgendes 
        void setExifTags( const QString &, const QMap<QString,QString> & ); //Indique les <tags Exifs / metadata>  utiliser pour la lgende de la photo spcifie
        void setFileInfo( const QString &, const QFileInfo & );             //Indique les infos fichier  utiliser par la lgende de la photo spcifie
        bool captionsEdited();    //Les lgendes ont-elle t dite depuis le dernier appel de legencaptionedReset() ?
        void captionsEditedReset();

    private:
        void setNumber( int nb );//Positionne la liste de photos et envoie le signal d'affichage du thumb correspondant
        int  remapCaptionList( ); //Refabrique une map de lgendes correspondant au modle associ

    signals:
        void displayThumbnailSignal( QModelIndex );
        void displayCaptionSignal( QString );
        void displayPreviewSignal( QString );

    public slots:
        void onPrevious();
        void onNext();
        void onListClicked( QModelIndex );
        void onCaptionTextEdited( QString );
        void onCaptionHeaderEdited(QString);
        void onCaptionEndingEdited(QString);

    private:
        int m_photoIndex;                       //Pour dplacement dans la liste des photos, en connection avec la listView
        QListView* m_p_photoList;               //La listView qui permet d'obtenir les lgendes
  //      QMap<QString,QString> m_captionList;     //La liste des lgendes proprement dites <filename,lengend> (pas le path complet)
        QMap<QString,CCaption> m_captionMap;     //La liste des lgendes proprement dites <filename,lengend> (pas le path complet)
        bool m_f_captionsEdited;                //Les lgendes ont-elle t dite depuis le dernier appel de legencaptionedReset() ?
};

#endif // CCaptionManager_H
