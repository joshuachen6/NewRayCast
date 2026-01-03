local Coin = {}

function Coin:on_start()
	self:set_model("resources/models/coin.csv")
	self.radius = 10
	self.mass = 1
	self.is_static = true
	self.has_collision = false

	self:add_tag("coin")
	self.id = "penny"
end

function Coin:on_collide(other)
	if other then
		if other:has_tag("player") then
			GameData.score = GameData.score + 1
			game:get_world():destroy_entity(self)
			audio:play_sound_at("resources/sounds/coin.ogx", physics.squash(self.location))
		end
	end
end

function Coin:on_update(dt)
	local loc = self.location
	loc.z = loc.z + dt * 3
	self.location = loc
end

return Coin
