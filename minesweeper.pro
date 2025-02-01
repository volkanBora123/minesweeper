QT += widgets \
      core
SOURCES += \
    Main.cpp \
    gamehandler.cpp \
    minebutton.cpp \
    scorelabel.cpp

HEADERS += \
    gamehandler.h \
    minebutton.h \
    scorelabel.h

DISTFILES += \
    assets/0.png \
    assets/1.png \
    assets/2.png \
    assets/3.png \
    assets/4.png \
    assets/5.png \
    assets/6.png \
    assets/7.png \
    assets/8.png \
    assets/empty.png \
    assets/flag.png \
    assets/hint.png \
    assets/mine.png \
    assets/wrong-flag.png

RESOURCES += \
    assets.qrc
