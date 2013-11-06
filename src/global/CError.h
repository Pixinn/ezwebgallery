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



#ifndef CError_H
#define CError_H

#include <QString>

#include "IMessage.h"

class CError : public IMessage
{

    Q_OBJECT

public:

    enum e_errors
    {
        Internal,
        FileSaving,
        FileOpening,
        FileCreation,
        DirectoryCreation,
        InvalidDirectory,
        SourceFileNotFound,
        WatermarkInvalid,
        InvalidFiles
    };

    CError( const CError& other );

    CError( void );
    CError( e_errors, const QString& details );
    CError( const QString& summary,  const QString& details );

    static QString error( e_errors );  //Returns a properly translated standard error

    QString message( void ) const;

    QColor color( void ) const;
    QString summary( void ) const;
    QString details( void ) const;
    //QString informativeText( void ) const;

private:
    static const QColor s_color;
    QString m_summary;
    //QString m_info;
    QString m_details;
    
};

#endif // CError_H
