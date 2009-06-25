#ifndef DMZ_QT_PLUGIN_WAVE_STATE_DOT_H
#define DMZ_QT_PLUGIN_WAVE_STATE_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <QtGui/QWidget>
#include <ui_waveState.h>

namespace dmz {

   class QtPluginWaveState :
         public QWidget,
         public Plugin,
         public ObjectObserverUtil {

      Q_OBJECT

      public:
         QtPluginWaveState (const PluginInfo &Info, Config &local);
         ~QtPluginWaveState ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

      protected slots:
         void on_speedSlider_valueChanged (int value);
         void on_amplitudeSlider_valueChanged (int value);
         void on_periodSlider_valueChanged (int value);

      protected:
         void _update_attribute (const Handle Attribute, const Float64 Value);
         void _init (Config &local);

         Log _log;
         Ui::waveState _ui;

         ObjectType _waveType;

         Handle _wave;
         Handle _waveSpeedAttributeHandle;
         Handle _waveAmplitudeAttributeHandle;
         Handle _waveNumberAttributeHandle;
         Boolean _inUpdate;

         Float64 _speedMin;
         Float64 _speedRange;

         Float64 _amplitudeMin;
         Float64 _amplitudeRange;

         Float64 _periodMin;
         Float64 _periodRange;
        

      private:
         QtPluginWaveState ();
         QtPluginWaveState (const QtPluginWaveState &);
         QtPluginWaveState &operator= (const QtPluginWaveState &);

   };
};

#endif // DMZ_QT_PLUGIN_WAVE_STATE_DOT_H
