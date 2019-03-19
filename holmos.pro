TEMPLATE = subdirs
SUBDIRS = holmos_viewer \
    holmos_imglib \
    test

test.depends = holmos_imglib
