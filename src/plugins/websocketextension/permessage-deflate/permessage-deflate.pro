TARGET  = permessage-deflate

PLUGIN_TYPE = websocketextensions
PLUGIN_CLASS_NAME = QWebSocketPerMessageDeflateExt
load(qt_plugin)

QT += core-private network websockets

LIBS += -lz

SOURCES += main.cpp qwebsocketpmcdeflateextenstion.cpp
HEADERS += main.h qwebsocketpmcdeflateextenstion_p.h
OTHER_FILES += permessage-deflate.json
