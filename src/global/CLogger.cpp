

#include "CLogger.h"
#include <QDateTime> 

//Static instance for singleton
CLogger CLogger::s_instance;


CLogger::CLogger(void) : QObject()
{ }

void CLogger::log( PtrMessage msg )
{
    emit displayMessage( msg );
}
