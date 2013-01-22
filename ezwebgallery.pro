
CONFIG  += debug_and_release \
#       += link_prl

QT += xml

### User defined variables ###
Debug|Release{
    SCRIPTDIR = $$PWD/scripts
    BUILDDIR = $$PWD/build
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
#            versiontarget.commands = $$system(bzr version-info --custom --template=\'$${LITERAL_HASH}define BUILD_DATE \"{build_date}\"\' > src/builddate.h)
#            versiontarget.depends = FORCE
#            PRE_TARGETDEPS += builddate.h
#            QMAKE_EXTRA_TARGETS += versiontarget
    }
    Release{
            scripts.commands = $$system( chmod ug+x $$SCRIPTDIR/*.sh && mkdir -p $$DESTDIR/skins && mkdir $$DESTDIR/res )
            versiontarget.target = builddate.h
            versiontarget.commands = $$system(bzr version-info --custom --template=\'$${LITERAL_HASH}define BUILD_DATE \"{build_date}\"\' > src/builddate.h)
            versiontarget.depends = FORCE
            PRE_TARGETDEPS += builddate.h
            QMAKE_EXTRA_TARGETS += versiontarget
    }
}

LIBS += -lMagick++ -lMagickCore -lMagickWand

UI_DIR = $${PWD}/src/ui

INCLUDEPATH +=  ./src \
                ./src/generation \
                ./src/widgets \
                ./src/ui \
                ./src/global
unix:INCLUDEPATH += /usr/include/ImageMagick/
macx:INCLUDEPATH += /usr/local/include/ImageMagick/

DEPENDPATH += ./src/widgets

HEADERS += ./src/generation/CGalleryGenerator.h \
    ./src/generation/CDirChecker.h \
    ./src/ui/mainwin.h \
    ./src/ui/IUserInterface.h \
    ./src/ui/WinSkinDesigner.h \
    ./src/ui/CTerminalUi.h \
    ./src/ui/WinConfigure.h \
    ./src/ui/CLanguageManager.h \
    ./src/global/CPlatform.h \
    ./src/global/GlobalDefinitions.h \
    ./src/global/CError.h \
    ./src/global/CWarning.h \
    ./src/global/CMessage.h \
    ./src/global/IMessage.h \
    ./src/global/CLogger.h \
    ./src/widgets/CImagePicker.h \
    ./src/widgets/CColorPicker.h \
    ./src/CPhotoProperties.h \
    ./src/CPhoto.h \
    ./src/CPhotoProcessor.h \
    ./src/CCss.h \
    ./src/CTaggedString.h \
    ./src/CCaption.h \
    ./src/CCaptionManager.h \
    ./src/CProjectParameters.h \
    ./src/IParameters.h \
    ./src/CSkinParameters.h \
    ./src/IPhotoFeeder.h \
    ./src/CPhotoFeederDirectory.h \
    ./src/CPhotoDatabase.h \
    ./src/CThumbnailLoader.h \
    ./src/Json.h

SOURCES += ./src/main.cpp \
    ./src/generation/CGalleryGenerator.cpp \
    ./src/generation/CDirChecker.cpp \
    ./src/ui/mainwin.cpp \
    ./src/ui/WinSkinDesigner.cpp \
    ./src/ui/CTerminalUi.cpp \
    ./src/ui/WinConfigure.cpp \
    ./src/ui/CLanguageManager.cpp \
    ./src/global/CError.cpp \
    ./src/global/CWarning.cpp \
    ./src/global/CMessage.cpp \
    ./src/global/CPlatform.cpp \
    ./src/global/CLogger.cpp \
    ./src/widgets/CImagePicker.cpp \
    ./src/widgets/CColorPicker.cpp \
    ./src/CPhotoProperties.cpp \
    ./src/CPhoto.cpp \
    ./src/CPhotoProcessor.cpp \
    ./src/CCss.cpp \
    ./src/CTaggedString.cpp \
    ./src/CCaption.cpp \
    ./src/CCaptionManager.cpp \
    ./src/CProjectParameters.cpp \
    ./src/CSkinParameters.cpp \
    ./src/CPhotoFeederDirectory.cpp \
    ./src/CPhotoDatabase.cpp \
    ./src/CThumbnailLoader.cpp \
    ./src/Json.cpp

FORMS += ./src/ui/mainwin.ui \
    ./src/ui/WinSkinDesigner.ui \
    ./src/ui/WinConfigure.ui

TRANSLATIONS += ./src/ressources/languages/GalleryDesigner_en.ts \
    ./src/ressources/languages/GalleryDesigner_fr.ts

RESOURCES = ./src/ressources/GalleryDesigner.qrc
win32:RC_FILE = ./src/ressources/win32/GalleryDesigner.rc

### POST BUILD STEPS : calling the deployment script
unix|macx {
    Release{ QMAKE_POST_LINK += $$SCRIPTDIR/compress_javascript.sh $$MINJS_DIR && \
                                $$SCRIPTDIR/create_release.sh $$DESTDIR/$$TARGET $$MINJS_DIR $$DESTDIR
    }
}


### DEPLOYMENT
unix {
        binfile.files += $$DESTDIR/$$TARGET
        binfile.path = /usr/bin/
        data.path = /usr/share/$$TARGET
        data.files = $$DESTDIR/data
        skins.path = /usr/share/$$TARGET
        skins.files = $$DESTDIR/skins
        icon.path = /usr/share/$$TARGET
        icon.files = $$PWD/src/ressources/images/EZWG-Icone.svg
        desktop.path = /usr/share/applications/
        desktop.files = $$PWD/src/ressources/linux/ezwebgallery.desktop
        INSTALLS += binfile
        INSTALLS += skins
        INSTALLS += data
        INSTALLS += icon
        INSTALLS += desktop
}



### CLEAN
QMAKE_CLEAN += $$MINJS_DIR/common-min.js \
               $$MINJS_DIR/image-min.js \
               $$MINJS_DIR/index-min.js \
               $$MINJS_DIR/core/classDisplayManager-min.js \
               $$MINJS_DIR/core/classFifo-min.js \
               $$MINJS_DIR/core/classHashTable-min.js \
               $$MINJS_DIR/core/classObjetAjustable-min.js \
               $$MINJS_DIR/core/classTablePhoto-min.js \
               $$MINJS_DIR/core/ \
               $$PWD/src/builddate.h \
               -r $$BUILDDIR

QMAKE_DISTCLEAN += -r $$BUILDDIR

