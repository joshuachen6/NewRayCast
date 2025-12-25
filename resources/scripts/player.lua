local Player = {}

function Player.on_start(self)
	print("Spawned player")
	self.mass = 100
end

function Player.on_update(self, dt)
	-- 1. Get current state (Assuming these return copies)
	local loc = self.location
	local vel = self.velocity
	local rotation_speed = (4 * math.pi / 7) * dt
	local move_speed = 3.5 * 100 * dt

	-- 2. Rotation (Use Key.A if you registered the Key table)
	if key_pressed(Key.A) then
		loc.z = physics.scale_angle(loc.z + rotation_speed)
	elseif key_pressed(Key.D) then
		loc.z = physics.scale_angle(loc.z - rotation_speed)
	end

	-- 3. Movement
	if key_pressed(Key.W) then
		if physics.mag(vel) < 200 then
			vel.x = vel.x + math.cos(loc.z) * move_speed
			vel.y = vel.y + math.sin(loc.z) * move_speed
		end
	elseif key_pressed(Key.S) then
		if physics.mag(vel) < 200 then
			vel.x = vel.x - math.cos(loc.z) * move_speed
			vel.y = vel.y - math.sin(loc.z) * move_speed
		end
	end

	-- 4. Apply changes back to C++
	self.location = loc
	self.velocity = vel

	-- Sync camera
	world.camera = loc
end

return Player
