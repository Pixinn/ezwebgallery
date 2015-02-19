/* 
 *  EZWebGallery:
 *  Copyright (C) 2015 Christophe Meneboeuf <dev@ezwebgallery.org>
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


#ifndef WINPREVIEW_H
#define WINPREVIEW_H

#include <memory>

#include <QMainWindow>
#include <QUrl>
#include <QThread>
#include <QDir>

#include "ui_WinPreview.h" //Gnr par qmake. Ncessaire pour accs  la classe gnre par le formulaire .ui
 
namespace Ui { //Voir le bas du fichier ui_winConfigure.h
    class Preview;
}

class QWebEngineView;

class WinPreview : public QMainWindow
{
    Q_OBJECT

        /********** Methodes *********/
public:
    WinPreview(QWidget* parent = 0);
    ~WinPreview(void);

    void show(const QString& path);

public slots:
    void onLoadFinished(bool);
    void onRatio4_3(void);
    void onRatio16_9(void);
    void onRotate(void);
    void onOpenFolder(void);

signals:
    void load(const QUrl url);
    void reload(void);

private:
    Ui::Preview* m_ui;
    QWebEngineView* m_webView;
    bool m_wasShown;

    QDir m_outDir;

};





#endif
