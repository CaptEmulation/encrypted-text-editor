QT += widgets network

macx {
  QMAKE_INFO_PLIST = Info-app.plist
  ICON = icon.icns
}

TARGET = "Encrypted Text Editor"

HEADERS       = mainwindow.h \
                mdichild.h \
    cryptoaes.h \
    passworddialog.h \
    finddialog.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                mdichild.cpp \
    cryptoaes.cpp \
    passworddialog.cpp \
    finddialog.cpp
RESOURCES     = mdi.qrc

# install
# Crypto++ library location (see Crypto++ documentation for building) :-(
CRYPTOPP_LIB=$$PWD/../cryptopp562

win32:CONFIG(release, debug|release): LIBS += -L$$CRYPTOPP_LIB/release/ -lcryptopp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$CRYPTOPP_LIB/debug/ -lcryptopp
else:unix: LIBS += -L$$CRYPTOPP_LIB/ -lcryptopp

INCLUDEPATH += $$CRYPTOPP_LIB
DEPENDPATH += $$CRYPTOPP_LIB

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$CRYPTOPP_LIB/release/libcryptopp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$CRYPTOPP_LIB/debug/libcryptopp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS +=$$CRYPTOPP_LIB/release/cryptopp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$CRYPTOPP_LIB/debug/cryptopp.lib
else:unix: PRE_TARGETDEPS += $$CRYPTOPP_LIB/libcryptopp.a

OTHER_FILES += \
    icon.icns

FORMS += \
    passworddialog.ui \
    finddialog.ui
