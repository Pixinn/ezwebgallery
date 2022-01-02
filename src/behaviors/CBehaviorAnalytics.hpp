/*
*  EZWebGallery:
*  Copyright (C) 2017 Christophe Meneboeuf <christophe@xtof.info>
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

#ifndef BEHAVIORS_CBEHAVIORANALYTICS_H__
#define BEHAVIORS_CBEHAVIORANALYTICS_H__

#include <QString>

#include "CProjectParameters.h"
#include "IBehavior.hpp"

/// @brief Interface to embed some optionnal behavior code into the gallery
class CBehaviorAnalytics : public IBehavior
{
public:
    /// @Constructor
    /// @param ui Main UI
    CBehaviorAnalytics(const t_galleryConf& config) :
        _config{ config }
    {   }
    virtual ~CBehaviorAnalytics() = default;
    /// @brief embeds the custom code
    void embedInHtml(QString& html) const override final;
private:
    /// @brief Returns true if the ID is valid
    bool isIdValid(const QString& id) const {
        return (id.indexOf("UA-") == 0u);   // TODO Enhance the test
    }

    static const QString CODE; ///< String containing the (to be customized) code to embed
    static const QString TAG_ID;  ///< TAG to be replaced with the tracking ui
    static const QString TAG_BEHAVIOR; ///< TAG to be replaced in the HTML CODE by the behavior's
    const t_galleryConf  _config;   ///< Project parameters, containing the tracking ID
};


#endif
