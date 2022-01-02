/* 
 *  EZWebGallery:
 *  Copyright (C) 2011-2022 Christophe Meneboeuf <christophe@xtof.info>
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
#include "CWarning.h"
#include "CError.h"
#include "CMessage.h"
#include "CLogger.h"

using namespace std;


/*********************************************** DEFINES ********************************/
#define qApp (static_cast<QApplication *>(QCoreApplication::instance())) /* pour avoir accès à l'instance de l'application */

/********************************************** PUBLIC MEMBERS ********************************************/



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

//Affiche ou cache les paramètres de sharpening en fonction de la stratégie d'optimisation
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
    if( m_ui->groupBox_Watermark->isChecked() ) //Ne pas activer des widgets alors que le groupbox n'est pas sélectionné !
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
            QList<QWidget *> widgetImageList = m_ui->tab_2_Presentation->findChildren<QWidget *>( selectorImage ); //On ne peut pas sélectionner les enfants à partir du groupbox...
            foreach( widget, widgetImageList ){
                widget->setDisabled( false );
            }
            //Désactiver tous les widgets "watermark texte"   
            QList<QWidget *> widgetTextList = m_ui->tab_2_Presentation->findChildren<QWidget *>( selectorText );
            foreach( widget, widgetTextList ){
                widget->setDisabled( true );
            }
        }
        else //Type texte
        { 
            //Désactiver tous les widgets "watermark images"
            QList<QWidget *> widgetImageList = m_ui->tab_2_Presentation->findChildren<QWidget *>( selectorImage ); //On ne peut pas sélectionner les enfants à partir du groupbox...
            foreach( widget, widgetImageList ){
                widget->setDisabled( true );
            }
            //Activer tous les widgets "watermark texte"   
            QList<QWidget *> widgetTextList = m_ui->tab_2_Presentation->findChildren<QWidget *>( selectorText );
            foreach( widget, widgetTextList ){
                widget->setDisabled( false );
            }
            //Désactiver le CColorPicker en cas de couleur automatique
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
    else if( m_ui->groupBox_Watermark->isChecked() ){ //important de tester le isChecked(), car sinon on peut enabler à mauvais essient
        m_ui->cColorPicker_WatermarkText->setDisabled( false );
    }
}

void MainWin::onProgressBar( int completion, QString color, PtrMessage msg, int timeout )
{
    //La couleur de la barre ne change pas..
    QPalette barPalette = m_ui->progressBar_Generation->palette();
    barPalette.setColor(QPalette::Window, color ); //cf. http://lists.trolltech.com/qt-interest/2001-06/thread00701-0.html
    this->m_ui->progressBar_Generation->setPalette( barPalette );

    this->m_ui->progressBar_Generation->setValue( completion );
    this->m_ui->statusbar->showMessage( msg->message(), timeout );
    CLogger::getInstance().log( PtrMessage(new CMessage( msg->message() )) );
}

void MainWin::onEnablePreview(bool enabled)
{
    m_ui->action_Preview->setEnabled(enabled && !m_projectParameters.m_photosConfig.f_regeneration);
}

void MainWin::onLogMsg( PtrMessage msg )
{
    m_p_logDisplay->setTextColor( msg->color() );
    m_p_logDisplay->append( QDateTime::currentDateTime().toString( "dd.MM.yyyy - hh:mm:ss.zzz\t" ) + msg->message() );
}


void MainWin::onForceStoppedFinished( PtrMessageList errorMessages )
{
    onLogMsg( PtrMessage(new CWarning( tr("Forced Stop") )) );
    //On affiche un message d'erreur
    if( errorMessages.size() > 0){
        
        QString errors;
        for(  PtrMessageList::iterator i = errorMessages.begin(); i < errorMessages.end(); i++ ){
            errors.append( (*i)->message() + QString("\n") );
            onLogMsg( *i );
        }
        QMessageBox* alertBox = new QMessageBox ( QMessageBox::Critical, tr("Error"), errors );
        alertBox->exec();
        delete alertBox;
    }

    if(  m_stateGeneration == eNotGenerating ) //Quand la boite d'erreur s'affiche, on peut entrer deux fois ici :(
    {
        swapButtons( );
        m_ui->progressBar_Generation->setValue( 0 );
        m_ui->pushButton_Generation->setDisabled( false );
        m_ui->statusbar->showMessage( tr("Generation cancelled. Gallery's files are out of sync and have to be produced again.") );
    }
}

//-------------//


//--------------------------------
//------ Gallery successfully generated
//--------------------------------
void MainWin::onGalleryGenerationFinished( QList<CPhotoProperties> propertiesList )
{
    
    foreach( CPhotoProperties photoProperties, propertiesList )    {
        //Updatating files properties in order not to regenerate non-updated photos
        m_photoDatabase.refreshFileInfo( photoProperties.fileName() );
    }
    
    //By default, we consider that the generation was sucessfull for all the photos
    m_projectParameters.m_galleryConfig.f_regeneration = false;
    m_projectParameters.m_photosConfig.f_regeneration = false;
    m_projectParameters.m_thumbsConfig.f_regeneration = false;

    swapButtons( ); //Bouton "cancel" redevient "generate" et réactivation des actions
    m_ui->statusbar->showMessage( tr("Generation successfully completed."), 7000 );

    m_ui->action_Preview->setEnabled(m_p_configureWindow->isPreviewGalleryAllowed());
    m_ui->action_OpenGalleryFolder->setDisabled(false);
    
    //Afterwards action?
    //Preview
    if (m_p_configureWindow->previewGallery() || 
        (m_p_winPreview != nullptr && m_p_winPreview->isVisible()) )   
    {
        onPreviewGallery();
    }
    //Open the folder
    else if (m_p_configureWindow->openGalleryFolder()) {
        onOpenGalleryFolder();
    }
}


/***************************************************************************************************/


MainWin::MainWin( CGalleryGenerator &galleryGenerator, CProjectParameters& projectParameters, QWidget *parent ) :
    QMainWindow( parent ),
    m_ui(new Ui::MainWin), m_p_winPreview(nullptr),
    m_projectParameters( projectParameters ),
    m_photoFeeder( static_cast<CPhotoFeederDirectory&>(CPhotoFeederDirectory::getInstance()) ),
    m_photoDatabase( CPhotoDatabase::getInstance() ),
    m_galleryGenerator( galleryGenerator )
{

    //Pour pouvoir utiliser dans les signaux/slots:
    qRegisterMetaType<CPhotoProperties>("CPhotoProperties");

    setWindowIcon( QIcon(":/icons/app_icon") );
    m_ui->setupUi(this); //"this" hérite de QMainWindow via IUserInterface

    m_lastSelectedDir = QDir::homePath();
    
    m_stateGeneration = eGenerating;  
    
    //Fenêtre de debug
    m_p_logDisplay = new QTextEdit( 0 );
    m_p_logDisplay->setReadOnly( true );
    m_p_logDisplay->setWindowTitle( tr("Logs") );
    m_p_logDisplay->resize(720,480);
    //Fenêtre de tags   
    m_p_tagsWindow = new QTextEdit( 0 );
    m_p_tagsWindow->setReadOnly( true );
    m_p_tagsWindow->setWindowTitle( tr("TAG list") );
    m_p_tagsWindow->resize(720,720);
    m_p_tagsWindow->setHtml( CPlatform::readTranslatedTextFile( "tagList.html" ) );
    //Fenêtre de conception de skin
    m_p_skinDesignerWindow = new WinSkinDesigner( &m_skinParameters /*, this*/ ); //La connection des paramètres avec l'ui se fait dans ce constructeur
    //Fenêtre de Configuration
    m_p_configureWindow = new WinConfigure( this );
    //Légendes
    m_ui->listView_Photos->setModel( (QStringListModel*)( &(m_photoDatabase.model()) ) );

    /**** Connections SIGNAUX/SLOTS ******/  
    //Ouvrir/Fermer/Sauver Session
    connect( this->m_ui->action_NewSession, SIGNAL(triggered()), this, SLOT(onNewSession()));
    connect( this->m_ui->action_OpenSession, SIGNAL(triggered()), this, SLOT(onOpenSession()));
    connect( this->m_ui->action_SaveSession, SIGNAL(triggered()), this, SLOT(onSaveSession()));
    connect( this->m_ui->action_SaveSessionAs, SIGNAL(triggered()), this, SLOT(onSaveSessionAs()));
    connect( this->m_ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()) );
    //Paramètres (=Session)
    connect( &m_projectParameters, SIGNAL(loaded(QString)), this, SLOT(sessionLoaded(QString)));
    connect( &m_projectParameters, SIGNAL(saved(QString)), this, SLOT(sessionSaved(QString)));
    connect( &m_projectParameters, SIGNAL(warning(PtrMessage)), this, SLOT(warning(PtrMessage)));
    //Afficher la fenêtre de tags
    connect( this->m_ui->action_DisplayTags, SIGNAL(triggered()), this, SLOT(showTagsWindow()));
    connect( this->m_ui->pushButton_CaptionShortcuts, SIGNAL(clicked()), this, SLOT(showTagsWindow()));
    connect( this->m_ui->pushButton_WatermarkTextShortcuts, SIGNAL(clicked()), this, SLOT(showTagsWindow()));
    //Afficher la fenêtre de debug    
    connect( this->m_ui->action_ShowLogWindow, SIGNAL(triggered()), this, SLOT(showLogWindow()));
    //Afficher la fenêtre de configuration
    connect( this->m_ui->action_Configure, SIGNAL(triggered()), this, SLOT(showConfigureWindow()));
    //Preview the gallery
    connect(this->m_ui->action_Preview, SIGNAL(triggered()), this, SLOT(onPreviewGallery()));
    connect(this->m_ui->action_OpenGalleryFolder, SIGNAL(triggered()), this, SLOT(onOpenGalleryFolder()));
    //Help
    connect( this->m_ui->action_OnlineManual, SIGNAL(triggered()), this, SLOT(onlineManual()));
    connect( this->m_ui->action_About, SIGNAL(triggered()), this, SLOT(about()));
    connect( this->m_ui->action_AboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect( this->m_ui->action_AboutImageMagick, SIGNAL(triggered()), this, SLOT(aboutImageMagick()));   
    connect( this->m_ui->action_AboutEasyexif, SIGNAL(triggered()), this, SLOT(aboutEasyExif()));
    //Répertoire source
    connect( this->m_ui->pushButton_SourceFolder, SIGNAL(clicked()), this, SLOT(choosePhotosDir()));
    connect( this->m_ui->lineEdit_SourceFolder, SIGNAL(editingFinished()), this, SLOT(choosePhotosDirManually()));
    //Répertoire destination
    connect( this->m_ui->pushButton_DestinationFolder, SIGNAL(clicked()), this, SLOT(chooseDestDir()));
    connect( this->m_ui->lineEdit_DestinationFolder, SIGNAL(editingFinished()), this, SLOT(chooseDestDirManually()));
    //Génération de la gallery
    connect( this->m_ui->pushButton_Generation, SIGNAL(clicked()), this, SLOT( generateGallery() ));
    //Ouverture / Conception d'une skin
    connect( this->m_ui->pushButton_DesignSkin, SIGNAL(clicked( )), this, SLOT( openSkinDesigner() ) );
    connect( this->m_ui->pushButton_OpenSkin, SIGNAL(clicked( )), m_p_skinDesignerWindow, SLOT( onOpenSkin()) );
    connect( &m_skinParameters, SIGNAL(nameChanged(QString)), this, SLOT(skinNameChanged(QString)) );
    connect( &m_skinParameters, SIGNAL(skinOpened(QString)), this, SLOT(skinPathChanged(QString)) );
    connect( &m_skinParameters, SIGNAL(skinSaved(QString)), this, SLOT(skinPathChanged(QString)) );
    //Navigation dans les Thumbnail de "tab Légendes"
    connect( this->m_ui->pushButton_UpdatePhotoList, SIGNAL(clicked()), this, SLOT( refresh() ) );
    connect( this->m_ui->checkBox_GalleryThumb, SIGNAL(stateChanged(int)), this, SLOT(thumnailChanged(int)));
    connect( this->m_ui->pushButton_PrevPhoto, SIGNAL(clicked( )), &(this->m_captionManager), SLOT( onPrevious() ) );
    connect( this->m_ui->pushButton_nextPhoto, SIGNAL(clicked( )), &(this->m_captionManager), SLOT( onNext() ) );
    connect( this->m_ui->listView_Photos, SIGNAL(pressed(QModelIndex)), &(this->m_captionManager), SLOT( onListPressed(QModelIndex)) );
    connect( this->m_ui->lineEdit_Caption, SIGNAL(textEdited(QString)), &(this->m_captionManager), SLOT(onCaptionTextEdited(QString)) );
    connect( this->m_ui->lineEdit_captionEnding, SIGNAL(textEdited(QString)), &(this->m_captionManager), SLOT(onCaptionEndingEdited(QString)) );
    connect( this->m_ui->lineEdit_captionHeader, SIGNAL(textEdited(QString)), &(this->m_captionManager), SLOT(onCaptionHeaderEdited(QString)) );
    connect( &(this->m_captionManager), SIGNAL(displayThumbnailSignal(QModelIndex)), this, SLOT(displayThumbnail(QModelIndex)) );
    connect( &(this->m_captionManager), SIGNAL(displayCaptionSignal(QString)), this, SLOT(displayCaption(QString)) );
    connect( &(this->m_captionManager), SIGNAL(displayPreviewSignal(QString)), this, SLOT(previewCaption(QString)) );
    connect( &(this->m_captionManager), SIGNAL(displayHighlightIndex(QModelIndex)), this, SLOT(highlightPhoto(QModelIndex)) );
    connect( &(this->m_captionManager), SIGNAL(clearThumbnail(void)), this, SLOT(clearThumbnailTab(void)) );
    //Onglet Présentation
    connect( this->m_ui->checkBox_HiDPI, SIGNAL(stateChanged (int)), this, SLOT(onHiDPI(int)) );
    connect( this->m_ui->checkBox_ManualConf, SIGNAL(stateChanged (int)), this, SLOT(onManualPhotoConf(int)) );
    connect( this->m_ui->horizontalSlider_PhotoSharpeningRadius, SIGNAL(valueChanged(int)), this, SLOT(sharpeningRadiusChanged(int)) );
    connect( this->m_ui->doubleSpinBox_PhotoSharpeningRadius, SIGNAL(valueChanged(double)), this, SLOT(sharpeningRadiusChanged(double)) );
    connect( this->m_ui->comboBox_ImageQualityStrategy, SIGNAL(currentIndexChanged(int)), this, SLOT(imageOptimizationStrategyChanged(int)) );
    connect( this->m_ui->comboBox_WatermarkType, SIGNAL(activated(int)), this, SLOT(watermarkTypeChanged(int)) );
    connect( this->m_ui->groupBox_Watermark, SIGNAL(toggled(bool)), this, SLOT(watermarkGroupChecked(bool)) );
    connect( this->m_ui->checkBox_WatermarkTextColorAuto, SIGNAL(stateChanged(int)), this, SLOT(watermarkAutoColorChecked(int)));    
    //Preferences
    connect(m_p_configureWindow, SIGNAL(languageChanged()), &m_languageManager, SLOT(translate()));
    connect(m_p_configureWindow, SIGNAL(enablePreview(bool)), this, SLOT(onEnablePreview(bool)));
    connect(m_p_configureWindow, SIGNAL(embedAnalytics(QString)), this, SLOT(onEmbedAnalytics(QString)));
}

MainWin::~MainWin()
{
    delete m_p_configureWindow;
    delete m_p_skinDesignerWindow;
    if (m_p_winPreview != nullptr) {
        delete m_p_winPreview;
    }
}

void MainWin::init( void )
{
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
    m_newProjectParameters.fromUi( ); //Cela initialise les valeurs à celles par défaut dans l'ui
    
    //Dernières sessions
    QSettings settings;
    m_recentSessions = settings.value( SETTINGS_RECENTPROJECTS ).toStringList();
    for( int i = 0; i < NBMAXRECENTFILES; i++ ){ //Création du tableau d'actions pour ouvrir les fichiers récents
        m_recentSessionsActions[i] = new QAction( this );
        m_ui->menuFichiers->addAction( m_recentSessionsActions[i] );
        connect(m_recentSessionsActions[i], SIGNAL(triggered()),this, SLOT(onOpenRecentSession()));
    }
    displayRecentFiles( );
    
    //UI tweaking
    m_ui->action_SaveSession->setDisabled( true );
    m_ui->action_Preview->setDisabled(true);
    m_ui->action_OpenGalleryFolder->setDisabled(true);

    bool f_lastSessionOK = false;
    //Par défaut : Nouvelle session
    newSession( );
    //m_skinParameters.load( m_projectParameters.m_galleryConfig.skinPath ); //skin par défaut
    //Chargement de la dernière session si configuré ainsi
    if( (bool)settings.value( SETTINGS_OPENMOSTRECENTPJT ).toInt() )
    {
        f_lastSessionOK = !m_recentSessions.isEmpty(); //Sera vide si on lance EZWG pour la première fois
        if( f_lastSessionOK ){ //Une dernière session existe
            QString  fileToLoad = m_recentSessions.first();
            openSession( fileToLoad );
        }
    }
}

void MainWin::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //Il faut sauver / recharger les paramètres, car les combobox reviennent sur les valeurs par défaut
        if( m_projectParameters.initialized() ) {
            m_projectParameters.fromUi();
        }
        if( m_skinParameters.initialized() ) {
            m_skinParameters.fromUi();
        }
        //traductions
        m_ui->retranslateUi(this);
        m_p_skinDesignerWindow->retranslate();
        m_p_configureWindow->retranslate();
        //Restauration des paramètres
        if( m_projectParameters.initialized() ) {
            m_projectParameters.toUi();
        }
        if( m_skinParameters.initialized() ) {
            m_skinParameters.toUi();
        }
        //Traduction des textes supplémentaires (html, etc)
        m_p_tagsWindow->setHtml( CPlatform::readTranslatedTextFile( "tagList.html" ) );
        break;
    default:
        break;
    }
}

//Surcharge de la fermeture de la fenêtre
// -> sauvegardes du projet et de la skin avant de quitter
 void MainWin::closeEvent(QCloseEvent *event)
 {   
    //On regarde ce qui n'a pas été sauvé
    m_projectParameters.fromUi();
    bool f_skinUnsaved = m_p_skinDesignerWindow->isUnsaved();
    bool f_sessionUnsaved = isUnsaved( );
    //Affichage de la boite de dialogue et sauvegardes si choix de l'utilisateur
    int ret = displayUnsavedMsgBox( f_sessionUnsaved || m_photoDatabase.hasChanged(), f_skinUnsaved );
    if( ret == QMessageBox::Cancel ){ //CANCEL -> ne pas fermer
        event->ignore(); 
        return;
    }
                      
    m_p_logDisplay->close();
    m_p_tagsWindow->close();
    m_p_tagsWindow->deleteLater();
    m_p_skinDesignerWindow->close();
    m_p_configureWindow->close();
    if (m_p_winPreview != nullptr) {
        m_p_winPreview->close();
    }

    //fermeture
    event->accept();
 }

/****************************************** SLOTS INTERNES ********************************************/

//--------------------------------
//------ Ouvrir / Sauver une session
//--------------------------------

void MainWin::onNewSession( )
{    
    //Si le projet courant n'a pas été sauvé, on propose de le faire    
    //On propose la sauvegarde de la skin ici et non à l'ouverture effecture de la nouvelle :
    //en effet, il faut pouvoir annuler le chargement de la session et de sa skin très en amont si
    //pb ou annulation lors de la sauvegarde de la skin courante.
    int retDlg = QMessageBox::Ok;
    bool f_sessionUnsaved = isUnsaved();
    bool f_skinUnsaved = m_p_skinDesignerWindow->isUnsaved();
    if( f_sessionUnsaved || f_skinUnsaved || m_photoDatabase.hasChanged() ){
        retDlg = displayUnsavedMsgBox( f_sessionUnsaved || m_photoDatabase.hasChanged(), f_skinUnsaved );
    }
    
    if( retDlg != QMessageBox::Cancel ){
        newSession();
    }
}



void MainWin::newSession( )
{
        m_projectParameters = m_newProjectParameters;
        //Rechargement de la skin par défaut
        m_projectParameters.m_galleryConfig.skinPath = CSkinParameters::defaultSkin();
        m_skinParameters.load( m_projectParameters.m_galleryConfig.skinPath  );         
        m_projectParameters.toUi( );                             //Maj UI
        m_photoFeeder.clear();
        m_photoDatabase.build( m_photoFeeder.getPhotoList() );
        m_captionManager.reset( );
        //saving a reference sate
        m_referenceProjectParameters = m_projectParameters;
        m_photoDatabase.saveState();
        //UI
        m_ui->action_SaveSession->setDisabled( true );    //Il *faut* désactiver l'option save pour ne pas écraser le fichier le plus récent
        m_ui->label_thumbPhoto->clear();                  //Clearing the photo preview
        //Using settings
        const auto id_google_analytics = m_p_configureWindow->defaultGaId();
        if (id_google_analytics.size() != 0u) {
            onEmbedAnalytics(id_google_analytics);
        }
}


void MainWin::onOpenSession( )
{
    QString selectedFilter = QString("*") + QString( XMLSESSIONEXTENSION );
    QString sessionFile  = QFileDialog::getOpenFileName(this,
                                                        tr("Open a project"),
                                                        m_lastSelectedDir,
                                                        selectedFilter,
                                                        &selectedFilter);
    if( !sessionFile.isEmpty() ) { //Vide si cancel, ce qui génèrerait une erreur...
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

    //TODO : ASK THE USER IF HE WANTS TO CANCEL BEFORE LOADING THE PROJECT,
    //THEN LOAD, VERIFY IF MISSING PHOTOS !!

    // Test if the current project is unsaved
    //Si le projet courant n'a pas été sauvé, on propose de le faire    
    //On propose la sauvegarde de la skin ici et non à l'ouverture effecture de la nouvelle :
    //en effet, il faut pouvoir annuler le chargement de la session et de sa skin très en amont si
    //pb ou annulation lors de la sauvegarde de la skin courante.
    int retDlg = QMessageBox::Ok;
    bool f_sessionUnsaved = isUnsaved();
    bool f_skinUnsaved = m_p_skinDesignerWindow->isUnsaved();
    //Affichage de la boite de dialogue si la session courante n'a pas été sauvée, récupération du code retour
    if (f_sessionUnsaved || f_skinUnsaved || m_photoDatabase.hasChanged()) {
        retDlg = displayUnsavedMsgBox(f_sessionUnsaved || m_photoDatabase.hasChanged(), f_skinUnsaved);
    }
    //Si annulation demandée
    if (retDlg == QMessageBox::Cancel) {
        return;
    }

    //Si lecture du fichier réussit
    else if( m_projectParameters.load( sessionFile ) )
    {
        m_lastSelectedDir = QFileInfo(sessionFile).absolutePath();        
        QMessageBox alertBox(this);
        QStringList missingPhotos; 

        //Affichage d'un message si la version du projet est plus récente que l'application
        if( m_projectParameters.version( ) > CPlatform::revisionInt() ){
            displayMoreRecentMsgBox( );
        }
            
        m_captionManager.reset();
       
        //Chargement de la skin
        QList<CError> errors;
        QString skinToLoad = m_projectParameters.m_galleryConfig.skinPath; //On sauve dans une variable car sera changé en "default" si erreur de chargement et donc le err msg affichera "default"
        errors = m_p_skinDesignerWindow->openSkin( skinToLoad );
        //Si des erreurs sont survenues
        if( !errors.isEmpty() )
        {      
            QStringList errorsStr;
            foreach( CError error, errors ) {
                errorsStr << error.message();
                CLogger::getInstance().log( PtrMessage(new CError(error)) );
            }                                                              
            QMessageBox msgBox( this );
            msgBox.setInformativeText( CError::error(CError::SkinOpening) );
            msgBox.setDetailedText( errorsStr.join("\n") );
            msgBox.setIcon( QMessageBox::Information );
            msgBox.exec();                                     
        }
        else {    
            CLogger::getInstance().log( PtrMessage(new CMessage( tr("Skin loaded: ") + m_skinParameters.name())) );
        }            
    }
        
    else //Erreur de chargement de la session
    {   
        CLogger::getInstance().log( PtrMessage(new CError( "[Session]", sessionFile + " not loaded" )) );
        QMessageBox* alertBox = new QMessageBox( QMessageBox::Critical, tr("Error"), CError::error(CError::FileOpening), QMessageBox::Close);
        alertBox->setDetailedText( sessionFile );
        alertBox->exec();
        delete alertBox;
    }
}


//Si possible, sauve la session sur le fichier le plus récent,
//sinon, appelle saveAs()
bool MainWin::onSaveSession( )
{
    QString sessionFile;
    bool fileSaved = false;

    //Par défaut, sauvegarde sur le fichier le plus récent
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
        
        //Si la sauvegarde a échouée, avertissement
        if( !fileSaved ){
                QMessageBox msgBox( this );
                msgBox.setText( tr("The session could not be saved.") );
                msgBox.setIcon( QMessageBox::Warning ); 
                msgBox.setStandardButtons( QMessageBox::Ok );
                msgBox.exec();
        }
    }
    //Mais si le fichier le plus récent n'éxiste pas, on ouvre la boite de dialogue "SaveAs"
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
        //Ajouter l'extension si ça n'a pas été fait par la boite de dialogue
        if( !sessionFile.contains(QString( XMLSESSIONEXTENSION )) ){
            sessionFile.append( QString(XMLSESSIONEXTENSION) );
        }    
    
        m_lastSelectedDir = QFileInfo(sessionFile).absolutePath();
        m_projectParameters.fromUi( );
        fileSaved = m_projectParameters.save( sessionFile );     

        //Si la sauvegarde a échouée, avertissement
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


void MainWin::onHiDPI( int status )
{
    if( status == Qt::Checked ) {
        m_ui->comboBox_ImageQualityStrategy->setCurrentIndex( t_photosConf::OPTIMIZE_SCREENUSAGE );
    }
}

void MainWin::onManualPhotoConf( int status )
{    
    enableAllWidgets( *m_ui->Layout_PhotoConf, (status == Qt::Checked) ); //Enable /disable all configure widgets
}

void MainWin::enableAllWidgets( const QLayout& layout, bool enabled )
{
    for (int i = 0; i < layout.count(); ++i)
    {
        QWidget* widget = layout.itemAt(i)->widget();
        QLayout* inLayout = layout.itemAt(i)->layout();
        if( widget ) {
            widget->setEnabled( enabled );
        } else if( inLayout ) {
                enableAllWidgets( *inLayout, enabled );
        }
    }
}

bool MainWin::isUnsaved()
{
    m_projectParameters.fromUi();
    return ( m_referenceProjectParameters != m_projectParameters || m_captionManager.captionsEdited() );
}



void MainWin::showTagsWindow( )
{
    m_p_tagsWindow->show();
    m_p_tagsWindow->activateWindow(); //Force la fenêtre au premier plan (utile si déjà ouverte)
}



void MainWin::showLogWindow( )
{
    if( !m_p_logDisplay->isVisible() ){
        m_p_logDisplay->show( );
    }
}


void MainWin::showConfigureWindow( )
{
    m_p_configureWindow->exec();
}


void MainWin::onlineManual( )
{
    QUrl url = QUrl( "http://www.ezwebgallery.org/documentation/" );
    QDesktopServices::openUrl( url );
}

void MainWin::about( )
{

    QMessageBox::about( this,
                        tr("About EZWebGallery - build date: ") + CPlatform::revision(),
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

void MainWin::aboutEasyExif()
{
  QFile fileToRead(":/text/aboutEasyExif.html");
  fileToRead.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream textStream(&fileToRead);
  textStream.setCodec("UTF-8");
  QMessageBox::about(this, tr("About easyexif"), 
                      textStream.readAll());
}

//--------------------------------
//------ Paramètres chargés
//--------------------------------
void MainWin::sessionLoaded( QString fileLoaded )
{
    QSettings settings;
    //Setting feeder and updating photo db with new / missing files
    m_photoFeeder.setDirectory( m_projectParameters.m_galleryConfig.inputDir );
    m_photoDatabase.refresh( m_photoFeeder.getPhotoList() );
    //Insertion du fichier dans la liste des fichiers récemment utilisés
    m_recentSessions.removeAll(fileLoaded);
    m_recentSessions.prepend(fileLoaded);
    if( m_recentSessions.size() > NBMAXRECENTFILES ){
        m_recentSessions.removeLast();
    }
    settings.setValue( SETTINGS_RECENTPROJECTS, m_recentSessions );
    //saving a reference state
    m_referenceProjectParameters = m_projectParameters;
    m_photoDatabase.saveState();

    m_captionManager.captionsEditedReset();
    //Affichage
    m_ui->action_SaveSession->setDisabled( false );
    m_projectParameters.toUi( );   

    displayRecentFiles();
    watermarkTypeChanged( m_projectParameters.m_photosConfig.watermark.type );
}
//--------------------------------
//------ Paramètres sauvés
//--------------------------------
void MainWin::sessionSaved( QString fileSaved )
{
    QSettings settings;
    //Insertion du fichier dans la liste des fichiers récemment utilisés
    m_recentSessions.removeAll(fileSaved);
    m_recentSessions.prepend(fileSaved);
    if( m_recentSessions.size() > NBMAXRECENTFILES ){
        m_recentSessions.removeLast();
    }
    settings.setValue( SETTINGS_RECENTPROJECTS, m_recentSessions );
    //saving a reference state
    m_referenceProjectParameters = m_projectParameters;
    m_photoDatabase.saveState();
    m_captionManager.captionsEditedReset();
    //Affichage
    m_ui->action_SaveSession->setDisabled( false );
    displayRecentFiles();
}
//--------------------------------
//------ Sélection Répertoire Source
//--------------------------------
void MainWin::choosePhotosDir()
{
    QString dir  = QFileDialog::getExistingDirectory( this,
                                                      tr("Please select an input directory containing your pictures."),
                                                      m_lastSelectedDir,
                                                      QFileDialog::ShowDirsOnly );
    if( !dir.isEmpty()
        //Si on n'a pas changé de répertoire, lenteurs inutiles dûe au rafrachissement de la vignette dans le "tab légendes"
        && dir!=m_projectParameters.m_galleryConfig.inputDir )
    {
        m_projectParameters.m_galleryConfig.inputDir = dir;
        m_photoFeeder.setDirectory( dir );
        m_photoDatabase.refresh( m_photoFeeder.getPhotoList() );
        m_lastSelectedDir = dir;
        m_ui->lineEdit_SourceFolder->setText( dir );//Affichage
        m_captionManager.reset( );
    }

}
void MainWin::choosePhotosDirManually()
{
    QDir sourceFolder = QDir( QDir::cleanPath(m_ui->lineEdit_SourceFolder->text()) );
    if( sourceFolder.exists( ) ){
        //Si on n'a pas changé de répertoire, lenteurs inutiles dûe au rafrachissement de la vignette dans le "tab légendes"
        if( sourceFolder.absolutePath() != m_photoFeeder.getDirectoryPath() ){
            m_projectParameters.m_galleryConfig.inputDir = sourceFolder.absolutePath();
            m_photoFeeder.setDirectory( sourceFolder );
            m_photoDatabase.refresh( m_photoFeeder.getPhotoList() );
            m_lastSelectedDir =  m_photoFeeder.getDirectoryPath();
            //On créé la liste des jpegs ici pour qu'elle soit dispo sous l'onglet "Légendes"
            //buildPhotoLists( );
            m_captionManager.reset( );
        }
    }
    else{
        m_ui->lineEdit_SourceFolder->setText( tr("Invalid directory.") );
        m_projectParameters.m_galleryConfig.inputDir.clear(); //Sinon bug lorsque l'on rechoisi le répertoire d'avant
        m_photoFeeder.setDirectory( QDir::home() );
        m_photoDatabase.build( m_photoFeeder.getPhotoList() );
    }
}
//--------------------------------
//----------- Sélection Répertoire Destination
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
//--------- Génération de la gallerie
//--------------------------------
void MainWin::generateGallery( )
{    
    QString errorMsg;
    QMessageBox info(this);

    info.setStandardButtons( QMessageBox::Ok );
    info.setIcon( QMessageBox::Information );

    //La génération n'est pas en cours -> la lancer
    if( !m_galleryGenerator.isGenerationInProgress() )
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
            //MAJ des paramètres
            m_projectParameters.fromUi( );
            m_skinParameters.fromUi( );
            //Vérification de la skin et affichage d'une infobox si pb
            m_skinParameters.check( &skinErrorList, &skinWarningList );
            if( !skinErrorList.isEmpty() )
            {
                QString detailedText;
                //Détail des erreurs
                foreach( QString error, skinErrorList ){
                    detailedText.append( error );
                    detailedText.append( "\n" );
                }
                QMessageBox* alertBox = new QMessageBox( QMessageBox::Critical, tr("Error"),
                                                         tr("Your skin is not valid.                                                                                ", "Please leave 60 blank space !"), //60 espacements pour élargir la fenêtre
                                                         QMessageBox::Close);
                alertBox->setDetailedText( detailedText );
                alertBox->exec();
                delete alertBox;
                return; //On abandonne la génération !
            }
            //Vérification de la liste des photos d'entrée et reconstruction des légendes si besoin
            onProgressBar( 0, "green", PtrMessage(new CMessage(tr("Building photo list."))) );
            QStringList photosModified = m_photoDatabase.photosModified(); //to be executed before refresh() or you'll miss when some photo are removed
            QStringList newPhotos = m_photoDatabase.refresh( m_photoFeeder.getPhotoList() );
            m_captionManager.reset();
            m_projectParameters.m_photosConfig.f_regeneration |= ( !photosModified.isEmpty() || !newPhotos.isEmpty() );
            
            //GENERATION !
            if( m_photoDatabase.size() != 0 )
            {
                emit generationInProgress();

                //Affichage bouton annulation et désativation de certaines actions
                swapButtons( );

                //Generating a list of the photo properties
                QList<CPhotoProperties*> propertyList;
                for( int i = 0; i < m_photoDatabase.size(); i++ ) {
                    propertyList << m_photoDatabase.properties(i);
                }

                //Génération
                m_galleryGenerator.generateGallery( CGalleryGeneratorFeeder( m_projectParameters, m_skinParameters, propertyList ) );
            }
            else
            {
                this->m_ui->statusbar->clearMessage();
                info.setText( tr("No photo to process!") );
                info.exec();
            }
        }
    }
    else
    {
    	m_ui->pushButton_Generation->setDisabled( true );
        m_ui->statusbar->showMessage( tr("Canceling...") );
        m_galleryGenerator.abordGeneration( );
    }


}


//--------------------------------
//--------- Affichage d'une vignette à légender
//--------------------------------
void MainWin::displayThumbnail( QModelIndex indexPhotoName )
{
    QVariant photoSelected = m_photoDatabase.model().data( indexPhotoName, Qt::DisplayRole );
    QString photoFilename = photoSelected.toString();
    QFileInfo photoFileInfo( m_photoFeeder.getDirectory(), photoFilename );
    CLogger::getInstance().log( PtrMessage(new CMessage( tr("Displaying thumbnail: ") + photoFilename  )) );

    m_ui->label_thumbPhoto->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
   // m_ui->label_thumbPhoto->setScaledContents(true);
   
    if( m_photoDatabase.contains( photoFilename ) )
    {
        const QSize size{ m_ui->label_thumbPhoto->width(), m_ui->label_thumbPhoto->height() };
        const QImage& photoToDisplay = m_photoDatabase.thumbnail( indexPhotoName.row() );
        QPixmap photoPxMap = QPixmap::fromImage(photoToDisplay.scaled(size, Qt::AspectRatioMode::KeepAspectRatio));
        m_ui->label_thumbPhoto->setPixmap( photoPxMap ); //Displaying the thumb
    }
    else{
       m_ui->label_thumbPhoto->setText( tr("No preview available.") ); //Displaying a default text
    }

    //Thumbnail checkbox : cocher la case si la photo est la vignette de la galerie
    if( photoFilename == QFileInfo (m_projectParameters.m_galleryConfig.thumbPhoto).fileName() ){
        m_ui->checkBox_GalleryThumb->setCheckState( Qt::Checked );
    }else{
        m_ui->checkBox_GalleryThumb->setCheckState( Qt::Unchecked );
    }
}


//--------------------------------
//--------- Clearing the fields of the thumbnail tab
//--------------------------------
void MainWin::clearThumbnailTab( void )
{
    CLogger::getInstance().log( PtrMessage( new CMessage(tr("Clearing thumbnails"))) );
    m_ui->label_thumbPhoto->clear();
    m_ui->textEdit_captionPreview->clear();
    m_ui->lineEdit_Caption->clear();
    m_ui->lineEdit_captionEnding->clear();
    m_ui->lineEdit_captionHeader->clear();
}


//--------------------------------
//--------- La vignette représentant la gallerie a changé
//--------------------------------
void MainWin::thumnailChanged( int state )
{
    if( state == Qt::Checked ) {
        m_projectParameters.m_galleryConfig.thumbPhoto = m_photoDatabase.properties( m_captionManager.selectedPhoto() )->fileInfo().absoluteFilePath();
    }
}

void MainWin::displayCaption( QString text )
{
    m_ui->lineEdit_Caption->setText( text );
}

void MainWin::error( PtrMessage err )
{
    CLogger::getInstance().log( err );
    QMessageBox modalErrorBox( this );
    modalErrorBox.setText( err->summary() );
    modalErrorBox.setDetailedText( err->details() );
    modalErrorBox.setIcon( QMessageBox::Critical );
    modalErrorBox.exec();
}


void MainWin::warning( PtrMessage warning )
{
    CLogger::getInstance().log( warning );
    QMessageBox modalErrorBox( this );
    modalErrorBox.setText( warning->summary() );
    modalErrorBox.setDetailedText( warning->details() );
    modalErrorBox.setIcon( QMessageBox::Warning );
    modalErrorBox.exec();
}


void MainWin::information( PtrMessage info )
{
    CLogger::getInstance().log( info );
    QMessageBox modalErrorBox( 0 );
    modalErrorBox.setText( info->summary() );
    modalErrorBox.setDetailedText( info->details() );
    modalErrorBox.setIcon( QMessageBox::Information );
    modalErrorBox.exec();
}


void MainWin::onPreviewGallery(void)
{
    QString indexPath = QDir(m_projectParameters.m_galleryConfig.outputDir).absoluteFilePath("index.html");
    if (m_p_winPreview == nullptr) {
        m_p_winPreview = new WinPreview();
        connect(m_p_winPreview, SIGNAL(refresh()), this, SLOT(generateGallery()));
        connect(m_p_winPreview, SIGNAL(enabled(bool)), this->m_ui->action_Preview, SLOT(setEnabled(bool)));
        connect(this, SIGNAL(generationInProgress()), m_p_winPreview, SLOT(onDisable()));
    }
    m_p_winPreview->show(indexPath);
}


void MainWin::onEmbedAnalytics(QString id)
{
    m_ui->checkBox_Analytics->setChecked(true);
    if (m_ui->lineEdit_GaTrackingId->text().size() == 0u) {
        m_ui->lineEdit_GaTrackingId->setText(id);
    }
}


void MainWin::onOpenGalleryFolder(void)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_projectParameters.m_galleryConfig.outputDir));
}



void MainWin::openSkinDesigner( )
{        
    m_p_skinDesignerWindow->show( );
    m_p_skinDesignerWindow->activateWindow(); //Force la fenêtre au premier plan (utile si déjà ouverte)
}

void MainWin::skinNameChanged( QString newName )
{
    m_ui->label_SkinName->setText( newName );
}

void MainWin::skinPathChanged( QString filePath )
{
    m_projectParameters.m_galleryConfig.skinPath = filePath;
    CLogger::getInstance().log( PtrMessage(new CMessage(tr("Skin Path: ") + filePath )) );
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


void MainWin::swapButtons( )
{
    if( m_stateGeneration == eNotGenerating ) {
        m_ui->pushButton_Generation->setText( tr("&Generate") );        
        m_ui->action_NewSession->setEnabled( true );
        m_ui->action_OpenSession->setEnabled( true );
        m_stateGeneration = eGenerating;
    }
    else{
        m_ui->pushButton_Generation->setText( tr("&Cancel") );        
        m_ui->action_NewSession->setEnabled( false );
        m_ui->action_OpenSession->setEnabled( false );
        m_stateGeneration = eNotGenerating;
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
* Affiche une message box indiquant que le projet et/ou la skin n'ont pas été savegardés
* Sauve les modification si c'est le choix de l'utilisateur
* In: (bool) f_sessionUnsaved, (bool) f_skinUnsaved indiquent ce qui n'a pas été sauvé.
* Return: le bouton pressé par l'utilisateur
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
        case 1:   //Session non sauvée            
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
        case 2:   //Skin non sauvée
            ret = m_p_skinDesignerWindow->displayUnsavedMsgBox( );
            break;
        case 3:   //Skin et session non sauvées
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
        default:  //Tout avait été sauvé
            ret = QMessageBox::Discard;
            break;
    }  
    return ret;
}

/*************************
* int displayMoreRecentMsgBox( )
*----------------------
* Affiche une alerte si on essaie d'ouvrir un projet généré avec une version d'EZWG plus récente
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
* Vérifie la présence des photos de la base dans le répertoire fourni
* Retourne la liste des photos manquantes
**************************/
QStringList MainWin::checkPhotosInDir( const QDir &dir )
{
    QStringList photosInDir;
	QStringList photoFileTypes;
    QString photo;
    QStringList missingPhotos;

	photoFileTypes << "*.jpeg" << "*.jpg" << "*.tiff" << "*.tif"; //Formats d'image supportés en entrée
    photosInDir = CPlatform::getImagesInDir( dir, photoFileTypes );

    for( int i = 0; i < m_photoDatabase.size(); i++ )
    {   
        if( !photosInDir.contains( m_photoDatabase.properties(i)->fileName(), Qt::CaseSensitive ) ) {
            missingPhotos << photo;
        }
    }

    return missingPhotos;
}



/*************************
* refresh
*----------------------
* Updates the database with a fresh list from the feeder
**************************/
void MainWin::refresh( void )
{
    m_photoDatabase.refresh( m_photoFeeder.getPhotoList() );
    m_captionManager.reset();
}

/*************************
* highlightPhoto( QModelIndex )
*----------------------
* Highlights a photo in the photo ListView
**************************/
void MainWin::highlightPhoto( QModelIndex modelIndex )
{
    m_ui->listView_Photos->setCurrentIndex( modelIndex );
}


/*************************
* previewCaption
*----------------------
* Affiche une prévisualisation de la légende
* In : (QString) caption - légende à afficher
**************************/
void MainWin::previewCaption( QString caption )
{
    this->m_ui->textEdit_captionPreview->setText( caption );
}

/*************************
* checkCreateDir
*----------------------
* Vérifie l'existence d'un répertoire et propose sa création le cas échéant
* Retourne true si le répertoire éxiste ou a pu être créé.
* Attention QString & dir peut-être MODIFIE !
**************************/
bool MainWin::checkCreateDir( QString & dir ){

    if( dir.isEmpty() ){return false;} //On ressort directement sinon on va créer le répertoire "/"

    QDir destFolder = QDir( QDir::cleanPath(dir) );

    if( destFolder.exists( ) ){
        return true;
    }
    else{//Le répertoire n'éxiste pas
        QMessageBox msgBox(this);

        msgBox.setText( QString(" \"")+QDir::cleanPath(dir)+QString("\": ") + tr("this directory does not exist.") );
        msgBox.setInformativeText( tr("Do you want to create it ?") );
        msgBox.setIcon( QMessageBox::Question );
        msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton( QMessageBox::Yes );

        //L'uilisateur veut créer le répertoire
        if( msgBox.exec() == QMessageBox::Yes ){
            //Création du répertoire réussie
            if( destFolder.mkpath( QDir::cleanPath( dir ) ) ){
                dir = destFolder.absoluteFilePath("");          // => dir est modifié !!
                return true;
            }
            else{ //Création échouée
                QMessageBox* alertBox = new QMessageBox ( QMessageBox::Critical, tr("Error"),
                                                          CError::error(CError::DirectoryCreation) );
                alertBox->exec();
                delete alertBox;
                return false;
            }
        }
        else{//L'utilisateur ne veut pas créer le répertoire
            return false;
        }

    }
}
