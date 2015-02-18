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


#include <QSettings>
#include <QStringList>
#include <QPushButton>

#include "WinConfigure.h"
#include "GlobalDefinitions.h"
#include "CPlatform.h"
#include "CLanguageManager.h"

/*************************
* Constructeur
*************************/
WinConfigure::WinConfigure( QWidget* parent )
    : QDialog( parent ), m_ui( new Ui::Configure )
{
    //UI
    //Init
    m_ui->setupUi( this ); //Obligatoire en tte d'init d'ui!
    initLanguageCombobox();

    //Recupration des settings
    QSettings settings;
    if( settings.contains( SETTINGS_OPENMOSTRECENTPJT ) ) {
        bool openPjt = (settings.value(SETTINGS_OPENMOSTRECENTPJT).toInt() == 1);
        m_ui->checkBox_RecentProject->setChecked(openPjt);
    }

    //---- Connnections
    connect( this->m_ui->buttonBox, SIGNAL(accepted( )), this, SLOT(onOK()) );
    connect( this->m_ui->buttonBox, SIGNAL(rejected( )), this, SLOT(onCancel()) );

}


/*************************
* Destructeur
*************************/
WinConfigure::~WinConfigure()
{

}

/*************************
* exec( )
* ---------------
* Surcharge de QDialog::exec() afin d'initialiser la slection des comboboxs
*************************/
int WinConfigure::exec()
{
    //language
    int currentLangIndex;
    currentLangIndex = m_ui->comboBox_Language->findText( CLanguageManager::currentLanguage() );
    m_ui->comboBox_Language->setCurrentIndex( currentLangIndex );
    //action after generation
    QSettings settings;
    if (settings.contains(SETTINGS_AFTERGENERATION)) { //On doit test si la clef est contenue : ce n'est pas le cas au 1er lancement...
        m_ui->comboBox_AfterProduction->setCurrentIndex(settings.value(SETTINGS_AFTERGENERATION).toInt());
    }

    return QDialog::exec();
}


/*************************
* initLanguageCombobox
* -----------
* Initialise la combobox de choix de langue
*************************/
void WinConfigure::initLanguageCombobox()
{
    QStringList languages = CLanguageManager::supportedLanguages();
    languages.sort(); //Tri par ordre alphabtique
    m_ui->comboBox_Language->clear();
    m_ui->comboBox_Language->addItems( languages );
}


/*************************
* retranslate
* -----------
* Traduit la fentre
*************************/
void WinConfigure::retranslate( )
{
    initLanguageCombobox(); //Les items sont traduits, il faut les rcuprer
    m_ui->retranslateUi(this);
}


/************** SLOTS ****************/


/*************************
* onOK( )
* ---------------
* Slot si appuie sur le boutons OK
*************************/
void WinConfigure::onOK( )
{
    QSettings* settings = new QSettings();
    QString currentLanguage = settings->value( SETTINGS_LANGUAGE ).toString();
    QString newLanguage = CLanguageManager::languageCode( m_ui->comboBox_Language->currentText() );

    //sauvegarde de la config
    int openPjt = 0;
    if (m_ui->checkBox_RecentProject->isChecked()) {    openPjt = 1;      }
    settings->setValue(SETTINGS_OPENMOSTRECENTPJT, openPjt);
    settings->setValue( SETTINGS_AFTERGENERATION, (int)m_ui->comboBox_AfterProduction->currentIndex() );
    if( currentLanguage != newLanguage ) { //seulement si la langue change
        settings->setValue( SETTINGS_LANGUAGE, newLanguage );
        emit languageChanged( );  //Traduit tout ce qui est ncessaire
    }

    delete settings;

    close();
}

/*************************
* onCancel( )
* ---------------
* Slot si appuie sur le boutons OK
*************************/
void WinConfigure::onCancel( )
{
    close();
}



/*************************
* previewGallery( )
* ---------------
*************************/
bool WinConfigure::previewGallery()
{
    return (m_ui->comboBox_AfterProduction->currentIndex() == OPEN_PREVIEW);
}


/*************************
* openGalleryFolder( )
* ---------------
*************************/
bool WinConfigure::openGalleryFolder()
{
    return (m_ui->comboBox_AfterProduction->currentIndex() == OPEN_FOLDER);
}




/*************************
* openMostRecentProject( )
* ---------------
* Renvoie true si on a configur l'ouverture du projet
* le plus rcent au dmarrage du programme.
*************************/
bool WinConfigure::openMostRecentProject()
{
    return m_ui->checkBox_RecentProject->isChecked();
}
