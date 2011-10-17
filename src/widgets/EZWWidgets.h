#ifndef EZWWIDGETS_H
#define EZWWIDGETS_H

#include <QtDesigner/QtDesigner>
#include <QtCore/qplugin.h>

class EZWWidgets : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    explicit EZWWidgets(QObject *parent = 0);

    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
    QList<QDesignerCustomWidgetInterface*> m_widgets;
};

#endif
