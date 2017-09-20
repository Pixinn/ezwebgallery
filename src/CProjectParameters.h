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



#ifndef CPROJECTPARAMETERS_H
#define CPROJECTPARAMETERS_H

#include <QDomDocument>
#include <QColor>

#include "IParameters.h"
#include "GlobalDefinitions.h"
#include "CCaptionManager.h"
#include "CCaption.h"
#include "CSkinParameters.h"
#include "CTaggedString.h"
#include "CPhotoFeederDirectory.h"

namespace Ui { //Pour diffrencier de la classe MainWin de mainwin.h et accder  la *vraie* ui
    class MainWin;
}

/***** Types *****/
typedef struct
{
    //Text
    CTaggedString text;
    QString fontName;
    QString colorName;
    bool f_autocolor;
    //Image
    QString imageFile;
    //Common
    bool enabled;
    int type;
    int position;
    int orientation;
    int relativeSize;
    int opacity;

    enum e_type{
        IMAGE = 0,
        TEXT
    };
    enum e_position
    {
        SOUTHEAST = 0,  //Valeur par dfaut
        SOUTH,
        SOUTHWEST,
        WEST,
        NORTHWEST,
        NORTH,
        NORTHEAST,
        EAST,
        CENTER
    };
    enum e_rotation
    {
        ROTATION_0 = 0,
        ROTATION_CW_90,
        ROTATION_CCW_90
    };

}t_watermark;


/**** Classes de paramtres ****/



class t_galleryConf{
public:
    bool operator==(const t_galleryConf& ); //<- METTRE A JOUR SI ON AJOUTE DES CHAMPS !!
    bool operator!=(const t_galleryConf& );

    QString title;
    QString description;
    QString inputDir;
    QString outputDir;
    QString skinPath;
    QString thumbPhoto;
    QString url;
    struct {
        bool enabled = false;
        QString id;
    } analytics;
    int nbPhotosToPrefetch;
    int prefetchCacheSize;     
    bool f_rightClickEnabled;    
    bool f_buttonFullscreen;

    bool f_regeneration;
}; //Configuration gnrale de la gallerie

class t_thumbsConf{
public:
    bool operator==(const t_thumbsConf& ); //<- METTRE A JOUR SI ON AJOUTE DES CHAMPS !!
    bool operator!=(const t_thumbsConf& );

    int nbColumns;
    bool f_regeneration;
}; //Configuration des vignettes

class t_photosConf{
public:
    bool operator==(const t_photosConf& ); //<- METTRE A JOUR SI ON AJOUTE DES CHAMPS !!
    bool operator!=(const t_photosConf& );
    int minSizeH;
    int minSizeW;
    int maxSizeH;
    int maxSizeW;
    int imageOptimizationStrategy;
    int nbIntermediateResolutions;
    int quality;
    double sharpeningRadius;
    int sharpeningAmount;
    t_watermark watermark;
    bool f_regeneration;
    bool f_hiDpi;
    bool f_manualConf;

    //Ces énumérations doivent correspondre Ã  la position des items dans les comboboxs
    enum e_optimizationStrategy
    {
        OPTIMIZE_SCREENUSAGE = 0,
        OPTIMIZE_QUALITY    
    };

}; //Configuration des photos


/*****************************
 * CProjectParameters
 * ----------------------
 * Contient et gre les paramtres du projet
 * Doit tre connect  l'UI et au caption manager
 ******************************/
class CProjectParameters :  public QObject, public IParameters<Ui::MainWin>
{
    Q_OBJECT

public:
    CProjectParameters( void ) :
        QObject( ),
        f_initialized( false ),
        m_feeder( CPhotoFeederDirectory::getInstance() )
    {           
        m_galleryConfig.f_regeneration = true;
        m_photosConfig.f_regeneration = true;
        m_thumbsConfig.f_regeneration = true;
    }

    CProjectParameters( const CProjectParameters & );
    CProjectParameters& operator=(const CProjectParameters & );  //Ncessit de surcharger l'oprateur d'assignement lorsqu'on hrite de QObject
    bool operator==(const CProjectParameters & );
    bool operator!=(const CProjectParameters & );
    
    void fromUi( );                                              //Rcupre les donnes provenant de l'UI
    void fromDomDocument( QDomDocument & );                      //Rcupre les donnes provenant d'un QDomDocument
    void toUi(  );                                               //Rempli l'UI
    QDomDocument toDomDocument(  );                              //Gnre un QDomDocument*/
    void setUi( Ui::MainWin* );                                  //Connecte les paramtres  l'UI
    void setcaptionManager( CCaptionManager* );                   //Connecte les paramtres au "lcaptionManager
    void setSkin( CSkinParameters* );                            //Connecte la skin
    CSkinParameters* skin( );                                    //Renvoie un pointeur vers la skin
    bool photosChanged( const CProjectParameters &);             //Vrifie si les photos on chang
    bool load( const QString & );                                //Chargement d'un fichier de paramtres
    bool save( const QString & );                                //Sauvegarde d'un fichier de paramtres
    unsigned int version( void ) {return m_version;}             //Retoune le status du projet
    inline bool initialized( void ) { return f_initialized; }
private:
    static const unsigned int s_versionFilePath = 20111217;
    QDomDocument convertFromOldVersion( const QDomDocument &document, const int version );    //Convertion du projet d'une version prcdante en la version actuelle

signals:
    void loaded(QString);
    void saved(QString);
    void warning(PtrMessage);


// !! BIEN METTRE A JOUR operator= , == et != en cas d'ajout d'attribut !! //
public:
    t_galleryConf m_galleryConfig;
    t_thumbsConf m_thumbsConfig;
    t_photosConf m_photosConfig;
private:
    Ui::MainWin* m_p_ui;
    CCaptionManager* m_p_captionManager;
    CSkinParameters* m_p_skin;
    unsigned int m_version;
    bool f_initialized;
    CPhotoFeederDirectory& m_feeder;
};

#endif // CPROJECTPARAMETERS_H

