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


#include "CMessage.h"
#include "GlobalDefinitions.h"


const QColor CMessage::s_color( 63,63,63 );

/*******************************************************************
* message( e_message message )
* ------------------------
* Returns a properly translated message
* TR not available on static QStrig as their instanciation is too early
********************************************************************/
QString CMessage::message( e_message message )
{
    QString returnedMessage;
    switch( message )
    {
        //-- OTHER
        case Info_RemovingPhotos:
            returnedMessage = QObject::tr("Some photos can not be found and have been removed from the project.");
            break;
        default:
            returnedMessage = QObject::tr("Undefined message.");
            break;
    }   

    return returnedMessage;
}


CMessage::CMessage( void ) :
    m_summary(),
    m_info(),
    m_details()
{}

CMessage::CMessage( const QString& summary ) :
    m_summary( summary ),
    m_info(),
    m_details()
    {   }

CMessage::CMessage( const QString& summary, const QString& info ) :
    m_summary( summary ),
    m_info( info ),
    m_details()
    {   }

CMessage::CMessage( const QString& summary, const QString& info, const QString& details ) :
    m_summary( summary ),
    m_info( info ),
    m_details( details )
    {   }

CMessage::CMessage( const CMessage& other ) :
    m_summary( other.m_summary ),
    m_info( other.m_info ),
    m_details( other.m_details )
    {   }

QString CMessage::message( void ) const
{
    QString message = m_summary;
    if( !m_info.isEmpty() ) { 
        message += ("\n" + m_info);
    }
    if( !m_details.isEmpty() ) { 
        message += ("\n" + m_details);
    }

    return message;
}

QColor CMessage::color( void ) const
{
    return s_color;
}

QString CMessage::summary( void ) const
{
    return m_summary;
}

QString CMessage::details( void ) const
{
    return m_details;
}

QString CMessage::informativeText( void ) const
{ 
    return m_info;
}

bool CMessage::isEmpty( void ) const
{
    return( m_info.isEmpty() & m_details.isEmpty() & m_summary.isEmpty() );
}
