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
#include <QFile>
#include <QTextStream>
#include <QString>

#include "CCss.h"
#include "CToolbar.h"

CToolbarStyleFactory CToolbarStyleFactory::s_Instance;

IToolbarStyle::eStyle CToolbarStyleFactory::computeStyle( const QColor & color ) const
{
    qreal red = color.redF();
    qreal green = color.greenF();
    qreal blue = color.blueF();

    qreal luma = ( 0.299*red + 0.587*green + 0.114*blue ); //http://en.wikipedia.org/wiki/Luminosity
     
    if( luma > 0.5 ) { return IToolbarStyle::DARK; }
    else { return IToolbarStyle::LIGHT; }
}


const IToolbarStyle& CToolbarStyleFactory::getStyle( const QColor & color )
{
    if( computeStyle( color ) == IToolbarStyle::DARK ) {
        return m_dark;
    }
    else {
        return m_light;
    }
}

QString CToolbarStyleDark::getHtml( void ) const
{
    QString html;

    html += "<li><img src=\"resources/images/toolbar_icon_white_browse.png\" class=\"button_browse\"></li>\n";
    html += "<li><img src=\"resources/images/toolbar_icon_white_share.png\" class=\"button_share display_optional\"></li>\n";
    html += "<li><a href=\"http://www.ezwebgallery.org/\"><img src=\"resources/images/toolbar_icon_browse_ezwebgallery.png\" class=\"button_ezwebgallery\"></a></li>";
    html += "<!--<li><img src=\"resources/images/toolbar_icon_white_caption.png\"></li>\n";
    html += "<li><img src=\"resources/images/toolbar_icon_white_share.png\"></li> !-->\n";

    return html;
}


CCssSelection CToolbarStyleDark::getCss( void ) const
{
    CCssSelection toolbar(".toolbar" );
    toolbar.setProperty( "background-color", "#111111" );
    return toolbar;
}


QString CToolbarStyleLight::getHtml( void ) const
{
    QString html;

    html += "<li><img src=\"resources/images/toolbar_icon_black_browse.png\" class=\"button_browse\"></li>";
    html += "<li><img src=\"resources/images/toolbar_icon_black_share.png\" class=\"button_share\"></li>\n";
    html += "<li><a href=\"http://www.ezwebgallery.org/\"><img src=\"resources/images/toolbar_icon_browse_ezwebgallery.png\" class=\"button_ezwebgallery\"></a></li>";
    html += "<!--<li><img src=\"resources/images/toolbar_icon_black_caption.png\"></li>\n";
    html += "<li><img src=\"resources/images/toolbar_icon_black_share.png\"></li> !-->\n";

    return html;
}


CCssSelection CToolbarStyleLight::getCss( void ) const
{
    CCssSelection toolbar(".toolbar" );
    toolbar.setProperty( "background-color", "#EEEEEE" );
    return toolbar;
}



//////////////////////////////////////////////////


const QString CToolbarBehavior::SHARE_BUTTON( "[SHARE_BUTTON]" );
const QString CToolbarBehavior::SHARE_SCREEN( "[SHARE_SCREEN]" );


CToolbarBehavior::CToolbarBehavior(  const t_Buttons& buttons ) :
    m_buttons( buttons )
{
    QFile jsFile( ":/web/Toolbar.js" ); 
    jsFile.open( QIODevice::Text|QIODevice::ReadOnly );
    QTextStream jsTextStream( &jsFile );
    jsTextStream.setCodec( "UTF-8" );
    m_toolbarJs = jsTextStream.readAll();

    QFile shareButtonJsFile( ":/web/ShareButton.js" ); 
    shareButtonJsFile.open( QIODevice::Text|QIODevice::ReadOnly );
    QTextStream shareButtonJsTextStream( &shareButtonJsFile );
    shareButtonJsTextStream.setCodec( "UTF-8" );
    m_shareButtonJs = shareButtonJsTextStream.readAll();

    QFile shareScreenJsFile( ":/web/ShareScreen.js" );
    shareScreenJsFile.open( QIODevice::Text|QIODevice::ReadOnly );
    QTextStream shareScreenJsTextStream( &shareScreenJsFile );
    shareScreenJsTextStream.setCodec( "UTF-8" );
    m_shareScreenJs = shareScreenJsTextStream.readAll();
}


    
QString CToolbarBehavior::getJavascript( void ) const
{
    QString javascript( m_toolbarJs );

    if( m_buttons.share ) {
        javascript.replace( SHARE_BUTTON, m_shareButtonJs );
        javascript.replace( SHARE_SCREEN, m_shareScreenJs );
    }
    else {
        javascript.remove( SHARE_BUTTON );
        javascript.remove( SHARE_SCREEN );
    }

    return javascript;
}
