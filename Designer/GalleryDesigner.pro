# RECUPERATION REVISION - http://wiki.qtcentre.org/index.php?title=Version_numbering_using_QMake
#VERSION = $$system(svnversion)
#VERSTR = '\\"$${VERSION}\\"' # place quotes around the version string
#DEFINES += REVISION=\"$${VERSTR}\" # create a VER macro containing the version string


CONFIG += link_prl
QT += xml
OTHER_FILES += Makefile.Release \
    Makefile.Debug \
    Makefile

### Prebuilding step on unix ###
# L'equivalent windows est configure via les tape de precompilation du projet Visual
unix {
        versiontarget.target = bazaarrev.h
        versiontarget.commands = $$system(bzr version-info --custom --template=\"static const unsigned int BAZAAR_REVISION = 133 + {revno};\" > bazaarrev.h)
        versiontarget.depends = FORCE

        PRE_TARGETDEPS += bazaarrev.h
        QMAKE_EXTRA_TARGETS += versiontarget
}


    
LIBS += -lMagick++ -lMagickCore -lMagickWand

UI_DIR = $${PWD}/ui

INCLUDEPATH += ./widgets \
               ./ui \
               ./global
unix:INCLUDEPATH += /usr/include/ImageMagick/

DEPENDPATH += ./widgets

HEADERS += ui/mainwin.h \
    ui/IUserInterface.h \
    ui/WinSkinDesigner.h \
    ui/CTerminalUi.h \
    ui/WinConfigure.h \
    ui/CLanguageManager.h \
    global/CPlatform.h \
    global/GlobalDefinitions.h \
    global/CErrorMessages.h \
    widgets/CImagePicker.h \
    widgets/CColorPicker.h \
    CGalleryGenerator.h \
    CPhotoProperties.h \
    CPhoto.h \
    CPhotoProcessor.h \
    CCss.h \
    CTaggedString.h \
    CCaption.h \
    CCaptionManager.h \
    CProjectParameters.h \
    IParameters.h \
    CSkinParameters.h

SOURCES += main.cpp \
    ui/mainwin.cpp \
    ui/WinSkinDesigner.cpp \
    ui/CTerminalUi.cpp \
    ui/WinConfigure.cpp \
    ui/CLanguageManager.cpp \
    global/CErrorMessages.cpp \
    global/CPlatform.cpp \
    widgets/CImagePicker.cpp \
    widgets/CColorPicker.cpp \
    CGalleryGenerator.cpp \
    CPhotoProperties.cpp \
    CPhoto.cpp \
    CPhotoProcessor.cpp \
    CCss.cpp \
    CTaggedString.cpp \
    CCaption.cpp \
    CCaptionManager.cpp \
    CProjectParameters.cpp \    
    CSkinParameters.cpp

FORMS += ui/mainwin.ui \
    ui/WinSkinDesigner.ui \
    ui/WinConfigure.ui

TRANSLATIONS += ressources/languages/GalleryDesigner_en.ts \
    ressources/languages/GalleryDesigner_fr.ts

RESOURCES = ressources/GalleryDesigner.qrc
win32:RC_FILE = ressources/win32/GalleryDesigner.rc
