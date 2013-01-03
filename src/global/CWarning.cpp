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


#include "CWarning.h"

const QColor CWarning::s_color("blue");

CWarning::CWarning( const CWarning& other ) :
    m_summary( other.m_summary ),
    m_details( other.m_details )
{   }

CWarning::CWarning( const QString& summary ) :
    m_summary( summary ),
    m_details( )
{   }

CWarning::CWarning( const QString& summary,  const QString& details ) :
    m_summary( summary ),
    m_details( details )
{   }


QString CWarning::message( void ) const
{
    QString message = m_summary;
    if( !m_details.isEmpty() ) {
        message += ("\n" + m_details);
    }
    return message;
}

QColor CWarning::color( void ) const
{
    return s_color;
}

QString CWarning::summary( void ) const
{
    return m_summary;
}

QString CWarning::details( void ) const
{
    return m_details;
}
