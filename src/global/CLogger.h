
/* 
 *  EZWebGallery:
 *  Copyright (C) 2011 Christophe Meneboeuf <christophe@xtof.info>
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



#ifndef CLOGGER_H
#define CLOGGER_H

#include <QObject>
#include <QTextEdit>

#include "IMessage.h"


class CLogger : public QObject
{

    Q_OBJECT

public:    
    ~CLogger(void) { }
    
    static CLogger& getInstance( void ) {
        return s_instance;
    }

    void log( PtrMessage );

signals:
    void displayMessage( PtrMessage ); 

private:
    explicit CLogger(void);
    static CLogger s_instance;
};

#endif
