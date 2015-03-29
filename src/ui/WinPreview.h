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


#include "CPlatform.h"

#include "ui_WinPreview.h" //Gnr par qmake. Ncessaire pour accs  la classe gnre par le formulaire .ui
 
namespace Ui { //Voir le bas du fichier ui_winConfigure.h
    class Preview;
}


// ++++ FACADE TO THE WEBVIEW, DEPENDING ON THE PLATFORM
#ifdef Q_OS_LINUX
#include <QWebView>
class CWebViewFacade
{
public:
    CWebViewFacade(QWidget* parent = 0) {
        _concreteView = new QWebView(parent);
        _concreteView->setObjectName(QStringLiteral("webView"));
        _concreteView->setEnabled(true);
        _concreteView->setMinimumSize(QSize(0, 0));
        _concreteView->setUrl(QUrl(QStringLiteral("about:blank")));
        _concreteView->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    }
    ~CWebViewFacade(void) {
        _concreteView->deleteLater();
    }
    inline void addToLayout(QLayout* layout) {
        layout->addWidget(_concreteView);
    }
    inline void hide(void) const {
        _concreteView->hide();
    }
    inline void show(void) const {
        _concreteView->show();
    }
    void load(const QUrl& url) {
        QWebSettings::clearMemoryCaches();
        _concreteView->load(url);
    }
    void reload(void) {
        QWebSettings::clearMemoryCaches();
        _concreteView->reload();
    }

    QWebView* _concreteView;
};
#endif
#ifdef Q_OS_WIN32
    #include <QWebEngineView>
    class CWebViewFacade
    {
    public:
        CWebViewFacade(QWidget* parent = 0) {
            _concreteView = new QWebEngineView(parent);
            _concreteView->setObjectName(QStringLiteral("webView"));
            _concreteView->setEnabled(true);
            _concreteView->setMinimumSize(QSize(0, 0));
            _concreteView->setUrl(QUrl(QStringLiteral("about:blank")));
        }
        ~CWebViewFacade(void) {
            _concreteView->deleteLater();
        }
        inline void addToLayout(QLayout* layout) {
            layout->addWidget(_concreteView);
        }
        inline void hide(void) const {
            _concreteView->hide();
        }
        inline void show(void) const {
            _concreteView->show();
        }
        void load(const QUrl& url) {
            _concreteView->load(url);
        }
        void reload(void) {
            _concreteView->reload();
        }
 
        QWebEngineView* _concreteView;
    };
#endif


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
    void onRefresh(void);
    void onEnable(void);
    void onDisable(void);

signals:
    void load(const QUrl url);
    void reload(void);
    void refresh(void);
    void enabled(bool);

private:
    Ui::Preview* m_ui;
    CWebViewFacade* m_webView;
    bool m_wasShown;

    QDir    m_outDir;
    QUrl    m_Url;
};





#endif
