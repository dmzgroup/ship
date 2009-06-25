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
      MessageObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _wave (0),
      _waveSpeedAttributeHandle (0),
      _waveAmplitudeAttributeHandle (0),
      _waveNumberAttributeHandle (0),
      _inUpdate (False),
      _speedMin (0.0),
      _speedRange (5.0),
      _amplitudeMin (0.0),
      _amplitudeRange (30.0),
      _periodMin (20.0),
      _periodRange (980.0) {


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

// Message Observer Interface
void
dmz::QtPluginWaveState::receive_message (
      const Message &Type,
      const Handle MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _showMsg) {

      show ();
      activateWindow ();
   }
}


// Object Observer Interface
void
dmz::QtPluginWaveState::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (Type.is_of_type (_waveType)) { _wave = ObjectHandle; }
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

   if (False == _inUpdate) {

      if (AttributeHandle == _waveSpeedAttributeHandle) {

         if (_speedRange > 0.0) {

            _ui.speedLabel->setNum (Value);
            _ui.speedSlider->setValue (int (((Value - _speedMin) / _speedRange) * 100.0));
         }
      }
      else if (AttributeHandle == _waveAmplitudeAttributeHandle) {

         if (_amplitudeRange > 0.0) {

            _ui.amplitudeLabel->setNum (Value);
            _ui.amplitudeSlider->setValue (
               int (((Value - _amplitudeMin) / _amplitudeRange) * 100.0));
         }
      }
      else if (AttributeHandle == _waveNumberAttributeHandle) {

         if ((Value > 0.0) && (_periodRange > 0.0)) {

            _ui.periodLabel->setNum (1.0 / Value);
            _ui.periodSlider->setValue (
               int ((((1.0 / Value) - _periodMin) / _periodRange) * 100.0));
         }
      }
   }
}


// QtPluginWaveState Interface
void
dmz::QtPluginWaveState::on_speedSlider_valueChanged (int value) {

   const Float64 Update =
      (Float64 (_ui.speedSlider->value ()) * 0.01 * _speedRange) + _speedMin;

   _ui.speedLabel->setNum (Update);

   _update_attribute (_waveSpeedAttributeHandle, Update);
}


void
dmz::QtPluginWaveState::on_amplitudeSlider_valueChanged (int value) {

   const Float64 Update =
      (Float64 (_ui.amplitudeSlider->value ()) * 0.01 * _amplitudeRange) + _amplitudeMin;

   _ui.amplitudeLabel->setNum (Update);

   _update_attribute (_waveAmplitudeAttributeHandle, Update);
}


void
dmz::QtPluginWaveState::on_periodSlider_valueChanged (int value) {

   const Float64 Update = (Float64 (value) * 0.01 * _periodRange) + _periodMin; 

   _ui.periodLabel->setNum (Update);

   _update_attribute (_waveNumberAttributeHandle, Update > 0.0 ? 1.0 / Update : 0.001);
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
      config_to_string ("wave-speed-attribute.name", local, "DMZ_Wave_State_Speed"),
      ObjectScalarMask);

   _waveAmplitudeAttributeHandle = activate_object_attribute (
      config_to_string (
         "wave-amplitude-attribute.name",
         local,
         "DMZ_Wave_State_Amplitude"),
      ObjectScalarMask);

   _waveNumberAttributeHandle = activate_object_attribute (
      config_to_string ("wave-number-attribute.name", local, "DMZ_Wave_State_Number"),
      ObjectScalarMask);

   _waveType = config_to_object_type ("wave-type.name", local, "wave-state", context);

   if (context) {

      Config session (get_session_config (get_plugin_name (), context));

      QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
      restoreGeometry (geometry);

      if (config_to_boolean ("window.visible", session, False)) { show (); }

      _showMsg = config_create_message (
         "message-show.name",
         local,
         "DMZ_Show_Wave_Control_Panel",
         context,
         &_log);

      subscribe_to_message (_showMsg);
   }

   if (config_to_boolean ("console.show", local, False) && !isVisible ()) {

      show ();
      activateWindow ();
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
