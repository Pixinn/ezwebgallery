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


#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QColorDialog>
#include <QSizePolicy>

#include "CColorPicker.h"


/*****************************************
* --------- CLASS COLORBUTTON ------------
*****************************************/
CColorButton::CColorButton( QWidget * parent ) : QFrame(parent)
{
    //UI
    setMinimumSize( 16, 16 );
    setMaximumSize( 16, 16 );
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    setLineWidth( 1 );
}


void CColorButton::setColor(const QColor &color)
{
    m_color = color;
    update();
}


void CColorButton::paintEvent(QPaintEvent *e)
{
        int frameWidth = this->frameWidth(); //this à utiliser pour lever une ambiguité
        int buttonInnerWidth = width( ) - 2*frameWidth;
        int buttonInnerHeight = height( ) - 2*frameWidth;
        QRect innerRect = QRect( frameWidth, frameWidth, buttonInnerWidth, buttonInnerHeight );

    //NB: Dans le cas d'un Widget, Qpainter ne peut tre utilis que dans paintEvent()...
        QFrame::paintEvent( e ); //Il faut l'appeler si on veut que TOUT soit xcut.
                                                         //Ncessaire notamment pour dessiner le cadre
    QPainter painter( this );
    painter.fillRect( innerRect, m_color ); //On ne dessine que l'intrieur !
}

//Besoin de rimplmenter pour que ce driv de QFrame puisse se faire cliquer dessus comme un bouton
void CColorButton::mousePressEvent(QMouseEvent *e)
{
    if( e->button() == Qt::LeftButton ){
                e->accept();
        emit clicked( );
    }
}

/*****************************************
* --------- CLASS COLORHEXTEXT -----------
*****************************************/
CColorHexText::CColorHexText( QWidget * parent ) : QLineEdit(parent)
{
    setInputMask("HHHHHH");
    setFixedSize( 42, 18 ); //Nul -> devrait dpendre de la taille de la police !
    //Il faudrait rendre le curseur moins moche...
}


void CColorHexText::setColor(const QColor &color)
{
    QString colorName = color.name();
    colorName.remove( QChar('#') );
    this->setText( colorName );
}

/*****************************************
* --------- CLASS COLORPICKER  --------- *
*****************************************/
CColorPicker::CColorPicker( QWidget * parent ) : QWidget(parent)
{
    m_color= QColor("White");
    init();
}

CColorPicker::CColorPicker( const QColor &color, QWidget * parent ) : QWidget(parent)
{
    m_color = color;
    init();
}

CColorPicker::~CColorPicker()
{
    delete m_p_colorBox;
    delete m_p_colorText;
    delete m_p_layout;
}

void CColorPicker::init( )
{
    //Layout
    m_p_layout = new QHBoxLayout(this);
    m_p_layout->setContentsMargins(0,0,0,0);
    //Sub-Widgets
    m_p_colorBox = new CColorButton( );
    m_p_colorText = new CColorHexText( );
    m_p_colorText->setMaxLength( 6 ); //RGB
    //Connections
    connect(m_p_colorText, SIGNAL(textChanged(QString)),this,SLOT(onTextEdited(QString) ));
    connect(m_p_colorBox,SIGNAL(clicked()),this,SLOT(onColorBoxClicked()));
    //Positionnement
    m_p_layout->addWidget( m_p_colorBox );
    m_p_layout->addWidget( m_p_colorText );
    m_p_layout->addStretch( ); //Permet de laisser align  gauche si on agrandi le widget
    setLayout( m_p_layout );

    setColor(m_color);
}

void CColorPicker::setColor( QColor color )
{
    m_color = color;
    m_p_colorBox->setColor( m_color );
    m_p_colorText->setColor( m_color );
}

void CColorPicker::setColor( QString colorName )
{
    m_color = QColor(colorName);
    m_p_colorBox->setColor( m_color );
    m_p_colorText->setColor( QColor(colorName) );
}

QColor CColorPicker::color()
{
        return m_color;
}

QString CColorPicker::value()
{
        return m_color.name();
}

void CColorPicker::onColorBoxClicked()
{
    QColorDialog* colorDialog = new QColorDialog( );
    connect(colorDialog,SIGNAL(colorSelected(QColor)),this,SLOT(setColor(QColor)));
    colorDialog->setCurrentColor(m_color);
    colorDialog->exec();
    delete colorDialog;
}

void CColorPicker::onTextEdited(QString colorString )
{
    if( colorString.size() == 6){
        setColor( QColor(QString("#") + colorString) );
    }
}
