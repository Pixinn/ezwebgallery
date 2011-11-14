#include "CColorPickerPlugin.h"
#include "CImagePickerPlugin.h"
#include "EZWWidgets.h"

EZWWidgets::EZWWidgets(QObject *parent)
        : QObject(parent)
{
    m_widgets.append(new CColorPickerPlugin(this));
    m_widgets.append(new CImagePickerPlugin(this));
 //   m_widgets.append(new CTexturePickerPlugin(this));

}

QList<QDesignerCustomWidgetInterface*> EZWWidgets::customWidgets() const
{
    return m_widgets;
}

Q_EXPORT_PLUGIN2(ezwwidgetsplugin, EZWWidgets)
