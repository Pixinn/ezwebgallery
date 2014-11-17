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



#include <algorithm>
#include <QFileInfo>
#include <QTextStream>

#include "CSkinParameters.h"
#include "CToolbar.h"
#include "CPlatform.h"
#include "GlobalDefinitions.h"
#include "CError.h"

//----------- Définitions ---------//
#define RESOURCES_TAG_NAME     "Ressources"
#define RESOURCEFILE_TAG_NAME  "File"


//-------------- Variables statiques ---------//



//----------- Méthodes ------------//


/****************************************************
* CSkinParameters( )
* ---------
* Constructeur par dfaut
*****************************************************/
CSkinParameters::CSkinParameters() :
    QObject(),
    f_initialized( false )
{        
    //--- Inits
    m_styleSheet = CCssSheet("skin");
    setName( QString("noname") );
    m_p_ui = NULL;
}


CSkinParameters::CSkinParameters( const CSkinParameters & other ) :
    QObject()
{
    *this = other;
}


/****************************************************
* operator=(const CSkinParameters& source)
* ---------
* L'hritage de QObject oblige de redfinir cet oprateur
* voir: http://www.fredosaurus.com/notes-cpp/oop-overloading/overloadassign.html
*****************************************************/

// !!! ATTENTION : BIEN METTRE A JOUR CETTE FONCTION EN CAS D'AJOUTS D'ATTRIBUTS !!!!

CSkinParameters& CSkinParameters::operator=(const CSkinParameters &source)
{
    if( this != &source)
    {
        this->thumbImgBorderSize = source.thumbImgBorderSize;
        this->thumbBoxBorderSize = source.thumbBoxBorderSize;
        this->photoPaddingSize = source.photoPaddingSize;
        this->m_name = source.m_name;
        this->m_resourcesPath = source.m_resourcesPath;
        this->m_p_ui = source.m_p_ui;
        this->m_resources = source.m_resources;
        this->m_styleSheet = source.m_styleSheet;
        this->m_filePath = source.m_filePath;
        this->m_version = source.m_version;
        this->f_initialized = source.f_initialized;
    }
    return *this;
}

/****************************************************
* operator==(const CSkinParameters& source)
* ---------
* L'hritage de QObject oblige de redfinir cet oprateur
* voir: http://www.fredosaurus.com/notes-cpp/oop-overloading/overloadassign.html
*****************************************************/
bool CSkinParameters::operator==( const CSkinParameters &source)
{
    if( this->thumbImgBorderSize != source.thumbImgBorderSize ){    return false;   }
    if( this->thumbBoxBorderSize != source.thumbBoxBorderSize ){    return false;   }
    if( this->photoPaddingSize != source.photoPaddingSize ){    return false;   }
    if( this->m_name != source.m_name ){    return false;   }
    if( this->m_resourcesPath != source.m_resourcesPath ){    return false;   }
    if( this->m_resources != source.m_resources ){    return false;   }
    if( this->m_styleSheet != source.m_styleSheet ){    return false;   }
    if( this->m_filePath != source.m_filePath ){    return false;   }
    if( this->m_version != source.m_version ){ return false; }
    if( this->f_initialized != source.f_initialized ){ return false; }
    return true;
}


/****************************************************
* setUi( )
* ---------
* Lie l'instance de classe  une interface graphique
*****************************************************/
void CSkinParameters::setUi( Ui::SkinDesigner* ui)
{
    m_p_ui = ui;
}

/*******************************************************************
* setName( )
* ---------
* Dfini le nom de la skin
********************************************************************/
//NB: utilisation interne de "setName()" au lieu de "m_name =" permet d'mettre le signal
void CSkinParameters::setName( const QString &name )
{
    if( !name.isEmpty() ){
        m_name = name;
        emit nameChanged( m_name );
    }
}


/*******************************************************************
* name( )
* ---------
* Retourne le nom de la skin
********************************************************************/
QString CSkinParameters::name( ) const
{
    return m_name;
}


/*******************************************************************
* name( )
* ---------
* Retourne le path de la skin. Vide si non sauve
********************************************************************/
QString CSkinParameters::filePath( ) const
{
    return m_filePath;
}



/*******************************************************************
* fromUi( )
* ---------
* Rempli les structures internes avec les donnes rcupres de l'UI
********************************************************************/
void CSkinParameters::fromUi( )
{
    if( m_p_ui == NULL ){ return; } //Si on est en mode console, pas d'ui link !

    m_styleSheet.clear();

    /****** Proprits diverses ******/
    setName( m_p_ui->lineEdit_SkinName->text() );
    /****** Ressources ******/
    //Construction de la liste de ressources ncessaires
    m_resources.clear();
    m_resources.insert( "IndexPage_BckgTexture", m_p_ui->cImagePicker_Index_BckgTexture->fileName() );
    m_resources.insert( "Mosaic_BckgTexture", m_p_ui->cImagePicker_Mosaic_BckgTexture->fileName( ) );
    m_resources.insert( "PhotoFraming_Texture", m_p_ui->CImagePicker_PhotoFraming_Texture->fileName() );
    m_resources.insert( "PhotoPage_BckgTexture",m_p_ui->cImagePicker_Photo_BckgTexture->fileName() );    
    m_resources.insert( "PhotoButtonPrevious", m_p_ui->CImagePicker_PreviousButton_Icon->fileName() );
    m_resources.insert( "PhotoButtonNext", m_p_ui->CImagePicker_NextButton_Icon->fileName() );
    //Filtrage des ressources:
    removeEmptyResources( ); // On enlve de la liste les images "vides" (ie no image)
     
    /****** Creation du CSS ******/

    //-------------- ///////////////// INDEX //////////////// ----------------//
    //ScreenIndex
    CCssSelection screenIndex( QString( "#screenIndex" ) );
        screenIndex.setProperty( QString("background-image"), QString("url(../images/") + QFileInfo(m_p_ui->cImagePicker_Index_BckgTexture->fileName()).fileName() + QString(")") );
        screenIndex.setProperty( QString("background-color"), m_p_ui->cColorPicker_Index_BckgColor->value() );
        if( m_p_ui->checkBox_Index_BckgCover->isChecked() ) { screenIndex.setProperty( QString("background-size"), QString("cover") ); }
    m_styleSheet.addSelection( screenIndex );
    //Title
    CCssSelection indexTitle( QString("div#indexTitle"));
        indexTitle.setProperty( QString("text-shadow"), QString("4px 2px 4px rgba(150, 150, 150, 0.8)") );
        indexTitle.setProperty( QString("text-align"), m_p_ui->comboBox_VTitleText_Alignment->currentText().toLower() );
        indexTitle.setProperty( QString("color"), m_p_ui->cColorPicker_VTitleText_TextColor->value() );
        indexTitle.setProperty( QString("font-size"), QString::number( m_p_ui->doubleSpinBox_VTitle_FontSize->value() ) + QString("em") );
        indexTitle.setProperty( QString("font-family"), m_p_ui->comboBox_VTitleText_FontFamily->currentText() );
    m_styleSheet.addSelection( indexTitle );
    //Mosaic
    CCssSelection thumbsMosaic( QString("#mosaic"));
        thumbsMosaic.setProperty( QString("background-image"), QString("url(../images/") + QFileInfo(m_p_ui->cImagePicker_Mosaic_BckgTexture->fileName()).fileName() + QString(")") );
        if( m_p_ui->checkBox_Mosaic_BckgColor_Enabled->isChecked() ) { //optional background color
            thumbsMosaic.setProperty( QString("background-color"), m_p_ui->cColorPicker_Mosaic_BckgColor->value() );            
        } else {
            thumbsMosaic.setProperty( QString("ezwg-background-color-disabled"), "true" );
        }
        thumbsMosaic.setProperty( QString("border-width"), QString::number( m_p_ui->spinBox_Mosaic_BorderWidth->value() ) + QString("px") );
        thumbsMosaic.setProperty( QString("border-color"), m_p_ui->cColorPicker_Mosaic_BorderColor->value() );
     m_styleSheet.addSelection( thumbsMosaic );
    CCssSelection thumbSpacing( QString(".thumbBox") );  //Espace entre les vignettes
        this->thumbBoxBorderSize = m_p_ui->spinBox_Mosaic_SpacingWidth->value();
        thumbSpacing.setProperty( QString("border-width"), QString::number( thumbBoxBorderSize ) + QString("px") );
        thumbSpacing.setProperty( QString("border-color"), m_p_ui->cColorPicker_Mosaic_SpacingColor->value() );
    //.thumbBox img -> thumbnail
    CCssSelection thumbnail( QString("img"));
        this->thumbImgBorderSize = m_p_ui->spinBox_Thumbnail_BorderWidth->value();
        thumbnail.setProperty( QString("border-width"), QString::number( thumbImgBorderSize ) + QString("px") );
        thumbnail.setProperty( QString("border-color"), m_p_ui->cColorPicker_Thumbnail_BorderColor->value() );
    thumbSpacing.addSubSelection( thumbnail );
    CCssSelection thumbnailHovered( QString("img:hover"));
        thumbnailHovered.setProperty( QString("border-color"), m_p_ui->cColorPicker_Thumbnail_Hovered_BorderColor->value() );
    thumbSpacing.addSubSelection( thumbnailHovered );
    m_styleSheet.addSelection( thumbSpacing );

    //-------------- ///////////////// PHOTO //////////////// ----------------//
    //ScreenPhoto
   CCssSelection screenPhoto( QString( "#screenPhoto" ) );
        screenPhoto.setProperty( QString("background-image"), QString("url(../images/") + QFileInfo(m_p_ui->cImagePicker_Photo_BckgTexture->fileName()).fileName() + QString(")") );
        screenPhoto.setProperty( QString("background-color"), m_p_ui->cColorPicker_Photo_BckgColor->value() );
        if( m_p_ui->checkBox_Photo_BckgCover->isChecked() ) { screenPhoto.setProperty( QString("background-size"), QString("cover") ); }
    m_styleSheet.addSelection( screenPhoto );
    //Framing - cadre
    CCssSelection photoFrame( QString( "div#cadrePhoto" ) );
        photoFrame.setProperty( QString("background-image"), QString("url(../images/") + QFileInfo(m_p_ui->CImagePicker_PhotoFraming_Texture->fileName()).fileName() + QString(")") );
        photoFrame.setProperty( QString("background-color"), m_p_ui->cColorPicker_PhotoFraming_Color->value() );
        photoFrame.setProperty( QString("padding"), QString::number(m_p_ui->spinBox_PhotoFraming_Width->value()) + QString("px") );
    m_styleSheet.addSelection( photoFrame );
    //Framing - classe (runtime) mainphoto - Permet de controler la petite bordure autour de la photo (padding dans la gui)
    photoPaddingSize = m_p_ui->spinBox_PhotoFraming_PaddingWidth->value();
    CCssSelection photoPadding( QString( ".mainPhoto" ) );
        photoPadding.setProperty( QString("border-color"), m_p_ui->cColorPicker_PhotoFraming_PaddingColor->value() );
        photoPadding.setProperty( QString("border-width"), QString::number(photoPaddingSize) + QString("px") );
    m_styleSheet.addSelection( photoPadding );

    //Légendes
    CCssSelection photoCaptions( QString("div.photoCaption") );
        photoCaptions.setProperty( QString("font-weight"), m_p_ui->comboBox_PhotoCaption_FontWeight->currentText() );
        photoCaptions.setProperty( QString("font-family"), m_p_ui->comboBox_PhotoCaption_FontFamily->currentText() );
        photoCaptions.setProperty( QString("font-size"), QString::number(m_p_ui->doubleSpinBox_PhotoCaption_FontSize->value()) + QString("em")  );
        photoCaptions.setProperty( QString("color"), m_p_ui->cColorPicker_PhotoCaption_TextColor->value() );
    m_styleSheet.addSelection( photoCaptions );
    
    //Toolbar
    CCssSelection toolbarCss = CToolbarStyleFactory::GetInstance().getStyle( m_p_ui->cColorPicker_PhotoFraming_Color->value() ).getCss();
    m_styleSheet.addSelection( toolbarCss );
    
    //--- Css
    //#progressbarWrapper
    CCssSelection progressBarWrapper( QString( "#progressbarWrapper" ) );
        progressBarWrapper.setProperty( QString("font-family"), m_p_ui->comboBox_Box_FontFamily->currentText() );
        progressBarWrapper.setProperty( QString("color"), m_p_ui->cColorPicker_Box_TextColor->value() );
        progressBarWrapper.setProperty( QString("background-color"), m_p_ui->cColorPicker_Box_BackgroundColor->value() );
        progressBarWrapper.setProperty( QString("border-color"), m_p_ui->cColorPicker_Box_BorderColor->value() );
        progressBarWrapper.setProperty( QString("border-width"), QString::number( m_p_ui->spinBox_Box_BorderWidth->value() )  + QString("px") );
    m_styleSheet.addSelection( progressBarWrapper ); 
    //#progressbar (= partie non charge )
    CCssSelection progressBar( QString( "#progressbar" ) );
        progressBar.setProperty( QString("color"), m_p_ui->cColorPicker_ProgressBar_UnloadedTextColor->value() );
        progressBar.setProperty( QString("background-color"), m_p_ui->cColorPicker_ProgressBar_UnloadedBackgroundColor->value() );
        progressBar.setProperty( QString("border-color"), m_p_ui->cColorPicker_ProgressBar_BorderColor->color().name() );
        progressBar.setProperty( QString("border-width"), QString::number( m_p_ui->spinBox_ProgressBar_BorderWidth->value() ) + QString("px") );        
        CCssSelection progressBarLoaded( QString( "div.progress" ) );
            progressBarLoaded.setProperty( QString("color"), m_p_ui->cColorPicker_ProgressBar_LoadedTextColor->value() );
            progressBarLoaded.setProperty( QString("background-color"), m_p_ui->cColorPicker_ProgressBar_LoadedBackgroundColor->value() );
        progressBar.addSubSelection( progressBarLoaded );
    m_styleSheet.addSelection( progressBar );
     
}

/*******************************************************************
* void fromDomDocument( )
* ------------------------
* Met  jour les paramtres internes  partir d'un QDomDocument
* Note: Pour la cration de la liste de ressources, regarder les commentaires
* internes  la fonction
********************************************************************/
void CSkinParameters::fromDomDocument( QDomDocument &document )
{

    QDomElement root = document.firstChildElement( "Skin" );

    //--- Conversion automatique de la skin si elle a t cre par une version antrieure  la version actuelle
    m_version = root.attribute( "release" ).remove( QChar('M') ).toInt( );
    unsigned int currentVersion = CPlatform::revisionInt();
    if( m_version < currentVersion ){
        document = convertFromOldVersion( document, m_version );
        root = document.firstChildElement( "Skin" );
    }

    QDomNamedNodeMap rootAttributes = root.attributes();
    //Css
    QDomElement styleSheetElement = root.firstChildElement( STYLESHEET_TAG_NAME );
    m_styleSheet = CCssSheet( styleSheetElement );
    //Attributs utiles pour calculs
    QStringList selectors;
    selectors << ".thumbBox";
    CCssSelection thumbSpacing = m_styleSheet.selection( selectors );
    this->thumbBoxBorderSize = thumbSpacing.property("border-width").remove("px").toInt();
    selectors.clear();
    selectors << ".mainPhoto";
    CCssSelection photoPadding = m_styleSheet.selection( selectors );
    this->photoPaddingSize = photoPadding.property("border-width").remove("px").toInt();
    selectors.clear();
    selectors << ".thumbBox" << "img";
    CCssSelection thumbnail = m_styleSheet.selection( selectors );
    this->thumbImgBorderSize = thumbnail.property("border-width").remove("px").toInt();
    selectors.clear();
    selectors << ".thumbsWrapper";
    CCssSelection thumbsMosaic = m_styleSheet.selection( selectors );
    //Update toolbar's style
    selectors.clear();
    selectors << "div#cadrePhoto";
    CCssSelection photoFrame = m_styleSheet.selection( selectors );
    //Properties
    setName( rootAttributes.namedItem( "name" ).nodeValue() );
    QDomElement miscProperties = root.firstChildElement( "properties" );
    //Ressources: construction de la liste de ressources
    m_resources.clear();
    QDomElement resourcesElement = root.firstChildElement( RESOURCES_TAG_NAME );
    QDomNode node = resourcesElement.firstChild();
    while( !node.isNull() ){ //On parcourt toutes les ressources
        if( node.isElement() ){ //Seuls les lments nous intressent
            QDomElement element = node.toElement();            
            if( element.tagName() == RESOURCEFILE_TAG_NAME ){ //On l'utilise si c'est bien un fichier de ressource
                QDomNode filename = element.firstChild();
                if( filename.isText() ){ //Ca doit tre du texte
                    QFileInfo ressourceFile( filename.toText().data() );
                    if( !ressourceFile.exists() ){                                            //Si le fichier n'existe pas dans le rpertoire indiqu, il est peut-tre
                        ressourceFile = QFileInfo( getImagePath( ressourceFile.fileName()) ); //dans le rpertoire d'images de la skin (ie une skin cre sur un autre pc)
                    }
                    m_resources.insert( element.attribute("id"), ressourceFile.absoluteFilePath() );
                }

            }
        }
        node = node.nextSibling();
    }
f_initialized = true;
}


/*******************************************************************
* void toUi( )
* ------------------------
* Met  jour l'UI avec les paramtres internes
********************************************************************/
void CSkinParameters::toUi(  )
{

    // !! ATTENTION : Faire les setImages avant les setColor correspondant (et connects...) !!

    int index; //Pour afficher dans une comboBox. Ncessite que l'item soit bien prsent !        
    QStringList path;
    
    // ->on commence par clearer tous les CImagePicker, sinon leur proprits risquent de rester
    // en cas d'ouverture de skin alors qu'on tait en train d'diter
    //--> NE PAS OUBLIER D'AJOUTER LES NOUVEAUX...
    m_p_ui->CImagePicker_NextButton_Icon->clearImage();
    m_p_ui->CImagePicker_PhotoFraming_Texture->clearImage();
    m_p_ui->CImagePicker_PreviousButton_Icon->clearImage();
    m_p_ui->cImagePicker_Index_BckgTexture->clearImage();
    m_p_ui->cImagePicker_Mosaic_BckgTexture->clearImage();
    m_p_ui->cImagePicker_Photo_BckgTexture->clearImage();

    m_p_ui->lineEdit_SkinName->setText( m_name );    

    //-------------- ///////////////// INDEX //////////////// ----------------//
    path.clear();
    //ScreenIndex
    path <<  "#screenIndex";
    CCssSelection screenIndex = m_styleSheet.selection( path );
         m_p_ui->cImagePicker_Index_BckgTexture->setImage( getImagePath( m_resources.value("IndexPage_BckgTexture").absoluteFilePath() ) );
         m_p_ui->cColorPicker_Index_BckgColor->setColor( screenIndex.property( "background-color" ));
         if( screenIndex.property( "background-size" ) == "cover" ){ 
             m_p_ui->checkBox_Index_BckgCover->setChecked(true);
         } else {
             m_p_ui->checkBox_Index_BckgCover->setChecked(false);
         }
    path.clear();
    // title
    path << "div#indexTitle";
        CCssSelection indexTitle = m_styleSheet.selection( path );
        QString alignment = indexTitle.property( "text-align" );
        int idx =  m_p_ui->comboBox_VTitleText_Alignment->findText( alignment );
        if( idx != -1 ) {
            m_p_ui->comboBox_VTitleText_Alignment->setCurrentIndex( idx );
        }
        idx = m_p_ui->comboBox_VTitleText_FontFamily->findText( indexTitle.property("font-family") );
        m_p_ui->comboBox_VTitleText_FontFamily->setCurrentIndex( idx );
        m_p_ui->doubleSpinBox_VTitle_FontSize->setValue(  indexTitle.property("font-size").remove(QString("em")).toDouble() );
        m_p_ui->cColorPicker_VTitleText_TextColor->setColor( indexTitle.property("color") );
    path.clear();
    
    //.thumbsWrapper mosaic
    path << "#mosaic";
    CCssSelection thumbsMosaic = m_styleSheet.selection( path );        
        m_p_ui->cColorPicker_Mosaic_BorderColor->setColor( thumbsMosaic.property("border-color") );
        m_p_ui->spinBox_Mosaic_BorderWidth->setValue( thumbsMosaic.property("border-width").remove("px").toInt() );
        m_p_ui->cImagePicker_Mosaic_BckgTexture->setImage( m_resources.value("Mosaic_BckgTexture").absoluteFilePath() );
        //background color is optional
        bool fHasBckgColor = (thumbsMosaic.property("ezwg-background-color-disabled") != "true");
        m_p_ui->cColorPicker_Mosaic_BckgColor->setEnabled( fHasBckgColor );
        if( fHasBckgColor ) { 
            m_p_ui->cColorPicker_Mosaic_BckgColor->setColor( thumbsMosaic.property("background-color") );            
        }
        m_p_ui->checkBox_Mosaic_BckgColor_Enabled->setChecked( fHasBckgColor );

    path.clear();
    path << ".thumbBox";
    CCssSelection thumbSpacing = m_styleSheet.selection( path );
        m_p_ui->spinBox_Mosaic_SpacingWidth->setValue( thumbSpacing.property("border-width").remove("px").toInt() );
        m_p_ui->cColorPicker_Mosaic_SpacingColor->setColor( thumbSpacing.property("border-color") );
    //.thumbBox img     Vignette
        path << "img";
        CCssSelection thumbnail = m_styleSheet.selection( path );
            m_p_ui->spinBox_Thumbnail_BorderWidth->setValue( thumbnail.property("border-width").remove("px").toInt() );
            m_p_ui->cColorPicker_Thumbnail_BorderColor->setColor( thumbnail.property("border-color") );
    //.thumbBox img:hover     Vignette survole
    path.clear();
    path << ".thumbBox" << "img:hover";
    CCssSelection thumbnailHovered = m_styleSheet.selection( path );
        m_p_ui->cColorPicker_Thumbnail_Hovered_BorderColor->setColor( thumbnailHovered.property("border-color") );

    //-------------- ///////////////// PHOTO //////////////// ----------------//
    path.clear();
    //ScreenPhoto
    path <<  "#screenPhoto";
    CCssSelection screenPhoto = m_styleSheet.selection( path );
        m_p_ui->cImagePicker_Photo_BckgTexture->setImage( getImagePath( m_resources.value("PhotoPage_BckgTexture").absoluteFilePath() ) );
        m_p_ui->cColorPicker_Photo_BckgColor->setColor( screenPhoto.property("background-color") );
        if( screenPhoto.property( "background-size" ) == "cover" ){ 
            m_p_ui->checkBox_Photo_BckgCover->setChecked(true);
        } else {
            m_p_ui->checkBox_Photo_BckgCover->setChecked(false);
        }
    path.clear();
    //Framing - cadre
    path << "div#cadrePhoto";
    CCssSelection photoFrame = m_styleSheet.selection( path );
        m_p_ui->CImagePicker_PhotoFraming_Texture->setImage( getImagePath( m_resources.value("PhotoFraming_Texture").absoluteFilePath() ));
        m_p_ui->cColorPicker_PhotoFraming_Color->setColor( photoFrame.property("background-color") );
        m_p_ui->spinBox_PhotoFraming_Width->setValue( photoFrame.property("padding").remove("px").toInt() );
    path.clear();
    //Framing - classe (runtime) mainphoto - Permet de controler la petite bordure autour de la photo (padding dans la gui)
    path << ".mainPhoto";
    CCssSelection photoPadding = m_styleSheet.selection( path );
        m_p_ui->cColorPicker_PhotoFraming_PaddingColor->setColor( photoPadding.property("border-color") );
        m_p_ui->spinBox_PhotoFraming_PaddingWidth->setValue( photoPadding.property("border-width").remove("px").toInt() );
    path.clear();
    //Lgendes
    path << "div.photoCaption";
    CCssSelection photoCaptions = m_styleSheet.selection( path );
        index = m_p_ui->comboBox_PhotoCaption_FontWeight->findText( photoCaptions.property("font-weight") );
        m_p_ui->comboBox_PhotoCaption_FontWeight->setCurrentIndex( index );
        index = m_p_ui->comboBox_PhotoCaption_FontFamily->findText( photoCaptions.property("font-family") );
        m_p_ui->comboBox_PhotoCaption_FontFamily->setCurrentIndex( index );
        m_p_ui->doubleSpinBox_PhotoCaption_FontSize->setValue( photoCaptions.property("font-size").remove("em").toDouble() );
        m_p_ui->cColorPicker_PhotoCaption_TextColor->setColor( photoCaptions.property("color") );
    path.clear();
    //Button Icons
    m_p_ui->CImagePicker_PreviousButton_Icon->setImage( getImagePath( m_resources.value("PhotoButtonPrevious").absoluteFilePath() ));
    m_p_ui->CImagePicker_NextButton_Icon->setImage( getImagePath( m_resources.value("PhotoButtonNext").absoluteFilePath() ));

    //-------------- ///////////////// MISC //////////////// ----------------//
    path.clear();
    //--- Progress Bar ---//
    path << "#progressbarWrapper";
    CCssSelection progressBar = m_styleSheet.selection( path );
        index = m_p_ui->comboBox_Box_FontFamily->findText( progressBar.property("font-family") );
        m_p_ui->comboBox_Box_FontFamily->setCurrentIndex( index );
        m_p_ui->cColorPicker_Box_TextColor->setColor( progressBar.property("color") );
        m_p_ui->spinBox_Box_BorderWidth->setValue( progressBar.property("border-width").remove("px").toInt() );
        m_p_ui->cColorPicker_Box_BorderColor->setColor( progressBar.property("border-color") );
        m_p_ui->cColorPicker_Box_BackgroundColor->setColor(  progressBar.property("background-color") );
    path.clear();
    path << "#progressbar";
    progressBar = m_styleSheet.selection( path );
        m_p_ui->spinBox_ProgressBar_BorderWidth->setValue(progressBar.property("border-width").remove("px").toInt() );
        m_p_ui->cColorPicker_ProgressBar_BorderColor->setColor(progressBar.property("border-color") );
        m_p_ui->cColorPicker_ProgressBar_UnloadedTextColor->setColor( progressBar.property("color") );
        m_p_ui->cColorPicker_ProgressBar_UnloadedBackgroundColor->setColor( progressBar.property("background-color") );
        path << "div.progress";
        progressBar = m_styleSheet.selection( path );
            m_p_ui->cColorPicker_ProgressBar_LoadedTextColor->setColor( progressBar.property("color") );
            m_p_ui->cColorPicker_ProgressBar_LoadedBackgroundColor->setColor( progressBar.property("background-color") );        
}


/*******************************************************************
* QDomDocument toDomDocument( )
* ------------------------
* Cr un QDomDocument contenant tous les paramtres
********************************************************************/
QDomDocument CSkinParameters::toDomDocument( )
{
    QDomDocument document;
    QDomElement root = document.createElement( SKIN_TAG_NAME );
    root.setAttribute( QString("release"), QString::number(CPlatform::revisionInt()) );
    root.setAttribute( QString("name"), m_name );
    document.appendChild( root );  
    
    //Style
    QDomElement styleSheetElement = m_styleSheet.toDomElement( document );
    root.appendChild( styleSheetElement );

    //Ressources
    QDomElement resourcesElement = document.createElement( RESOURCES_TAG_NAME );
    root.appendChild( resourcesElement );
    foreach( QString widget, m_resources.keys() ) {
        QDomElement fileElement = document.createElement( RESOURCEFILE_TAG_NAME ); // <File id="widget">FileName</File>
        fileElement.setAttribute( "id",widget );
        resourcesElement.appendChild( fileElement );
        fileElement.appendChild( document.createTextNode( m_resources.value(widget).fileName() ) );
    }
    return document;
}


/*******************************************************************
* QSize mosaicDecoration( unsigned int nbCols )
* ------------------------
* Returns space used by the mosaic decoration on the index page
*   ie: thumbBorders + thumbBoxBorders + MosaicBorders
* Input: nb rows & cols of the mosaic
********************************************************************/
QSize CSkinParameters::mosaicDecoration( unsigned int nbCols )
{    
    QStringList selectors;
    selectors << ".thumbBox";
    CCssSelection thumbSpacing = m_styleSheet.selection( selectors );
    thumbBoxBorderSize = thumbSpacing.property("border-width").remove("px").toInt();
    selectors << "img";
    CCssSelection thumbnail = m_styleSheet.selection( selectors );
    thumbImgBorderSize = thumbnail.property("border-width").remove("px").toInt();
    selectors.clear();
    selectors << ".thumbsWrapper";
    CCssSelection thumbsMosaic = m_styleSheet.selection( selectors );
    int mosaicBorderSize = thumbsMosaic.property("border-width").remove("px").toInt();

    return QSize( 2*nbCols*(thumbImgBorderSize + thumbBoxBorderSize) + 2*mosaicBorderSize,
                  0 );
}


/*******************************************************************
* QList<CCss> toCssList( )
* ------------------------
* Cr une liste de feuilles CSS  partir des paramtres
* ATTENTION: Trs dpendant de la syntaxe attendue pour les fichiers CSS des skins
********************************************************************/
CCssSheet CSkinParameters::toCss( ) const
{
    return m_styleSheet;
}


/*******************************************************************
* saveSkin( const QDir & destFile, QStringList &errorMsgs )
* ------------------------
* Sauvegarde la skin sur le disque. Copie les fichiers ressources dans
* l'arborescence de la skin.
* in dir : rpertoire o sauver la skin
* return : False si erreurs
********************************************************************/
bool CSkinParameters::saveSkin( const QString &destFile )
{
    QString clearedString;    

    m_lastErrors.clear();;
    clearedString.clear();

    //--- MAJ
    fromUi( );

    //--- Vrification que l'on essaie pas d'craser la skin par dfaut
    QFileInfo defaultSkin(CSkinParameters::defaultSkin());
    if( QFileInfo(destFile) == defaultSkin ){ //QFileInfo ncessaire pour vrifier qu'il s'agit bien d'un mme fichier:
        m_lastErrors.append( CError(  tr("Cannot overwrite default skin."), defaultSkin.absoluteFilePath() ) );        //comparer le path est trop limit. Exemple: pb de case sensitive sous windows.
        return false;
    }
    //--- Cration du XML
    QFile skinXmlFile( destFile );
    QTextStream skinXmlStream( &skinXmlFile );
    skinXmlStream.setCodec( "UTF-8" );
    skinXmlStream.setGenerateByteOrderMark (true);
    if ( skinXmlFile.open(QIODevice::WriteOnly | QIODevice::Text) ){        
        fromUi( ); //rcupration des paramtres courants
        QDomDocument skinXmlDoc = toDomDocument( );        
        skinXmlStream << skinXmlDoc.toString( 4 ); //criture dans le fichier
        skinXmlFile.close();
    }
    
    //--- Copie des fichiers ressources ncessaires
    QDir outSkinPath = QFileInfo( destFile ).absolutePath();
    m_resourcesPath = QFileInfo( destFile ).fileName().remove(SKINSESSIONEXTENSION) + QString("_files/");
    const QString skinImgPath =  m_resourcesPath + QString(SKINRESIMGDIR);
    if( !outSkinPath.mkpath( skinImgPath ) || !outSkinPath.cd( skinImgPath ) ){
        m_lastErrors.append( CError(CError::DirectoryCreation, skinImgPath) );
        return false; //Il faut absolument un point de sortie : sinon on risque d'effacer des fichiers n'importe ou !
    }
    //On efface le contenu du rpertoire pour viter que les fichiers ne s'accumulent    
    outSkinPath.setFilter( QDir::Files );
    foreach(QFileInfo fileInfo, outSkinPath.entryInfoList()) {
        //Attention : si on a ouvert la skin: il y a des chances qu'on essaie de rcrire au les fichiers ressources sur eux-mme ! Il faut viter a...
        if( !m_resources.contains( m_resources.key(fileInfo) ) ){
            if(fileInfo.isFile() && fileInfo.isWritable() ){
                outSkinPath.remove( fileInfo.fileName() );
            }
        }
    }
    //Copie des fichiers
    bool f_copySuccessful = copyResources( outSkinPath );
    if( f_copySuccessful ){
        m_filePath = destFile;
        emit skinSaved( destFile );
    }

    return f_copySuccessful;
}


/*******************************************************************
* copyResources( QDir outputDir )
* ------------------------
* Copie tous les fichiers ressources vers le rpertoire spcifi
* In: (QDir) outputDir - rpertoire o copier les fichiers ressource
* Return: true si succs, false si echec
********************************************************************/
bool CSkinParameters::copyResources( QDir outputDir )
{
    int size = m_lastErrors.size();

    foreach( QFileInfo resourceFile, m_resources )
    {
        QFileInfo destFileInfo( outputDir.absoluteFilePath(resourceFile.fileName()) );
        //Si il existe un fichier destination exatement identique au fichier source, pas la peine de s'embter
        if( destFileInfo.size() != resourceFile.size() &&
            destFileInfo.lastModified() != resourceFile.lastModified() )
        {
           // Si un fichier du mme nom existe dj: le supprimer (sinon erreur lors de la copie)
            if( destFileInfo.exists() )
            {
                if( !outputDir.remove( destFileInfo.fileName() ) ){//Impossible de supprimer le fichier
                    m_lastErrors.append( CError(CError::FileCreation, destFileInfo.absoluteFilePath()) );
                }
            }
            //Copie
            if( !QFile::copy( resourceFile.absoluteFilePath(), destFileInfo.absoluteFilePath()) ){
                m_lastErrors.append( CError(CError::FileCreation, destFileInfo.absoluteFilePath()) );
            }
        }
    }

    if( size == m_lastErrors.size() ){
        return true;
    }
    else{
        return false;
    }

}


/*******************************************************************
* load( const QString &skinFileName )
* ------------------------
* Charge une skin: lit le contenu du fichier XML et affiche dans l'UI
* in skinFileName : Le chemin vers le XML de la skin  ouvrir
* return : true si l'ouverture c'est bien passe
********************************************************************/
bool CSkinParameters::load( const QString &skinFileName )
{
    QDomDocument skinDoc;
    QString* errorMsg = new QString();
    int ln, col;
    QString skinFileNameCopy = skinFileName;
    QFile skinFile( skinFileName );

    if( !QFile(skinFileName).exists() ) {
        m_lastErrors.append( CError( CError::FileOpening, skinFileName) );
        return false;
    }

    if( skinDoc.setContent( &skinFile, false, errorMsg, &ln, &col ) )
    {
        m_resourcesPath = skinFileNameCopy.remove(SKINSESSIONEXTENSION) + QString("_files/");
        m_filePath = skinFileName;
        fromDomDocument( skinDoc );
        emit skinOpened( skinFileName );
        emit nameChanged( m_name );
        delete errorMsg;
        return true;
    }
    else{
        m_lastErrors.append( CError( tr("An error occurred while loading the skin file ") + skinFileName, *errorMsg) );
        delete errorMsg;
        return false;
    }

}

/*******************************************************************
* check( QStringList* errorList, QStringList* warningList)
* ------------------------
* Vrifie la validit de la skin
* out : errorList, Liste des erreurs releves
* out : warningList, Liste des avertissements releves
********************************************************************/
void CSkinParameters::check( QStringList* errorList, QStringList* )
{
    //Maj
    fromUi();
    //Caractre invalide dans les noms des ressources
    foreach( QFileInfo ressource, m_resources )
    {
        if( ressource.completeBaseName().contains( CPlatform::forbiddenCharacters() )){
            errorList->append( QString("\"") + ressource.fileName() + QString("\" - ") + tr("invalid character in filename.") );
        }
    }
    //Prsence des boutons ncessaires
    if( !m_resources.contains("PhotoButtonPrevious") ){
        errorList->append( tr("Button \"Previous\" - an image is required.") );
    }
    if( !m_resources.contains("PhotoButtonNext") ){
        errorList->append( tr("Button \"Next\" - an image is required.") );
    }
}


/*******************************************************************
* defaultSkin(  )
* ------------------------
* Renvoie le chemin absolu de la skin par dfaut
* Fonction statique
********************************************************************/
QString CSkinParameters::defaultSkin()
{
    QString defaultSkinPath = CPlatform::skinDirPath() +  QString("/") + QString(DEFAULTSKINNAME) + QString(SKINSESSIONEXTENSION);
    return defaultSkinPath;
}


/*******************************************************************
* convertFromOldVersion( const QDomDocument &document, int oldVersionNumber )
* ---------
* Conversion d'une ancienne skin dans le format actuel
* In/Out: (QDomDocument) document  convertir -> Sera modifi !
* In : (int) oldVersionNumber - version d'EZWG ayant servi  sauver le projet
* Return : (bool) true si quelque chose a t modifi, false sinon
********************************************************************/
QDomDocument CSkinParameters::convertFromOldVersion(  const QDomDocument &document, const int oldVersionNumber )
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



/*******************************************************************
* getImagePath( const QString &img)
* ------------------------
* Renvoie le chemin absolu de l'image passe en paramtre
* in img : Image
* return : Le chemin absolu de l'image
********************************************************************/
QString CSkinParameters::getImagePath( const QString &img ) const
{
    QDir dir = QDir( m_resourcesPath );
    dir.cd( SKINRESIMGDIR );
    return dir.absoluteFilePath( img );
}

/*******************************************************************
* removeEmptyResources( const QString &img)
* ------------------------
* Enlve les fichiers "vides" de la liste des ressources.
* ex: une image qui n'a pas t choisie (widget affichant "no image")
********************************************************************/
void CSkinParameters::removeEmptyResources( )
{
	foreach( QFileInfo fileinfo, m_resources ){
		if( !fileinfo.isFile() ){
			m_resources.remove( m_resources.key( fileinfo ) );
		}
	}
}


/*******************************************************************
* buttonImage( int button )
* ------------------------
* Renvoie le nom du fichier de l'icone dsigne en paramtre
* in button : le nom de l'icone
* return : le nom du fichier image correspondant
********************************************************************/
QString CSkinParameters::buttonImage( int button ) const
{
    QString resource;
    QStringList cssPath;
    CCssSelection photoFrame;

    switch( button )
    {
    case CSkinParameters::buttonNext:
        resource = m_resources.value("PhotoButtonNext").fileName();
        break;
    case CSkinParameters::buttonPrevious:
        resource = m_resources.value("PhotoButtonPrevious").fileName();
        break;
    case CSkinParameters::toolbar:        
        cssPath << "div#cadrePhoto";
        photoFrame = m_styleSheet.selection( cssPath );
        resource = CToolbarStyleFactory::GetInstance().getStyle( photoFrame.property("background-color") ).getHtml();
        break;
    default:
        resource = QString("Error");
    }

    return resource;
}



