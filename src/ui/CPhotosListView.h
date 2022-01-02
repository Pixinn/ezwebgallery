/* 
 *  EZWebGallery:
 *  Copyright (C) 2012 Christophe Meneboeuf <christophe@xtof.info>
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



#ifndef _CPHOTOLISTVIEW_H_

#include <QListView>


// -----------
// This class is to override QListView::srollTo().
// Indeed, it is used to replace the scroolbar to the top of the view
// any time the associated model is modified.
// -----------
class CPhotosListView : public QListView
{
public:
    CPhotosListView( QWidget* parent = 0 ) :
      QListView( parent )
    {   }
    ~CPhotosListView (void )
    {   }

    void scrollTo(const QModelIndex &, ScrollHint)
    {

    }
    
};

#endif


