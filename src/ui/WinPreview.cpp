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

#include <QFileInfo>
#include <QDesktopServices>



#include "WinPreview.h"


/*************************
* Constructor
*************************/
WinPreview::WinPreview(QWidget* parent)
    :   QMainWindow( parent ), m_ui( new Ui::Preview ),
        m_wasShown(false)
{
    //Init UI
    m_ui->setupUi( this );
    m_webView = new CWebViewFacade(m_ui->centralwidget);
    m_webView->hide();   
    m_webView->addToLayout(m_ui->horizontalLayout);
    m_ui->action_Refresh->setEnabled(false);

    //---- Connnections
    connect(this->m_webView->_concreteView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    connect( this->m_ui->actionRatio_16_9, SIGNAL(triggered()), this, SLOT(onRatio16_9()) );
    connect( this->m_ui->actionRatio_4_3, SIGNAL(triggered()), this, SLOT(onRatio4_3()) );
    connect( this->m_ui->actionRotate, SIGNAL(triggered()), this, SLOT(onRotate()) );
    connect(this->m_ui->action_Refresh, SIGNAL(triggered()), this, SLOT(onRefresh()));
    connect(this->m_webView->_concreteView, SIGNAL(loadStarted()), this, SLOT(onDisable()));
    connect(this->m_webView->_concreteView, SIGNAL(loadFinished(bool)), this, SLOT(onEnable()));
}


/*************************
* Destructor
*************************/
WinPreview::~WinPreview(void)
{
 //   delete m_webView; // throws an exception????
}

/**************************
* Display the window
**************************/
void WinPreview::show( const QString& path )
{
    m_Url = QUrl::fromLocalFile(path);
    m_outDir = QFileInfo(path).dir();
    
    QMainWindow::show();
    m_webView->hide();
    m_ui->label_BuildingPreview->show();
    if (!m_wasShown) {
        resize(1024, 768);
        m_webView->load(m_Url);
        m_wasShown = true;
    }
    else {
        m_webView->reload();
    }
}


/**************************
* Refresh the preview
**************************/
void WinPreview::onRefresh(void)
{
    onDisable();
    emit refresh();
}

/**************************
* On load finished,
* hide the label and sho the webview
**************************/
void WinPreview::onLoadFinished(bool)
{
    m_ui->label_BuildingPreview->hide();
    m_webView->show();
}



/**************************
* Ratio 16:9
**************************/
void WinPreview::onRatio16_9(void)
{
    //Landscape
    if (this->size().width() > this->size().height()) {
        this->resize(QSize{ 1280, 720 });
        m_webView->reload();
    }
    else if (this->size().width() < this->size().height()){ //portrait
        this->resize(QSize{ 720, 1080 });
        m_webView->reload();
    }
    
}


/**************************
* Ratio 4:3
**************************/
void WinPreview::onRatio4_3(void)
{
    //Landscape
    if (size().width() > size().height()) {
        resize(QSize{ 1024, 768 });
        m_webView->reload();
    }
    else if (size().width() < size().height()) { //portrait
        resize(QSize{ 768, 1024 });
        m_webView->reload();
    }
    
}


/*************************
* Rotate
*************************/
void WinPreview::onRotate(void)
{
    auto h = size().height();
    auto w = size().width();
    if (w != h) {
        resize(h, w);
        m_webView->reload();
    }
}


/*************************
* Open the output folder
*************************/
void WinPreview::onOpenFolder(void)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_outDir.absolutePath()));
}


/*************************
* Enable all the actions
*************************/
void WinPreview::onEnable(void)
{
    m_ui->actionRatio_16_9->setEnabled(true);
    m_ui->actionRatio_4_3->setEnabled(true);
    m_ui->actionRotate->setEnabled(true);
    m_ui->action_Refresh->setEnabled(true);
    statusBar()->showMessage(tr("THIS IS NOT A REALTIME PREVIEW! Click on \"Refresh\" or \"Generate\" to produce a gallery reflecting any change."));
    emit enabled(true);
}


/*************************
* Disable all the actions
*************************/
void WinPreview::onDisable(void)
{
    m_ui->actionRatio_16_9->setEnabled(false);
    m_ui->actionRatio_4_3->setEnabled(false);
    m_ui->actionRotate->setEnabled(false);
    m_ui->action_Refresh->setEnabled(false);
    statusBar()->showMessage(tr("Waiting for an updated gallery."));
    emit enabled(false);
}

