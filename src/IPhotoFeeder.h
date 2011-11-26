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

#ifndef IPHOTOSFEEDER_H
#define IPHOTOSFEEDER_H

#include <QStringList>


/*****************************************
 * IPhotoFeeder
 * ----------------------
 * Abstract class feeding the application
 * with photos
 *****************************************/
class IPhotoFeeder : public QObject
{
    Q_OBJECT

public:
    IPhotoFeeder( void ){ }
    virtual ~IPhotoFeeder( void ){ }
    virtual bool isValid( void ) = 0;               //Is the feeder set and valid?
    virtual QStringList getPhotoList( void ) = 0;    //Returns an alphabetically ordered list of the
                                                    //absolute filepath of ALL the photos passed to EZWG                                                    
signals:
     void newPhotoList( QStringList );  //Emit a list containing  the absolute filepath of ALL the photos passed to EZWG, usually when it has been updated
};

#endif
