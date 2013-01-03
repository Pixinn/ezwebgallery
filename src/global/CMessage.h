/* 
 *  EZWebGallery:
 *  Copyright (C) 2011-2012 Christophe Meneboeuf <dev@ezwebgallery.org>
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



#ifndef CMessage_H
#define CMessage_H

#include "IMessage.h"

class CMessage : public IMessage
{

public:

    enum e_message
    {
        Info_RemovingPhotos
    };

    CMessage( void );
    CMessage( const CMessage& other );
    CMessage( const QString& summary );
    CMessage( const QString& summary, const QString& info );
    CMessage( const QString& summary, const QString& info, const QString& details );

    static QString message( e_message );  //Returns a properly translated warning

    QString message( void ) const;
    QColor color( void ) const;
    QString summary( void ) const;
    QString details( void ) const;

    inline QString informativeText( void ) const { return m_info; }


private:
    static const QColor s_color;
    QString m_summary;
    QString m_info;
    QString m_details;
};

#endif // CMessage_H

