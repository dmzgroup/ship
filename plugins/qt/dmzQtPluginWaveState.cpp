#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzQtConfigRead.h>
#include <dmzQtConfigWrite.h>
#include "dmzQtPluginWaveState.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>



dmz::QtPluginWaveState::QtPluginWaveState (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _wave (0),
      _waveSpeedAttributeHandle (0),
      _waveAmplitudeAttributeHandle (0),
      _waveNumberAttributeHandle (0),
      _inUpdate (False) {


   _ui.setupUi (this);
   _init (local);
}


dmz::QtPluginWaveState::~QtPluginWaveState () {

}


// Plugin Interface
void
dmz::QtPluginWaveState::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      show ();
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      RuntimeContext *context (get_plugin_runtime_context ());

      if (context) {

         String data;

         Config session (get_plugin_name ());

         session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));

         if (isVisible ()) {

            session.add_config (boolean_to_config ("window", "visible", True));
         }

         set_session_config (context, session);
      }
   }
}


void
dmz::QtPluginWaveState::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Object Observer Interface
void
dmz::QtPluginWaveState::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (Type.is_of_type (_waveType)) {

      _wave = ObjectHandle;

      // Update widgets here

      ObjectModule *objMod = get_object_module ();

      if (objMod) {

         Float64 value (0.0);

         if (objMod->lookup_scalar (_wave, _waveSpeedAttributeHandle, value)) {

            _ui.speedSpinBox->setValue (value);
         }
         else {

            objMod->store_scalar (
               _wave,
               _waveSpeedAttributeHandle,
               _ui.speedSpinBox->value ());
         }

         if (objMod->lookup_scalar (_wave, _waveAmplitudeAttributeHandle, value)) {

            _ui.amplitudeSpinBox->setValue (value);
         }
         else {

            objMod->store_scalar (
               _wave,
               _waveAmplitudeAttributeHandle,
               _ui.amplitudeSpinBox->value ());
         }

         if (objMod->lookup_scalar (_wave, _waveNumberAttributeHandle, value)) {

            if (value > 0.0) { _ui.periodSpinBox->setValue (1.0 / value); }
         }
         else {

            value =  _ui.periodSpinBox->value ();

            if (value > 0.0) {

               objMod->store_scalar (_wave, _waveNumberAttributeHandle, 1.0 / value);
            }
         }
      }
   }
}


void
dmz::QtPluginWaveState::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _wave) { _wave = 0; }
}


void
dmz::QtPluginWaveState::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   ObjectModule *objMod = get_object_module ();

   if (False == _inUpdate) {

      if (AttributeHandle == _waveSpeedAttributeHandle) {

      }
      else if (AttributeHandle == _waveAmplitudeAttributeHandle) {

      }
      else if (AttributeHandle == _waveNumberAttributeHandle) {

      }
   }
}


// QtPluginWaveState Interface
void
dmz::QtPluginWaveState::on_speedSpinBox_valueChanged (double value) {

   _update_attribute (_waveSpeedAttributeHandle, value);
}


void
dmz::QtPluginWaveState::on_amplitudeSpinBox_valueChanged (double value) {

   _update_attribute (_waveAmplitudeAttributeHandle, value);
}


void
dmz::QtPluginWaveState::on_periodSpinBox_valueChanged (double value) {

   _update_attribute (_waveNumberAttributeHandle, value > 0.0 ? (1.0 / value) : 0.001);
}


void
dmz::QtPluginWaveState::_update_attribute (const Handle Attribute, const Float64 Value) {

   _inUpdate = True;

   ObjectModule *objMod = get_object_module ();

   if (objMod && _wave) { objMod->store_scalar (_wave, Attribute, Value); }

   _inUpdate = False;
}


void
dmz::QtPluginWaveState::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   activate_default_object_attribute (ObjectCreateMask | ObjectDestroyMask);

   _waveSpeedAttributeHandle = activate_object_attribute (
      config_to_string ("wave-speed.attribute", local, "DMZ_Wave_State_Speed"),
      ObjectScalarMask);

   _waveAmplitudeAttributeHandle = activate_object_attribute (
      config_to_string ("wave-amplitude.attribute", local, "DMZ_Wave_State_Amplitude"),
      ObjectScalarMask);

   _waveNumberAttributeHandle = activate_object_attribute (
      config_to_string ("wave-number.attribute", local, "DMZ_Wave_State_Number"),
      ObjectScalarMask);

   _waveType = config_to_object_type ("wave-type.name", local, "wave-state", context);

   if (context) {

      Config session (get_session_config (get_plugin_name (), context));

      QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
      restoreGeometry (geometry);

      if (config_to_boolean ("window.visible", session, False)) { show (); }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginWaveState (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginWaveState (Info, local);
}

};
