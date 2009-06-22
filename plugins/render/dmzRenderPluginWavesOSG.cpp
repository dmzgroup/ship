#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginWavesOSG.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesConsts.h>

#include <osg/CullFace>
#include <osg/Material>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/PolygonOffset>
#include <osg/Matrix>
#include <osg/RenderInfo>
#include <osg/Texture2D>

#include <osgDB/ReadFile>


dmz::RenderPluginWavesOSG::RenderPluginWavesOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      _log (Info),
      _rc (Info),
      _tileSize (50.0),
      _minGrid (-5000.0),
      _maxGrid (5000.0),
      _core (0) {

   _init (local);
}


dmz::RenderPluginWavesOSG::~RenderPluginWavesOSG () {

}


// Plugin Interface
void
dmz::RenderPluginWavesOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginWavesOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);

         if (_core) { _create_grid (); }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core &&  (_core == RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
   }
}


void
dmz::RenderPluginWavesOSG::_create_grid () {

   const String FoundFile (_rc.find_file (_imageResource));

   osg::ref_ptr<osg::Image> img =
      (FoundFile ? osgDB::readImageFile (FoundFile.get_buffer ()) : 0);

   if (img.valid () && _core) {

      const Int32 GridSize (Int32 ((_maxGrid - _minGrid) / _tileSize));

      const Int32 ArraySize ((GridSize + 1) * (GridSize + 1));

      struct vertex {

         Float32 x, y;
         vertex () : x (0.0f), y (0.0f) {;}
      };

      vertex *gridPoints = new vertex[ArraySize];

      osg::Geode* geode = new osg::Geode ();

      if (gridPoints && geode) {

         for (Int32 ix = 0; ix <= GridSize; ix++) {

            const Float32 TheX ((Float32 (ix) * _tileSize) + _minGrid);

            for (Int32 jy = 0; jy <= GridSize; jy++) {

               const Float32 TheY ((Float32 (jy) * _tileSize) + _minGrid);

               const Int32 Offset ((ix * (GridSize + 1)) + jy);
               gridPoints[Offset].x = TheX;
               gridPoints[Offset].y = TheY;
            }
         }

         osg::Geometry* geom = new osg::Geometry;

         osg::Vec3Array* normals = new osg::Vec3Array;
         normals->push_back (osg::Vec3 (0.0f, 1.0f, 0.0f));
         geom->setNormalArray (normals);
         geom->setNormalBinding (osg::Geometry::BIND_OVERALL);

         osg::Vec4Array* colors = new osg::Vec4Array;
         colors->push_back (osg::Vec4 (0.0f, 0.9f, 0.9f, 1.0f));
         geom->setColorArray (colors);
         geom->setColorBinding (osg::Geometry::BIND_OVERALL);

         osg::StateSet *stateset = geom->getOrCreateStateSet ();
         stateset->setMode (GL_BLEND, osg::StateAttribute::ON);
         stateset->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);

         osg::Vec3Array *vertices = new osg::Vec3Array;
         osg::Vec2Array *tcoords = new osg::Vec2Array;

         osg::Texture2D *tex = new osg::Texture2D (img.get ());
         tex->setWrap (osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
         tex->setWrap (osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

         stateset->setTextureAttributeAndModes (0, tex, osg::StateAttribute::ON);

         stateset->setAttributeAndModes (new osg::CullFace (osg::CullFace::BACK));

         unsigned int count (0);

         for (Int32 ix = 0; ix < GridSize; ix++) {

            for (Int32 jy = 0; jy < GridSize; jy++) {

               vertex *v (0);

               float k = 20.0f;
               float f = 0.01f;
               float h = 0.0f;
               float g = HalfPi32;
               v = &(gridPoints[(ix * (GridSize + 1)) + jy + 1]);
               h = sin (v->y * f + g) * k;
               vertices->push_back (osg::Vec3 (v->x, h, v->y));
               v = &(gridPoints[((ix + 1) * (GridSize + 1)) + jy + 1]);
               h = sin (v->y * f + g) * k;
               vertices->push_back (osg::Vec3 (v->x, h, v->y));
               v = &(gridPoints[((ix + 1) * (GridSize + 1)) + jy]);
               h = sin (v->y * f + g) * k;
               vertices->push_back (osg::Vec3 (v->x, h, v->y));
               v = &(gridPoints[(ix * (GridSize + 1)) + jy]);
               h = sin (v->y * f + g) * k;
               vertices->push_back (osg::Vec3 (v->x, h, v->y));

               const float Factor (1.0);
               tcoords->push_back (osg::Vec2 (0.0, 0.0));
               tcoords->push_back (osg::Vec2 (0.0, Factor));
               tcoords->push_back (osg::Vec2 (Factor, Factor));
               tcoords->push_back (osg::Vec2 (Factor, 0.0));

               count += 4;
            }
         }

         geom->addPrimitiveSet (new osg::DrawArrays (GL_QUADS, 0, count));

         geom->setVertexArray (vertices);
         geom->setTexCoordArray (0, tcoords);
         geode->addDrawable (geom);

         osg::Group *s = _core->get_static_objects ();

         if (s) { s->addChild (geode); }
         else { _log.error << "Failed to add geode!" << endl; }

         delete []gridPoints;
      }
      else if (gridPoints) { delete []gridPoints; }
   }
}


void
dmz::RenderPluginWavesOSG::_init (Config &local) {

   _imageResource = config_to_string ("image.resource", local, "water");

   _tileSize = config_to_float64 ("tile.size", local, _tileSize);
   _minGrid = config_to_float64 ("tile.min", local, _minGrid);
   _maxGrid = config_to_float64 ("tile.max", local, _maxGrid);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginWavesOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginWavesOSG (Info, local);
}

};
