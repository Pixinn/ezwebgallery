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

#include "CError.h"
#include "GlobalDefinitions.h"


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
        case missingFiles:
            returnedError = QObject::tr("Some files are missing. They will be removed from the project.");
            break;
        default:
            returnedError = QObject::tr("Unknown error.");
            break;
    }   

    return returnedError;
}
