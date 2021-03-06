﻿/* 
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
            returnedError = QObject::tr("Internal Error. ");
            break;
        case FileSaving:
            returnedError = QObject::tr("An error occured while saving the file. ");
            break;
        case FileOpening:
            returnedError = QObject::tr("An error occured while opening the file. ");
            break;
        case FileCreation:
            returnedError = QObject::tr("An error occured while creating the file. ");
            break;
        case DirectoryCreation:
            returnedError = QObject::tr("An error occured while creating the directory. ");
            break;
        case InvalidDirectory:
            returnedError = QObject::tr("Invalid directory. ");
            break;
        case SourceFileNotFound:
            returnedError = QObject::tr("Resource files not found. ");
            break;
        case WatermarkInvalid:
            returnedError = QObject::tr("Watermark not valid. ");
            break;
        case InvalidFiles:
            returnedError = QObject::tr("Some photos cannot be read from the disk. ");
            break;
        case SkinOpening:
            returnedError = QObject::tr("Cannot load the skin. Using default skin instead.");
            break;
        default:
            returnedError = QObject::tr("Unknown error. ");
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

CError::CError( const CError& src ) :
    m_summary( src.m_summary ),
    m_details( src.m_details )
{   }

CError& CError::operator=(const CError &src)
{
    if( this != &src) {
        this->m_summary = src.m_summary;
        this->m_details = src.m_details;
    }
    return *this;
}

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

bool CError::isEmpty( void ) const
{
    return( m_summary.isEmpty() & m_details.isEmpty() );
}
