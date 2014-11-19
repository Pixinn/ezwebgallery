#include "CColorPickerPlugin.h"
#include "CImagePickerPlugin.h"
#include "EZWWidgets.h"

EZWWidgets::EZWWidgets(QObject *parent)
        : QObject(parent)
{
    m_widgets.append(new CColorPickerPlugin(this));
    m_widgets.append(new CImagePickerPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> EZWWidgets::customWidgets() const
{
    return m_widgets;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(EZWWidgetsPlugin, EZWWidgets)
#endif // QT_VERSION < 0x050000
