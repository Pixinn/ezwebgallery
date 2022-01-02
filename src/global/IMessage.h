/* 
 *  EZWebGallery:
 *  Copyright (C) 2011-2012 Christophe Meneboeuf <christophe@xtof.info>
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



#ifndef IMessage_H
#define IMessage_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QColor>
#include <QMetaType>

class IMessage : public QObject
{
    Q_OBJECT

public:
    virtual QColor color(void) const = 0;
    virtual QString message(void) const = 0;
    virtual QString summary( void ) const = 0;
    virtual QString details( void ) const = 0;
    virtual bool isEmpty( void ) const = 0;
};

Q_DECLARE_INTERFACE(IMessage,"com.trolltech.EZWebGallery.IMessage/1.0")

typedef QSharedPointer<IMessage> PtrMessage;
Q_DECLARE_METATYPE(PtrMessage);

typedef QList<PtrMessage> PtrMessageList;
Q_DECLARE_METATYPE(PtrMessageList);

 
#endif //IMessage_H
