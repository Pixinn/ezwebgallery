#include "CColorPicker.h"
#include "CColorPickerPlugin.h"

#include <QtCore/QtPlugin>

CColorPickerPlugin::CColorPickerPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void CColorPickerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool CColorPickerPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *CColorPickerPlugin::createWidget(QWidget *parent)
{
    return new CColorPicker(parent);
}

QString CColorPickerPlugin::name() const
{
    return QLatin1String("CColorPicker");
}

QString CColorPickerPlugin::group() const
{
    return QLatin1String("Input Widgets");
}

QIcon CColorPickerPlugin::icon() const
{
    return QIcon(QLatin1String(":/ressources/ColorPicker_256.png"));
}

QString CColorPickerPlugin::toolTip() const
{
    return QLatin1String("Color Picker");
}

QString CColorPickerPlugin::whatsThis() const
{
    return QLatin1String("Let you choose a color by clicking or providing an hexadecimal value.");
}

bool CColorPickerPlugin::isContainer() const
{
    return false;
}

QString CColorPickerPlugin::domXml() const
{
    return QLatin1String("<widget class=\"CColorPicker\" name=\"cColorPicker\">\n</widget>\n");
}

QString CColorPickerPlugin::includeFile() const
{
    return QLatin1String("CColorPicker.h");
}
