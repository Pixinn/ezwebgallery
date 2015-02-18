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


#ifndef WINCONFIGURE_H
#define WINCONFIGURE_H


#include <QDialog>

#include "ui_WinConfigure.h" //Gnr par qmake. Ncessaire pour accs  la classe gnre par le formulaire .ui

namespace Ui { //Voir le bas du fichier ui_winConfigure.h
    class Configure;
}

class WinConfigure : public QDialog
{
    Q_OBJECT

    /********** Mthodes *********/
public:
    WinConfigure( QWidget* parent = 0 );
    ~WinConfigure();
    int exec(); //Surcharge de QDialog::exec() pour initialiser combobox de langue
    void retranslate(); //Traduit la fentre dans la nouvelle langue
    bool previewGallery();
    bool openGalleryFolder();
    bool openMostRecentProject();

    /********* SLOTS ********/
public slots:
    //void onChangeLanguage( );
    void onOK( );
    void onCancel( );

    /****** SIGNAUX ******/
signals:
    void languageChanged( );

private:
    void initLanguageCombobox( ); //Initialise la combobox de choix de langue

    /********** Membres *********/
private:
    Ui::Configure* m_ui;

    enum {
        OPEN_PREVIEW = 0,
        OPEN_FOLDER,
        DO_NOTHING
    };

};

#endif
