
CONFIG  += debug_and_release \
#       += link_prl
        
QT += xml

### User defined variables ###
Debug|Release{
    SCRIPTDIR = $$PWD/..
    BUILDDIR = $$PWD/../build
}

### Output ###
Debug {
    DESTDIR =        $$BUILDDIR/debug
    OBJECTS_DIR =    $$BUILDDIR/.obj
    MOC_DIR =        $$BUILDDIR/.moc
    RCC_DIR =        $$BUILDDIR/.rcc
    UI_DIR =         $$BUILDDIR/.ui
    MINJS_DIR =      $$BUILDDIR/.min-js
}
Release {
    DESTDIR =      $$BUILDDIR/release
    OBJECTS_DIR =  $$BUILDDIR/.obj
    MOC_DIR =      $$BUILDDIR/.moc
    RCC_DIR =      $$BUILDDIR/.rcc
    UI_DIR =       $$BUILDDIR/.ui
    MINJS_DIR =    $$BUILDDIR/.min-js
}



### Prebuilding step on unix & macos ###
# The windows equivalent is done via some custom prebuild step defined  in the vcxproj file.
unix|macx {
    Debug{
#           scripts.commands = $$system( chmod ug+x $$SCRIPTDIR/*.sh && mkdir -p $$DESTDIR/skins && mkdir $$DESTDIR/res )
#           versiontarget.target = builddate.h
#            versiontarget.commands = $$system(bzr version-info --custom --template=\'$${LITERAL_HASH}define BUILD_DATE \"{build_date}\"\' > builddate.h)
#            versiontarget.depends = FORCE
#            PRE_TARGETDEPS += builddate.h
#            QMAKE_EXTRA_TARGETS += versiontarget
    }
    Release{
            scripts.commands = $$system( chmod ug+x $$SCRIPTDIR/*.sh && mkdir -p $$DESTDIR/skins && mkdir $$DESTDIR/res )
            versiontarget.target = builddate.h
            versiontarget.commands = $$system(bzr version-info --custom --template=\'$${LITERAL_HASH}define BUILD_DATE \"{build_date}\"\' > builddate.h)
            versiontarget.depends = FORCE
            PRE_TARGETDEPS += builddate.h
            QMAKE_EXTRA_TARGETS += versiontarget
    }
}
    
LIBS += -lMagick++ -lMagickCore -lMagickWand

UI_DIR = $${PWD}/ui

INCLUDEPATH += ./widgets \
               ./ui \
               ./global
unix:INCLUDEPATH += /usr/include/ImageMagick/
macx:INCLUDEPATH += /usr/local/include/ImageMagick/

DEPENDPATH += ./widgets

HEADERS += ui/mainwin.h \
    ui/IUserInterface.h \
    ui/WinSkinDesigner.h \
    ui/CTerminalUi.h \
    ui/WinConfigure.h \
    ui/CLanguageManager.h \
    global/CPlatform.h \
    global/GlobalDefinitions.h \
    global/CError.h \
    global/CMessage.h \
	global/CDebug.h \
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
    CSkinParameters.h \
    IPhotoFeeder.h \
    CPhotoFeederDirectory.h \
    CPhotoDatabase.h

SOURCES += main.cpp \
    ui/mainwin.cpp \
    ui/WinSkinDesigner.cpp \
    ui/CTerminalUi.cpp \
    ui/WinConfigure.cpp \
    ui/CLanguageManager.cpp \
    global/CError.cpp \
    global/CMessage.cpp \
    global/CPlatform.cpp \
	global/CDebug.cpp \
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
    CSkinParameters.cpp \
    CPhotoFeederDirectory.cpp \
    CPhotoDatabase.cpp

FORMS += ui/mainwin.ui \
    ui/WinSkinDesigner.ui \
    ui/WinConfigure.ui

TRANSLATIONS += ressources/languages/GalleryDesigner_en.ts \
    ressources/languages/GalleryDesigner_fr.ts

RESOURCES = ressources/GalleryDesigner.qrc
win32:RC_FILE = ressources/win32/GalleryDesigner.rc

### POST BUILD STEPS : calling the deployment script
unix|macx {
    Debug: message( "Don\'t forget to copy the necessary ressource files into \"build/debug\" in order to debug. Use compress_javascript.sh and create_release.sh scripts." )
    Release{ QMAKE_POST_LINK += $$SCRIPTDIR/compress_javascript.sh $$MINJS_DIR && \
                                $$SCRIPTDIR/create_release.sh $$DESTDIR/$$TARGET $$MINJS_DIR $$DESTDIR
    }
}


### DEPLOYMENT
unix {
        INSTALLDIR = /opt/$$TARGET
        ressources.path = $$INSTALLDIR
        ressources.files = $$DESTDIR/res $$DESTDIR/skins $$DESTDIR/$$TARGET
        INSTALLS += ressources
}


### CLEAN
QMAKE_CLEAN +=  $$MINJS_DIR/common-min.js \
                $$MINJS_DIR/image-min.js \
                $$MINJS_DIR/index-min.js \
                $$MINJS_DIR/core/classDisplayManager-min.js \
                $$MINJS_DIR/core/classFifo-min.js \
                $$MINJS_DIR/core/classHashTable-min.js \
                $$MINJS_DIR/core/classObjetAjustable-min.js \
                $$MINJS_DIR/core/classTablePhoto-min.js \
                $$MINJS_DIR/core/
