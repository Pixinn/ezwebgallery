/*
 *  EZWebGallery:
 *  Copyright (C) 2014 Christophe Meneboeuf <christophe@xtof.info>
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

#include "behaviors\CBehaviorAnalytics.hpp"
#include "CGalleryGeneratorFeeder.h"
#include "CDirChecker.h"


//Thubnail mosaic targeted sizes.
//Only width are currently in use
const unsigned int CGalleryGeneratorFeeder::s_thumbMosaicSizes[ s_nbMosaicSizes ] = {
    580, 670, 750, 900, 1100, 1250, 1440, 1700
};


/**************************************
* CGalleryGeneratorFeeder( ...  )
*
* The constructor will modify / add field in order to please the CGalleryGenerator
*
***************************************/
// 
CGalleryGeneratorFeeder::CGalleryGeneratorFeeder(  const CProjectParameters & p_ProjectParams,
                          const CSkinParameters & p_SkinParams,
                          const QList<CPhotoProperties*> & p_PhotoProperties) :
    m_ProjectParams( p_ProjectParams ), m_SkinParams( p_SkinParams )
{
    //Copy the properties - they will be used in another thread
     m_PhotoProperties.clear();
     foreach( CPhotoProperties* properties, p_PhotoProperties )    {
        m_PhotoProperties << *properties;
     }

    //Gallery Configuration
    m_ProjectParams.m_galleryConfig.description.replace("\n"," ");
    m_ProjectParams.m_galleryConfig.url.remove(' ');

    //Computes
    computePhotoSizes();
    computeThumbSizes();
    optionnalBehaviors();
}


/**************************************
* checkImages(  )
*
* Returns true if the photos and the thumbnails to be generated
* are present in the proper dirs
***************************************/
bool CGalleryGeneratorFeeder::isOutFolderUpToDate( void ) const
{
    QStringList photosNames;
    foreach( CPhotoProperties prop, m_PhotoProperties ) {
        photosNames << prop.encodedFilename();
    }
    QStringList photoSubDirs;
    for( int i = 1; i <= m_ProjectParams.m_photosConfig.nbIntermediateResolutions; i++ ) {
        photoSubDirs << ( RESOLUTIONPATH + QString::number(i) );
    }
    QStringList thumbSubDirs;
    for( int i = 1; i <= s_nbMosaicSizes; i++ ) {
        thumbSubDirs << ( RESOLUTIONPATH + QString::number(i) );
    }
    QDir photosDir( m_ProjectParams.m_galleryConfig.outputDir );
    CDirChecker photoCheck( photosDir.absoluteFilePath( PHOTOSPATH ) );
    QDir thumbsDir( m_ProjectParams.m_galleryConfig.outputDir );
    CDirChecker thumbsCheck( thumbsDir.absoluteFilePath( THUMBSPATH ) );

    return ( photoCheck.areFilesInSubdir( photosNames, photoSubDirs ) &&  thumbsCheck.areFilesInSubdir( photosNames, thumbSubDirs ) );
}


/**************************************
* computePhotoSizes(  )
*
* Computes the size of the photos to be generated
* Returns: Computed sizes. Key: folder to save.
***************************************/
void CGalleryGeneratorFeeder::computePhotoSizes( void )
{
    int n = 1; //folder appendix
   
    //HiDPI ??
    const int hiDpiSize = 2560; //Nexus 10
    if( m_ProjectParams.m_photosConfig.f_hiDpi == true ) {
        m_PhotoSizes.insert( QString(RESOLUTIONPATH) + QString::number(n++),
                  QSize( hiDpiSize, hiDpiSize ) );
    }

    //max size
    m_PhotoSizes.insert( QString(RESOLUTIONPATH) + QString::number(n++),
                  QSize( m_ProjectParams.m_photosConfig.maxSizeW, m_ProjectParams.m_photosConfig.maxSizeH ) );
    //intermediate sizes
    if( m_ProjectParams.m_photosConfig.nbIntermediateResolutions > 2)
    {
        int spaceW;
        int spaceH;
        int nbRes = m_ProjectParams.m_photosConfig.nbIntermediateResolutions - 2;
        spaceW = (m_ProjectParams.m_photosConfig.maxSizeW - m_ProjectParams.m_photosConfig.minSizeW)/(nbRes+1);
        spaceH = (m_ProjectParams.m_photosConfig.maxSizeH - m_ProjectParams.m_photosConfig.minSizeH)/(nbRes+1);
        for(int i = 1; i <= nbRes; i++){
            m_PhotoSizes.insert( QString(RESOLUTIONPATH) + QString::number(n++),
                          QSize( m_ProjectParams.m_photosConfig.maxSizeW - i*spaceW, m_ProjectParams.m_photosConfig.maxSizeH - i*spaceH ) );
        }
    }
    //min size
    m_PhotoSizes.insert( QString(RESOLUTIONPATH) + QString::number(n++),
                  QSize( m_ProjectParams.m_photosConfig.minSizeW, m_ProjectParams.m_photosConfig.minSizeH ) );
}


/**************************************
* computeThumbSizes(  )
*
* Computes the size of the thumbnails to be generated
* Returns: Computed sizes. Key: folder to save.
***************************************/
void CGalleryGeneratorFeeder::computeThumbSizes( void )
{
  QList<int> sizeList; //in this "size" list width = height
  unsigned int nbCols =  m_ProjectParams.m_thumbsConfig.nbColumns;
  QSize unavailableSpace = m_SkinParams.mosaicDecoration( nbCols );
  //Iterating on the mosaic size constraints
  for( int i = 0; i < s_nbMosaicSizes; i++ ) {
      sizeList << ( s_thumbMosaicSizes[i] - unavailableSpace.width() ) / nbCols;
  }
  //Downward sorting sizes
  qSort( sizeList.begin(), sizeList.end(), qGreater<int>() );
  //returning sizes
  int n = 1;  //folder appendix
  foreach( int size, sizeList ) {
      m_ThumbSizes.insert( QString(RESOLUTIONPATH) + QString::number(n++), QSize( size, size ) );
  }

}



/**************************************
* optionnalBehaviors(  )
*
* Adds the optional behaviors of the gallery
***************************************/
void CGalleryGeneratorFeeder::optionnalBehaviors()
{
    // Google Analytics
    // Added anyway or the tag won't be cleared in HTML
    m_BehaviorsOptionnal.append(QSharedPointer<IBehavior>{new CBehaviorAnalytics{ m_ProjectParams.m_galleryConfig }});
}
