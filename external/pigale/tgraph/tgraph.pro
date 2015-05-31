TEMPLATE = lib

win32 {
      include(..\wpigale.inc)
      DESTDIR=.
      QMAKE_CXXFLAGS_WARN_ON =  -Wall -Wextra
      message(Windows $$DISTPATH)
      }else{
      include(../pigale.inc)
      QMAKE_CXXFLAGS_WARN_ON = -pedantic -ansi -Werror -Wall -Wextra
      QMAKE_CXXFLAGS_RELEASE += -O3 -fomit-frame-pointer
      message(Linux $$DISTPATH)
}


INCLUDEPATH = ../incl
DEPENDPATH = ../incl

SOURCES =\
    3-ConOrientTriang.cpp\
    3-ConShelling1.cpp\
    3-ConShelling2.cpp\
    Barycenter.cpp\
    BFS.cpp\
    Biconnect.cpp\
    Bipolar.cpp\
    BipPlanarize.cpp\
    CotreeCritical.cpp\
    Debug.cpp\
    DFS.cpp\
    DFSGraph.cpp\
    DFSMap.cpp\
    Diagonalise.cpp\
    Embed2Pages.cpp\
    EmbedPolrec.cpp\
    EmbedFPP.cpp\
    EmbedCCD.cpp\
    EmbedPolyline.cpp\
    EmbedTutteCircle.cpp\
    EmbedVision.cpp\
    FastTwit.cpp\
    File.cpp\
    Generate.cpp\
    GeoAlgs.cpp\
    Graph.cpp\
    HeapSort.cpp\
    k-InfOrient.cpp\
    LR-Algo.cpp\
    LR-Map.cpp\
    mark.cpp\
    MaxPlanar.cpp\
    more.cpp\
    netcut.cpp\
    NewPolar.cpp\
    NpBiconnect.cpp\
    Planar.cpp\
    PropName.cpp\
    PropTgf.cpp\
    PSet.cpp\
    random.cpp\
    reduce.cpp\
    SchaefferGen.cpp\
    Schnyder.cpp\
    SchnyderWood.cpp\
    SWShelling.cpp\
    STList.cpp\
    Tgf.cpp\
    TopoAlgs.cpp\
    Twit.cpp\
    VertexPacking.cpp\
    Vision.cpp


CONFIG = thread $$MODE
contains(ENABLE_STATIC,"yes") {
 CONFIG += static
}


CONFIG(debug, debug|release)  {
    TARGET = tgraph_debug
    DEFINES += TDEBUG
    unix:OBJECTS_DIR = ./.odb
    win32:OBJECTS_DIR = ./odb
    #message(tgraph: $$QMAKE_CXXFLAGS_DEBUG)
    }else {
    TARGET = tgraph
    unix:OBJECTS_DIR = ./.opt
    win32:OBJECTS_DIR = ./opt
    #message(tgraph: $$QMAKE_CXXFLAGS_RELEASE)
    }

unix {
     distdir.commands =
     QMAKE_EXTRA_TARGETS += distdir
     DESTDIR=$$DISTPATH/lib
      # awk
      awk.target = PropName.cpp
      awk.depends = propname.awk ../incl/TAXI/propdef.h
      awk.commands = $$AWK -f propname.awk ../incl/TAXI/propdef.h > PropName.cpp
      QMAKE_EXTRA_TARGETS += awk
      PRE_TARGETDEPS =  PropName.cpp
}

contains(ENABLE_STATIC,"yes") {
  message(configuring the static library $$TARGET version:$$VERSION ($$OBJECTS_DIR))
}else{
  message(configuring the library $$TARGET version:$$VERSION ($$OBJECTS_DIR))
}
