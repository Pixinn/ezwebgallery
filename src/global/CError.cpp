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

#include <cstdlib>
#include <iostream>

#include "CError.h"
#include "GlobalDefinitions.h"


const QColor CError::s_color("red");

/*******************************************************************
* error( e_errors error )
* ------------------------
* Returns a properly translated error
********************************************************************/
QString CError::error( e_errors error )
{
    QString returnedError;
    switch( error )
    {
        case Internal:
            returnedError = QObject::tr("Internal Error.");
            break;
        case FileSaving:
            returnedError = QObject::tr("Unspecified error while saving the file: ");
            break;
        case FileOpening:
            returnedError = QObject::tr("Unspecified error while opening the file: ");
            break;
        case FileCreation:
            returnedError = QObject::tr("Unspecified error while creating the file: ");
            break;
        case DirectoryCreation:
            returnedError = QObject::tr("Unspecified error while creating the directory: ");
            break;
        case InvalidDirectory:
            returnedError = QObject::tr("Invalid directory: ");
            break;
        case SourceFileNotFound:
            returnedError = QObject::tr("Ressource files not found.");
            break;
        case WatermarkInvalid:
            returnedError = QObject::tr("Watermark not valid: ");
            break;
        case InvalidFiles:
            returnedError = QObject::tr("Some photos cannot be read from the disk. ");
            break;
        default:
            returnedError = QObject::tr("Unknown error.");
            break;
    }   

    return returnedError;
}

CError::CError( void ) :
    m_summary(),
    m_details()
{   }

CError::CError( e_errors err, const QString& details ) :
    m_summary( CError::error( err ) ),
    m_details( details )
{
    // NOT THREAD SAFE !! std::cerr << m_summary.toAscii().data() << details.toAscii().data() << std::endl;
}

CError::CError( const QString& summary,  const QString& details ) :
    m_summary( summary ),
    m_details( details )
{
    // NOT THREAD SAFE !! std::cerr << m_summary.toAscii().data() << m_details.toAscii().data() << std::endl;
}

CError::CError( const CError& other ) :
    m_summary( other.m_summary ),
   // m_info( other.m_info ),
    m_details( other.m_details )
{   }

QString CError::message( void ) const
{
    return tr("Error: ") + m_summary + "\n" + m_details;
}

QColor CError::color( void ) const
{
    return s_color;
}

QString CError::summary( void ) const
{
    return m_summary;
}

QString CError::details( void ) const
{
    return m_details;
}

//QString CError::informativeText( void ) const
//{ 
//    return m_info;
//}
