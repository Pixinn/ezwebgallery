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



#ifndef MAINWIN_H
#define MAINWIN_H

#define DEBUG_MODE

#include <QMainWindow>
#include <QAction>
#include <QTranslator>
#include <QCloseEvent>
#include <QFileDialog>
#include <QTextEdit>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QModelIndex>
#include <QDir>
#include <QMap>
#include <QList>

#include "ui_mainwin.h" //Gnr par qmake. Ncessaire pour accs  la classe gnre par le formulaire .ui

#include "GlobalDefinitions.h"
#include "IUserInterface.h"
#include "CGalleryGenerator.h"
#include "CProjectParameters.h"
#include "CSkinParameters.h"
#include "WinSkinDesigner.h"
#include "WinConfigure.h"
#include "CCaptionManager.h"
#include "CLanguageManager.h"
#include "CPhotoFeederDirectory.h"
#include "CPhotoDatabase.h"
#include "CMessage.h"

namespace Ui { //Pour diffrencier de la classe MainWin de mainwin.h et accder  la *vraie* ui
    class MainWin;
}


/*********** DEFINITIONS **********/
#define NBMAXRECENTFILES    5


/************* CLASSES **************/
class MainWin : public QMainWindow, IUserInterface
{
    Q_OBJECT
/************ METODES *************/
public:
    MainWin( CGalleryGenerator &/*, IPhotoFeeder &*/, CProjectParameters &, QWidget *parent = 0);
    ~MainWin();
    //void setGenerator( CGalleryGenerator* );

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

private:
    void openSession( const QString &);
    void newSession( );
    bool isUnsaved( );          //Renvoie true si le projet n'a pas été sauvédepuis les dernières modifications
    void displayRecentFiles( ); //Affiche la liste des fichiers rcemments ouverts
    int displayUnsavedMsgBox( bool, bool ); //Affiche une message box indiquant que le projet et/ou la skin n'ont pas été savegardés. Retourne le bouton choisi
    int displayMoreRecentMsgBox( ); //Affiche une alerte si on essaie d'ouvrir un projet gnr avec une version d'EZWG plus rcente
    void swapButtons( ); //swap certains boutons pour cause de génération de galerie
    bool checkForGeneration( QString & );//Vérifie les paramètres fournis pour la génération
    QStringList checkPhotosInDir( const QStringList&, const QDir & ); //Vérifie la présence des photos de la liste dans le répertoire
    bool checkCreateDir( QString & ); //Vérifie l'existance d'un répertoire et propose sa création via msgBox le cas échant

/********* SLOTS ********/
public slots:
    //-- internes -> relis  des signaux de l'UI
    //Windows
    void onNewSession( );
    void onOpenSession( );
    void onOpenRecentSession( );
    bool onSaveSession( );
    bool onSaveSessionAs( );
    void showTagsWindow( );
    void showLogWindow( );
    void showConfigureWindow( );
    void onlineManual( );    
    void about( );
    void aboutImageMagick( );
    //Configuration
    void choosePhotosDir(); // Choix du rpertoire contenant les photos
    void choosePhotosDirManually( );
    void chooseDestDir( );  // Choix du rpertoire de destination de la gallerie
    void chooseDestDirManually( );
    void sessionLoaded( QString );
    void sessionSaved( QString );
    //Présentation
    void sharpeningRadiusChanged( int );
    void sharpeningRadiusChanged( double );
    void imageOptimizationStrategyChanged( int );
    void watermarkTypeChanged( int );
    void watermarkGroupChecked( bool );
    void watermarkAutoColorChecked( int );
    //Légendes
    int  buildPhotoLists( ); //Parcourt le rpertoire d'entre et mets  jour le QMap de CPhotoPropertiesExtended avec les donnes disponibles
    void previewCaption( QString );  //Affiche un prrendu de la lgende.
    //skinning
    void openSkinDesigner( );
    void skinNameChanged( QString );
    void skinPathChanged( QString );
    //Generation
    void generateGallery( );
    //-- externes
    void onLogMsg( QString );
    void onProgressBar( int completion, QString color, QString message, int timeout = 0 );
    void onGalleryGenerationFinished( QList<CPhotoPropertiesExtended>  );
    void onForceStoppedFinished( QStringList );
    void displayThumbnail( QModelIndex ); //Affiche la vignette  lgender correspondant  l'index
    void thumnailChanged( int ); //Une photo a t choisie pour devenir vignette de la galerie
    void displayCaption( QString );        //Affiche le texte dans le lineEdit de la lgende
    void error( CMessage );           //An error occured
    void warning( CMessage );         //A warning occured
    void information( CMessage );     //Display an iformative message

/******** ATTRIBUTS *********/
private:
    //ui
    Ui::MainWin* m_ui;
    QTextEdit* m_p_logDisplay;
    QTextEdit* m_p_tagsWindow;
    WinSkinDesigner* m_p_skinDesignerWindow;
    WinConfigure* m_p_configureWindow;
    QStringListModel m_photosListModel;
    enum { eGenerating, eNotGenerating } m_stateGeneration;
    QStringList m_debugMessages;
    QStringList m_recentSessions;           //Liste contenant les NBMAXRECENTFILES derniers fichiers projets ouverts
    QAction *m_recentSessionsActions[NBMAXRECENTFILES];
    CLanguageManager m_languageManager;
    //conf    
    CCaptionManager m_captionManager;
    CProjectParameters m_newProjectParameters;
    CProjectParameters& m_projectParameters;
    CProjectParameters m_referenceProjectParameters;
    CSkinParameters m_skinParameters;
    QString m_lastSelectedDir;
    //Photo feeder
    CPhotoFeederDirectory &m_photoFeeder; //This ui knows the nature of feeder use
    //Photo Database
    CPhotoDatabase &m_photoDatabase;
    //Generation
    CGalleryGenerator& m_galleryGenerator;

};

#endif // MAINWIN_H
