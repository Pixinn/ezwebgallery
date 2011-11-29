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
#include "CPlatform.h"
#include "GlobalDefinitions.h"
#include "CError.h"

//----------- Dfinitions ---------//
#define RESSOURCES_TAG_NAME     "Ressources"
#define RESSOURCEFILE_TAG_NAME  "File"


//-------------- Variables statiques ---------//



//----------- Mthodes ------------//


/****************************************************
* CSkinParameters( )
* ---------
* Constructeur par dfaut
*****************************************************/
CSkinParameters::CSkinParameters()
{        
    //--- Inits
    m_styleSheet = CCssSheet("skin");
    setName( QString("noname") );
    m_p_ui = NULL;
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
    if( this != &source){
        this->thumbImgBorderSize = source.thumbImgBorderSize;
        this->thumbBoxBorderSize = source.thumbBoxBorderSize;
        this->photoPaddingSize = source.photoPaddingSize;
        this->m_name = source.m_name;
        this->m_ressourcesPath = source.m_ressourcesPath;
        this->m_p_ui = source.m_p_ui;
        this->m_misc = source.m_misc;
        this->m_ressources = source.m_ressources;
        this->m_styleSheet = source.m_styleSheet;
        this->m_filePath = source.m_filePath;
        this->m_version = source.m_version;
    }
    return *this;
}

/****************************************************
* operator=(const CSkinParameters& source)
* ---------
* L'hritage de QObject oblige de redfinir cet oprateur
* voir: http://www.fredosaurus.com/notes-cpp/oop-overloading/overloadassign.html
*****************************************************/
bool CSkinParameters::operator ==( const CSkinParameters &source)
{
    if( this->thumbImgBorderSize != source.thumbImgBorderSize ){    return false;   }
    if( this->thumbBoxBorderSize != source.thumbBoxBorderSize ){    return false;   }
    if( this->photoPaddingSize != source.photoPaddingSize ){    return false;   }
    if( this->m_name != source.m_name ){    return false;   }
    if( this->m_ressourcesPath != source.m_ressourcesPath ){    return false;   }
    if( this->m_misc.titlePosition != source.m_misc.titlePosition ){    return false;   }
    if( this->m_ressources != source.m_ressources ){    return false;   }
    if( this->m_styleSheet != source.m_styleSheet ){    return false;   }
    if( this->m_filePath != source.m_filePath ){    return false;   }
    if( this->m_version != source.m_version ){ return false; }
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
QString CSkinParameters::name( )
{
    return m_name;
}


/*******************************************************************
* name( )
* ---------
* Retourne le path de la skin. Vide si non sauve
********************************************************************/
QString CSkinParameters::filePath( )
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
    m_misc.titlePosition = m_p_ui->comboBox_VTitle_Position->currentIndex();
    setName( m_p_ui->lineEdit_SkinName->text() );
    /****** Ressources ******/
    //Construction de la liste de ressources ncessaires
    m_ressources.clear();
    m_ressources.insert( "VTitle_BackgroundTexture", m_p_ui->cImagePicker_VTitle_BackgroundTexture->fileName() );
    m_ressources.insert( "IndexPage_BckgTexture", m_p_ui->cImagePicker_Index_BckgTexture->fileName() );
    m_ressources.insert( "Mosaic_BckgTexture", m_p_ui->cImagePicker_Mosaic_BckgTexture->fileName( ) );
    m_ressources.insert( "PhotoFraming_Texture", m_p_ui->CImagePicker_PhotoFraming_Texture->fileName() );
    m_ressources.insert( "PhotoPage_BckgTexture",m_p_ui->cImagePicker_Photo_BckgTexture->fileName() );    
    m_ressources.insert( "PhotoButtonPrevious", m_p_ui->CImagePicker_PreviousButton_Icon->fileName() );
    m_ressources.insert( "PhotoButtonNext", m_p_ui->CImagePicker_NextButton_Icon->fileName() );
//    m_ressources.insert( "PhotoButtonIndex", m_p_ui->CImagePicker_BrowseButton_Icon->fileName() );
    //Ressources ajoutes sous conditions
    if( m_p_ui->groupBox_PhotoNavBlocksBckg->isChecked() ){
        m_ressources.insert( "PhotoNavBlocks_BckgTexture", m_p_ui->CImagePicker_PhotoNavBlocks_BckgTexture->fileName() );
    }
    //Filtrage des ressources:
    removeEmptyRessources( ); // On enlve de la liste les images "vides" (ie no image)
     
    /****** Cration du CSS ******/

    //-------------- ///////////////// INDEX //////////////// ----------------//
    //ScreenIndex
    CCssSelection screenIndex( QString( "#screenIndex" ) );
        screenIndex.setProperty( QString("background-image"), QString("url(../images/") + QFileInfo(m_p_ui->cImagePicker_Index_BckgTexture->fileName()).fileName() + QString(")") );
        screenIndex.setProperty( QString("background-color"), m_p_ui->cColorPicker_Index_BckgColor->value() );
    m_styleSheet.addSelection( screenIndex );
    //ul.indexNavigation
    CCssSelection indexNav( QString("ul.indexNavigation") );
        CCssSelection indexNavTab( QString("li") );
            indexNavTab.setProperty( QString("background-color"), m_p_ui->cColorPicker_BrowsingTabs_BckgColor->value() );
                CCssSelection indexNavTabText( QString("a") );
                    indexNavTabText.setProperty( QString("color"), m_p_ui->cColorPicker_BrowsingTabs_TextColor->value() );
            indexNavTab.addSubSelection( indexNavTabText );
        CCssSelection indexNavTabSelected( QString("li.navTabSelected") );
            indexNavTabSelected.setProperty( QString("background-color"), m_p_ui->cColorPicker_BrowsingTabs_Selected_BckgColor->value() );
            indexNavTabSelected.setProperty( QString("border-width"), QString::number(m_p_ui->spinBox_BrowsingTabs_Selected_BorderWidth->value()) + QString("px") );
            indexNavTabSelected.setProperty( QString("border-color"), m_p_ui->cColorPicker_BrowsingTabs_Selected_BorderColor->value() );
            CCssSelection indexNavTabSelectedText( QString("a") );
                indexNavTabSelectedText.setProperty( QString("color"), m_p_ui->cColorPicker_BrowsingTabs_Selected_TextColor->value() );
            indexNavTabSelected.addSubSelection( indexNavTabSelectedText );
     indexNav.addSubSelection( indexNavTabSelected );
     indexNav.addSubSelection( indexNavTab );
     m_styleSheet.addSelection( indexNav );
     //.thumbsWrapperContainer Mosaque
     CCssSelection thumbsMosaic( QString(".thumbsWrapperContainer"));
        thumbsMosaic.setProperty( QString("background-image"), QString("url(../images/") + QFileInfo(m_p_ui->cImagePicker_Mosaic_BckgTexture->fileName()).fileName() + QString(")") );
        thumbsMosaic.setProperty( QString("background-color"), m_p_ui->cColorPicker_Mosaic_BckgColor->value() );
        thumbsMosaic.setProperty( QString("border-width"), QString::number( m_p_ui->spinBox_Mosaic_BorderWidth->value() ) + QString("px") );
        thumbsMosaic.setProperty( QString("border-color"), m_p_ui->cColorPicker_Mosaic_BorderColor->value() );
     m_styleSheet.addSelection( thumbsMosaic );
    CCssSelection thumbSpacing( QString(".thumbBox") );  //Espace entre les vignettes
        this->thumbBoxBorderSize = m_p_ui->spinBox_Mosaic_SpacingWidth->value();
        thumbSpacing.setProperty( QString("border-width"), QString::number( thumbBoxBorderSize ) + QString("px") );
        thumbSpacing.setProperty( QString("border-color"), m_p_ui->cColorPicker_Mosaic_SpacingColor->value() );
    //.thumbBox img     Vignette
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

    //Titre
    /*CCssSelection photoTitle( QString("div#photoTitle") );
        photoTitle.setProperty( QString("font-weight"), m_p_ui->comboBox_PhotoTitle_FontWeight->currentText() );
        photoTitle.setProperty( QString("font-family"), m_p_ui->comboBox_PhotoTitle_FontFamily->currentText() );
        photoTitle.setProperty( QString("font-variant"), m_p_ui->comboBox_PhotoTitle_FontVariant->currentText() );
        photoTitle.setProperty( QString("font-size"), QString::number(m_p_ui->doubleSpinBox_PhotoTitle_FontSize->value()) + QString("em")  );
        //photoTitle.setProperty( QString("min-height"),  QString::number(m_p_ui->doubleSpinBox_PhotoTitle_FontSize->value()) + QString("em")  ); //Mme taille que Title Font-Size
        photoTitle.setProperty( QString("color"), m_p_ui->cColorPicker_PhotoTitle_TextColor->value() );
    m_styleSheet.addSelection( photoTitle );*/
    //Lgendes
    CCssSelection photoCaptions( QString("div.photoCaption") );
        photoCaptions.setProperty( QString("font-weight"), m_p_ui->comboBox_PhotoCaption_FontWeight->currentText() );
        photoCaptions.setProperty( QString("font-family"), m_p_ui->comboBox_PhotoCaption_FontFamily->currentText() );
        photoCaptions.setProperty( QString("font-size"), QString::number(m_p_ui->doubleSpinBox_PhotoCaption_FontSize->value()) + QString("em")  );
        //photoCaptions.setProperty( QString("min-height"),  QString::number(m_p_ui->doubleSpinBox_PhotoTitle_FontSize->value()) + QString("em")  ); //Mme taille que Title Font-Size
        photoCaptions.setProperty( QString("color"), m_p_ui->cColorPicker_PhotoCaption_TextColor->value() );
    m_styleSheet.addSelection( photoCaptions );
    //Buttons
    int buttonBorderW = m_p_ui->spinBox_PhotoButtonsEnabled_BorderWidth->value(); //Idem pour enabled / disabled
    int buttonHoveredBorderW = m_p_ui->spinBox_PhotoButtonsHovered_BorderWidth->value();
    CCssSelection buttonEnabled( QString(".photoButtonEnabled") );
        buttonEnabled.setProperty( QString("border-width"), QString::number(buttonBorderW) + QString("px") );
        buttonEnabled.setProperty( QString("border-color"), m_p_ui->cColorPicker_PhotoButtonsEnabled_BorderColor->value() );
    m_styleSheet.addSelection( buttonEnabled );
    CCssSelection buttonHovered( QString(".photoButtonEnabled:hover") );
        buttonHovered.setProperty( QString("border-width"), QString::number(buttonHoveredBorderW) + QString("px") );
        buttonHovered.setProperty( QString("border-color"), m_p_ui->cColorPicker_PhotoButtonsHovered_BorderColor->value() );
        buttonHovered.setProperty( QString("margin"), QString::number(buttonBorderW - buttonHoveredBorderW) + QString("px") );
    m_styleSheet.addSelection( buttonHovered );
    CCssSelection buttonDisabled( QString(".photoButtonDisabled") );
        buttonDisabled.setProperty( QString("border-width"), QString::number(buttonBorderW) + QString("px") ); //=.photoButtonEnabled::border-width
        buttonDisabled.setProperty( QString("border-color"), m_p_ui->cColorPicker_PhotoButtonsDisabled_BorderColor->value() );
    m_styleSheet.addSelection( buttonDisabled );
    //Navigation Divs

    CCssSelection photoLefter( QString("div#photoLefter") );
        photoLefter.setProperty( QString("width"), QString::number(m_p_ui->spinBox_PhotoNavBlocks_Width->value()) + QString("px"));
        if( m_p_ui->groupBox_PhotoNavBlocksBckg->isChecked() ){ //Fond non-transparent
            photoLefter.setProperty( QString("background-color"), m_p_ui->cColorPicker_PhotoNavBlocks_BckgColor->value() );
            photoLefter.setProperty( QString("background-image"), QString("url(../images/") + QFileInfo(m_p_ui->CImagePicker_PhotoNavBlocks_BckgTexture->fileName()).fileName() + QString(")") );
        }else{ //Fond transparent
            photoLefter.setProperty( QString("background-color"), QString("transparent") );
        }
     m_styleSheet.addSelection( photoLefter );
     CCssSelection photoRighter( QString("div#photoRighter") );
        photoRighter.setProperty( QString("width"), QString::number(m_p_ui->spinBox_PhotoNavBlocks_Width->value()) + QString("px"));
         if( m_p_ui->groupBox_PhotoNavBlocksBckg->isChecked() ){ //Fond non-transparent
            photoRighter.setProperty( QString("background-color"), m_p_ui->cColorPicker_PhotoNavBlocks_BckgColor->value() );
             photoRighter.setProperty( QString("background-image"), QString("url(../images/") + QFileInfo(m_p_ui->CImagePicker_PhotoNavBlocks_BckgTexture->fileName()).fileName() + QString(")") );
        }else{ //Fond transparent
             photoRighter.setProperty( QString("background-color"), QString("transparent") );
        }
    m_styleSheet.addSelection( photoRighter );

    //-------------- ///////////////// MISC //////////////// ----------------//
    //--- Propritts conditionnelles
    QString titleBorderRight( QString::number( m_p_ui->spinBox_VTitleBlock_BorderWidth->value() ) );
    QString titleBorderLeft("0");
    if( m_misc.titlePosition != TITLE_POSITION_LEFT ){
        titleBorderRight = titleBorderLeft;
        titleBorderLeft = QString::number( m_p_ui->spinBox_VTitleBlock_BorderWidth->value() );
    }
    QString titleDisplay("none");
    if( m_p_ui->groupBox_VTitle->isChecked() ){ titleDisplay = QString("block");   }
    
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
    //div#indexTitle
     CCssSelection title( QString( "div#indexTitle" ) );
        title.setProperty( QString("display"), titleDisplay );
        title.setProperty( QString("width"), QString::number( m_p_ui->spinBox_VTitleBlock_Width->value() ) + QString("px") );
        title.setProperty( QString("border-right-width"), titleBorderRight + QString("px") );
        title.setProperty( QString("border-left-width"), titleBorderLeft + QString("px") );
        title.setProperty( QString("border-color"), m_p_ui->cColorPicker_VTitleBlock_BorderColor->value() );
        title.setProperty( QString("color"), m_p_ui->cColorPicker_VTitleText_TextColor->value() );
        title.setProperty( QString("font-size"), QString::number( m_p_ui->doubleSpinBox_VTitle_FontSize->value() ) + QString("em") );
        title.setProperty( QString("line-height"), QString::number( m_p_ui->doubleSpinBox_VTitle_LineHeight->value() ) + QString("em") );
        title.setProperty( QString("font-family"), m_p_ui->comboBox_VTitleText_FontFamily->currentText() );
        if( m_p_ui->groupBox_VTitleBlock_Background->isChecked() ){ //Bckg Non-transparent
            title.setProperty( QString("background-image"), QString("url(../images/") + QFileInfo(m_p_ui->cImagePicker_VTitle_BackgroundTexture->fileName()).fileName() + QString(")") );
            title.setProperty( QString("background-color"), m_p_ui->cColorPicker_VTitle_BackgroundColor->value() );
        }else{ //Bckg Transparent
            title.setProperty( QString("background-color"), "transparent" );
        }
    m_styleSheet.addSelection( title );
     
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
    int currentVersion = CPlatform::revisionInt();
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
    //Properties
    setName( rootAttributes.namedItem( "name" ).nodeValue() );
    QDomElement miscProperties = root.firstChildElement( "properties" );
    m_misc.titlePosition = miscProperties.attribute("title-position").toInt();
    //Ressources: construction de la liste de ressources
    m_ressources.clear();
    QDomElement ressourcesElement = root.firstChildElement( RESSOURCES_TAG_NAME );
    QDomNode node = ressourcesElement.firstChild();
    while( !node.isNull() ){ //On parcourt toutes les ressources
        if( node.isElement() ){ //Seuls les lments nous intressent
            QDomElement element = node.toElement();            
            if( element.tagName() == RESSOURCEFILE_TAG_NAME ){ //On l'utilise si c'est bien un fichier de ressource
                QDomNode filename = element.firstChild();
                if( filename.isText() ){ //Ca doit tre du texte
                    QFileInfo ressourceFile( filename.toText().data() );
                    if( !ressourceFile.exists() ){                                            //Si le fichier n'existe pas dans le rpertoire indiqu, il est peut-tre
                        ressourceFile = QFileInfo( getImagePath( ressourceFile.fileName()) ); //dans le rpertoire d'images de la skin (ie une skin cre sur un autre pc)
                    }
                    m_ressources.insert( element.attribute("id"), ressourceFile.absoluteFilePath() );
                }

            }
        }
        node = node.nextSibling();
    }

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
//    m_p_ui->CImagePicker_BrowseButton_Icon->clearImage();
    m_p_ui->CImagePicker_NextButton_Icon->clearImage();
    m_p_ui->CImagePicker_PhotoFraming_Texture->clearImage();
    m_p_ui->CImagePicker_PhotoNavBlocks_BckgTexture->clearImage();
    m_p_ui->CImagePicker_PreviousButton_Icon->clearImage();
    m_p_ui->cImagePicker_Index_BckgTexture->clearImage();
    m_p_ui->cImagePicker_Mosaic_BckgTexture->clearImage();
    m_p_ui->cImagePicker_Photo_BckgTexture->clearImage();
    m_p_ui->cImagePicker_VTitle_BackgroundTexture->clearImage();

    m_p_ui->lineEdit_SkinName->setText( m_name );    

    //-------------- ///////////////// INDEX //////////////// ----------------//
    path.clear();
    //ScreenIndex
    path <<  "#screenIndex";
    CCssSelection screenIndex = m_styleSheet.selection( path );
         m_p_ui->cImagePicker_Index_BckgTexture->setImage( getImagePath( m_ressources.value("IndexPage_BckgTexture").absoluteFilePath() ) );
         m_p_ui->cColorPicker_Index_BckgColor->setColor( screenIndex.property( "background-color" ));
    path.clear();
    //ul.indexNavigation
    path << "ul.indexNavigation" << "li";
    CCssSelection indexNavTab = m_styleSheet.selection( path );
        m_p_ui->cColorPicker_BrowsingTabs_BckgColor->setColor( indexNavTab.property("background-color") );
        path << "a";
        CCssSelection indexNavTabText = m_styleSheet.selection( path );
            m_p_ui->cColorPicker_BrowsingTabs_TextColor->setColor( indexNavTabText.property("color") );
    path.clear();
    path << "ul.indexNavigation" << "li.navTabSelected";
    CCssSelection indexNavTabSelected = m_styleSheet.selection( path );
        m_p_ui->cColorPicker_BrowsingTabs_Selected_BckgColor->setColor( indexNavTabSelected.property("background-color") );
        m_p_ui->spinBox_BrowsingTabs_Selected_BorderWidth->setValue( indexNavTabSelected.property("border-width").remove("px").toInt() );
        m_p_ui->cColorPicker_BrowsingTabs_Selected_BorderColor->setColor( indexNavTabSelected.property("border-color"));
        path << "a";
        CCssSelection indexNavTabSelectedText = m_styleSheet.selection( path );
            m_p_ui->cColorPicker_BrowsingTabs_Selected_TextColor->setColor( indexNavTabSelectedText.property("color") );
    path.clear();
    //.thumbsWrapperContainer Mosaque
    path << ".thumbsWrapperContainer";
    CCssSelection thumbsMosaic = m_styleSheet.selection( path );
        m_p_ui->cImagePicker_Mosaic_BckgTexture->setImage( m_ressources.value("Mosaic_BckgTexture").absoluteFilePath() );
        m_p_ui->cColorPicker_Mosaic_BckgColor->setColor( thumbsMosaic.property("background-color") );
        m_p_ui->spinBox_Mosaic_BorderWidth->setValue( thumbsMosaic.property("border-width").remove("px").toInt() );
        m_p_ui->cColorPicker_Mosaic_BorderColor->setColor( thumbsMosaic.property("border-color") );
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
        m_p_ui->cImagePicker_Photo_BckgTexture->setImage( getImagePath( m_ressources.value("PhotoPage_BckgTexture").absoluteFilePath() ) );
        m_p_ui->cColorPicker_Photo_BckgColor->setColor( screenPhoto.property("background-color") );
    path.clear();
    //Framing - cadre
    path << "div#cadrePhoto";
    CCssSelection photoFrame = m_styleSheet.selection( path );
        m_p_ui->CImagePicker_PhotoFraming_Texture->setImage( getImagePath( m_ressources.value("PhotoFraming_Texture").absoluteFilePath() ));
        m_p_ui->cColorPicker_PhotoFraming_Color->setColor( photoFrame.property("background-color") );
        m_p_ui->spinBox_PhotoFraming_Width->setValue( photoFrame.property("padding").remove("px").toInt() );
    path.clear();
    //Framing - classe (runtime) mainphoto - Permet de controler la petite bordure autour de la photo (padding dans la gui)
    path << ".mainPhoto";
    CCssSelection photoPadding = m_styleSheet.selection( path );
        m_p_ui->cColorPicker_PhotoFraming_PaddingColor->setColor( photoPadding.property("border-color") );
        m_p_ui->spinBox_PhotoFraming_PaddingWidth->setValue( photoPadding.property("border-width").remove("px").toInt() );
    path.clear();
    //Titre
    /*path << "div#photoTitle";
    CCssSelection photoTitle = m_styleSheet.selection( path );
        index = m_p_ui->comboBox_PhotoTitle_FontWeight->findText( photoTitle.property("font-weight") );
        m_p_ui->comboBox_PhotoTitle_FontWeight->setCurrentIndex( index );
        index = m_p_ui->comboBox_PhotoTitle_FontFamily->findText( photoTitle.property("font-family") );
        m_p_ui->comboBox_PhotoTitle_FontFamily->setCurrentIndex( index );
        index = m_p_ui->comboBox_PhotoTitle_FontVariant->findText( photoTitle.property("font-variant") );
        m_p_ui->comboBox_PhotoTitle_FontVariant->setCurrentIndex( index );
        m_p_ui->doubleSpinBox_PhotoTitle_FontSize->setValue( photoTitle.property("font-size").remove("em").toDouble() );
        m_p_ui->cColorPicker_PhotoTitle_TextColor->setColor( photoTitle.property("color") );
    path.clear();*/
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
    //Buttons
    int buttonBorderW; //Idem pour enabled / disabled
    int buttonHoveredBorderW;
    path << ".photoButtonEnabled";
    CCssSelection buttonEnabled = m_styleSheet.selection( path );
        buttonBorderW = buttonEnabled.property("border-width").remove("px").toInt();
        m_p_ui->spinBox_PhotoButtonsEnabled_BorderWidth->setValue( buttonBorderW );
        m_p_ui->cColorPicker_PhotoButtonsEnabled_BorderColor->setColor( buttonEnabled.property("border-color") );
    path.clear();
    path << ".photoButtonEnabled:hover";
    CCssSelection buttonHovered = m_styleSheet.selection( path );
        buttonHoveredBorderW = buttonHovered.property("border-width").remove("px").toInt() ;
        m_p_ui->spinBox_PhotoButtonsHovered_BorderWidth->setValue( buttonHoveredBorderW );
        m_p_ui->cColorPicker_PhotoButtonsHovered_BorderColor->setColor( buttonHovered.property("border-color") );
    path.clear();
    path << ".photoButtonDisabled";
    CCssSelection buttonDisabled = m_styleSheet.selection( path );
        m_p_ui->cColorPicker_PhotoButtonsDisabled_BorderColor->setColor( buttonDisabled.property("border-color") );
    path.clear();
    //Button Icons
    m_p_ui->CImagePicker_PreviousButton_Icon->setImage( getImagePath( m_ressources.value("PhotoButtonPrevious").absoluteFilePath() ));
    m_p_ui->CImagePicker_NextButton_Icon->setImage( getImagePath( m_ressources.value("PhotoButtonNext").absoluteFilePath() ));
//  m_p_ui->CImagePicker_BrowseButton_Icon->setImage( getImagePath( m_ressources.value("PhotoButtonIndex").absoluteFilePath() ));
    //Navigation Divs
    path << "div#photoLefter";
    CCssSelection photoLeftAndRight = m_styleSheet.selection( path );
        m_p_ui->spinBox_PhotoNavBlocks_Width->setValue( photoLeftAndRight.property("width").remove("px").toInt() );
        m_p_ui->groupBox_PhotoNavBlocksBckg->setChecked( true );
        if(  photoLeftAndRight.property( "background-color" ) == QString("transparent") ){
            m_p_ui->groupBox_PhotoNavBlocksBckg->setChecked( false );
        }
        m_p_ui->CImagePicker_PhotoNavBlocks_BckgTexture->setImage( getImagePath( m_ressources.value("PhotoNavBlocks_BckgTexture").absoluteFilePath() ));
        m_p_ui->cColorPicker_PhotoNavBlocks_BckgColor->setColor( photoLeftAndRight.property("background-color") );

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
    //--- Title ---//
    path.clear();
    path << "div#indexTitle";
    CCssSelection title =  m_styleSheet.selection( path );
        m_p_ui->groupBox_VTitle->setChecked( true );
        if( title.property("display") != QString("block") ){ m_p_ui->groupBox_VTitle->setChecked( false ); }
        m_p_ui->cImagePicker_VTitle_BackgroundTexture->setImage( getImagePath( m_ressources.value("VTitle_BackgroundTexture").absoluteFilePath() ));
        m_p_ui->cColorPicker_VTitle_BackgroundColor->setColor( title.property( "background-color" ) );
        m_p_ui->groupBox_VTitleBlock_Background->setChecked(true);
        if(  title.property( "background-color" ) == QString("transparent") ){
            m_p_ui->groupBox_VTitleBlock_Background->setChecked(false); }
        m_p_ui->cColorPicker_VTitleText_TextColor->setColor( title.property("color") );
        m_p_ui->cColorPicker_VTitleBlock_BorderColor->setColor( title.property( "border-color" ) );
        m_p_ui->spinBox_VTitleBlock_BorderWidth->setValue( std::max( title.property( "border-left-width" ).remove("px").toInt(),
                                                                     title.property( "border-right-width" ).remove("px").toInt() ) );
        index = m_p_ui->comboBox_VTitleText_FontFamily->findText( title.property("font-family") );
        m_p_ui->comboBox_VTitleText_FontFamily->setCurrentIndex( index );
        m_p_ui->doubleSpinBox_VTitle_FontSize->setValue(  title.property("font-size").remove(QString("em")).toDouble() );
        m_p_ui->doubleSpinBox_VTitle_LineHeight->setValue( title.property("line-height").remove(QString("em")).toDouble() );
        m_p_ui->spinBox_VTitleBlock_Width->setValue( title.property("width").remove("px").toInt() );    
        m_p_ui->comboBox_VTitle_Position->setCurrentIndex( m_misc.titlePosition ); //Position
        
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
   
    //Proprits diverses
    QDomElement properties = document.createElement( "properties" );
        properties.setAttribute( "title-position", m_misc.titlePosition );
    root.appendChild( properties );     
    
    //Style
    QDomElement styleSheetElement = m_styleSheet.toDomElement( document );
    root.appendChild( styleSheetElement );

    //Ressources
    QDomElement ressourcesElement = document.createElement( RESSOURCES_TAG_NAME );
    root.appendChild( ressourcesElement );
    foreach( QString widget, m_ressources.keys() ){
        QDomElement fileElement = document.createElement( RESSOURCEFILE_TAG_NAME ); // <File id="widget">FileName</File>
        fileElement.setAttribute( "id",widget );
        ressourcesElement.appendChild( fileElement );
        fileElement.appendChild( document.createTextNode( m_ressources.value(widget).fileName() ) );
    }
    return document;
}



/*******************************************************************
* QList<CCss> toCssList( )
* ------------------------
* Cr une liste de feuilles CSS  partir des paramtres
* ATTENTION: Trs dpendant de la syntaxe attendue pour les fichiers CSS des skins
********************************************************************/
CCssSheet CSkinParameters::toCss( )
{
    return m_styleSheet;
}


/*******************************************************************
* QMap<QString,QFileInfo>  ressourceFiles( )
* ------------------------
* retourne la liste des ressources ncessaires  la cration de la skin
********************************************************************/
QMap<QString,QFileInfo>  CSkinParameters::ressourceFiles( ){
    return m_ressources;
}


/*******************************************************************
* saveSkin( const QDir & destFile, QStringList &errorMsgs )
* ------------------------
* Sauvegarde la skin sur le disque. Copie les fichiers ressources dans
* l'arborescence de la skin.
* in dir : rpertoire o sauver la skin
* return : False si erreurs
********************************************************************/
bool CSkinParameters::saveSkin( const QString &destFile/*, QStringList &errorMsgs*/ )
{
    QString clearedString;    

    m_lastErrors.clear();;
    clearedString.clear();

    //--- MAJ
    fromUi( );

    //--- Vrification que l'on essaie pas d'craser la skin par dfaut
    if( QFileInfo(destFile) == QFileInfo(CSkinParameters::defaultSkin()) ){ //QFileInfo ncessaire pour vrifier qu'il s'agit bien d'un mme fichier:
        m_lastErrors << translateError( overwrittingDefault );                          //comparer le path est trop limit. Exemple: pb de case sensitive sous windows.
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
    m_ressourcesPath = QFileInfo( destFile ).fileName().remove(SKINSESSIONEXTENSION) + QString("_files/");
    const QString skinImgPath =  m_ressourcesPath + QString(SKINRESIMGDIR);
    outSkinPath.mkpath( skinImgPath );
    if( !outSkinPath.cd( skinImgPath ) ){
         m_lastErrors << translateError( destFolder );
         return false; //Il faut absolument un point de sortie : sinon on risque d'effacer des fichiers n'importe o !
    }
    //On efface le contenu du rpertoire pour viter que les fichiers ne s'accumulent    
    outSkinPath.setFilter( QDir::Files );
    foreach(QFileInfo fileInfo, outSkinPath.entryInfoList()) {
        //Attention : si on a ouvert la skin: il y a des chances qu'on essaie de rcrire au les fichiers ressources sur eux-mme ! Il faut viter a...
        if( !m_ressources.contains( m_ressources.key(fileInfo) ) ){
            if(fileInfo.isFile() && fileInfo.isWritable() ){
                outSkinPath.remove( fileInfo.fileName() );
            }
        }
    }
    //Copie des fichiers
    bool f_copySuccessful = copyRessources( outSkinPath/*, errorMsgs*/ );
    m_lastErrors.removeDuplicates();
    if( f_copySuccessful ){
        m_filePath = destFile;
        emit skinSaved( destFile );
    }

    return f_copySuccessful;
}


/*******************************************************************
* copyRessources( QDir outputDir )
* ------------------------
* Copie tous les fichiers ressources vers le rpertoire spcifi
* In: (QDir) outputDir - rpertoire o copier les fichiers ressource
* Return: true si succs, false si echec
********************************************************************/
bool CSkinParameters::copyRessources( QDir outputDir/*, QStringList &errorMsg*/ )
{
    int size = m_lastErrors.size();

    foreach( QFileInfo resourceFile, m_ressources )
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
                    m_lastErrors.append( CError::error(CError::FileCreation) + destFileInfo.absoluteFilePath() );
                }
            }
            //Copie
            if( !QFile::copy( resourceFile.absoluteFilePath(), destFileInfo.absoluteFilePath()) ){
                m_lastErrors.append( CError::error(CError::FileCreation) + destFileInfo.absoluteFilePath() );
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

    if( skinDoc.setContent( &skinFile, false, errorMsg, &ln, &col ) )
    {
        m_ressourcesPath = skinFileNameCopy.remove(SKINSESSIONEXTENSION) + QString("_files/");
        m_filePath = skinFileName;
        fromDomDocument( skinDoc );
        emit skinOpened( skinFileName );
        emit nameChanged( m_name );
        delete errorMsg;
        return true;
    }
    else{
        m_lastErrors << *errorMsg;
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
void CSkinParameters::check( QStringList* errorList, QStringList* warningList)
{
    //Maj
    fromUi();
    //Caractre invalide dans les noms des ressources
    foreach( QFileInfo ressource, m_ressources )
    {
        if( ressource.completeBaseName().contains( CPlatform::forbiddenCharacters() )){
            errorList->append( QString("\"") + ressource.fileName() + QString("\" - ") + tr("invalid character in filename.") );
        }
    }
    //Prsence des boutons ncessaires
    if( !m_ressources.contains("PhotoButtonPrevious") ){
        errorList->append( tr("Button \"Previous\" - an image is required.") );
    }
    if( !m_ressources.contains("PhotoButtonNext") ){
        errorList->append( tr("Button \"Next\" - an image is required.") );
    }
    /*if( !m_ressources.contains("PhotoButtonIndex") ){
        errorList->append( tr("Button \"Browse\" - an image is required.") );
    }*/
    //Taille des blocs OK ?
    if( m_p_ui->spinBox_PhotoNavBlocks_Width->value() <= m_p_ui->CImagePicker_NextButton_Icon->imageSize().width() ||
        m_p_ui->spinBox_PhotoNavBlocks_Width->value() <= m_p_ui->CImagePicker_PreviousButton_Icon->imageSize().width() ){
        warningList->append( tr("The zones containing the Previous/Next buttons are smaller than the buttons themselves."));
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
    QString defaultSkinPath = CPlatform::applicationDirPath() + QString("/") + QString(SKINPATH) +  QString("/") + QString(DEFAULTSKINNAME) + QString(SKINSESSIONEXTENSION); 
    return defaultSkinPath;
}

/*******************************************************************
* error(  )
* ------------------------
* Retourne la dernire erreur survenue
********************************************************************/
QString CSkinParameters::error( )
{
    return m_lastError;
}


/*******************************************************************
* errors(  )
* ------------------------
* Retourne la dernire srie d'erreurs survenue
********************************************************************/
QStringList CSkinParameters::errors( )
{
    return m_lastErrors;
}

/*******************************************************************
* version  )
* ------------------------
* Retourne la version d'EZWG ayant cr la skin
********************************************************************/
int CSkinParameters::version( )
{
    return m_version;
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
QString CSkinParameters::getImagePath( const QString &img )
{
    QDir dir = QDir( m_ressourcesPath );
    dir.cd( SKINRESIMGDIR );
    return dir.absoluteFilePath( img );
}

/*******************************************************************
* removeEmptyRessources( const QString &img)
* ------------------------
* Enlve les fichiers "vides" de la liste des ressources.
* ex: une image qui n'a pas t choisie (widget affichant "no image")
********************************************************************/
void CSkinParameters::removeEmptyRessources( )
{
	foreach( QFileInfo fileinfo, m_ressources ){
		if( !fileinfo.isFile() ){
			m_ressources.remove( m_ressources.key( fileinfo ) );
		}
	}
}

/*******************************************************************
* error( int error )
* ------------------------
* retourne la traduction de l'erreur passe en paramtre
* NB: on est oblig de pass par une fonction, car lupdate parse les
* fonctions  la recherche de tr("") afin de construire les fichiers ls.
* Donc on ne peut pas traduire de static const QSTRING ou de #define !
********************************************************************/
QString CSkinParameters::translateError( int error )
{
    QString returnedError;

    switch( error )
    {
        case noError:
            returnedError = tr("No error.");
            break;
        case overwrittingDefault:
            returnedError = tr("Cannot overwrite default skin.");
            break;
        case destFolder:
            returnedError = tr("Destination folder doesn't exists.");
            break;
        case loadingError:
            returnedError = tr("Cannot load the skin.");
        default:
            returnedError = tr("Unknown error.");
            break;
    }

    return returnedError;
}


/*******************************************************************
* buttonImage( int button )
* ------------------------
* Renvoie le nom du fichier de l'icone dsigne en paramtre
* in button : le nom de l'icone
* return : le nom du fichier image correspondant
********************************************************************/
QString CSkinParameters::buttonImage( int button )
{
    QString filename;

    switch( button )
    {
    /*case CSkinParameters::buttonIndex:
        filename = m_ressources.value("PhotoButtonIndex").fileName();
        break;*/
    case CSkinParameters::buttonNext:
        filename = m_ressources.value("PhotoButtonNext").fileName();
        break;
    case CSkinParameters::buttonPrevious:
        filename = m_ressources.value("PhotoButtonPrevious").fileName();
        break;
    default:
        filename = QString("Error");
    }

    return filename;
}



