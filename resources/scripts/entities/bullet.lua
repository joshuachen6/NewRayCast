local Bullet = {}
local lifetime = 0

function Bullet:on_start()
	self.mass = 1
	self.radius = 5
	self.model = "resources/models/bullet.csv"
end

function Bullet:on_collide(other)
	if other.script ~= "resources/scripts/entities/player.lua" then
		game:get_world():destroy_entity(self)
		other:damage(100)
		audio:play_sound("resources/sounds/pea_hit.ogx")
	end
end

function Bullet:on_update(dt)
	lifetime = lifetime + dt
	if lifetime > 5 then
		game:get_world():destroy_entity(self)
	end
end

return Bullet
