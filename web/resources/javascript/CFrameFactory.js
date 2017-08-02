/*
 *  EZWebGallery:
 *  Copyright (C) 2013 Christophe Meneboeuf <xtof@ezwebgallery.org>
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

function CFrameFactory( p_photoList )
{
    var that = this;
    this.strPhoto = "divPhoto";
    this.strFrame = "cadrePhoto";
    this.strCaption ="photoCaption";
    this.paddingCaption = "0";
    this.captions = [];

    for( i = 0; i < p_photoList.length; i++ ) {
        this.captions[i] = p_photoList[i].caption;
    }

    this.frame = function( $parent, id )  {
        $parent.append( "<div id=\""+that.strFrame+"\"><div id=\""+that.strTitle+"\"></div><div id=\""+that.strPhoto+"\"></div><div class=\""+that.strCaption+"\"></div></div>" );
        that.caption( $parent.find(that.getStrCaption()), id );
    }

    this.caption = function( $parent, id )
    {
        $parent.empty()
                .append( that.captions[ id ] );
        if(that.captions[ id ] === "") {
            $parent.css("padding-top", "0").css("padding-bottom", "0");
        }
        else {
            $parent.css("padding-top", that.paddingCaption).css("padding-bottom", that.paddingCaption);
        }
    }

    this.getStrPhoto = function( ) {
        return "#"+ that.strPhoto;
    }

    this.getStrFrame = function( )  {
        return "#"+ that.strFrame;
    }

    this.getStrCaption = function( )  {
        return "."+ that.strCaption;
    }

    this.paddingCaption = $(".photoCaption").length === 0 ? 0 : $(this.getStrCaption()).css("padding-bottom");
};
