TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += websockets plugins
qtHaveModule(quick): SUBDIRS += imports
