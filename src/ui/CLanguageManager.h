/* 
 *  EZWebGallery:
 *  Copyright (C) 2011 Christophe Meneboeuf <christophe@xtof.info>
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



#ifndef CLANGUAGEMANAGER_H
#define CLANGUAGEMANAGER_H

#include <QObject>
#include <QTranslator>
#include <QStringList>
#include <QMap>
#include <QString>


/*******************************
 * CLanguageManager
 * ----------------------
 * Classe permettant de grer les changements de langue
 ******************************/
class CLanguageManager : public QObject
{
    Q_OBJECT

    //METHODES
public:
    explicit CLanguageManager(QObject *parent = 0); //Il faut au moins une INSTANCE de cet objet

    static QStringList supportedLanguages( );  //Retourne la liste des langues supportes par l'ui
    static QString languageCode( const QString &language); //Retourne le code de la langue indique
    static QString currentLanguage( ); //Retourne la langue courante

signals:


public slots:
    void translate( ); //Traduit l'appli

private:
    void initSupportedLanguages(); //Insrer toutes les langues supportes par l'ui ici

    //MEMBRES
private:
    static QMap<QString,QString> m_supportedLanguages;  //QMap de toutes les langues supportes par l'ui : <Code/Langue>
    static QTranslator* m_p_trAppli; //Translator pour l'appli
    static QTranslator* m_p_trQtLibs; //Translator pour les libs Qt (notamment les widgets)
};

#endif // CLANGUAGEMANAGER_H
