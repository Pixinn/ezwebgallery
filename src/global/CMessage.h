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



#ifndef CMessage_H
#define CMessage_H

#include <QObject>
#include <QString>

class CMessage : public QObject
{
    Q_OBJECT

public:

    enum e_message
    {
        //Errors
        Err_FileSaving,
        Err_FileOpening,
        Err_FileCreation,
        Err_DirectoryCreation,
        Err_InvalidDirectory,
        Err_SourceFileNotFound,
        Err_WatermarkInvalid,
        Err_InvalidFiles,
        //Warning

        //Other
        Info_RemovingPhotos,
    };

    CMessage() : QObject()
        {   }
    CMessage( const CMessage& other ) :
        QObject(),
        m_summary( other.m_summary ),
        m_info( other.m_info ),
        m_details( other.m_details )
        {   }
    CMessage( const QString& summary, const QString& info = QString(), const QString& details = QString() ) :
        QObject(),
        m_summary( summary ),
        m_info( info ),
        m_details( details )
        {   }

    static QString message( e_message );  //Returns a properly translated warning

    void setSummary( const QString& summary ) { m_summary = summary; }
    void setInformativeText( const QString& info ) { m_info = info; }
    void setDetails( const QString& details ) { m_summary = details; }

    QString summary( void ) const { return m_summary; }
    QString informativeText( void ) const { return m_info; }
    QString details( void ) const { return m_details; }

private:
    QString m_summary;
    QString m_info;
    QString m_details;
};

#endif // CMessage_H

