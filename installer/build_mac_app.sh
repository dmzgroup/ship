#!/bin/sh
DEPTH=../../..
lmk -m opt -b
cp -RL $DEPTH/bin/macos-opt/lcsdemo.app $DEPTH
mkdir $DEPTH/lcsdemo.app/Contents/Frameworks/Qt
cp $DEPTH/depend/Qt/QtCore $DEPTH/lcsdemo.app/Contents/Frameworks/Qt
cp $DEPTH/depend/Qt/QtGui $DEPTH/lcsdemo.app/Contents/Frameworks/Qt
cp $DEPTH/depend/Qt/QtXml $DEPTH/lcsdemo.app/Contents/Frameworks/Qt
cp $DEPTH/depend/Qt/QtSvg $DEPTH/lcsdemo.app/Contents/Frameworks/Qt
cp $DEPTH/depend/Qt/QtOpenGL $DEPTH/lcsdemo.app/Contents/Frameworks/Qt
OSGDIR=$DEPTH/lcsdemo.app/Contents/Frameworks/osg
OSGPLUGINDIRPRE=$DEPTH/lcsdemo.app/Contents/PlugIns/
#OSG Version is not need for OSG plugins to load in App bundle.
#OSGPLUGINDIR=$DEPTH/lcsdemo.app/Contents/PlugIns/osgPlugins-2.8.1
OSGPLUGINDIR=$DEPTH/lcsdemo.app/Contents/PlugIns/
mkdir $OSGDIR
mkdir $OSGPLUGINDIRPRE
mkdir $OSGPLUGINDIR
cp $DEPTH/depend/osg/lib/libosg.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/libosgSim.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/libosgGA.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/libosgViewer.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/libosgParticle.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/libosgUtil.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/libOpenThreads.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/libosgText.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/libosgTerrain.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/libosgDB.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/libosgFX.dylib $OSGDIR
cp $DEPTH/depend/osg/lib/osgdb_qt.so $OSGPLUGINDIR
cp $DEPTH/depend/osg/lib/osgdb_ive.so $OSGPLUGINDIR
cp $DEPTH/depend/osg/lib/osgdb_osgsim.so $OSGPLUGINDIR
cp $DEPTH/depend/osg/lib/osgdb_freetype.so $OSGPLUGINDIR
hdiutil create -srcfolder $DEPTH/lcsdemo.app $DEPTH/lcsdemo-`cat $DEPTH/tmp/macos-opt/mbraapp/buildnumber.txt`.dmg
hdiutil internet-enable -yes -verbose $DEPTH/lcsdemo-`cat $DEPTH/tmp/macos-opt/mbraapp/buildnumber.txt`.dmg
rm -rf $DEPTH/lcsdemo.app/
