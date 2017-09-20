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


#include "CBehaviorAnalytics.hpp"

// Note: code snipset found at https://developers.google.com/analytics/devguides/collection/gtagjs/
const QString CBehaviorAnalytics::CODE = "<script async src = 'https://www.googletagmanager.com/gtag/js?id=GA_TRACKING_ID'></script>\n\t\t<script>\n\twindow.dataLayer = window.dataLayer || [];\n\t\tfunction gtag() { dataLayer.push(arguments) };\n\t\tgtag('js', new Date());\n\t\tgtag('config', 'GA_TRACKING_ID');\n\t\t</script>";
const QString CBehaviorAnalytics::TAG_ID = "GA_TRACKING_ID";
const QString CBehaviorAnalytics::TAG_BEHAVIOR = "[BEHAVIOR_GOOGLE_ANALYTICS]";

void CBehaviorAnalytics::embedInHtml(QString& html) const
{
    if (!_config.analytics.enabled || !isIdValid(_config.analytics.id)) {
        html.replace(TAG_BEHAVIOR, "");
        return;
    }
    else {
        auto code = CODE;
        const auto& track_id = _config.analytics.id;
        code.replace(TAG_ID, track_id);
        html.replace(TAG_BEHAVIOR, code);
    }
}

