#ifndef DMZ_RENDER_PLUGIN_WAVES_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_WAVES_OSG_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzRuntimeTime.h>

#include <osg/Geometry>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginWavesOSG :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         RenderPluginWavesOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginWavesOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // ObjectObserverUtil Interface
         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

         virtual void update_object_time_stamp (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

      protected:
         struct VertexStruct {
            Float32 x, y;
            VertexStruct () : x (0.0f), y (0.0f) {;}
         };

         void _create_grid ();
         void _init (Config &local);

         Log _log;
         Time _time;
         Resources _rc;

         String _imageResource;
         Float32 _minGrid;
         Float32 _maxGrid;
         Int32 _tileCountX;
         Int32 _tileCountY;
         Float32 _tileSizeX;
         Float32 _tileSizeY;
         Float32 _texFactorX;
         Float32 _texFactorY;

         Handle _waveSpeedAttributeHandle;
         Handle _waveAmplitudeAttributeHandle;
         Handle _waveNumberAttributeHandle;

         Float64 _waveTime;
         Float32 _waveSpeed;
         Float32 _waveAmplitude;
         Float32 _waveNumber; // Wave number is the inverse of the wave period.

         RenderModuleCoreOSG *_core;

         VertexStruct *_gridPoints;
         osg::ref_ptr<osg::Geometry> _surface;

      private:
         RenderPluginWavesOSG ();
         RenderPluginWavesOSG (const RenderPluginWavesOSG &);
         RenderPluginWavesOSG &operator= (const RenderPluginWavesOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_WAVES_OSG_DOT_H
