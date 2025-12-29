local Coin = {}

function Coin:on_start()
	self.model = "resources/models/coin.csv"
	self.radius = 10
	self.mass = 1
	self.is_static = true
end

function Coin:on_collide(other)
	if other.script == "resources/scripts/entities/player.lua" then
		GameData.score = GameData.score + 1
		game:get_world():destroy_entity(self)
		audio:play_sound_at("resources/sounds/coin.ogx", physics.squash(self.location))
	end
end

function Coin:on_update(dt)
	local loc = self.location
	loc.z = loc.z + dt * 3
	self.location = loc
end

return Coin
