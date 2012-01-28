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



#ifndef CCaptionManager_H
#define CCaptionManager_H




#include <QLabel>
#include <QListView>
#include <QMap>

#include "CCaption.h"
#include "CPhotoDatabase.h"

/*****************************
 * CCaptionManager
 * ----------------------
 * Classe controlant les lgendes
 ******************************/


class CCaptionManager : public QObject
{
    Q_OBJECT

    public:
        CCaptionManager( );
        CCaptionManager( const CCaptionManager & );
        CCaptionManager operator=(const CCaptionManager &);
        void reset( void ); //reinit the manager
        QString displayedPhoto( );                          //Retourne le nom de la photo affiche
       // QList<CCaption> captionList( );          //Retourne la liste de lgendes sous forme de QStringList
       // QMap<QString,CCaption> captionMap( );           //Retourne la liste de lgendes sous forme de QMap<QString,QString>
       // void setCaptionMap( QMap<QString,CCaption> &);   //Set la liste de lgendes
        void setExifTags( const QString &, const QMap<QString,QString> & ); //Indique les <tags Exifs / metadata>  utiliser pour la légende de la photo spécifiée
        void setFileInfo( const QString &, const QFileInfo & );             //Indique les infos fichier  utiliser par la lgende de la photo spcifie
        bool captionsEdited( void );    //Les légendes ont-elle été éditées depuis le dernier appel de legencaptionedReset() ?
        void captionsEditedReset( void );

    private:
        void display( int nb );//Positionne la liste de photos et envoie le signal d'affichage du thumb correspondant
        int  remapCaptionList( ); //Refabrique une map de légendes correspondant au modèle associé

    signals:
        void displayThumbnailSignal( QModelIndex );
        void displayCaptionSignal( QString );
        void displayPreviewSignal( QString );
        void displayHighlightIndex( QModelIndex );

    public slots:
        void onListClicked( QModelIndex );
        void onCaptionTextEdited( QString );
        void onCaptionHeaderEdited(QString);
        void onCaptionEndingEdited(QString);
        void onPrevious( void ); //Previous caption requested
        void onNext( void ); //Previous caption requested 

    private:
        CPhotoDatabase& m_photoDb;
        int m_photoIndex;                       //Pour déplacement dans la liste des photos, en connection avec la listView
        //QListView* m_p_listView;               //La listView qui permet d'obtenir les lgendes
        //QMap<QString,CCaption> m_captionMap;     //La liste des légendes proprement dites <filename,lengend> (pas le path complet)
        bool m_f_captionsEdited;                //Les lgendes ont-elle t dite depuis le dernier appel de legencaptionedReset() ?
};

#endif // CCaptionManager_H
