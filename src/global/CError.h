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



#ifndef CError_H
#define CError_H

#include <QObject>
#include <QString>


class CError : public QObject
{
    Q_OBJECT

public:

    enum e_errors
    {
        FileSaving,
        FileOpening,
        FileCreation,
        DirectoryCreation,
        InvalidDirectory,
        SourceFileNotFound,
        WatermarkInvalid,
        InvalidFiles
    };

    CError() : QObject()
        {   }
    CError( const CError& other ) :
        QObject(),
        m_summary( other.m_summary ),
        m_details( other.m_details )
        {   }
    CError( const QString& summary,  const QString& details = QString() ) :
        QObject(),
        m_summary( summary ),
        m_details( details )
        {   }

    static QString error( e_errors );  //Returns a properly translated error

    void setSummary( const QString& summary ) { m_summary = summary; }
    void setDetails( const QString& details ) { m_summary = details; }

    QString summary( void ) const { return m_summary; }
    QString details( void ) const { return m_details; }

private:
    QString m_summary;
    QString m_details;
};

#endif // CError_H

