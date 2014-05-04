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


#include <QString>
#include <QColor>

#include "CCss.h"
#include "CToolbar.h"

CToolbar::CToolbar( const QColor & color )
{
    setStyle( color );
}

QString CToolbar::getHtml( void ) const
{
    QString html;

    if( m_style == DARK ) {
        html += "<li><img src=\"resources/images/toolbar_icon_white_browse.png\" id=\"boutonIndex\"></li>\n";
        html += "<!--<li><img src=\"resources/images/toolbar_icon_white_caption.png\"></li>\n";
        html += "<li><img src=\"resources/images/toolbar_icon_white_share.png\"></li> !-->\n";
    }
    else {
        html += "<li><img src=\"resources/images/toolbar_icon_black_browse.png\" id=\"boutonIndex\"></li>";
        html += "<!--<li><img src=\"resources/images/toolbar_icon_black_caption.png\"></li>\n";
        html += "<li><img src=\"resources/images/toolbar_icon_black_share.png\"></li> !-->\n";
    }

    return html;
}

CCssSelection CToolbar::getCss( void ) const
{
    CCssSelection toolbar("#toolbar" );
    
    if( m_style == DARK ) {
        toolbar.setProperty( "background-color", "#111111" );
    }
    else {
        toolbar.setProperty( "background-color", "#EEEEEE" );
    }

    return toolbar;
}

void CToolbar::setStyle( const QColor & color )
{
    qreal red = color.redF();
    qreal green = color.greenF();
    qreal blue = color.blueF();

    qreal luma = ( 0.299*red + 0.587*green + 0.114*blue ); //http://en.wikipedia.org/wiki/Luminosity
     
    if( luma > 0.5 ) { m_style = DARK; }
    else { m_style = LIGHT; }
}
