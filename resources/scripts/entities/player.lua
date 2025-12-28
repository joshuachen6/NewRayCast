local Player = {}
local cooldown = 0

function Player:on_start()
	self.mass = 100
	self.radius = 5
	self.health = 100

	GameData.player = self
end

function Player:on_update(dt)
	local loc = self.location
	local vel = self.velocity
	local rotation_speed = (4 * math.pi / 7) * dt
	local move_speed = 3.5 * 100 * dt

	if self.health <= 0 then
		GameData.game_over = true
		game:get_world():destroy_entity(self)
		GameData.player = nil
		return
	end

	if controls:key_pressed(Key.A) then
		loc.z = physics.scale_angle(loc.z + rotation_speed)
	elseif controls:key_pressed(Key.D) then
		loc.z = physics.scale_angle(loc.z - rotation_speed)
	end

	if controls:key_pressed(Key.W) then
		if physics.mag(vel) < 200 then
			vel.x = vel.x + math.cos(loc.z) * move_speed
			vel.y = vel.y + math.sin(loc.z) * move_speed
		end
	elseif controls:key_pressed(Key.S) then
		if physics.mag(vel) < 200 then
			vel.x = vel.x - math.cos(loc.z) * move_speed
			vel.y = vel.y - math.sin(loc.z) * move_speed
		end
	end

	self.location = loc
	self.velocity = vel

	game:get_world().camera = loc

	if controls:key_pressed(Key.Space) and cooldown <= 0 then
		cooldown = 0.5
		local offset = self.radius + 10
		local spawn_loc = Vector3(loc.x + math.cos(loc.z) * offset, loc.y + math.sin(loc.z) * offset, loc.z)
		local bullet = game:get_world():spawn_entity("resources/scripts/entities/bullet.lua", spawn_loc)
		local velocity = Vector2(math.cos(loc.z) * 1000, math.sin(loc.z) * 1000)
		bullet.velocity = velocity

		audio:play_sound("resources/sounds/pea_shoot.ogx")
	end

	cooldown = cooldown - dt
end

function Player:on_damage(damage)
	self.health = self.health - damage
end

return Player
