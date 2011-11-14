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



#ifndef CTERMINALUI_H
#define CTERMINALUI_H

#include <QString>
#include <QStringList>
#include <QTextStream>

#include "IUserInterface.h"
#include "CGalleryGenerator.h"
#include "CProjectParameters.h"
#include "CSkinParameters.h"
#include "CLanguageManager.h"
#include "CCaptionManager.h"

class CTerminalUi : public QObject, IUserInterface
{
    Q_OBJECT

public:
    CTerminalUi( const QString &projectFile );
    ~CTerminalUi( );
    void show();                //Redfinition car on ne veut pas qu'une fentre apparaisse...
    void setGenerator( CGalleryGenerator* );
signals:
    void done();
public slots:
    void run( );                        //Contient le code  xcuter pour lancer la gnration
    void onLogMsg( QString );
    void onProgressBar( int completion, QString color, QString message, int timeout = 0 ); //Affiche un % d'avancement
    void onGalleryGenerationFinished( /*bool success*/QList<CPhotoProperties>);
    void onForceStoppedFinished( QStringList );

protected:
    void keyPressEvent ( QKeyEvent * event );

private:
    //Sorties console
    static QTextStream cout;
    static QTextStream cerr;

    QString m_projectFile;
    CGalleryGenerator* m_p_galleryGenerator;
    CProjectParameters m_projectParameters;
    CSkinParameters m_skinParameters;
    CCaptionManager m_captionManager;
    CLanguageManager m_languageManager;
};

#endif // CTERMINALUI_H
