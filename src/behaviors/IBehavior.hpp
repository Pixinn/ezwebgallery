/*
*  EZWebGallery:
*  Copyright (C) 2017 Christophe Meneboeuf <dev@ezwebgallery.org>
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

#ifndef BEHAVIORS_IBEHAVIOR_H__
#define BEHAVIORS_IBEHAVIOR_H__

/// @brief Interface to embedd some optionnal behavior code into the gallery
class IBehavior
{
public:
    IBehavior() = default;
    virtual ~IBehavior() = default;
    /// @brief embeds the custom code
    virtual void embedInHtml(QString& html) const = 0;
};


#endif
