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


#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QMap>
#include <QSettings>

#include "WinSkinDesigner.h"
#include "ui/ui_WinSkinDesigner.h" //Gnr par qmake
#include "GlobalDefinitions.h"
#include "CPlatform.h"


//--- Variables statiques ----//
const QString WinSkinDesigner::WindowTitle = QString(QObject::tr("Skin Designer"));
const QStringList WinSkinDesigner::FontFamilies = ( QStringList()<< QString("Georgia, \"DejaVu Serif\", Norasi, serif") // Valeur par dfaut
                                                                 << QString("Times, \"Times New Roman\", \"Liberation Serif\", FreeSerif, serif")
                                                                 << QString("Verdana, \"DejaVu Sans\", \"Bitstream Vera Sans\", Geneva, sans-serif")
                                                                 << QString("Arial, Helvetica, \"Liberation Sans\", FreeSans, sans-serif")
                                                                 << QString("\"Comic Sans MS\", \"Comic Sans MS5\", cursive") );
const QStringList WinSkinDesigner::FontVariants = ( QStringList()<< QString("normal") // Valeur par dfaut
                                                                 << QString("small-caps") );
const QStringList WinSkinDesigner::FontWeights =  ( QStringList()<< QString("normal") // Valeur par dfaut
                                                                 << QString("bold")
                                                                 << QString("bolder")
                                                                 << QString("lighter") );

//--- METHODES ---/

WinSkinDesigner::WinSkinDesigner( CSkinParameters* skinParameters, QWidget *parent )
    : QMainWindow( parent ), m_ui(new Ui::SkinDesigner)
{
    //UI
    m_ui->setupUi(this);
    //Filling the text comboboxes
    m_ui->comboBox_Box_FontFamily->insertItems( 0, WinSkinDesigner::FontFamilies );
    m_ui->comboBox_PhotoCaption_FontFamily->insertItems( 0, WinSkinDesigner::FontFamilies );
    //m_ui->comboBox_PhotoTitle_FontFamily->insertItems( 0, WinSkinDesigner::FontFamilies );
    m_ui->comboBox_VTitleText_FontFamily->insertItems( 0, WinSkinDesigner::FontFamilies );
    m_ui->comboBox_PhotoCaption_FontWeight->insertItems( 0, WinSkinDesigner::FontWeights );
    //m_ui->comboBox_PhotoTitle_FontWeight->insertItems( 0, WinSkinDesigner::FontWeights );
    //m_ui->comboBox_PhotoTitle_FontVariant->insertItems( 0, WinSkinDesigner::FontVariants );
 
    //Empty skin
    m_emptySkin.setUi( m_ui );
    m_emptySkin.fromUi( );
    //Working Skin
    m_p_parameters = skinParameters;
    m_p_parameters->setUi( m_ui );

    QSettings settings;
    m_recentSkins = settings.value( SETTINGS_RECENTSKINS ).toStringList();

    //Fichiers rcents
    for( int i = 0; i < NBMAXRECENTFILES; i++ ){ //Cration du tableau d'actions pour ouvrir les fichiers rcents
        m_recentSkinssActions[i] = new QAction( this );
        m_ui->menuFile->addAction( m_recentSkinssActions[i] );
        connect(m_recentSkinssActions[i], SIGNAL(triggered()),this, SLOT(onOpenRecentSkin()));
    }
    displayRecentFiles( );
    m_lastSelectedDir = CPlatform::skinDirPath();

    //Dsactivation de l'action "Save"
    m_ui->action_Save->setDisabled( true );

    //--- Connections ---//
    //Nouveau/Ouvrir/Sauver Skin
    connect( this->m_ui->action_New, SIGNAL(triggered()), this, SLOT(onNewSkin()));
    connect( this->m_ui->action_Open, SIGNAL(triggered()), this, SLOT(onOpenSkin()));
    connect( this->m_ui->action_Save, SIGNAL(triggered()), this, SLOT(onSaveSkin()));
    connect( this->m_ui->action_SaveAs, SIGNAL(triggered()), this, SLOT(onSaveSkinAs()));
    connect( this->m_ui->action_Check_Skin, SIGNAL(triggered()), this, SLOT(checkSkin()));    
    connect( skinParameters, SIGNAL(skinOpened(QString)), this, SLOT(changeWindowTitle(QString)));
    connect( skinParameters, SIGNAL(skinSaved(QString)), this, SLOT(changeWindowTitle(QString)));
    connect (skinParameters, SIGNAL(skinSaved(QString)), this, SLOT(skinSaved(QString)));
    connect (skinParameters, SIGNAL(skinOpened(QString)), this, SLOT(skinLoaded(QString)));
}

WinSkinDesigner::~WinSkinDesigner( )
{

}

bool WinSkinDesigner::isUnsaved( )
{
    m_p_parameters->fromUi();
    if( m_reference == *m_p_parameters ){
        return false;
    }
    else{
        return true;
    }
}



/*******************************************************************
* openSkin( const QString &fileToLoad )
* ------------------------
* Sauvegarde la skin sur le disque. Copie les fichiers ressources dans
* l'arborescence de la skin.
* in  : (QString) fichier  ouvrir
* return : (QStringList) Liste des erreurs survenue. Vide si aucune
********************************************************************/
QStringList WinSkinDesigner::openSkin( const QString &fileToLoad )
{
    QStringList errors;

     if( !m_p_parameters->load( fileToLoad ) )
    {
        //Chargement chou
        errors << m_p_parameters->error();       

        //On bascule sur la skin par dfaut
        m_p_parameters->setName( "default" );                  
        m_p_parameters->load(  CSkinParameters::defaultSkin() );
    }

    return errors;
}


int WinSkinDesigner::displayUnsavedMsgBox( )
{
    QMessageBox msgBox( this );
    msgBox.setIcon( QMessageBox::Information );
    msgBox.setText( tr("Your skin has been modified.") );
    msgBox.setInformativeText( tr("Do you want to save your changes?") );
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    if( ret == QMessageBox::Save ){
        if( !onSaveSkin() ){ //Si la sauvegarde a chou, on fait comme si on avait appuy sur Cancel
            ret = QMessageBox::Cancel; 
        }
    }
    return ret;
}

int WinSkinDesigner::displayMoreRecentMsgBox( )
{
    QMessageBox msgBox( this );
    msgBox.setIcon( QMessageBox::Information );
    msgBox.setText( tr("You are opening a skin made with a version of EZWebGallery more recent than this one. Some parameters may get lost.") );
    msgBox.setStandardButtons( QMessageBox::Ok );
    return msgBox.exec();
}

void WinSkinDesigner::retranslate( )
{
    m_ui->retranslateUi( this );
}

void WinSkinDesigner::closeEvent( QCloseEvent *evt )
{
    m_p_parameters->fromUi( );
    evt->accept();
}


void WinSkinDesigner::displayRecentFiles( )
{
    int i = 0;
    int nbRecentFiles = m_recentSkins.size();
    for( i = 0; i < nbRecentFiles; i++ ) {
        QString fileName = QFileInfo( m_recentSkins.at(i) ).fileName();
        QString text = QString::number(i+1) + QString(" ") + fileName;
        m_recentSkinssActions[i]->setText(text);
        m_recentSkinssActions[i]->setData( m_recentSkins.at(i) );
        m_recentSkinssActions[i]->setVisible(true);
    }
    while( i < NBMAXRECENTFILES ){
        m_recentSkinssActions[i++]->setVisible(false);
    }       
}


bool WinSkinDesigner::saveSkinFile( QString skinFileName )
{
    if( !skinFileName.isEmpty() )
    {
        //Ajouter l'extension si il n'y en a pas
        if( !skinFileName.contains( QString(SKINSESSIONEXTENSION) ) ){
            skinFileName.append( QString(SKINSESSIONEXTENSION) );
        }

        /*m_p_parameters->saveSkin(skinFileName , errorMsgs);*/

        //Grer une ventuelle erreur
        if( !m_p_parameters->saveSkin(skinFileName) )
        {
            QStringList errorMsgs = m_p_parameters->errors();
            QString errorsDetail;
            
            foreach( QString error, errorMsgs){
                errorsDetail.append( error + QString("\n") );
            }

            QMessageBox* alertBox = new QMessageBox( QMessageBox::Critical, tr("Error"),
                                                     CError::error(CError::FileSaving),
                                                     QMessageBox::Close );
            alertBox->setDetailedText( errorsDetail );
            alertBox->exec();
            delete alertBox;
            return false;
        }        
    }
    else{
        return false;
    }

    return true;

}


/***************** SLOTS ***************/
void WinSkinDesigner::onNewSkin( )
{
    int ret = QMessageBox::Ok;
    m_p_parameters->fromUi();
    if( isUnsaved() ){
       ret = displayUnsavedMsgBox();
    }
    if( ret != QMessageBox::Cancel ){
        m_emptySkin.toUi();        
        m_reference = *m_p_parameters;
        m_ui->action_Save->setDisabled( true ); //dsactivation de l'action "Save"
    }
}

void WinSkinDesigner::onOpenRecentSkin( )
{
    QString fileToLoad;
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) //=0 si le sender n'est pas une action
    { 
        //On regarde si la skin avait bien t sauve    
        int ret = QMessageBox::Ok;
        if( isUnsaved() ){
            ret = displayUnsavedMsgBox();
        }
        if( ret != QMessageBox::Cancel )
        {
            fileToLoad = action->data().toString();        
            m_p_parameters->load( fileToLoad ); //<- faudrait checker erreur...
            //Affichage d'un message si la version de la skin est plus rcente que l'application
            if( m_p_parameters->version( ) > CPlatform::revisionInt() ){
                displayMoreRecentMsgBox( );
            }
            //Le fichier ouvert a t insr, par le slot skinLoaded, dans la liste des derniers fichiers.
            //Utilisation de cette liste pour mise  jour du dernier rpertoire utilis.
            QString lastFile = m_recentSkins.first();
            if( QFileInfo(lastFile).isFile() ){
                m_lastSelectedDir = QFileInfo(lastFile).absolutePath();
            }
        }
    }    
}


void WinSkinDesigner::onOpenSkin( )
{
    //On regarde si la skin avait bien t sauve    
    int ret = QMessageBox::Ok;
    if( isUnsaved() ){
        ret = displayUnsavedMsgBox();
    }
    
    if( ret != QMessageBox::Cancel )
    {

        QString lastDir = m_lastSelectedDir;
        QString selectedFilter = QString("*") + QString( SKINSESSIONEXTENSION );
        QString skinFile  = QFileDialog::getOpenFileName(0,                         //0 -> permet d'appeler cette fonction en dehors de WinSkinDesigner::
                                                         tr("Choose a skin to open."),
                                                        lastDir,
                                                        selectedFilter,
                                                        &selectedFilter);
        //Chargement de la skin
        if( !skinFile.isEmpty() )
        {
            m_p_parameters->load( skinFile ); //<- Faudrait checker erreur
            //Affichage d'un message si la version de la skin est plus rcente que l'application
            if( m_p_parameters->version( ) > CPlatform::revisionInt() ){
                displayMoreRecentMsgBox( );
            }
        }

        //Le fichier ouvert a t insr, par le slot skinLoaded, dans la liste des derniers fichiers.
        //Utilisation de cette liste pour mise  jour du dernier rpertoire utilis.
        lastDir = m_recentSkins.first();
        if( QFileInfo(lastDir).isFile() ){
            m_lastSelectedDir = QFileInfo(lastDir).absolutePath();
        }
    }

}



bool WinSkinDesigner::onSaveSkinAs( )
{
    QString skinFileName = QFileDialog::getSaveFileName(this,
                                                        tr("Choose a file to save your skin."),
                                                        m_recentSkins.first(),
                                                        tr("Skin Files")+QString(" *")+QString(SKINSESSIONEXTENSION));
    return saveSkinFile( skinFileName );
}

//Si possible, sauve la skin sur le fichier le plus rcent
//Sinon appelle saveAs
bool WinSkinDesigner::onSaveSkin()
{
    QString skinFileName = m_recentSkins.first();
    if( !skinFileName.isEmpty()
        && QFileInfo( skinFileName ).exists()
        && m_ui->action_Save->isEnabled() ){
        return saveSkinFile( skinFileName );
    }
    else{ //Si les condictions ne sont pas runies, on ouvre la boite de dialogue "SaveAS"
        return onSaveSkinAs( );    
    }
}


void WinSkinDesigner::checkSkin( )
{
    QStringList warningList;
    QStringList errorList;
    QMessageBox* infoBox;
    QString detailedText;

    m_p_parameters->check( &errorList, &warningList );

    //Dtail des erreurs
    foreach( QString error, errorList ){
        detailedText.append( tr("Error: ") );
        detailedText.append( error );
        detailedText.append( "\n" );
    }
    //Dtail des warnings
    foreach( QString warning, warningList ){
        detailedText.append( tr("Warning: ") );
        detailedText.append( warning );
        detailedText.append( "\n" );
    }

    //Si erreurs -> affichage d'une boite de dialogue dtaillant les pbs
    if( !errorList.isEmpty() )
    {
        infoBox = new QMessageBox( QMessageBox::Critical,
                                   tr("Failure"),
                                   tr("Your skin is not valid.                                                                                ", "Please leave 60 blank space !"), //60 espacements pour largir la fentre
                                   QMessageBox::Close);                                                                //dont on ne peut pas prciser la taille : http://bugreports.qt.nokia.com/browse/QTBUG-3696
        infoBox->setDetailedText( detailedText );
    }
    //Si warnings -> affichage d'une boite de dialogue dtaillant les pbs
    else if( !warningList.isEmpty() ){
        infoBox = new QMessageBox( QMessageBox::Warning,
                                   tr("Warnings"),
                                   tr("Some warnings were found.                                                                                ", "Please leave 60 blank space !"), //60 espacements pour largir la fentre
                                   QMessageBox::Close);                                                                //dont on ne peut pas prciser la taille : http://bugreports.qt.nokia.com/browse/QTBUG-3696
        infoBox->setDetailedText( detailedText );
    }
    //Sinon -> on informe que tout va bien
    else
    {
        infoBox = new QMessageBox( QMessageBox::Information,
                                   tr("Success"),
                                   tr("Your skin is valid !"),
                                   QMessageBox::Close);
    }

    infoBox->exec();
    delete infoBox;
}

void WinSkinDesigner::changeWindowTitle( QString skinFileName )
{
    this->setWindowTitle( WindowTitle + QString(" - ") + QFileInfo(skinFileName).fileName() );
}

void WinSkinDesigner::skinLoaded( QString fileLoaded )
{    
    QSettings settings;
    //Mise  jour de l'UI
    m_ui->action_Save->setDisabled( false );
    m_p_parameters->toUi();
    //Insertion du fichier dans la liste des fichiers rcemment utiliss
    m_recentSkins.removeAll(fileLoaded);
    m_recentSkins.prepend(fileLoaded);
    if( m_recentSkins.size() > NBMAXRECENTFILES ){
        m_recentSkins.removeLast();
    }
    settings.setValue( SETTINGS_RECENTSKINS, m_recentSkins );
    //Affichage des fichiers rcents
    displayRecentFiles();
    //Mise  jour de la skin de "rfrence"
    m_reference = *m_p_parameters;
    
}

void WinSkinDesigner::skinSaved( QString fileLoaded )
{   
    QSettings settings;
    //Mise  jour de l'UI
    m_ui->action_Save->setDisabled( false );
    //Insertion du fichier dans la liste des fichiers rcemment utiliss
    m_recentSkins.removeAll(fileLoaded);
    m_recentSkins.prepend(fileLoaded);
    if( m_recentSkins.size() > NBMAXRECENTFILES ){
        m_recentSkins.removeLast();
    }
    settings.setValue( SETTINGS_RECENTSKINS, m_recentSkins );
    //Affichage des fichiers rcents
    displayRecentFiles();
    //Mise  jour de la skin de "rfrence"
    m_reference = *m_p_parameters;

    //Le fichier ouvert vient d'tre insr dans la liste des fichiers rcents.
    //Utilisation de cette liste pour mise  jour du dernier rpertoire utilis.
    QString lastFile = m_recentSkins.first();
    if( QFileInfo(lastFile).isFile() ){
        m_lastSelectedDir = QFileInfo(lastFile).absolutePath();
    }
}
