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

#ifndef CGALLERYGENERATORFEEDER_H
#define CGALLERYGENERATORFEEDER_H

#include <QList>
#include <QMap>
#include <QQueue>
#include <QString>
#include <QSize>

#include "behaviors/IBehavior.hpp"
#include "CProjectParameters.h"
#include "CSkinParameters.h"



/*********************************************************************************/
/*                                                                               */
/* This class is used to preprocess the parameters required by CGalleryGenerator */
/*                                                                               */
/*********************************************************************************/

//WORK IN PROGRESS : MORE AND MORE PARAMETERS HAVE TO BE PROCESSED HERE !!!

class CGalleryGeneratorFeeder
{
public:
    CGalleryGeneratorFeeder(void) = default;
    CGalleryGeneratorFeeder(  const CProjectParameters & p_ProjectParams,
                              const CSkinParameters & p_SkinParams,
                              const QList<CPhotoProperties*> & p_PhotoProperties);

    bool isOutFolderUpToDate( void ) const;

    //TEMPORARY HANDLES TO PARAMETERS in order to spped up the transition to this feeder
    inline const CProjectParameters & getProjectParams( void ) const { return m_ProjectParams; }
    inline const CSkinParameters & getSkinParams( void ) const { return m_SkinParams; }
    inline QList<CPhotoProperties> getPhotoProperties( void ) { return m_PhotoProperties; }
    
    //Requested by CGalleryGenerator
    inline const QMap<QString,QSize> & getPhotoSizes( void ) const {
        return m_PhotoSizes;
    }
    inline const QMap<QString,QSize> & getThumbSizes( void ) const { 
        return m_ThumbSizes;
    }
    inline const QList<QSharedPointer<IBehavior>>& getOptionnalBehaviors() const {
        return m_BehaviorsOptionnal;
    }
    inline void setThumbGallerySize(const QSize& size) {
        m_ThumbGallerySize = size;
    }
    inline QSize getThumbGallerySize(void) const {
        return m_ThumbGallerySize;
    }

private:
   void computePhotoSizes( void );
   void computeThumbSizes( void );
   void optionnalBehaviors();

   CProjectParameters m_ProjectParams;
   CSkinParameters m_SkinParams;
   QList<CPhotoProperties> m_PhotoProperties;
   QList<QSharedPointer<IBehavior>> m_BehaviorsOptionnal;

   QMap<QString,QSize> m_PhotoSizes;
   QMap<QString,QSize> m_ThumbSizes;
   QQueue<int> m_Qualities; // <<< SHOULD BE PART OF PHOTOPROPERTIES ????!!!

   QSize m_ThumbGallerySize = QSize{ 0, 0 };

    //static const
    static const int s_nbMosaicSizes = 8;
    static const unsigned int s_thumbMosaicSizes[ s_nbMosaicSizes ];
};

#endif // CGALLERYGENERATORFEEDER_H
