# Note: For now, we will just assume that rvi_lib is
# installed on the system, if not the build will fail
# and the user can deal with ensuring the rvi_lib is
# available for the QtRvi module

# TODO: Add logic to include and build 3rdparty/rvi_lib subdir
# if rvi_lib is not present on the build system

TEMPLATE = subdirs

rvinode.subdir = rvinode
rvinode.target = rvinode-lib
SUBDIRS += rvinode

plugins.subdir = imports
plugins.depends += rvinode-lib
plugins.target = plugins-lib

SUBDIRS += plugins
