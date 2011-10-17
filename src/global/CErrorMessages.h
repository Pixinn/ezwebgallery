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



#ifndef CERRORMESSAGES_H
#define CERRORMESSAGES_H

#include <QString>

//Enumration de toutes les erreurs
enum e_errors{
    FileSaving,
    FileOpening,
    FileCreation,
    DirectoryCreation,
    SourceFileNotFound,
    WatermarkInvalid
};

class CErrorMessages
{

public:
    CErrorMessages();
    static QString error( e_errors );  //Retourne l'erreur correctement traduite
};

#endif // CERRORMESSAGES_H
