#include <QDir>
#include <QString>

class CDirChecker
{
public:
    CDirChecker( const QString& path ) {
        m_dir = QDir(path);
    }
    
    bool areFilesInSubdir( const QStringList& files, const QStringList& subdirs ); //returns true if the provided files are in the provided subdirs

private:
    bool areFilesInDir( const QStringList& files );                                //returns true if the provided files are in the dir

    QDir m_dir;
};
