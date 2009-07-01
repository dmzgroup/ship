local function update_time_slice (self, time)
   local ctime = dmz.time.frame_time ()
   if math.random () * 900.0 < math.pow (self.amplitude, 2) then
      local offset = dmz.object.position (dmz.object.hil ())
      if not offset then offset = dmz.vector.new () end
      local handle = dmz.object.create ("clutter")
      local range = math.pow (math.random (), 2) * 1000
      local pos = dmz.matrix.new (
         dmz.math.up (), dmz.math.TwoPi * math.random ()):transform ({0, 0, -range}) +
         offset
      dmz.object.position (handle, nil, pos)
      dmz.object.set_temporary (handle)
      dmz.object.activate (handle)
      self.clutter[handle] = ctime + (3.0 - (1 - math.pow (math.random (), 2)))
   end
   for obj, timeout in pairs (self.clutter) do
      if timeout < ctime then
         dmz.object.destroy (obj)
         self.clutter[obj] = nil
      end
   end
end

local function update_wave_amplitude (self, object, attribute, value)
   self.amplitude = value
end

local function start (self)
   self.timeslice = self.timeSlice:create (update_time_slice, self, self.name)
   self.objObs:register (
      "DMZ_Wave_State_Amplitude",
      {update_object_scalar = update_wave_amplitude},
      self)
end

local function stop (self)
   if self.timeslice and self.timeSlice then self.timeSlice:destroy (self.timeslice) end
end

function new (config, name)
   local self = {
      clutter = {},
      amplitude = 0,
      start_plugin = start,
      stop_plugin = stop,
      name = name,
      log = dmz.log.new ("lua." .. name),
      objObs = dmz.object_observer.new (),
      timeSlice = dmz.time_slice.new (),
      config = config,
   }
   self.log:info ("Creating plugin: " .. name)
   return self
end
