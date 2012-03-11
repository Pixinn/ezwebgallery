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



#ifndef CSKINPARAMETERS_H
#define CSKINPARAMETERS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QFileInfo>
#include <QDir>

#include "IParameters.h"
#include "CCss.h"
#include "CError.h"

#include "ui_WinSkinDesigner.h" //Gnr par qmake. Ncessaire pour accs  la classe gnre par le formulaire .ui

namespace Ui { //Voir le bas du fichier ui_skinDesigner.h
    class SkinDesigner;
}


//----- Structures internes ------//
typedef struct {
    int titlePosition;
}t_misc;

//Position du titre :  mettre en phase avec l'index du menu droulant correspondant dans la GUI
#define TITLE_POSITION_LEFT 0
#define SKIN_TAG_NAME       "Skin"

class CSkinParameters : public QObject, IParameters<Ui::SkinDesigner>
{

    Q_OBJECT

signals:
    void skinOpened( QString );
    void skinSaved( QString );
    void nameChanged( QString );
public:
    CSkinParameters( );
    CSkinParameters& operator=(const CSkinParameters & );        //Ncessit de surcharger l'oprateur d'assignement lorsqu'on hrite de QObject
    bool operator==(const CSkinParameters & );
    void setName( const QString & );                             //Donne le nom de la skin    
    QString name( );                                             //Rcupre le nom de la skin
    QString filePath( );                                         //Rcupre le path de la skin. Vide si non sauvegarde.
    void fromUi( );                                              //Rcupre les donnes provenant de l'UI
    void fromDomDocument( QDomDocument & );                      //Rcupre les donnes provenant d'un QDomDocument
    void toUi(  );                                               //Rempli l'UI = affichage des paramtres
    QMap<QString,QFileInfo> ressourceFiles( );                   //Retourne des infos sur les fichiers ressources ncessaires
    CCssSheet toCss( );                                          //Cr une feuille de style CSS
    QString buttonImage( int );                                  //Renvoie le nom du fichier de l'icone dsire
    QDomDocument toDomDocument( );                               //Gnre un QDomDocument rempli des paramtres
    void setDir( const QString & );                              //Indique le rpertoire de la skin
    void setUi( Ui::SkinDesigner* );                             //Connecte les paramtres  l'UI
    bool load( const QString &);                                 //Chargement de la skin
    bool saveSkin( const QString &/*, QStringList &*/ );         //Sauvegarde la skin. Retourne false si erreurs
    bool copyRessources( QDir /*, QStringList &*/ );             //Copie les fichiers de ressources vers le rpertoire spcifi
    void check( QStringList* , QStringList* );                   //Vrifie la skin et retourne les erreur et avertissements
    static QString defaultSkin( void );                          //Renvoie le chemin absolu de la skin par dfaut
    QString error( void ) { return m_lastError; }                //Retourne la dernire erreur survenue
    QStringList errors( void ) { return m_lastErrors; }          //Retourne la dernire srie d'erreurs survenues
    unsigned int version( void ) { return m_version; }           //Retourne le status du IParameters
    inline bool initialized( void ) { return f_initialized; }

private:
    QDomDocument convertFromOldVersion( const QDomDocument &document, const int version );    //Convertion d'une skin d'une version prcdante en la version actuelle
    QString getImagePath( const QString & );                     //Retourne le chemin absolu vers l'image
    void constructRessources( );                                 //Construit le tableau de ressources
    void removeEmptyRessources( );				                 //Enlve les fichiers vides de la liste de ressources
    QString translateError( int );                               //Renvoie l'erreur traduite dans la langue de l'ui

public:
    //Pour calculs
    int thumbImgBorderSize;
    int thumbBoxBorderSize;
    int photoPaddingSize;
    //Boutons
    enum Buttons{ //Sert  passer un paramtre  la fonction buttonImage( int )
        buttonNext = 0,
        buttonPrevious,
        buttonIndex
    };

private:
    QString m_name;                             //Le nom de la skin
    QString m_ressourcesPath;                   //Chemin vers les ressources ncessaires
    QString m_filePath;                         //Chemin vers la skin
    QString m_lastError;                        //Dernire erreur survenue
    QStringList m_lastErrors;                   //Liste des dernires erreurs survenues
    Ui::SkinDesigner* m_p_ui;
    t_misc m_misc;
    QMap<QString,QFileInfo> m_ressources;      //widget name ; file info
    CCssSheet m_styleSheet;
    unsigned int m_version;
    bool f_initialized;
    //Enumration des erreurs possibles
    enum e_errors{
        noError,
        overwrittingDefault,
        destFolder,
        loadingError
    };
};

#endif // CSKINPARAMETERS_H
