local Coin = {}

function Coin.on_start(self)
	self.model = "resources/models/coin.csv"
	self.radius = 10
	self.is_static = true
end

function Coin.on_collide(self, other)
	if Game then
		Game.score = Game.score + 1
		print("Collected Coin! Score: " .. Game.score .. "/" .. (Game.total_coins or "?"))

		if Game.total_coins and Game.score >= Game.total_coins then
			print("CONGRATULATIONS! YOU FOUND ALL COINS!")
		end
	end

	world:destroy_entity(self)
end

function Coin.on_update(self, dt)
	local loc = self.location
	loc.z = loc.z + dt * 3 -- Spin
	self.location = loc
end

return Coin
