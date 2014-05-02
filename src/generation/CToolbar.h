/*
 *  EZWebGallery:
 *  Copyright (C) 2014 Christophe Meneboeuf <dev@ezwebgallery.org>
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



#ifndef CToolbar_H
#define CToolbar_H

class QColor;
class QString;

class CCssSelection;

// ======= GENERATE CUSTOM CODE FOR THE TOOLBAR ====== //
class CToolbar
{
public:
    explicit CToolbar( const QColor & );
    ~CToolbar( void ) { }
    bool operator!=( const CToolbar& src ) { return this->m_style != src.m_style; }

    QString getHtml( void ) const;
    CCssSelection getCss( void ) const;


private:
    void setStyle( const QColor & color ); 

    typedef enum {
        LIGHT,
        DARK
    } eStyle;

    eStyle m_style;
};

#endif // CToolbar_H
