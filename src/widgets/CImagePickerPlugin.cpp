#include "CImagePicker.h"
#include "CImagePickerPlugin.h"

#include <QtCore/QtPlugin>

CImagePickerPlugin::CImagePickerPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void CImagePickerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool CImagePickerPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *CImagePickerPlugin::createWidget(QWidget *parent)
{
    return new CImagePicker(parent);
}

QString CImagePickerPlugin::name() const
{
    return QLatin1String("CImagePicker");
}

QString CImagePickerPlugin::group() const
{
    return QLatin1String("Input Widgets");
}

QIcon CImagePickerPlugin::icon() const
{
    return QIcon(QLatin1String(":/resources/ImagePicker_256.png"));
}

QString CImagePickerPlugin::toolTip() const
{
    return QLatin1String("Texture Picker");
}

QString CImagePickerPlugin::whatsThis() const
{
    return QLatin1String("Choose an image to use as a texure.");
}

bool CImagePickerPlugin::isContainer() const
{
    return false;
}

QString CImagePickerPlugin::domXml() const
{
    return QLatin1String("<widget class=\"CImagePicker\" name=\"CImagePicker\">\n</widget>\n");
}

QString CImagePickerPlugin::includeFile() const
{
    return QLatin1String("CImagePicker.h");
}

