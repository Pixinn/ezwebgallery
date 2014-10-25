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

// ======= GENERATES CUSTOM HTML/CSS CODE FOR THE TOOLBAR ====== //
class CToolbarStyle
{
public:
    typedef enum {
        LIGHT,
        DARK
    } eStyle;

    CToolbarStyle( const QColor & color ) :
        m_style( computeStyle( color ) )
    {       }
    bool operator!=( const CToolbarStyle& src ) { return (this->m_style != src.m_style); }
    
    inline bool isDark( void ) const { return (m_style == DARK); }
    inline bool isLight( void ) const { return (m_style == LIGHT); }
    
    const QString getHtml( void ) const;    
    const CCssSelection getCss( void ) const;
    
private:
    const eStyle computeStyle( const QColor & color ) const; 

    eStyle m_style;
};


// ======= GENERATES CUSTOM JS CODE FOR THE TOOLBAR ====== //
class CToolbarBehavior
{
public:
    //optional buttons
    typedef struct {
        bool share;
    } t_Buttons;

    explicit CToolbarBehavior( const t_Buttons& );
    ~CToolbarBehavior( void ) { }

    const QString getJavascript( void ) const;
    const QStringList getOptButtons( void ) const;

private:
    const static QString SHARE_BUTTON;
    const static QString SHARE_SCREEN;

    QString m_toolbarJs;
    QString m_shareButtonJs;
    QString m_shareScreenJs;
    t_Buttons m_buttons;
};

#endif // CToolbar_H
