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

class IToolbarStyle
{
public:    
    typedef enum {
        LIGHT,
        DARK
    } eStyle;

    virtual QString getHtml( void ) const = 0;
    virtual CCssSelection getCss( void ) const = 0;

protected:
    IToolbarStyle( void ) { }
    virtual ~IToolbarStyle( void ) {    }
};

class CToolbarStyleDark : public IToolbarStyle
{
public:
    CToolbarStyleDark( void ){  }
    ~CToolbarStyleDark( void ) {    }
    
    QString getHtml( void ) const;    
    CCssSelection getCss( void ) const;    
};


class CToolbarStyleLight : public IToolbarStyle
{
public:
    CToolbarStyleLight( void ){  }
    ~CToolbarStyleLight( void ) {    }
    
    QString getHtml( void ) const;    
    CCssSelection getCss( void ) const;    
};


class CToolbarStyleFactory
{
public:
    ~CToolbarStyleFactory( void ) { }

    static CToolbarStyleFactory& GetInstance( void ) { return s_Instance; }
    const IToolbarStyle& getStyle( const QColor & color );

private:
    CToolbarStyleFactory( void ) {  }

    IToolbarStyle::eStyle computeStyle( const QColor & color ) const; 

    static CToolbarStyleFactory s_Instance;
    CToolbarStyleDark m_dark;
    CToolbarStyleLight m_light;
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

    QString getJavascript( void ) const;

private:
    const static QString SHARE_BUTTON;
    const static QString SHARE_SCREEN;

    QString m_toolbarJs;
    QString m_shareButtonJs;
    QString m_shareScreenJs;
    t_Buttons m_buttons;
};

#endif // CToolbar_H
