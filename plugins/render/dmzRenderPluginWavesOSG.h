#ifndef DMZ_RENDER_PLUGIN_WAVES_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_WAVES_OSG_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>

#include <osg/Camera>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginWavesOSG : public Plugin {

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

      protected:
         void _create_grid ();
         void _init (Config &local);

         Log _log;
         Resources _rc;

         String _imageResource;
         Float32 _tileSize;
         Float32 _minGrid;
         Float32 _maxGrid;

         RenderModuleCoreOSG *_core;

      private:
         RenderPluginWavesOSG ();
         RenderPluginWavesOSG (const RenderPluginWavesOSG &);
         RenderPluginWavesOSG &operator= (const RenderPluginWavesOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_WAVES_OSG_DOT_H
