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



#include <QObject>

#include "CMessage.h"
#include "GlobalDefinitions.h"


/*******************************************************************
* warning( e_warnings warning )
* ------------------------
* Returns a properly translated warning
********************************************************************/
QString CMessage::message( e_message message )
{
    QString returnedMessage;
    switch( message )
    {
        //-- ERRORS
        case Err_FileSaving:
            returnedMessage = QObject::tr("Unspecified error while saving the file: ");
            break;
        case Err_FileOpening:
            returnedMessage = QObject::tr("Unspecified error while opening the file: ");
            break;
        case Err_FileCreation:
            returnedMessage = QObject::tr("Unspecified error while creating the file: ");
            break;
        case Err_DirectoryCreation:
            returnedMessage = QObject::tr("Unspecified error while creating the directory: ");
            break;
        case Err_InvalidDirectory:
            returnedMessage = QObject::tr("Invalid directory: ");
            break;
        case Err_SourceFileNotFound:
            returnedMessage = QObject::tr("Ressource files not found.");
            break;
        case Err_WatermarkInvalid:
            returnedMessage = QObject::tr("Watermark not valid: ");
            break;
        case Err_InvalidFiles:
            returnedMessage = QObject::tr("Some photos cannot be read from the disk.");
            break;
        //-- OTHER
        case Info_RemovingPhotos:
            returnedMessage = QObject::tr("Some photos can not be found and have been removed from the project.");
            break;
        default:
            returnedMessage = QObject::tr("Unknown message.");
            break;
    }   

    return returnedMessage;
}
