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



#ifndef CCOLORPICKER_H
#define CCOLORPICKER_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QColor>
#include <QLineEdit>
#include <QString>
//Important pour pouvoir manipuler les events:
#include <QMouseEvent>
#include <QPaintEvent>
//Pour exporter le widget ver QDesigner
#include <QtDesigner/QDesignerExportWidget>


class CColorButton : public QFrame
{
    Q_OBJECT

public:
    CColorButton( QWidget * parent = 0 );
protected:
        void paintEvent(QPaintEvent *);
        void mousePressEvent(QMouseEvent *);

public slots:
    void setColor(const QColor &);    

signals:
        void clicked( );

private:
    QColor m_color;

};





class CColorHexText : public QLineEdit
{
    Q_OBJECT

public:
    CColorHexText( QWidget * parent = 0 );

public slots:
    void setColor(const QColor &);
};




class CColorPicker : public QWidget
{
    Q_OBJECT

public:
    CColorPicker( QWidget * parent = 0 );
    CColorPicker( const QColor &color, QWidget * parent = 0 );
    ~CColorPicker();
    QColor color();
    QString value();
public slots:
    void setColor( QColor );
    void setColor( QString );
protected slots:
    void onTextEdited( QString );
    void onColorBoxClicked( );
private:
    void init( );

    QHBoxLayout* m_p_layout;
    CColorButton* m_p_colorBox;
    CColorHexText* m_p_colorText;
    QColor m_color;

};

#endif // CCOLORPICKER_H
