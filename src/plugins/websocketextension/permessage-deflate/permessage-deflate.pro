TARGET  = permessage-deflate

PLUGIN_TYPE = websocketextensions
PLUGIN_CLASS_NAME = QWebSocketPerMessageDeflateExtension
load(qt_plugin)

QT += core-private network websockets

LIBS += -lz

HEADERS += main.h \
           qwebsocketpermessagedeflateextension.h
SOURCES += main.cpp \
           qwebsocketpermessagedeflateextension.cpp

OTHER_FILES += permessage-deflate.json
