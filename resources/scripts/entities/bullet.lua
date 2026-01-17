local Bullet = {}
local lifetime = 0

function Bullet:on_start()
	self.mass = 1
	self.radius = 5
	self:set_model("resources/models/bullet.csv")
end

function Bullet:on_collide(other)
	if not other then
		game:get_world():destroy_entity(self)
		return
	end
	if not other:has_tag("player") and other.has_collision then
		other:damage(100)
		audio:play_sound_at("resources/sounds/pea_hit.ogx", physics.squash(self.location))
		game:get_world():destroy_entity(self)
	end
end

function Bullet:on_update(dt)
	lifetime = lifetime + dt
	if lifetime > 5 then
		game:get_world():destroy_entity(self)
	end
end

return Bullet
