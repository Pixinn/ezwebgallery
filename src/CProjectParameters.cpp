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


#include <QMapIterator>

#include "CProjectParameters.h"
#include "ui_mainwin.h"     //Gnr par qmake. Ncessaire pour accs  la classe gnre par le formulaire .ui

#include "CPlatform.h"
#include "CLogger.h"
#include "CTaggedString.h"
#include "CPhotoDatabase.h"
#include "CMessage.h"


/*********************** STRUCTURES DE DONNEES ************************/
//--- BIEN METTRE A JOURS LES operator== EN CAS DE MODIFICATION DE CES CHAMPS ---//
bool t_galleryConf::operator==(const t_galleryConf& source)
{
    if( title == source.title
        && description == source.description
        && inputDir == source.inputDir
        && outputDir == source.outputDir
        && url == source.url
        && nbPhotosToPrefetch == source.nbPhotosToPrefetch
        && prefetchCacheSize == source.prefetchCacheSize   
        && f_rightClickEnabled == source.f_rightClickEnabled
        //&& f_regeneration == source.f_regeneration  Ne pas le comparer, car modifi automatique lors de la gnration
        && skinPath == source.skinPath
        && thumbPhoto == source.thumbPhoto
        && f_shareOnSocialNetworks == source.f_shareOnSocialNetworks  )        
    { return true; }
    else{
        return false;
    }
}
bool t_galleryConf::operator!=(const t_galleryConf &source)
{
    return( !(*this == source) );
}

//--- BIEN METTRE A JOURS LES operator== EN CAS DE MODIFICATION DE CES CHAMPS ---//
bool t_thumbsConf::operator==(const t_thumbsConf& source)
{
    if(  nbRows == source.nbRows
        && nbColumns == source.nbColumns
        //&& size == source.size 
        //&& quality == source.quality
        //&& f_regeneration == source.f_regeneration Ne pas le comparer, car modifi automatique lors de la gnration
    )
    { return true; }
    else{
        return false;
    }
}
bool t_thumbsConf::operator!=(const t_thumbsConf& source)
{
    return( !(*this == source) );
}

//--- BIEN METTRE A JOURS LES operator== EN CAS DE MODIFICATION DE CES CHAMPS ---//
bool t_photosConf::operator==(const t_photosConf& source)
{
    if( minSizeH == source.minSizeH
        && minSizeW == source.minSizeW
        && maxSizeH == source.maxSizeH
        && maxSizeW == source.maxSizeW
        && imageOptimizationStrategy == source.imageOptimizationStrategy
        && nbIntermediateResolutions == source.nbIntermediateResolutions
        && sharpeningAmount == source.sharpeningAmount
        && sharpeningRadius == source.sharpeningRadius
        && watermark.text == source.watermark.text
        && watermark.fontName == source.watermark.fontName
        && watermark.colorName == source.watermark.colorName
        && watermark.f_autocolor == source.watermark.f_autocolor
        && watermark.imageFile == source.watermark.imageFile
        && watermark.enabled == source.watermark.enabled
        && watermark.type == source.watermark.type        
        && watermark.position == source.watermark.position
        && watermark.orientation == source.watermark.orientation
        && watermark.relativeSize == source.watermark.relativeSize
        && watermark.opacity == source.watermark.opacity        
        && quality == source.quality
        //&& f_regeneration == source.f_regeneration Ne pas le comparer, car modifi automatique lors de la gnration
    )
    { return true; }
    else{
        return false;
    }
}
bool t_photosConf::operator!=(const t_photosConf& source)
{
    return( !(*this == source) );
}




// !!! ATTENTION : BIEN METTRE A JOUR CETTE FONCTION EN CAS D'AJOUTS D'ATTRIBUTS !!!!
CProjectParameters::CProjectParameters(const CProjectParameters &source) :
    QObject(),
    m_feeder( source.m_feeder )

{
    this->m_galleryConfig = source.m_galleryConfig;
    this->m_photosConfig = source.m_photosConfig;
    this->m_thumbsConfig = source.m_thumbsConfig;
//    this->m_photosList = source.m_photosList;
    this->m_p_captionManager = source.m_p_captionManager;
    this->m_p_skin = source.m_p_skin;
    this->m_p_ui = source.m_p_ui;
    this->m_version = source.m_version;
}



/****************************************************
* operator=(const CSkinParameters& source)
* ---------
* L'hritage de QObject oblige de redfinir cet oprateur
* voir: http://www.fredosaurus.com/notes-cpp/oop-overloading/overloadassign.html
*****************************************************/

// !!! ATTENTION : BIEN METTRE A JOUR CETTE FONCTION EN CAS D'AJOUTS D'ATTRIBUTS !!!!

CProjectParameters& CProjectParameters::operator=(const CProjectParameters &source)
{
    if( this != &source){
        this->m_galleryConfig = source.m_galleryConfig;
        this->m_photosConfig = source.m_photosConfig;
        this->m_thumbsConfig = source.m_thumbsConfig;
        this->m_p_captionManager = source.m_p_captionManager;
        this->m_p_skin = source.m_p_skin;
        this->m_p_ui = source.m_p_ui;
        this->m_version = source.m_version;
        this->m_feeder = source.m_feeder;
        this->f_initialized = source.f_initialized;
    }
    return *this;
}


/****************************************************
* operator==(const CSkinParameters& source)
* ---------
*****************************************************/

// !!! ATTENTION : BIEN METTRE A JOUR CETTE FONCTION EN CAS D'AJOUTS D'ATTRIBUTS !!!!
bool CProjectParameters::operator==(const CProjectParameters &source)
{
    bool f_result = true;
    if( /*m_photosList != source.m_photosList      <<-- Renvoie toujours false !!?
        &&*/ m_galleryConfig != source.m_galleryConfig
        || m_thumbsConfig != source.m_thumbsConfig
        || m_photosConfig != source.m_photosConfig
        || m_p_ui != source.m_p_ui
        || m_p_captionManager != source.m_p_captionManager
        || m_p_skin != source.m_p_skin
        || m_version != source.m_version
        || m_feeder != source.m_feeder
        || f_initialized != source.f_initialized
       )
    {
        f_result =  false;
    }

    return f_result;
    
}

/****************************************************
* operator!=(const CSkinParameters& source)
* ---------
*****************************************************/
bool  CProjectParameters::operator!=(const CProjectParameters &source)
{
    return( !(*this==source) );
}

void CProjectParameters::setUi( Ui::MainWin* ui )
{
    m_p_ui = ui;
}

void CProjectParameters::setcaptionManager( CCaptionManager* captionManager )
{
    m_p_captionManager = captionManager;
}

void CProjectParameters::setSkin( CSkinParameters* skin)
{
    m_p_skin = skin;
}

CSkinParameters* CProjectParameters::skin( ){
    return m_p_skin;
}

void CProjectParameters::fromUi( /*const Ui::MainWin* const ui */)
{
    t_thumbsConf oldThumbsConf = m_thumbsConfig;
    t_photosConf oldPhotosConf = m_photosConfig;

    //Remplissage structure Config Gallerie
    m_galleryConfig.title = m_p_ui->lineEdit_GalleryTitle->text();
    m_galleryConfig.description = m_p_ui->plainTextEdit_ShortDescription->toPlainText();
    m_galleryConfig.nbPhotosToPrefetch = m_p_ui->spinbox_nbPhotosPreCharger->value();
    m_galleryConfig.prefetchCacheSize = m_p_ui->spinbox_tailleDuCache->value();
    m_galleryConfig.inputDir = m_p_ui->lineEdit_SourceFolder->text();
    m_galleryConfig.outputDir = m_p_ui->lineEdit_DestinationFolder->text();
    m_galleryConfig.url = m_p_ui->lineEdit_URL->text();
    m_galleryConfig.f_rightClickEnabled = m_p_ui->checkBox_clicDroitAutorise->isChecked();
    m_galleryConfig.f_shareOnSocialNetworks = m_p_ui->checkBox_SocialNetworks->isChecked();

    //Remplissage structure Config Vignettes
    m_thumbsConfig.nbColumns = m_p_ui->spinBox_ThumbNbCol->value();
    m_thumbsConfig.nbRows = m_p_ui->spinBox_ThumbNbRow->value();

    //Remplissage structure Config Photos
    m_photosConfig.maxSizeH = m_p_ui->spinBox_PhotoMaxVSize->value();
    m_photosConfig.maxSizeW = m_p_ui->spinbox_PhotoMaxHSize->value();
    m_photosConfig.minSizeH = m_p_ui->spinbox_PhotoMinVSize->value();
    m_photosConfig.minSizeW = m_p_ui->spinbox_PhotoMinHSize->value();
    m_photosConfig.imageOptimizationStrategy = m_p_ui->comboBox_ImageQualityStrategy->currentIndex();
    m_photosConfig.nbIntermediateResolutions = m_p_ui->spinBox_PhotoNbRes->value();
    m_photosConfig.sharpeningAmount = m_p_ui->spinBox_PhotoSharpeningAmount->value();
    m_photosConfig.sharpeningRadius = m_p_ui->doubleSpinBox_PhotoSharpeningRadius->value();
    m_photosConfig.quality = m_p_ui->spinBox_PhotoJpegQuality->value();
    m_photosConfig.watermark.text = m_p_ui->lineEdit_WatermarkText->text();
    m_photosConfig.watermark.fontName = m_p_ui->fontComboBox_WatermarkText->currentFont().family();
    m_photosConfig.watermark.colorName = m_p_ui->cColorPicker_WatermarkText->color().name();
    m_photosConfig.watermark.f_autocolor = m_p_ui->checkBox_WatermarkTextColorAuto->isChecked();
    m_photosConfig.watermark.imageFile = m_p_ui->CImagePicker_WatermarkImage->fileName();
    m_photosConfig.watermark.enabled = m_p_ui->groupBox_Watermark->isChecked();
    m_photosConfig.watermark.type = m_p_ui->comboBox_WatermarkType->currentIndex();
    m_photosConfig.watermark.opacity = m_p_ui->spinBox_WatermarkOpacity->value();
    m_photosConfig.watermark.orientation = m_p_ui->comboBox_WatermarkOrientation->currentIndex();
    m_photosConfig.watermark.position = m_p_ui->comboBox_WatermarkPosition->currentIndex();
    m_photosConfig.watermark.relativeSize = m_p_ui->spinBox_WatermarkSize->value();


    if( !( m_photosConfig == oldPhotosConf ) ){
        m_photosConfig.f_regeneration = true;
    }
    if( !( m_thumbsConfig == oldThumbsConf ) ){
        m_thumbsConfig.f_regeneration = true;
    }
}

/*************************
* fromDomDocument
*----------------------
* Chargement  partir d'un document XML/DOM
**************************/
void CProjectParameters::fromDomDocument( QDomDocument &document )
{
    QDomElement root = document.firstChildElement( "Session" );

    //--- Conversion automatique du projet si il a t cr par une version antrieure  la version actuelle
    m_version = root.attribute( "release" ).remove( QChar('M') ).toInt( );
    unsigned int currentVersion = CPlatform::revisionInt();
    if( m_version < currentVersion ){
        document = convertFromOldVersion( document, m_version );
        root = document.firstChildElement( "Session" );
    }

    QDomElement galleryConfElem = root.firstChildElement( "GalleryConfig" );
    QDomElement thumbsConfElem = root.firstChildElement( "ThumbsConfig" );
    QDomElement photosConfElem = root.firstChildElement( "PhotosConfig" );
    QDomElement watermarkConfElem = root.firstChildElement( "WatermarkConfig" );
    QDomElement photoListElem = root.firstChildElement( "PhotoList" );


    //--- CONFIG GALLERY
    m_galleryConfig.title = galleryConfElem.firstChildElement( "title" ).text();
    m_galleryConfig.description = galleryConfElem.firstChildElement( "description" ).text();
    m_galleryConfig.nbPhotosToPrefetch = galleryConfElem.firstChildElement( "nbPhotosToPrefetch" ).text().toInt();
    m_galleryConfig.prefetchCacheSize = galleryConfElem.firstChildElement( "prefetchCacheSize" ).text().toInt();
    m_galleryConfig.f_rightClickEnabled = galleryConfElem.firstChildElement( "rightClickEnabled" ).text().toInt();
    QString inputFolder =  galleryConfElem.firstChildElement( "inputFolder" ).text();
    if( m_feeder.setDirectory( inputFolder ) ) {
        m_galleryConfig.inputDir = inputFolder;
    }
    else {
        m_galleryConfig.inputDir.clear();
        //emit message( inputFolder + tr(" doesn't exists") );
        PtrMessage message( new CMessage( tr("Folder doesnt exist: "), inputFolder ) );
        CLogger::getInstance().log( message );
    }
    m_galleryConfig.outputDir =  galleryConfElem.firstChildElement( "outputFolder" ).text(); //DO NOT TEST IF THIS DIR STILL EXIST: it breaks cli!! It will be created if needed.

    m_galleryConfig.url = galleryConfElem.firstChildElement( "url" ).text();
    m_galleryConfig.skinPath = galleryConfElem.firstChildElement( "Skin" ).firstChildElement( "Path" ).text();
    m_p_skin->setName( galleryConfElem.firstChildElement( "Skin" ).attribute("name") );
    m_galleryConfig.thumbPhoto = galleryConfElem.firstChildElement( "thumbnail" ).text();
    m_galleryConfig.f_shareOnSocialNetworks = galleryConfElem.firstChildElement( "shareOnSocialNetworks" ).text().toInt();

    //--- CONFIG THUMBS
    m_thumbsConfig.nbColumns = thumbsConfElem.firstChildElement( "nbColumns" ).text().toInt();
    m_thumbsConfig.nbRows = thumbsConfElem.firstChildElement( "nbRows" ).text().toInt();

    //--- CONFIG PHOTOS
    m_photosConfig.maxSizeH = photosConfElem.firstChildElement( "maxSizeH" ).text().toInt();
    m_photosConfig.maxSizeW = photosConfElem.firstChildElement( "maxSizeW" ).text().toInt();
    m_photosConfig.minSizeH = photosConfElem.firstChildElement( "minSizeH" ).text().toInt();
    m_photosConfig.minSizeW = photosConfElem.firstChildElement( "minSizeW" ).text().toInt();
    m_photosConfig.imageOptimizationStrategy = photosConfElem.firstChildElement( "imageOptimizationStrategy" ).text().toInt();
    m_photosConfig.nbIntermediateResolutions = photosConfElem.firstChildElement( "nbIntermediateResolutions" ).text().toInt();
    m_photosConfig.quality = photosConfElem.firstChildElement( "quality" ).text().toInt();
    m_photosConfig.sharpeningAmount = photosConfElem.firstChildElement( "sharpeningAmount" ).text().toInt();
    m_photosConfig.sharpeningRadius = photosConfElem.firstChildElement( "sharpeningRadius" ).text().toDouble();

    //--- CONFIG WATERMARK
    m_photosConfig.watermark.enabled = (bool)watermarkConfElem.attribute( "enabled", "0" ).toInt();
    m_photosConfig.watermark.type = watermarkConfElem.firstChildElement( "watermarkType" ).text().toInt();
    m_photosConfig.watermark.text = watermarkConfElem.firstChildElement( "watermarkText" ).text();
    m_photosConfig.watermark.fontName = watermarkConfElem.firstChildElement( "watermarkFontName" ).text();
    m_photosConfig.watermark.colorName = watermarkConfElem.firstChildElement( "watermarkColorName" ).text();
    m_photosConfig.watermark.f_autocolor = (bool)watermarkConfElem.firstChildElement( "watermarkAutocolor" ).text().toInt();
    m_photosConfig.watermark.imageFile = watermarkConfElem.firstChildElement( "watermarkImageFile" ).text();
    m_photosConfig.watermark.opacity = watermarkConfElem.firstChildElement( "watermarkOpacity" ).text().toInt();
    m_photosConfig.watermark.orientation = watermarkConfElem.firstChildElement( "watermarkOrientation" ).text().toInt();
    m_photosConfig.watermark.position = watermarkConfElem.firstChildElement( "watermarkPosition" ).text().toInt();
    m_photosConfig.watermark.relativeSize = watermarkConfElem.firstChildElement( "watermarkRelativeSize" ).text().toInt();

    //deprecated fileformat
    if( m_version < s_versionFilePath ) {
        PtrMessage message( new CMessage( tr("Importing deprecated project: "), QString::number( m_version )) );
        CLogger::getInstance().log( message );
        CPhotoDatabase::getInstance().importDeprecated( photoListElem, inputFolder );
    }
    //current file format
    else
    {
        QStringList missingFiles = CPhotoDatabase::getInstance().build( photoListElem );
        if( !missingFiles.isEmpty() )
        {   //Some files were not found
            QString details;
            foreach( QString file, missingFiles ) {
                details.append( file + QString('\n') );
            }
            CMessage info( CMessage::message(CMessage::Info_RemovingPhotos), QString(), details );
            emit message( info );
        }
    }

    f_initialized = true;
}


/*************************
* toDomDocument
*----------------------
* Retourne un document XML/DOM correspondant aux parametres
**************************/
QDomDocument CProjectParameters::toDomDocument( /*CCaptionManagerr &captions*/ )
{
    QDomDocument document;
    QDomElement root = document.createElement( "Session" );
    root.setAttribute( "release", QString::number(CPlatform::revisionInt()) );
    document.appendChild( root );

    
//--- CONFIG GALLERY
    QDomElement galleryConfig = document.createElement( "GalleryConfig" );
    root.appendChild( galleryConfig );
    QDomElement title = document.createElement( "title" );
    galleryConfig.appendChild( title );
    title.appendChild( document.createTextNode(  m_galleryConfig.title ) );
    QDomElement description = document.createElement( "description" );
    galleryConfig.appendChild( description );
    description.appendChild( document.createTextNode(  m_galleryConfig.description ) );
    QDomElement nbPhotosToPrefetch = document.createElement( "nbPhotosToPrefetch" );
    galleryConfig.appendChild( nbPhotosToPrefetch );
    nbPhotosToPrefetch.appendChild( document.createTextNode( QString::number(m_galleryConfig.nbPhotosToPrefetch) ) );
    QDomElement prefetchCacheSize = document.createElement( "prefetchCacheSize" );
    galleryConfig.appendChild( prefetchCacheSize );
    prefetchCacheSize.appendChild( document.createTextNode( QString::number(m_galleryConfig.prefetchCacheSize) ) );
    QDomElement rightClickEnabled = document.createElement( "rightClickEnabled" );
    galleryConfig.appendChild( rightClickEnabled );
    rightClickEnabled.appendChild( document.createTextNode( QString::number(m_galleryConfig.f_rightClickEnabled) ) );
    QDomElement inputFolder = document.createElement( "inputFolder" );
    galleryConfig.appendChild( inputFolder );
    inputFolder.appendChild( document.createTextNode( m_feeder.getDirectoryPath() ) );
    QDomElement outputFolder = document.createElement( "outputFolder" );
    galleryConfig.appendChild( outputFolder );
    outputFolder.appendChild( document.createTextNode(  m_galleryConfig.outputDir ) );
    QDomElement url = document.createElement( "url" );
    galleryConfig.appendChild( url );
    url.appendChild( document.createTextNode(  m_galleryConfig.url ) );
    QDomElement skin = document.createElement( "Skin" );
    skin.setAttribute( "name", m_p_skin->name() );
    galleryConfig.appendChild( skin );
    QDomElement skinPath = document.createElement( "Path" );
    skinPath.appendChild( document.createTextNode( m_galleryConfig.skinPath ) );
    skin.appendChild( skinPath );
    QDomElement thumbnail = document.createElement( "thumbnail" );
    thumbnail.appendChild( document.createTextNode( m_galleryConfig.thumbPhoto ) );
    galleryConfig.appendChild( thumbnail );
    QDomElement shareOnSocialNetworks = document.createElement( "shareOnSocialNetworks" );
    galleryConfig.appendChild( shareOnSocialNetworks );
    shareOnSocialNetworks.appendChild( document.createTextNode( QString::number(m_galleryConfig.f_shareOnSocialNetworks) ) );

    //--- CONFIG THUMB
    QDomElement thumbsConfig  = document.createElement( "ThumbsConfig" );
    root.appendChild( thumbsConfig );
    QDomElement nbColumns = document.createElement( "nbColumns" );
    thumbsConfig.appendChild( nbColumns );
    nbColumns.appendChild( document.createTextNode(  QString::number(m_thumbsConfig.nbColumns)) );
    QDomElement nbRows = document.createElement( "nbRows" );
    thumbsConfig.appendChild( nbRows );
    nbRows.appendChild( document.createTextNode(  QString::number(m_thumbsConfig.nbRows)) );


    //--- CONFIG PHOTO
    QDomElement photosConfig = document.createElement( "PhotosConfig" );
    root.appendChild( photosConfig );
    QDomElement maxSizeH = document.createElement( "maxSizeH" );
    photosConfig.appendChild( maxSizeH );
    maxSizeH.appendChild( document.createTextNode(  QString::number(m_photosConfig.maxSizeH)) );
    QDomElement maxSizeW = document.createElement( "maxSizeW" );
    photosConfig.appendChild( maxSizeW );
    maxSizeW.appendChild( document.createTextNode(  QString::number(m_photosConfig.maxSizeW)) );
    QDomElement minSizeH = document.createElement( "minSizeH" );
    photosConfig.appendChild( minSizeH );
    minSizeH.appendChild( document.createTextNode(  QString::number(m_photosConfig.minSizeH)) );
    QDomElement minSizeW = document.createElement( "minSizeW" );
    photosConfig.appendChild( minSizeW );
    minSizeW.appendChild( document.createTextNode(  QString::number(m_photosConfig.minSizeW)) );
    QDomElement imageOptimizationStrategy = document.createElement( "imageOptimizationStrategy" );
    photosConfig.appendChild( imageOptimizationStrategy );
    imageOptimizationStrategy.appendChild( document.createTextNode(  QString::number(m_photosConfig.imageOptimizationStrategy)) );
    QDomElement nbIntermediateResolutions = document.createElement( "nbIntermediateResolutions" );
    photosConfig.appendChild( nbIntermediateResolutions );
    nbIntermediateResolutions.appendChild( document.createTextNode(  QString::number(m_photosConfig.nbIntermediateResolutions)) );
    QDomElement quality = document.createElement( "quality" );
    photosConfig.appendChild( quality );
    quality.appendChild( document.createTextNode(  QString::number(m_photosConfig.quality)) );
    QDomElement sharpeningAmount = document.createElement( "sharpeningAmount" );
    photosConfig.appendChild( sharpeningAmount );
    sharpeningAmount.appendChild( document.createTextNode(  QString::number(m_photosConfig.sharpeningAmount)) );
    QDomElement sharpeningRadius = document.createElement( "sharpeningRadius" );
    photosConfig.appendChild( sharpeningRadius );
    sharpeningRadius.appendChild( document.createTextNode(  QString::number(m_photosConfig.sharpeningRadius)) );


    //--- CONFIG WATERMARK
    QDomElement watermarkConfig = document.createElement( "WatermarkConfig" );
    watermarkConfig.setAttribute( "enabled", QString::number((int)m_photosConfig.watermark.enabled) );
    root.appendChild( watermarkConfig );
    QDomElement watermarkType = document.createElement( "watermarkType" );
    watermarkConfig.appendChild( watermarkType );
    watermarkType.appendChild( document.createTextNode(  QString::number((int)m_photosConfig.watermark.type)) );
    QDomElement watermarkText = document.createElement( "watermarkText" );
    watermarkConfig.appendChild( watermarkText );
    watermarkText.appendChild( document.createTextNode(  m_photosConfig.watermark.text ) );
    QDomElement watermarkFontName = document.createElement( "watermarkFontName" );
    watermarkConfig.appendChild( watermarkFontName );
    watermarkFontName.appendChild( document.createTextNode(  m_photosConfig.watermark.fontName ));
    QDomElement watermarkColorName = document.createElement( "watermarkColorName" );
    watermarkConfig.appendChild( watermarkColorName );
    watermarkColorName.appendChild( document.createTextNode(  m_photosConfig.watermark.colorName ));
    QDomElement watermarkAutocolor = document.createElement( "watermarkAutocolor" );
    watermarkConfig.appendChild( watermarkAutocolor );
    watermarkAutocolor.appendChild( document.createTextNode(  QString::number((int)m_photosConfig.watermark.f_autocolor) ));    
    QDomElement watermarkImageFile = document.createElement( "watermarkImageFile" );
    watermarkConfig.appendChild( watermarkImageFile );
    watermarkImageFile.appendChild( document.createTextNode( m_photosConfig.watermark.imageFile ) );
    QDomElement watermarkOpacity = document.createElement( "watermarkOpacity" );
    watermarkConfig.appendChild( watermarkOpacity );
    watermarkOpacity.appendChild( document.createTextNode(  QString::number(m_photosConfig.watermark.opacity)) );
    QDomElement watermarkOrientation = document.createElement( "watermarkOrientation" );
    watermarkConfig.appendChild( watermarkOrientation );
    watermarkOrientation.appendChild( document.createTextNode(  QString::number(m_photosConfig.watermark.orientation)) );
    QDomElement watermarkPosition = document.createElement( "watermarkPosition" );
    watermarkConfig.appendChild( watermarkPosition );
    watermarkPosition.appendChild( document.createTextNode(  QString::number(m_photosConfig.watermark.position)) );
    QDomElement watermarkRelativeSize = document.createElement( "watermarkRelativeSize" );
    watermarkConfig.appendChild( watermarkRelativeSize );
    watermarkRelativeSize.appendChild( document.createTextNode(  QString::number(m_photosConfig.watermark.relativeSize)) );

    root.appendChild( CPhotoDatabase::getInstance().xml(document));

    return document;
}


void CProjectParameters::toUi( )
{
    //Lecture structure Config Gallerie
    m_p_ui->lineEdit_GalleryTitle->setText( m_galleryConfig.title );
    m_p_ui->plainTextEdit_ShortDescription->setPlainText( m_galleryConfig.description );
    m_p_ui->spinbox_nbPhotosPreCharger->setValue( m_galleryConfig.nbPhotosToPrefetch );
    m_p_ui->spinbox_tailleDuCache->setValue( m_galleryConfig.prefetchCacheSize );    
    m_p_ui->lineEdit_SourceFolder->setText( m_galleryConfig.inputDir );
    m_p_ui->lineEdit_DestinationFolder->setText( m_galleryConfig.outputDir );
    m_p_ui->lineEdit_URL->setText( m_galleryConfig.url );
    m_p_ui->checkBox_clicDroitAutorise->setChecked( m_galleryConfig.f_rightClickEnabled );
    m_p_ui->checkBox_SocialNetworks->setChecked( m_galleryConfig.f_shareOnSocialNetworks );

    //Lecture structure Config Vignettes
    m_p_ui->spinBox_ThumbNbCol->setValue( m_thumbsConfig.nbColumns );
    m_p_ui->spinBox_ThumbNbRow->setValue( m_thumbsConfig.nbRows );

    //Lecture structure Config Photos
    m_p_ui->spinBox_PhotoMaxVSize->setValue( m_photosConfig.maxSizeH );
    m_p_ui->spinbox_PhotoMaxHSize->setValue( m_photosConfig.maxSizeW );
    m_p_ui->spinbox_PhotoMinVSize->setValue( m_photosConfig.minSizeH );
    m_p_ui->spinbox_PhotoMinHSize->setValue( m_photosConfig.minSizeW );
    m_p_ui->comboBox_ImageQualityStrategy->setCurrentIndex( m_photosConfig.imageOptimizationStrategy );
    m_p_ui->spinBox_PhotoNbRes->setValue( m_photosConfig.nbIntermediateResolutions );
    m_p_ui->spinBox_PhotoJpegQuality->setValue( m_photosConfig.quality );
    m_p_ui->spinBox_PhotoSharpeningAmount->setValue( m_photosConfig.sharpeningAmount );
    m_p_ui->doubleSpinBox_PhotoSharpeningRadius->setValue( m_photosConfig.sharpeningRadius );    
    m_p_ui->comboBox_WatermarkType->setCurrentIndex( m_photosConfig.watermark.type );
    m_p_ui->lineEdit_WatermarkText->setText( m_photosConfig.watermark.text );
    m_p_ui->fontComboBox_WatermarkText->setCurrentFont( QFont(m_photosConfig.watermark.fontName) );
    m_p_ui->cColorPicker_WatermarkText->setColor( m_photosConfig.watermark.colorName );
    m_p_ui->checkBox_WatermarkTextColorAuto->setChecked( m_photosConfig.watermark.f_autocolor );
    m_p_ui->CImagePicker_WatermarkImage->setImage( m_photosConfig.watermark.imageFile );
    m_p_ui->spinBox_WatermarkOpacity->setValue( m_photosConfig.watermark.opacity );
    m_p_ui->spinBox_WatermarkSize->setValue( m_photosConfig.watermark.relativeSize );
    m_p_ui->comboBox_WatermarkOrientation->setCurrentIndex( m_photosConfig.watermark.orientation );
    m_p_ui->comboBox_WatermarkPosition->setCurrentIndex( m_photosConfig.watermark.position );
    m_p_ui->groupBox_Watermark->setChecked( m_photosConfig.watermark.enabled ); //A mettre en dernire position des proprits watermark pour bien enable/disable les widgets

    //Legendes
    //REM: le body est affiche via un signal du caption manager, car il depend du num de la photo a l'ecran
    /*QMap<QString,CCaption> captionMap = m_p_captionManager->captionMap();
    if( !captionMap.isEmpty() ){
        CCaption caption = captionMap.begin().value();
        m_p_ui->lineEdit_captionHeader->setText( caption.header() );
        m_p_ui->lineEdit_captionEnding->setText( caption.ending() );
    }*/

    //REM: le body est affiche via un signal du caption manager, car il depend du num de la photo a l'ecran
    CPhotoDatabase& photoDb = CPhotoDatabase::getInstance();
    if( photoDb.size() > 0 ) {
        CCaption caption = photoDb.properties( 0 )->caption();
        m_p_ui->lineEdit_captionHeader->setText( caption.header() );
        m_p_ui->lineEdit_captionEnding->setText( caption.ending() );
    }
}

bool CProjectParameters::load( const QString &fileToLoad )
{
    QDomDocument document;    
    QFile file_xml( fileToLoad );
    bool f_success = false;
    //Est-ce un fichier xml
    if( document.setContent( &file_xml ) )
    {
        fromDomDocument( document );
        //Est-ce un fichier EZWebGallery ?
        if( m_version > 0 ){
            f_success = true;
        }
    }
    if( f_success ){
        emit loaded( fileToLoad );
        return true;
    }
    else{
        f_initialized = false;
        return false;
    }
}

bool CProjectParameters::save( const QString &fileToSave )
{
    QDomDocument docToSave;
    QFile fileXml( fileToSave );
    QTextStream xmlFileStream( &fileXml );
    xmlFileStream.setCodec( "UTF-8" );
    xmlFileStream.setGenerateByteOrderMark (true);
    if ( fileXml.open(QIODevice::WriteOnly | QIODevice::Text) ){
        docToSave = toDomDocument( );
        //criture dans le fichier
        xmlFileStream << docToSave.toString( 4 );
        fileXml.close();
        emit saved( fileToSave );
        return true;
    }
    else{
        return false;
    }
}


/*******************************************************************
* convertFromOldVersion( const QDomDocument &document, int oldVersionNumber )
* ---------
* Conversion d'un ancien projet dans la version actuelle
* In/Out: (QDomDocument) document  convertir -> Sera modifi !
* In : (int) oldVersionNumber - version d'EZWG ayant servi  sauver le projet
* Return : (bool) true si quelque chose a t modifi, false sinon
********************************************************************/
QDomDocument CProjectParameters::convertFromOldVersion(  const QDomDocument &document, const int oldVersionNumber )
{
    QDomDocument newDocument;
    QString docString = document.toString( 4 );

    if( oldVersionNumber < 94 )
    {
        docString.replace("legend", "caption");
        docString.replace("Legend", "Caption");
        docString.replace("legends", "captions");
        docString.replace("Legends", "Captions");        
        newDocument.setContent( docString );
        
        return newDocument;
    }
    else{
        return document;
    }
    
}
