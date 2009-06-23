
local function isect (point)
   local result = nil
   local hitList = dmz.isect.do_isect (
      { type = dmz.isect.RayTest, start = point, vector = {0,-1,0}},
      { type = dmz.isect.ClosestPoint, })
   if hitList then
      for index, hit in ipairs (hitList) do
         if not hit.object then
            result = hit.point
            break
         end
      end
   end
   return result
end

local function update_time_slice (self, time)
   if self.ship then
      local pos = dmz.object.position (self.ship)
      --pos:set_z (pos:get_z () - 24 * time)
      local ori = dmz.object.orientation (self.ship)
      if not pos then pos = dmz.vector.new () end
      if not ori then ori = dmz.matrix.new () end
      local heading = dmz.math.heading (ori)
      local rot = dmz.matrix.new (dmz.math.up (), heading)
      local offset = dmz.vector.new ({pos:get_x (), 200, pos:get_z ()})
      local p1 = rot:transform (dmz.vector.new ({0, 0, -28.867513459481278})) + offset
      local p2 = rot:transform (dmz.vector.new ({25, 0, 14.433756729740647})) + offset
      local p3 = rot:transform (dmz.vector.new ({-25, 0, 14.433756729740647})) + offset
      dmz.isect.disable_isect (self.ship)
         p1 = isect (p1)
         p2 = isect (p2)
         p3 = isect (p3)
      dmz.isect.enable_isect (self.ship)
      if p1 and p2 and p3 then
         local v1 = p2 - p1
         local v2 = p3 - p1
         local up = v2:cross (v1)
         ori = dmz.matrix.new (dmz.math.up (), up) * rot
         dmz.object.orientation (self.ship, nil, ori)
         pos:set_y ((((p1 + p2 + p3) * (1 / 3)):get_y (a)) - 4.5)
         dmz.object.position (self.ship, nil, pos)
      end
   end
end

local function create_object (self, Object, Type)

end

local function destroy_object (self, Object)
   if Object == self.ship then self.ship = nil end
end

local function update_object_state (self, Object, Attribute, State, PreviousState)

end

local function store_ship_flag (self, Object, Attribute, Flag, PreviousFlag)
   if Flag then self.ship = Object end
cprint ("Ship: ", Object)
end

local function start (self)

   self.handle = self.timeSlice:create (update_time_slice, self, self.name)

   local callbacks = {
      create_object = create_object,
      destroy_object = destroy_object,
      update_object_state = update_object_state,
   }

   self.objObs:register (nil, callbacks, self)

   callbacks = {
      update_object_flag = store_ship_flag,
    }

   self.objObs:register ("Object_Ship_Local", callbacks, self)
end

local function stop (self)
   if self.handle and self.timeSlice then self.timeSlice:destroy (self.handle) end
end


function new (config, name)
   local self = {
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

