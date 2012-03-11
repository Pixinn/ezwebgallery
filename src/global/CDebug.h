
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



#ifndef CDEBUG_H
#define CDEBUG_H

#include <QObject>
#include <QString>

class CDebug :
    public QObject
{

    Q_OBJECT

public:
    CDebug(void) { }
    ~CDebug(void) { }
    static CDebug& getInstance( void ) {
        return s_instance;
    }

    void message( const QString &msg ) {
        emit displayMessage( msg );
    }

signals:
    void displayMessage( QString );

private:
    static CDebug s_instance;

};

#endif
