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


#include <QCoreApplication>
#include <QDesktopServices>
#include <QSettings>
#include <QUrl>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QDomDocument>
#include <QDomElement>
#include <QTextStream>
#include <QDateTime>
#include <QStringList>
#include <QStringListIterator>
#include <QVariant>
#include <QImageReader>
#include <QImage>
#include <QPixmap>
#include <QCursor>
#include <QIcon>
#include <QTextEdit>

#include <CPhoto.h>
#include <string>

#include "mainwin.h"
#include "CPlatform.h"

using namespace std;


/*********************************************** DEFINES ********************************/
#define qApp (static_cast<QApplication *>(QCoreApplication::instance())) /* pour avoir acc�s � l'instance de l'application */

/********************************************** PUBLIC MEMBERS ********************************************/

/******************************************* R�alisation Interface **********************************************/
//IUserInterface::IUserInterface( ){ } //Impl�mentation obligatoire car on l'utilse dans le constructeur de MainWin
//IUserInterface::~IUserInterface(){}//Impl�mentation du destructeur virtuel obligatoire


void MainWin::setGenerator( CGalleryGenerator* interface){
    this->m_p_galleryGenerator = interface;
}

//--- SLOTS ---//

void MainWin::sharpeningRadiusChanged( int value )
{
    double doubleValue = ( (double)value ) / 100.0;
    this->m_ui->doubleSpinBox_PhotoSharpeningRadius->setValue( doubleValue );
}

void MainWin::sharpeningRadiusChanged( double value )
{
    int intValue = (int)( value * 100 );
    this->m_ui->horizontalSlider_PhotoSharpeningRadius->setValue( intValue );
}

//Affiche ou cache les param�tres de sharpening en fonction de la strat�gie d'optimisation
void MainWin::imageOptimizationStrategyChanged( int strategy )
{
    if( strategy == t_photosConf::OPTIMIZE_SCREENUSAGE )
    {
        m_ui->label_PhotoSharpeningAmount->setDisabled( true );
        m_ui->spinBox_PhotoSharpeningAmount->setDisabled( true );
        m_ui->horizontalSlider_PhotoSharpeningAmount->setDisabled( true );
        m_ui->label_PhotoSharpeningRadius->setDisabled( true );
        m_ui->doubleSpinBox_PhotoSharpeningRadius->setDisabled( true );
        m_ui->horizontalSlider_PhotoSharpeningRadius->setDisabled( true );
    }
    else
    {
        m_ui->label_PhotoSharpeningAmount->setDisabled( false );
        m_ui->spinBox_PhotoSharpeningAmount->setDisabled( false );
        m_ui->horizontalSlider_PhotoSharpeningAmount->setDisabled( false );
        m_ui->label_PhotoSharpeningRadius->setDisabled( false );
        m_ui->doubleSpinBox_PhotoSharpeningRadius->setDisabled( false );
        m_ui->horizontalSlider_PhotoSharpeningRadius->setDisabled( false );
    }
}

void MainWin::watermarkGroupChecked( bool isChecked )
{
    if( isChecked ){
        watermarkTypeChanged( m_ui->comboBox_WatermarkType->currentIndex() );
    }
}

void MainWin::watermarkTypeChanged( int type )
{
    if( m_ui->groupBox_Watermark->isChecked() ) //Ne pas activer des widgets alors que le groupbox n'est pas s�lectionn� !
    {
        QWidget* widget;                
        QRegExp selectorImage;
        QRegExp selectorText;
        selectorImage.setPatternSyntax( QRegExp::Wildcard );
        selectorImage.setPattern( "*WatermarkImage*" );        
        selectorText.setPatternSyntax( QRegExp::Wildcard );
        selectorText.setPattern( "*WatermarkText*" );
    
        if( type == t_watermark::IMAGE )
        {
            //Activer tous les widgets "watermark images"
            QList<QWidget *> widgetImageList = m_ui->tab_2_Presentation->findChildren<QWidget *>( selectorImage ); //On ne peut pas s�lectionner les enfants � partir du groupbox...
            foreach( widget, widgetImageList ){
                widget->setDisabled( false );
            }
            //D�sactiver tous les widgets "watermark texte"   
            QList<QWidget *> widgetTextList = m_ui->tab_2_Presentation->findChildren<QWidget *>( selectorText );
            foreach( widget, widgetTextList ){
                widget->setDisabled( true );
            }
        }
        else //Type texte
        { 
            //D�sactiver tous les widgets "watermark images"
            QList<QWidget *> widgetImageList = m_ui->tab_2_Presentation->findChildren<QWidget *>( selectorImage ); //On ne peut pas s�lectionner les enfants � partir du groupbox...
            foreach( widget, widgetImageList ){
                widget->setDisabled( true );
            }
            //Activer tous les widgets "watermark texte"   
            QList<QWidget *> widgetTextList = m_ui->tab_2_Presentation->findChildren<QWidget *>( selectorText );
            foreach( widget, widgetTextList ){
                widget->setDisabled( false );
            }
            //D�sactiver le CColorPicker en cas de couleur automatique
            if( m_ui->checkBox_WatermarkTextColorAuto->isChecked() ){
                m_ui->cColorPicker_WatermarkText->setDisabled( true );
            }
        }
    }
}

void MainWin::watermarkAutoColorChecked( int state )
{

    if( state == Qt::Checked ){
        m_ui->cColorPicker_WatermarkText->setColor( QColor("White") );
        m_ui->cColorPicker_WatermarkText->setDisabled( true );
    }
    else if( m_ui->groupBox_Watermark->isChecked() ){ //important de tester le isChecked(), car sinon on peut enabler � mauvais essient
        m_ui->cColorPicker_WatermarkText->setDisabled( false );
    }
}


void MainWin::onProgressBar( int completion, QString color, QString message, int timeout )
{
    //La couleur de la barre ne change pas..
    QPalette barPalette = m_ui->progressBar_Generation->palette();
    barPalette.setColor(QPalette::Window, color ); //cf. http://lists.trolltech.com/qt-interest/2001-06/thread00701-0.html
    this->m_ui->progressBar_Generation->setPalette( barPalette );

    this->m_ui->progressBar_Generation->setValue( completion );
    this->m_ui->statusbar->showMessage( message, timeout );
    onLogMsg( message );
}

void MainWin::onLogMsg( QString str )
{
    m_debugMessages << QDateTime::currentDateTime().toString( "dd.MM.yyyy - hh:mm:ss.zzz\t" ) + str;
    if( m_p_logDisplay->isVisible() ){
        m_p_logDisplay->append( m_debugMessages.last() );
    }    
}


void MainWin::onForceStoppedFinished( QStringList errorMessages )
{
    onLogMsg("Force Stop");

    //On affiche un message d'erreur
    if( errorMessages.size() > 0){

        QStringListIterator iterator = QStringListIterator( errorMessages );
        QString errors;
        while( iterator.hasNext() ){
            errors.append( iterator.next() + QString("\n") );
        }
        QMessageBox* alertBox = new QMessageBox ( QMessageBox::Critical, tr("Error"),
                                                  errors );
        alertBox->exec();
        delete alertBox;
    }

    if(  m_stateButtonGenerate == stateCancel ) //Quand la boite d'erreur s'affiche, on peut entrer deux fois ici :(
    {
        swapButtonGenerate( );
        m_ui->progressBar_Generation->setValue( 0 );
        m_ui->pushButton_Generation->setDisabled( false );
        m_ui->statusbar->showMessage( tr("Generation cancelled."), 7000 );
    }
}

//-------------//


//--------------------------------
//------ Gallerie g�n�r�e
//--------------------------------
void MainWin::onGalleryGenerationFinished( /*bool success*/QList<CPhotoProperties> propertiesList )
{
    bool success = true;
    
    foreach( CPhotoProperties photoProperties, propertiesList )
    {
        //Mise � jour des propri�t�s des photos
        m_projectParameters.m_photoPropertiesMap.insert( photoProperties.fileName(), photoProperties );
        //La g�n�ration a-t-elle aboutie pour cette photo ?
        if( !photoProperties.processed() ){
            success = false; 
        }
    }

    //La g�n�ration a abouti pour toutes les photos
    if( success ){
        m_projectParameters.m_galleryConfig.f_regeneration = false;
        m_projectParameters.m_photosConfig.f_regeneration = false;
        m_projectParameters.m_thumbsConfig.f_regeneration = false;

        swapButtonGenerate( ); //Bouton "cancel" redevient "generate"
        m_ui->statusbar->showMessage( tr("Generation successfully completed."), 7000 );

        //Ouverture de la galerie
        if( m_p_configureWindow->openGeneratedGallery() ) {
            QUrl url;
            QString indexPath = QDir(m_projectParameters.m_galleryConfig.outputDir).absoluteFilePath("index.html");
            QDesktopServices::openUrl( url.fromLocalFile( indexPath ) );
        }
    }
}


/***************************************************************************************************/


MainWin::MainWin( QWidget *parent ) :
    QMainWindow( parent ),
    m_ui(new Ui::MainWin)
{

    //Pour pouvoir utiliser dans les signaux/slots:
    qRegisterMetaType<CPhotoProperties> ( "CPhotoProperties" );

    setWindowIcon( QIcon(":/icons/app_icon") );
    m_ui->setupUi(this); //"this" h�rite de QMainWindow via IUserInterface

    m_lastSelectedDir = QDir::homePath();
    m_p_galleryGenerator = NULL;
    
    m_stateButtonGenerate = stateGenerate;  
    
    //Fen�tre de debug
    m_p_logDisplay = new QTextEdit( 0 );
    m_p_logDisplay->setReadOnly( true );
    m_p_logDisplay->setWindowTitle( tr("Logs") );
    m_p_logDisplay->resize(720,480);
    //Fen�tre de tags   
    m_p_tagsWindow = new QTextEdit( 0 );
    m_p_tagsWindow->setReadOnly( true );
    m_p_tagsWindow->setWindowTitle( tr("TAG list") );
    m_p_tagsWindow->resize(720,500);
    m_p_tagsWindow->setHtml( CPlatform::readTranslatedTextFile( "tagList.html" ) );
    //Fen�tre de conception de skin
    m_p_skinDesignerWindow = new WinSkinDesigner( &m_skinParameters /*, this*/ ); //La connection des param�tres avec l'ui se fait dans ce constructeur
    //Fen�tre de Configuration
    m_p_configureWindow = new WinConfigure( this );
    //L�gendes
    m_ui->listView_Photos->setModel( &m_photosListModel );

    /**** Connections SIGNAUX/SLOTS ******/  
    //Ouvrir/Fermer/Sauver Session
    connect( this->m_ui->action_NewSession, SIGNAL(triggered()), this, SLOT(onNewSession()));
    connect( this->m_ui->action_OpenSession, SIGNAL(triggered()), this, SLOT(onOpenSession()));
    connect( this->m_ui->action_SaveSession, SIGNAL(triggered()), this, SLOT(onSaveSession()));
    connect( this->m_ui->action_SaveSessionAs, SIGNAL(triggered()), this, SLOT(onSaveSessionAs()));
    connect( this->m_ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()) );
    //Param�tres (=Session)
    connect( &m_projectParameters, SIGNAL(loaded(QString)), this, SLOT(sessionLoaded(QString)));
    connect( &m_projectParameters, SIGNAL(saved(QString)), this, SLOT(sessionSaved(QString)));
    connect( &m_projectParameters, SIGNAL(message(QString)), this, SLOT(onLogMsg(QString)));
    //Afficher la fen�tre de tags
    connect( this->m_ui->action_DisplayTags, SIGNAL(triggered()), this, SLOT(showTagsWindow()));
    //Afficher la fen�tre de debug    
    connect( this->m_ui->action_ShowLogWindow, SIGNAL(triggered()), this, SLOT(showLogWindow()));
    //Afficher la fen�tre de configuration
    connect( this->m_ui->action_Configure, SIGNAL(triggered()), this, SLOT(showConfigureWindow()));
    //Help
    connect( this->m_ui->action_OnlineManual, SIGNAL(triggered()), this, SLOT(onlineManual()));
    connect( this->m_ui->action_About, SIGNAL(triggered()), this, SLOT(about()));
    connect( this->m_ui->action_AboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect( this->m_ui->action_AboutImageMagick, SIGNAL(triggered()), this, SLOT(aboutImageMagick()));
    //Traductions    
    connect( m_p_configureWindow, SIGNAL( languageChanged() ), &m_languageManager, SLOT(translate()) );
    //R�pertoire source
    connect( this->m_ui->pushButton_SourceFolder, SIGNAL(clicked()), this, SLOT(choosePhotosDir()));
    connect( this->m_ui->lineEdit_SourceFolder, SIGNAL(editingFinished()), this, SLOT(choosePhotosDirManually()));
    //R�pertoire destination
    connect( this->m_ui->pushButton_DestinationFolder, SIGNAL(clicked()), this, SLOT(chooseDestDir()));
    connect( this->m_ui->lineEdit_DestinationFolder, SIGNAL(editingFinished()), this, SLOT(chooseDestDirManually()));
    //G�n�ration de la gallery
    connect( this->m_ui->pushButton_Generation, SIGNAL(clicked()), this, SLOT( generateGallery() ));
    //Ouverture / Conception d'une skin
    connect( this->m_ui->pushButton_DesignSkin, SIGNAL(clicked( )), this, SLOT( openSkinDesigner() ) );
    connect( this->m_ui->pushButton_OpenSkin, SIGNAL(clicked( )), m_p_skinDesignerWindow, SLOT( onOpenSkin()) );
    connect( &m_skinParameters, SIGNAL(nameChanged(QString)), this, SLOT(skinNameChanged(QString)) );
    connect( &m_skinParameters, SIGNAL(skinOpened(QString)), this, SLOT(skinPathChanged(QString)) );
    connect( &m_skinParameters, SIGNAL(skinSaved(QString)), this, SLOT(skinPathChanged(QString)) );
    //Navigation dans les Thumnail de "tab L�gendes"
    connect( this->m_ui->pushButton_UpdatePhotoList, SIGNAL(clicked()), this, SLOT( buildPhotoLists() ) );
    connect( this->m_ui->checkBox_GalleryThumb, SIGNAL(stateChanged(int)), this, SLOT(thumnailChanged(int)));
    connect( this->m_ui->pushButton_PrevPhoto, SIGNAL(clicked( )), &(this->m_captionManager), SLOT( onPrevious() ) );
    connect( this->m_ui->pushButton_nextPhoto, SIGNAL(clicked( )), &(this->m_captionManager), SLOT( onNext() ) );
    connect( this->m_ui->listView_Photos, SIGNAL(clicked(QModelIndex)), &(this->m_captionManager), SLOT( onListClicked(QModelIndex)) );
    connect( this->m_ui->lineEdit_Caption, SIGNAL(textEdited(QString)), &(this->m_captionManager), SLOT(onCaptionTextEdited(QString)) );
    connect( this->m_ui->lineEdit_captionEnding, SIGNAL(textEdited(QString)), &(this->m_captionManager), SLOT(onCaptionEndingEdited(QString)) );
    connect( this->m_ui->lineEdit_captionHeader, SIGNAL(textEdited(QString)), &(this->m_captionManager), SLOT(onCaptionHeaderEdited(QString)) );
    connect( &(this->m_captionManager), SIGNAL(displayThumbnailSignal(QModelIndex)), this, SLOT(displayThumbnail(QModelIndex)) );
    connect( &(this->m_captionManager), SIGNAL(displayCaptionSignal(QString)), this, SLOT(displayCaption(QString)) );
    connect( &(this->m_captionManager), SIGNAL(displayPreviewSignal(QString)), this, SLOT(previewCaption(QString)) );
    //Onglet Pr�sentation
    connect( this->m_ui->horizontalSlider_PhotoSharpeningRadius, SIGNAL(valueChanged(int)), this, SLOT(sharpeningRadiusChanged(int)) );
    connect( this->m_ui->doubleSpinBox_PhotoSharpeningRadius, SIGNAL(valueChanged(double)), this, SLOT(sharpeningRadiusChanged(double)) );
    connect( this->m_ui->comboBox_ImageQualityStrategy, SIGNAL(currentIndexChanged(int)), this, SLOT(imageOptimizationStrategyChanged(int)) );
    connect( this->m_ui->comboBox_WatermarkType, SIGNAL(activated(int)), this, SLOT(watermarkTypeChanged(int)) );
    connect( this->m_ui->groupBox_Watermark, SIGNAL(toggled(bool)), this, SLOT(watermarkGroupChecked(bool)) );
    connect( this->m_ui->checkBox_WatermarkTextColorAuto, SIGNAL(stateChanged(int)), this, SLOT(watermarkAutoColorChecked(int)));
    

    /**** Init ****/	
    //Traduction de l'interface
    m_languageManager.translate();
    //Session
    m_projectParameters.setUi( m_ui );
    m_projectParameters.setcaptionManager( &m_captionManager );
    m_projectParameters.setSkin( &m_skinParameters );
    //Init du projet vide
    m_newProjectParameters.setUi( m_ui );
    m_newProjectParameters.setcaptionManager( &m_captionManager );
    m_newProjectParameters.setSkin( &m_skinParameters );
    m_newProjectParameters.fromUi( ); //Cela initialise les valeurs � celles par d�faut dans l'ui
    
    //Derni�res sessions
    QSettings settings;
    m_recentSessions = settings.value( SETTINGS_RECENTPROJECTS ).toStringList();
    for( int i = 0; i < NBMAXRECENTFILES; i++ ){ //Cr�ation du tableau d'actions pour ouvrir les fichiers r�cents
        m_recentSessionsActions[i] = new QAction( this );
        m_ui->menuFichiers->addAction( m_recentSessionsActions[i] );
        connect(m_recentSessionsActions[i], SIGNAL(triggered()),this, SLOT(onOpenRecentSession()));
    }
    displayRecentFiles( );
    
    m_ui->action_SaveSession->setDisabled( true );


    bool f_lastSessionOK = false;
    //Par d�faut : Nouvelle session
    newSession( );
    //m_skinParameters.load( m_projectParameters.m_galleryConfig.skinPath ); //skin par d�faut
    //Chargement de la derni�re session si configur� ainsi
    if( (bool)settings.value( SETTINGS_OPENMOSTRECENTPJT ).toInt() )
    {
        f_lastSessionOK = !m_recentSessions.isEmpty(); //Sera vide si on lance EZWG pour la premi�re fois
        if( f_lastSessionOK ){ //Une derni�re session existe
            QString  fileToLoad = m_recentSessions.first();
            openSession( fileToLoad );
        }
    }


}

MainWin::~MainWin()
{
    delete m_p_configureWindow;
    delete m_p_skinDesignerWindow;
}



void MainWin::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //Il faut sauver / recharger les param�tres, car les combobox reviennent sur les valeurs par d�faut
        m_projectParameters.fromUi();
        m_skinParameters.fromUi();
        //traductions
        m_ui->retranslateUi(this);
        m_p_skinDesignerWindow->retranslate();
        m_p_configureWindow->retranslate();
        //Restauration des param�tres
        m_projectParameters.toUi();
        m_skinParameters.toUi();
        //Traduction des textes suppl�mentaires (htm, etc)
        m_p_tagsWindow->setHtml( CPlatform::readTranslatedTextFile( "tagList.html" ) );
        //CPlatform::setLanguage( langSuffix );
        break;
    default:
        break;
    }
}

//Surcharge de la fermeture de la fen�tre
// -> sauvegardes du projet et de la skin avant de quitter
 void MainWin::closeEvent(QCloseEvent *event)
 {   
    //On regarde ce qui n'a pas �t� sauv�
    m_projectParameters.fromUi();
    bool f_skinUnsaved = m_p_skinDesignerWindow->isUnsaved();
    bool f_sessionUnsaved = isUnsaved( );
    //Affichage de la boite de dialogue et sauvegardes si choix de l'utilisateur
    int ret = displayUnsavedMsgBox( f_sessionUnsaved, f_skinUnsaved );
    if( ret == QMessageBox::Cancel ){ //CANCEL -> ne pas fermer
        event->ignore(); 
        return;
    }
                      
    m_p_logDisplay->close();
    m_p_tagsWindow->close();
    m_p_tagsWindow->deleteLater();
    m_p_skinDesignerWindow->close();
    m_p_configureWindow->close();

    //fermeture
    event->accept();
 }

/****************************************** SLOTS INTERNES ********************************************/

//--------------------------------
//------ Ouvrir / Sauver une session
//--------------------------------

void MainWin::onNewSession( )
{    
    //Si le projet courant n'a pas �t� sauv�, on propose de le faire    
    //On propose la sauvegarde de la skin ici et non � l'ouverture effecture de la nouvelle :
    //en effet, il faut pouvoir annuler le chargement de la session et de sa skin tr�s en amont si
    //pb ou annulation lors de la sauvegarde de la skin courante.
    int retDlg = QMessageBox::Ok;
    bool f_sessionUnsaved = isUnsaved();
    bool f_skinUnsaved = m_p_skinDesignerWindow->isUnsaved();
    if( f_sessionUnsaved || f_skinUnsaved ){ 
        retDlg = displayUnsavedMsgBox( f_sessionUnsaved, f_skinUnsaved );
    }
    
    if( retDlg != QMessageBox::Cancel ){
        newSession();
    }
}



void MainWin::newSession( )
{
        m_projectParameters = m_newProjectParameters;
        //Rechargement de la skin par d�faut
        m_projectParameters.m_galleryConfig.skinPath = CSkinParameters::defaultSkin();
        m_skinParameters.load( m_projectParameters.m_galleryConfig.skinPath  );         
        m_projectParameters.toUi( );                             //Maj UI
        buildPhotoLists();                                       //Construction liste des photos (vide...)
        m_referenceProjectParameters = m_projectParameters;
        //UI
        m_ui->action_SaveSession->setDisabled( true );           //Il *faut* d�sactiver l'option save pour ne pas �craser le fichier le plus r�cent
}


void MainWin::onOpenSession( )
{
    QString selectedFilter = QString("*") + QString( XMLSESSIONEXTENSION );
    QString sessionFile  = QFileDialog::getOpenFileName(this,
                                                        tr("Open a project"),
                                                        m_lastSelectedDir,
                                                        selectedFilter,
                                                        &selectedFilter);
    if( !sessionFile.isEmpty() ) { //Vide si cancel, ce qui g�n�rerait une erreur...
        openSession( sessionFile );
    }
}
 

void MainWin::onOpenRecentSession( )
{
    QString fileToLoad;
    QAction *action = qobject_cast<QAction *>(sender());
    if (action){ //=0 si le sender n'est pas une action        
        fileToLoad = action->data().toString(); 
        openSession( fileToLoad );
    }    
}


void MainWin::openSession( const QString &sessionFile )
{    

    //Si lecture du fichier r�ussie
    if( m_projectParameters.load( sessionFile ) )
    {

       //Si le projet courant n'a pas �t� sauv�, on propose de le faire    
       //On propose la sauvegarde de la skin ici et non � l'ouverture effecture de la nouvelle :
       //en effet, il faut pouvoir annuler le chargement de la session et de sa skin tr�s en amont si
       //pb ou annulation lors de la sauvegarde de la skin courante.
       int retDlg = QMessageBox::Ok;
       bool f_sessionUnsaved = isUnsaved();
       bool f_skinUnsaved = m_p_skinDesignerWindow->isUnsaved();
       //Affichage de la boite de dialogue si la session courante n'a pas �t� sauv�e, r�cup�ration du code retour
       if( f_sessionUnsaved || f_skinUnsaved ){ 
           retDlg = displayUnsavedMsgBox( f_sessionUnsaved, f_skinUnsaved );
       }
       //Si annulation demand�e
       if( retDlg == QMessageBox::Cancel )   {
           return;
       }
       else //Si pas d'annulation demand�e
       {
            QDir inDir;
            m_lastSelectedDir = QFileInfo(sessionFile).absolutePath();        
            //bool checkPhotos = false;    //initialisation importante pour le while()
            QMessageBox alertBox(this);
            QStringList missingPhotos; 

//            m_projectParameters = m_newProjectParameters; //On repart d'une base saine : si une propri�t� est mal ouverte, elle sera ainsi sett�e par d�faut

            //Affichage d'un message si la version du projet est plus r�cente que l'application
            if( m_projectParameters.version( ) > CPlatform::revisionInt() ){
                displayMoreRecentMsgBox( );
            }
            //Le contenu du r�pertoire d'entre est-il toujours valable depuis la derni�re sauvegarde ?
            //ie : les photos source ont-elles chang de place depuis ??
            if( QFileInfo( m_projectParameters.m_galleryConfig.inputDir ).isDir() ){
                inDir = QDir( m_projectParameters.m_galleryConfig.inputDir  );
                missingPhotos = checkPhotosInDir( m_projectParameters.m_photoPropertiesMap.keys(), inDir );
            }
            else {
                missingPhotos << tr("Input folder does not exist.");
            }

            //Tant que le r�pertoire d'entr�e n'est pas valide on propose de le fournir
            while( !missingPhotos.isEmpty() )
            {
                //Boite de dialogue informant du probl�me
                alertBox.setText( tr("Input folder: ","Source folder containing the pictures")
                                  + m_projectParameters.m_galleryConfig.inputDir
                                  + tr("\nSome photos cannot be found. They will be removed from the project.\n")
                                  + tr("Do you want to manually provide their location ?") 
                                );
                alertBox.setInformativeText( tr("Error:\n") + missingPhotos.join("\n") );
                alertBox.setIcon( QMessageBox::Warning );
                alertBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No);
                alertBox.setDefaultButton( QMessageBox::Yes );
                if( alertBox.exec() ==  QMessageBox::Yes ){
                    //Boite de dialogue pour relocaliser les photos
                    m_lastSelectedDir  = QFileDialog::getExistingDirectory( this,
                                             tr("Please select the directory containing the project's photos.","RElocalisation des photos"),
                                             m_lastSelectedDir,
                                             QFileDialog::ShowDirsOnly );
                    if( !m_lastSelectedDir.isEmpty() ){
                        m_projectParameters.m_galleryConfig.inputDir = m_lastSelectedDir;
                        inDir = QDir( m_projectParameters.m_galleryConfig.inputDir );
                        m_ui->lineEdit_SourceFolder->setText( m_projectParameters.m_galleryConfig.inputDir );
                        missingPhotos = checkPhotosInDir( m_projectParameters.m_photoPropertiesMap.keys(), inDir );
                    }
                    else{ //L'utilisateur a annul� le choix d'un r�pertoire
                         m_lastSelectedDir = QDir::homePath();
                         missingPhotos.clear();
                    }
                }
                else{
                    missingPhotos.clear(); //L'utilisateur a d�clin�: on utilise le r�pertoire qui �tait dans la sauvegarde
                }
            }
            buildPhotoLists( );
       
            //Chargement de la skin
            QStringList errors;
            QString skinToLoad = m_projectParameters.m_galleryConfig.skinPath; //On sauve dans une variable car sera chang� en "default" si erreur de chargement et donc le err msg affichera "default"
            errors = m_p_skinDesignerWindow->openSkin( skinToLoad );
            //Si des erreurs sont survenues
            if( !errors.isEmpty() )
            {
                onLogMsg( QString("[Skin]. A problem occured when opening the file: ") + skinToLoad + errors.join("\n") );               
                        
                QMessageBox msgBox( this );
                msgBox.setInformativeText( tr("Cannot load the skin: ") + skinToLoad + tr(".\n\nUsing default skin instead.") );
                msgBox.setDetailedText( errors.join("\n") );
                msgBox.setIcon( QMessageBox::Information );
                msgBox.exec();                                     
            }
            else {    
                onLogMsg( QString("[Skin]. Skin loaded: ") + m_skinParameters.name() );
            }
            
        } //Fin du traitement de la session charg�e
    }
        
    else //Erreur de chargement de la session
    {   
        onLogMsg( "[Session]. Error: " + sessionFile + "not loaded" );
        QMessageBox* alertBox = new QMessageBox( QMessageBox::Critical, tr("Error"), CErrorMessages::error(FileOpening), QMessageBox::Close);
        alertBox->setDetailedText( sessionFile );
        alertBox->exec();
        delete alertBox;
    }
}


//Si possible, sauve la session sur le fichier le plus r�cent,
//sinon, appelle saveAs()
bool MainWin::onSaveSession( )
{
    QString sessionFile;
    bool fileSaved = false;

    //Par d�faut, sauvegarde sur le fichier le plus r�cent
    if( !m_recentSessions.isEmpty() ){
        sessionFile = m_recentSessions.first();
    }
    if( QFileInfo(sessionFile).exists() )
    {
        //Ajouter l'extension si il n'y en a pas
        if( !sessionFile.contains(QString( XMLSESSIONEXTENSION )) ){
            sessionFile.append( QString(XMLSESSIONEXTENSION) );
        }

        m_lastSelectedDir = QFileInfo(sessionFile).absolutePath();
        m_projectParameters.fromUi( );
        fileSaved = m_projectParameters.save( sessionFile );
        
        //Si la sauvegarde a �chou�e, avertissement
        if( !fileSaved ){
                QMessageBox msgBox( this );
                msgBox.setText( tr("The session could not be saved.") );
                msgBox.setIcon( QMessageBox::Warning ); 
                msgBox.setStandardButtons( QMessageBox::Ok );
                msgBox.exec();
        }
    }
    //Mais si le fichier le plus r�cent n'�xiste pas, on ouvre la boite de dialogue "SaveAs"
    else{
        fileSaved = onSaveSessionAs( );
    }
    
    return fileSaved;
}


bool MainWin::onSaveSessionAs( )
{
    bool fileSaved = false;
    QString selectedFilter = QString("*") + QString( XMLSESSIONEXTENSION );
    QString sessionFile  = QFileDialog::getSaveFileName(this,
                                                        tr("Save a project"),
                                                        m_lastSelectedDir,
                                                        selectedFilter,
                                                        &selectedFilter);
    if( !sessionFile.isEmpty() )
    {
        //Ajouter l'extension si �a n'a pas �t� fait par la boite de dialogue
        if( !sessionFile.contains(QString( XMLSESSIONEXTENSION )) ){
            sessionFile.append( QString(XMLSESSIONEXTENSION) );
        }    
    
        m_lastSelectedDir = QFileInfo(sessionFile).absolutePath();
        m_projectParameters.fromUi( );
        fileSaved = m_projectParameters.save( sessionFile );     

        //Si la sauvegarde a �chou�e, avertissement
        if( !fileSaved ){
                QMessageBox msgBox( this );
                msgBox.setText( tr("The session could not be saved.") );
                msgBox.setIcon( QMessageBox::Warning ); 
                msgBox.setStandardButtons( QMessageBox::Ok );
                msgBox.exec();
        }
    }

    return fileSaved;
}


bool MainWin::isUnsaved()
{
    m_projectParameters.fromUi();
    return ( m_referenceProjectParameters != m_projectParameters || m_captionManager.captionsEdited() );
}



void MainWin::showTagsWindow( )
{
    m_p_tagsWindow->show();
    m_p_tagsWindow->activateWindow(); //Force la fen�tre au premier plan (utile si d�j� ouverte)
}



void MainWin::showLogWindow( )
{
    if( !m_p_logDisplay->isVisible() ){
        QStringListIterator iterator(m_debugMessages);    
        m_p_logDisplay->clear( );
        while ( iterator.hasNext() ){
            m_p_logDisplay->append( iterator.next() );
        }        
        m_p_logDisplay->show( );       
    } 
}


void MainWin::showConfigureWindow( )
{
    m_p_configureWindow->exec();
}


void MainWin::onlineManual( )
{

    QString manual = QString( "http://www.ezwebgallery.org/spip.php?article6" ); //Version fran�aise
    //Ouvrir l'url en fran�ais
    if( CPlatform::language() != "fr" ){
        manual = QString( "http://www.ezwebgallery.org/spip.php?article11" );
    }
    QUrl url = QUrl( manual );
    QDesktopServices::openUrl( url );
}

void MainWin::about( )
{

    QMessageBox::about( this,
                        tr("About EZWebGallery - svn version ") + CPlatform::revision(),
                        CPlatform::readTranslatedTextFile( "about.html" ));

}

void MainWin::aboutImageMagick( )
{
    QFile fileToRead( ":/text/aboutImageMagick.html" );
    fileToRead.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream textStream( &fileToRead );
    textStream.setCodec("UTF-8");
    QMessageBox::about(this, tr("About ImageMagick"),
                       textStream.readAll() );
}

//--------------------------------
//------ Param�tres charg�s
//--------------------------------
void MainWin::sessionLoaded( QString fileLoaded )
{
    QSettings settings;
    //Insertion du fichier dans la liste des fichiers r�cemment utilis�s
    m_recentSessions.removeAll(fileLoaded);
    m_recentSessions.prepend(fileLoaded);
    if( m_recentSessions.size() > NBMAXRECENTFILES ){
        m_recentSessions.removeLast();
    }
    settings.setValue( SETTINGS_RECENTPROJECTS, m_recentSessions );
    m_referenceProjectParameters = m_projectParameters;
    m_captionManager.captionsEditedReset();
    //Affichage
    m_ui->action_SaveSession->setDisabled( false );
    m_projectParameters.toUi( );   

    displayRecentFiles();
    watermarkTypeChanged( m_projectParameters.m_photosConfig.watermark.type );
}
//--------------------------------
//------ Param�tres sauv�s
//--------------------------------
void MainWin::sessionSaved( QString fileSaved )
{
    QSettings settings;
    //Insertion du fichier dans la liste des fichiers r�cemment utilis�s
    m_recentSessions.removeAll(fileSaved);
    m_recentSessions.prepend(fileSaved);
    if( m_recentSessions.size() > NBMAXRECENTFILES ){
        m_recentSessions.removeLast();
    }
    settings.setValue( SETTINGS_RECENTPROJECTS, m_recentSessions );
    m_referenceProjectParameters = m_projectParameters;
    m_captionManager.captionsEditedReset();
    //Affichage
    m_ui->action_SaveSession->setDisabled( false );
    displayRecentFiles();
}
//--------------------------------
//------ S�lection R�pertoire Source
//--------------------------------
void MainWin::choosePhotosDir()
{

    QString dir  = QFileDialog::getExistingDirectory( this,
                                                      tr("Please select an input directory containing your pictures."),
                                                      m_lastSelectedDir,
                                                      QFileDialog::ShowDirsOnly );
    if( !dir.isEmpty()
        //Si on n'a pas chang� de r�pertoire, lenteurs inutiles d�e au rafrachissement de la vignette dans le "tab l�gendes"
        && dir!=m_projectParameters.m_galleryConfig.inputDir )
    {
        m_projectParameters.m_galleryConfig.inputDir = dir;
        m_lastSelectedDir = m_projectParameters.m_galleryConfig.inputDir;
        m_ui->lineEdit_SourceFolder->setText( dir );//Affichage
        //On cr�� la liste des jpegs ici pour qu'elle soit dispo sous l'onglet "L�gendes"
        buildPhotoLists( );
    }

}
void MainWin::choosePhotosDirManually()
{
    QDir sourceFolder = QDir( QDir::cleanPath(m_ui->lineEdit_SourceFolder->text()) );
    if( sourceFolder.exists( ) ){
        //Si on n'a pas chang� de r�pertoire, lenteurs inutiles d�e au rafrachissement de la vignette dans le "tab l�gendes"
        if( sourceFolder.absolutePath()!=m_projectParameters.m_galleryConfig.inputDir ){
            m_projectParameters.m_galleryConfig.inputDir = sourceFolder.absolutePath();
            m_lastSelectedDir = m_projectParameters.m_galleryConfig.inputDir;
            //On cr�� la liste des jpegs ici pour qu'elle soit dispo sous l'onglet "L�gendes"
            buildPhotoLists( );
        }
    }
    else{
        m_ui->lineEdit_SourceFolder->setText( tr("Invalid directory.") );
        m_projectParameters.m_galleryConfig.inputDir.clear(); //Sinon bug lorsque l'on rechoisi le r�pertoire d'avant
    }
}
//--------------------------------
//----------- S�lection R�pertoire Destination
//--------------------------------
void MainWin::chooseDestDir()
{
    QString dir  = QFileDialog::getExistingDirectory( this,
                                                      tr("Please select the output directory."),
                                                      m_lastSelectedDir,
                                                      QFileDialog::ShowDirsOnly );
    if( !dir.isEmpty()  ){
        m_lastSelectedDir = dir;
        m_ui->lineEdit_DestinationFolder->setText( dir );//Affichage
    }
}
void MainWin::chooseDestDirManually()
{
    QString destDir = m_ui->lineEdit_DestinationFolder->text();
    if( checkCreateDir( destDir ) ){
        m_lastSelectedDir = destDir;
        m_ui->lineEdit_DestinationFolder->setText( QDir(destDir).absoluteFilePath("") );
    }
    else{
        m_ui->lineEdit_DestinationFolder->setText("");
    }
}
//--------------------------------
//--------- G�n�ration de la gallerie
//--------------------------------
void MainWin::generateGallery( )
{    
    int nbCaptionsRemoved;
    QString errorMsg;
    QMessageBox info(this);

    info.setStandardButtons( QMessageBox::Ok );
    info.setIcon( QMessageBox::Information );

    //La g�n�ration n'est pas en cours -> la lancer
    if( !m_p_galleryGenerator->isGenerationInProgress() )
    {
        //Check des champs obligatoires
        if( !checkForGeneration( errorMsg ) )
        {
             QMessageBox alertBox(this);
             alertBox.setText( tr("Some configuration fields were not properly set.") );
             alertBox.setInformativeText( errorMsg );
             alertBox.setStandardButtons( QMessageBox::Close );
             alertBox.setIcon( QMessageBox::Warning );
             alertBox.exec();
        }
        else //Tous les champs sont remplis correctement
        {
            QStringList skinErrorList;
            QStringList skinWarningList;
            //MAJ des param�tres
            m_projectParameters.fromUi( );
            m_skinParameters.fromUi( );
            //V�rification de la skin et affichage d'une infobox si pb
            m_skinParameters.check( &skinErrorList, &skinWarningList );
            if( !skinErrorList.isEmpty() )
            {
                QString detailedText;
                //D�tail des erreurs
                foreach( QString error, skinErrorList ){
                    detailedText.append( error );
                    detailedText.append( "\n" );
                }
                QMessageBox* alertBox = new QMessageBox( QMessageBox::Critical, tr("Error"),
                                                         tr("Your skin is not valid.                                                                                ", "Please leave 60 blank space !"), //60 espacements pour �largir la fen�tre
                                                         QMessageBox::Close);
                alertBox->setDetailedText( detailedText );
                alertBox->exec();
                delete alertBox;
                return; //On abandonne la g�n�ration !
            }
            //V�rification de la liste des photos d'entr�e et reconstruction des l�gendes si besoin
            onProgressBar( 0, "green", tr("Building photo list.") );
            nbCaptionsRemoved = buildPhotoLists();   //Reconstruction de la liste de photos, cela peut prendre quelques secondes...
         
            if( nbCaptionsRemoved != 0)  //Certaines photos l�gend�es ne sont plus dans le r�pertoire d'entr�e : on informe
            {
                info.setText( tr("The input directory changed.\nThe captions have been automatically reassociated, but the process failed for %n caption(s).",
                                 "Some captions couldn't be remapped", nbCaptionsRemoved) );
                info.setInformativeText( errorMsg );
                info.exec();
            }
            //GENERATION !
            if( m_projectParameters.m_photoPropertiesMap.size() != 0 )
            {
                //Affichage bouton annulation
                swapButtonGenerate( );

                //G�n�ration
                m_p_galleryGenerator->generateGallery(  m_projectParameters,
                                                        m_skinParameters/*, 
                                                        m_captionManager.captionMap()*/ );
            }
            else
            {
                this->m_ui->statusbar->clearMessage();
                info.setText( tr("The input directory is empty !") );
                info.exec();
            }
        }
    }
    else
    {
    	m_ui->pushButton_Generation->setDisabled( true );
        m_ui->statusbar->showMessage( tr("Canceling...") );
        m_p_galleryGenerator->abordGeneration( );
    }


}


//--------------------------------
//--------- Affichage d'une vignette � l�gender
//--------------------------------
void MainWin::displayThumbnail( QModelIndex indexPhotoName )
{
    QVariant photoSelected = m_photosListModel.data( indexPhotoName, Qt::DisplayRole );
    QString photoFilename = photoSelected.toString();
    QFileInfo photoFileInfo( QDir( m_projectParameters.m_galleryConfig.inputDir ), photoFilename );
    onLogMsg( QString("[Thumbnail]. Affichage Vignette: ") + photoFilename  );

    m_ui->label_thumbPhoto->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    
    //Transformation du curseur en sablier car �a peut �tre un peu long...
    QApplication::setOverrideCursor ( Qt::WaitCursor );

    bool f_readSuccess = true;
    bool f_conversionSuccess = true;
    CPhoto photo;
    string filenameLocal8;
    
    if( !photo.load( photoFileInfo.absoluteFilePath( ) ) ){
        onLogMsg( QString("[Thumbnail]." ) + photo.error() );
        f_readSuccess = false;
    }   

    if( f_readSuccess )
    {
        //Resize
        QSize thumbSize = m_ui->label_thumbPhoto->size();
        photo.zoom( thumbSize, Qt::FastTransformation );
        //Orientation
        photo.orientationExif();
        //v�rification que tout est ok, sinon log des messages
        if( !photo.errors().isEmpty() ){
            foreach( QString err, photo.errors() ){
                onLogMsg( QString("[Thumbnail]." ) + err );
            }
            f_readSuccess = false;
        }
        //Affichage
        QImage photoToDisplay = photo.qimage();
        if( !photoToDisplay.isNull() ){
            QPixmap photoPxMap =  QPixmap::fromImage ( photoToDisplay );
            //Affichage
            m_ui->label_thumbPhoto->setPixmap( photoPxMap );            
        }
        else{
            f_conversionSuccess = false;
        }
        //Propagation des donn�es EXIF lues et autres infos au captionManager
        m_captionManager.setExifTags( photoFilename, photo.exifTags() );
        m_captionManager.setFileInfo( photoFilename, photoFileInfo );

    }

    if( !f_conversionSuccess || !f_readSuccess ){
            m_ui->label_thumbPhoto->setText( tr("No preview available.") );
    }

    //Thumbnail checkbox : cocher la case si la photo est la vignette de la galerie
    if( photoFilename == m_projectParameters.m_galleryConfig.thumbPhoto ){
        m_ui->checkBox_GalleryThumb->setCheckState( Qt::Checked );
    }else{
        m_ui->checkBox_GalleryThumb->setCheckState( Qt::Unchecked );
    }

    //Restauration du curseur
    QApplication::restoreOverrideCursor() ;
}

//--------------------------------
//--------- La vignette repr�sentant la gallerie a chang�
//--------------------------------
void MainWin::thumnailChanged( int state )
{
    if( state == Qt::Checked ) {
        m_projectParameters.m_galleryConfig.thumbPhoto = m_captionManager.photo();
    }
}

void MainWin::displayCaption( QString text )
{
    this->m_ui->lineEdit_Caption->setText( text );
}


void MainWin::openSkinDesigner( )
{        
    m_p_skinDesignerWindow->show( );
    m_p_skinDesignerWindow->activateWindow(); //Force la fen�tre au premier plan (utile si d�j� ouverte)
}

void MainWin::skinNameChanged( QString newName )
{
    m_ui->label_SkinName->setText( newName );
}

void MainWin::skinPathChanged( QString filePath )
{
    m_projectParameters.m_galleryConfig.skinPath = filePath;
    onLogMsg( QString("[Skin]. Skin Path: ") + filePath );
}


/************************************************ FIN PUBLIC ***********************************************/



/****************************************** PRIVATE MEMBERS ************************************************/
bool MainWin::checkForGeneration( QString &errorMsg )
{
    QString destDir = QDir::cleanPath( m_ui->lineEdit_DestinationFolder->text() );
    QDir sourceFolder = QDir( QDir::cleanPath(m_ui->lineEdit_SourceFolder->text()) );
    
    if( !sourceFolder.exists( ) ){
        errorMsg = tr("The input directory does not exist.","Check before gallery generation");
        return false;
    }
    if( m_ui->lineEdit_GalleryTitle->text().isEmpty() ){
        errorMsg = tr("You have to define a title for your gallery.","Check before gallery generation");
        return false;
    }
    if( !checkCreateDir( destDir ) ){
        errorMsg = tr("The output directory does not exist.","Check before gallery generation");
        return false;
    }

    return true;
}


void MainWin::swapButtonGenerate( )
{
    if( m_stateButtonGenerate == stateCancel ) {
        m_ui->pushButton_Generation->setText( tr("&Generate") );
        m_stateButtonGenerate = stateGenerate;        
    }
    else{
        m_ui->pushButton_Generation->setText( tr("&Cancel") );
        m_stateButtonGenerate = stateCancel;
    }
}


void MainWin::displayRecentFiles( )
{
    if( !m_recentSessions.isEmpty() )
    {
        int i = 0;
        int nbRecentFiles = m_recentSessions.size();

        for( i = 0; i < nbRecentFiles; i++ ){
            QString fileName = QFileInfo( m_recentSessions.at(i) ).fileName();
            QString text = QString::number(i+1) + QString(" ") + fileName;
            m_recentSessionsActions[i]->setText(text);
            m_recentSessionsActions[i]->setData( m_recentSessions.at(i) );
            m_recentSessionsActions[i]->setVisible(true);
        }

        while( i < NBMAXRECENTFILES ){
            m_recentSessionsActions[i++]->setVisible(false);
        }       
    }
}


/*************************
* int displayUnsavedMsgBox( bool f_sessionUnsaved, bool f_skinUnsaved )
*----------------------
* Affiche une message box indiquant que le projet et/ou la skin n'ont pas �t� savegard�s
* Sauve les modification si c'est le choix de l'utilisateur
* In: (bool) f_sessionUnsaved, (bool) f_skinUnsaved indiquent ce qui n'a pas �t� sauv�.
* Return: le bouton press� par l'utilisateur
**************************/
int MainWin::displayUnsavedMsgBox( bool f_sessionUnsaved, bool f_skinUnsaved )
{
    int ret;
    int unsaved = (((int)f_skinUnsaved) << 1)|(int)f_sessionUnsaved;  //Peux valoir 1,2 ou 3 -> ne marche que si bool vaut 0 ou 1...

    QMessageBox msgBox( this );
    msgBox.setInformativeText( tr("Do you want to save your changes?") );
    msgBox.setIcon( QMessageBox::Information );        
        
    switch( unsaved )
    {
        case 1:   //Session non sauv�e            
            msgBox.setText( tr("Your project has been modified.") );
            msgBox.setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
            msgBox.setDefaultButton(QMessageBox::SaveAll);
            ret = msgBox.exec();
            if( ret == QMessageBox::Save ){
                if( !onSaveSession() ){ //Si on n'a pas pu sauver -> annulation
                    ret = QMessageBox::Cancel; 
                }
            }
            break;
        case 2:   //Skin non sauv�e
            ret = m_p_skinDesignerWindow->displayUnsavedMsgBox( );
            break;
        case 3:   //Skin et session non sauv�es
            msgBox.setText( tr("Your project and your skin have been modified.") );
            msgBox.setStandardButtons( QMessageBox::SaveAll | QMessageBox::Discard | QMessageBox::Cancel );
            msgBox.setDefaultButton(QMessageBox::SaveAll);
            ret = msgBox.exec();
            if( ret == QMessageBox::SaveAll ){
                if( !onSaveSession() || !m_p_skinDesignerWindow->onSaveSkin() ){ //Si on n'a pas pu sauver -> annulation
                    ret = QMessageBox::Cancel; 
                }                
            }
            break;
        default:  //Tout avait �t� sauv�
            ret = QMessageBox::Discard;
            break;
    }  
    return ret;
}

/*************************
* int displayMoreRecentMsgBox( )
*----------------------
* Affiche une alerte si on essaie d'ouvrir un projet g�n�r� avec une version d'EZWG plus r�cente
**************************/
int MainWin::displayMoreRecentMsgBox( )
{
    QMessageBox msgBox( this );
    msgBox.setIcon( QMessageBox::Information );
    msgBox.setText( tr("You are opening a project made with a version of EZWebGallery more recent than this one. Some parameters may get lost.") );
    msgBox.setStandardButtons( QMessageBox::Ok );
    return msgBox.exec();
}

/*************************
* checkPhotosInDir( const QDir &dir )
*----------------------
* V�rifie la pr�sence d'une liste de photos dans un r�pertoire
* Retourne true m�me si des photos suppl�mentaires sont pr�sentes
* Retourne false si il manque une photo
**************************/
QStringList MainWin::checkPhotosInDir( const QStringList& photosInConfig, const QDir &dir )
{
    QStringList photosInDir;
	QStringList photoFileTypes;
    QString photo;
    QStringList missingPhotos;

	photoFileTypes << "*.jpeg" << "*.jpg" << "*.tiff" << "*.tif"; //Formats d'image support�s en entr�e
    photosInDir = CPlatform::getImagesInDir( dir, photoFileTypes );
    /*
    if( photosInConfig.size() > photosInDir.size() ){
        return false;
    }
    foreach( photo, photosInConfig){
        if( !photosInDir.contains( photo, Qt::CaseSensitive ) ){
            return false;
        }
    }*/
    foreach( photo, photosInConfig)
    {
        if( !photosInDir.contains( photo, Qt::CaseSensitive ) ) {
            missingPhotos << photo;
        }
    }

    return missingPhotos;
}

/*************************
* buildPhotoLists
*----------------------
* Parcourt le r�pertoire d'entr�e pour y trouver les photos � traiter et
* met � jour le QMap de CPhotoProperties avec les donn�es disponibles : 
* CPhotoProperties actuelles, l�gendes renseign�es et nouvelles photos
*
* 1 - Cr�ation de la liste des photos dipos dans le r�pertoire d'entr�e.
*     Ajout d'une entr�e dans le QMap des propri�t�s si nouvelle photo, suppression si photo introuvable.
* 2 - MAJ du gestionnaire des l�gendes. Maj UI, et r�cup�ration du nombre de photos introuvables dans le rep d'entr�e.
* 3 - R�cup�ration des l�gendes reseign�es pour MAJ du QMap de properties.
* 4 - MAJ de la vignette repr�sentant la galerie.
*
* Return : le nombre de CPhotosProperties retir�es du QMap car la photo correspondante n'est plus pr�sente dans le r�pertoire d'entr�e.
**************************/
int MainWin::buildPhotoLists( )
{
    QDir inputDir( m_projectParameters.m_galleryConfig.inputDir );
    QStringList photoList;
	QStringList photoFileTypes;
    QStringListIterator* p_photoListIterator;
    QString photoName;
    QFileInfo* p_photoFileInfo;
    QMap<QString,QDateTime> oldPhotosList;
    int nbPhotoNotFound;


    // 1 -Reconstruction de la liste des photos r�ellement pr�sentes dans le r�pertoire d'entr�e    
	photoFileTypes << "*.jpeg" << "*.jpg" << "*.tiff" << "*.tif"; //Formats d'image support�s en entr�e
    photoList = CPlatform::getImagesInDir( inputDir, photoFileTypes ); //R�cup�ration de la liste des photos du r�pertoire d'entr�e
    p_photoListIterator = new QStringListIterator( photoList );
    while( p_photoListIterator->hasNext() ) //On v�rifie que les param�tres des photos sont toujours � jour
    {    
        photoName = p_photoListIterator->next();
        p_photoFileInfo = new QFileInfo( inputDir.absoluteFilePath( photoName ) );
        //Si les param�tres de la galerie ne comportaient pas le fichier, on met � jour et on demande la reg�n�ration
        if( !m_projectParameters.m_photoPropertiesMap.contains( photoName ) )
        {
            CPhotoProperties newProperties;
            newProperties.setFileInfo( *p_photoFileInfo );
            newProperties.setLastModificationTime( p_photoFileInfo->lastModified() );
            m_projectParameters.m_photoPropertiesMap.insert( photoName, newProperties );
            m_projectParameters.m_photosConfig.f_regeneration = true;
            m_projectParameters.m_thumbsConfig.f_regeneration = true;
        }
        //Si les infos de date du fichier sont diff�rentes => on update et on demande la reg�ration �galement
        else
        {
            CPhotoProperties deprecatedProperties = m_projectParameters.m_photoPropertiesMap.value( photoName );
            if(  deprecatedProperties.lastModificationTime().toString() != p_photoFileInfo->lastModified().toString() ) { //Les QDateTime non convertis ne semblent pas bien se comparer ???
                deprecatedProperties.setLastModificationTime( p_photoFileInfo->lastModified() );
                deprecatedProperties.setProcessed( false );
				m_projectParameters.m_photoPropertiesMap.insert( photoName, deprecatedProperties );
                m_projectParameters.m_photosConfig.f_regeneration = true;
                m_projectParameters.m_thumbsConfig.f_regeneration = true;
            }
        }
        onLogMsg( QString("[Photolist]." ) + photoName + QString(": ") + p_photoFileInfo->lastModified().toString() );
        delete p_photoFileInfo;
    }
	//Suppression des entr�es qui ne correspondent pas � une photo trouv�e dans le r�pertoire
    nbPhotoNotFound = 0;
	foreach( photoName, m_projectParameters.m_photoPropertiesMap.keys() ){
		if( !photoList.contains( photoName ) ){
			m_projectParameters.m_photoPropertiesMap.remove( photoName );
            nbPhotoNotFound++;
		}
	}//NOTE: m_projectParameters.m_photoPropertiesMap.keys() correspond maintenant exactement aux photos trouv�es dans le rep d'entr�e    

    //2 - Maj du Caption Manager et affichage dans TAB "L�gendes"
    //- Mise � jour du Mod�le avec la liste des photos trouv�es
    m_photosListModel.setStringList( m_projectParameters.m_photoPropertiesMap.keys() /*photoList*/ ); //Mise  jour du Mod�le pour affichage
    //Mise  jour du gestionnaire de l�gendes : association avec le mod�le mis � jour
    m_captionManager.setPhotoList( m_ui->listView_Photos );
	
    //3 - R�cup�ration des l�gendes
	QMap<QString,CCaption> captionMap = m_captionManager.captionMap();
	foreach( photoName, captionMap.keys() ) {
			CPhotoProperties photoProperties = m_projectParameters.m_photoPropertiesMap.value( photoName );
			photoProperties.setCaption( captionMap.value( photoName ) );
			m_projectParameters.m_photoPropertiesMap.insert( photoName, photoProperties );
	}

    //4 - Vignette r�pr�sentant la galerie
    //Si elle n'est pas dans la galerie -> attribuer la premi�re photo
    if( !photoList.contains( m_projectParameters.m_galleryConfig.thumbPhoto )
        && !photoList.isEmpty() )
    {
        m_projectParameters.m_galleryConfig.thumbPhoto = photoList.at( 0 );
        m_ui->checkBox_GalleryThumb->setChecked( true ); //Comme c'est la 1ere photo qui est affich�e, on coche.
        onLogMsg( QString("[Photolist]. New thumbnail: ") + m_projectParameters.m_galleryConfig.thumbPhoto );
    }

    onLogMsg( QString("[Photolist]. Nb photos found: ") + QString::number( photoList.size() ) );

    delete p_photoListIterator;   
    return nbPhotoNotFound;
}

/*************************
* checkCreateDir
*----------------------
* Affiche une pr�visualisation de la l�gende
* In : (QString) caption - l�gende � afficher
**************************/
void MainWin::previewCaption( QString caption )
{
    this->m_ui->textEdit_captionPreview->setText( caption );
}

/*************************
* checkCreateDir
*----------------------
* V�rifie l'existence d'un r�pertoire et propose sa cr�ation le cas �ch�ant
* Retourne true si le r�pertoire �xiste ou a pu �tre cr��.
* Attention QString & dir peut-�tre MODIFIE !
**************************/
bool MainWin::checkCreateDir( QString & dir ){

    if( dir.isEmpty() ){return false;} //On ressort directement sinon on va cr�er le r�pertoire "/"

    QDir destFolder = QDir( QDir::cleanPath(dir) );

    if( destFolder.exists( ) ){
        return true;
    }
    else{//Le r�pertoire n'�xiste pas
        QMessageBox msgBox(this);

        msgBox.setText( QString(" \"")+QDir::cleanPath(dir)+QString("\": ") + tr("this directory does not exist.") );
        msgBox.setInformativeText( tr("Do you want to create it ?") );
        msgBox.setIcon( QMessageBox::Question );
        msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton( QMessageBox::Yes );

        //L'uilisateur veut cr�er le r�pertoire
        if( msgBox.exec() == QMessageBox::Yes ){
            //Cr�ation du r�pertoire r�ussie
            if( destFolder.mkpath( QDir::cleanPath( dir ) ) ){
                dir = destFolder.absoluteFilePath("");          // => dir est modifi� !!
                return true;
            }
            else{ //Cr�ation �chou�e
                QMessageBox* alertBox = new QMessageBox ( QMessageBox::Critical, tr("Error"),
                                                          CErrorMessages::error(DirectoryCreation) );
                alertBox->exec();
                delete alertBox;
                return false;
            }
        }
        else{//L'utilisateur ne veut pas cr�er le r�pertoire
            return false;
        }

    }
}
