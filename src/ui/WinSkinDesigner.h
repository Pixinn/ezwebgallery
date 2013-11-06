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



#ifndef SKINDESIGNERWIN_H
#define SKINDESIGNERWIN_H


#include <QMainWindow>
#include <QAction>

#include "CCss.h"

#include "ui_WinSkinDesigner.h" //Gnr par qmake. Ncessaire pour accs  la classe gnre par le formulaire .ui
#include "CSkinParameters.h"

namespace Ui { //Voir le bas du fichier ui_skinDesigner.h
    class SkinDesigner;
}

/************************** DEFINES *******************************/
#define NBMAXRECENTFILES 5


/************************ CLASSES ********************************/
class WinSkinDesigner : public QMainWindow //ncessaire pour appeler m_ui->setupUi(QMainWindow*);
{
    Q_OBJECT
/************ METODES *************/
public:
    WinSkinDesigner( CSkinParameters*, QWidget *parent = 0 );
    ~WinSkinDesigner();
    bool isUnsaved( );
    QStringList openSkin( const QString &);  //Ouvrir un thme
    int displayUnsavedMsgBox( );             //Affiche une message box indiquant que la skin n'a pas t savegarde
    int displayMoreRecentMsgBox( );          //Affiche un avertissement si on essaie d'ouvrir une skin plus rcente que le programme
    void retranslate();                      //Traduit l'UI dans une nouvelle langue    
public slots:
    void onMosaicBckgEnable( bool );
    void onNewSkin( );
    void onOpenSkin( );
    void onOpenRecentSkin( );
    bool onSaveSkin( );
    bool onSaveSkinAs( );
    void checkSkin( );                      //Vrifie si la skin est valide et affiche le rsultat dans une boite de dialogue
    void changeWindowTitle( QString );
    void skinLoaded( QString );
    void skinSaved( QString );
protected:
    void closeEvent(QCloseEvent *event);
    void displayRecentFiles( );
private:
    bool saveSkinFile( QString );
public:
    static const QString WindowTitle;
private:
    Ui::SkinDesigner* m_ui;
    CSkinParameters* m_p_parameters;
    CSkinParameters m_reference;
    CSkinParameters m_emptySkin;            //Skin vide de rfrence    
    QStringList m_recentSkins;              //Liste contenant les NBMAXRECENTFILES derniers fichiers skin ouverts
    QString m_lastSelectedDir;              //Le dernier rpertoire o l'on a ouvert ou sauv une skin
    QAction *m_recentSkinssActions[NBMAXRECENTFILES]; //Les actions permettant d'ouvrir les skin rcentes
    static const QStringList FontFamilies;
    static const QStringList FontVariants;
    static const QStringList FontWeights;
};

#endif // SKINDESIGNERWIN_H
