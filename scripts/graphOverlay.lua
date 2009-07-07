local HelpSpeed = 800.0
local RadarSpeed = 2.0

local function update_time_slice (self, time)
   if self.graphActive then
      local x = dmz.overlay.scale (self.graphSlider)
      if self.graphState then
         if x < 1.0 then x = x + (RadarSpeed * time) end
         if x >= 1.0 then
            x = 1.0
            self.graphActive = false
         end
      else
         if x > 0.001 then x = x - (RadarSpeed * time) end
         if x <= 0.001 then
            x = 0.001
            self.graphActive = false
            dmz.overlay.switch_state_all (self.graphSwitch, false)
         end
      end
      dmz.overlay.scale (self.graphSlider, x)
   end
end

local function update_channel_state (self, channel, state)
   if state then  self.active = self.active + 1
   else self.active = self.active - 1 end

   if self.active == 1 then
      self.timeSlice:start (self.handle)
   elseif self.active == 0 then
      self.timeSlice:stop (self.handle)
   end
end

local GKey = dmz.input.get_key_value ("g")

local function receive_key_event (self, channel, key)
   if key.value == GKey and key.state then
      self.graphState = not self.graphState
      if self.graphState then dmz.overlay.switch_state_all (self.graphSwitch, true) end
      self.graphActive = true
   end
end

local function start (self)
   self.handle = self.timeSlice:create (update_time_slice, self, self.name)

   self.inputObs:register (
      self.config,
      {
         update_channel_state = update_channel_state,
         receive_key_event = receive_key_event,
      },
      self)

   if self.handle and self.active == 0 then self.timeSlice:stop (self.handle) end
end


local function stop (self)
   if self.handle and self.timeSlice then self.timeSlice:destroy (self.handle) end
   self.inputObs:release_all ()
end


function new (config, name)
   local self = {
      start_plugin = start,
      stop_plugin = stop,
      name = name,
      log = dmz.log.new ("lua." .. name),
      timeSlice = dmz.time_slice.new (),
      inputObs = dmz.input_observer.new (),
      active = 0,
      config = config,
      graphSwitch = dmz.overlay.lookup_handle ("graph switch"),
      graphSlider = dmz.overlay.lookup_handle ("graph slider"),
      graphState = false,
      graphActive = false,
   }
   self.graphState = dmz.overlay.switch_state (self.graphSlider, 0) and true or false
   self.log:info ("Creating plugin: " .. name)
   return self
end

