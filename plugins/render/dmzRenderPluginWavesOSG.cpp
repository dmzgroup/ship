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
      TimeSlice (Info),
      _log (Info),
      _time (Info),
      _rc (Info),
      _minGrid (-5000.0f),
      _maxGrid (5000.0f),
      _tileCountX (2),
      _tileCountY (200),
      _tileSizeX ((_maxGrid - _minGrid) / Float32 (_tileCountX)),
      _tileSizeY ((_maxGrid - _minGrid) / Float32 (_tileCountY)),
      _core (0),
      _gridPoints (0) {

   _init (local);
}


dmz::RenderPluginWavesOSG::~RenderPluginWavesOSG () {

   if (_gridPoints) { delete []_gridPoints; _gridPoints = 0; }
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

      if (_core &&  (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _surface = 0;
         _core = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::RenderPluginWavesOSG::update_time_slice (const Float64 TimeDelta) {

   if (_surface.valid () && _gridPoints) {

      const Float64 CTime = _time.get_frame_time () * 1.0;

      osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

      unsigned int count (0);

      for (Int32 ix = 0; ix < _tileCountX; ix++) {

         for (Int32 jy = 0; jy < _tileCountY; jy++) {

            VertexStruct *v (0);

            float k = 2.5f;
            float f = 0.01f; // 0.1f;
            float h = 0.0f;
            float g = 0.0f;
            v = &(_gridPoints[(ix * (_tileCountY + 1)) + jy + 1]);
            h = sin ((v->y * f + g) + CTime) * k;
            vertices->push_back (osg::Vec3 (v->x, h, v->y));
            v = &(_gridPoints[((ix + 1) * (_tileCountY + 1)) + jy + 1]);
            h = sin ((v->y * f + g) + CTime) * k;
            vertices->push_back (osg::Vec3 (v->x, h, v->y));
            v = &(_gridPoints[((ix + 1) * (_tileCountY + 1)) + jy]);
            h = sin ((v->y * f + g) + CTime) * k;
            vertices->push_back (osg::Vec3 (v->x, h, v->y));
            v = &(_gridPoints[(ix * (_tileCountY + 1)) + jy]);
            h = sin ((v->y * f + g) + CTime) * k;
            vertices->push_back (osg::Vec3 (v->x, h, v->y));

            count += 4;
         }
      }

      _surface->setVertexArray (vertices.get ());
   }
}


void
dmz::RenderPluginWavesOSG::_create_grid () {

   const String FoundFile (_rc.find_file (_imageResource));

   osg::ref_ptr<osg::Image> img =
      (FoundFile ? osgDB::readImageFile (FoundFile.get_buffer ()) : 0);

   if (img.valid () && _core) {

      const Int32 ArraySize ((_tileCountX + 1) * (_tileCountY + 1));

      if (!_gridPoints) { _gridPoints = new VertexStruct[ArraySize]; }

      osg::Geode* geode = new osg::Geode ();

      if (_gridPoints && geode) {

         for (Int32 ix = 0; ix <= _tileCountX; ix++) {

            const Float32 TheX ((Float32 (ix) * _tileSizeX) + _minGrid);

            for (Int32 jy = 0; jy <= _tileCountY; jy++) {

               const Float32 TheY ((Float32 (jy) * _tileSizeY) + _minGrid);

               const Int32 Offset ((ix * (_tileCountY + 1)) + jy);
               _gridPoints[Offset].x = TheX;
               _gridPoints[Offset].y = TheY;
            }
         }

         _surface = new osg::Geometry;

         osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
         normals->push_back (osg::Vec3 (0.0f, 1.0f, 0.0f));
         _surface->setNormalArray (normals.get ());
         _surface->setNormalBinding (osg::Geometry::BIND_OVERALL);

         osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
         //colors->push_back (osg::Vec4 (0.0f, 0.5f, 1.0f, 0.8f));
         colors->push_back (osg::Vec4 (1.0f, 1.0f, 1.0f, 0.65f));
         _surface->setColorArray (colors.get ());
         _surface->setColorBinding (osg::Geometry::BIND_OVERALL);

         osg::StateSet *stateset = _surface->getOrCreateStateSet ();
         stateset->setMode (GL_BLEND, osg::StateAttribute::ON);
         stateset->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);

         osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
         osg::ref_ptr<osg::Vec2Array> tcoords = new osg::Vec2Array;

         osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D (img.get ());
         tex->setWrap (osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
         tex->setWrap (osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

         stateset->setTextureAttributeAndModes (0, tex.get (), osg::StateAttribute::ON);

         stateset->setAttributeAndModes (new osg::CullFace (osg::CullFace::BACK));

         unsigned int count (0);

         for (Int32 ix = 0; ix < _tileCountX; ix++) {

            for (Int32 jy = 0; jy < _tileCountY; jy++) {

               VertexStruct *v (0);

               float h = 0.0f;
               v = &(_gridPoints[(ix * (_tileCountY + 1)) + jy + 1]);
               vertices->push_back (osg::Vec3 (v->x, h, v->y));
               v = &(_gridPoints[((ix + 1) * (_tileCountY + 1)) + jy + 1]);
               vertices->push_back (osg::Vec3 (v->x, h, v->y));
               v = &(_gridPoints[((ix + 1) * (_tileCountY + 1)) + jy]);
               vertices->push_back (osg::Vec3 (v->x, h, v->y));
               v = &(_gridPoints[(ix * (_tileCountY + 1)) + jy]);
               vertices->push_back (osg::Vec3 (v->x, h, v->y));

               const float FactorX (1.0);
               const float FactorY (float (_tileCountY) / float (_tileCountX));
               tcoords->push_back (osg::Vec2 (0.0, 0.0));
               tcoords->push_back (osg::Vec2 (0.0, FactorY));
               tcoords->push_back (osg::Vec2 (FactorX, FactorY));
               tcoords->push_back (osg::Vec2 (FactorX, 0.0));

               count += 4;
            }
         }

         _surface->addPrimitiveSet (new osg::DrawArrays (GL_QUADS, 0, count));

         _surface->setVertexArray (vertices.get ());
         _surface->setTexCoordArray (0, tcoords.get ());
         _surface->setUseDisplayList (false);
         _surface->setUseVertexBufferObjects (false);
         geode->addDrawable (_surface);

         osg::Group *s = _core->get_static_objects ();

         if (s) { s->addChild (geode); }
         else { _log.error << "Failed to add geode!" << endl; }

      }
   }
}


void
dmz::RenderPluginWavesOSG::_init (Config &local) {

   _imageResource = config_to_string ("image.resource", local, "water");

/*
   _tileSize = config_to_float64 ("tile.size", local, _tileSize);
   _minGrid = config_to_float64 ("tile.min", local, _minGrid);
   _maxGrid = config_to_float64 ("tile.max", local, _maxGrid);
*/
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
