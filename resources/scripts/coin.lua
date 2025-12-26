local Coin = {}

function Coin.on_start(self)
	self.model = "resources/models/coin.csv"
	self.radius = 10
	self.is_static = true
end

function Coin.on_collide(self, other)
	if Game then
		-- specific check to avoid rapid firing if multiple collisions happen before cleanup
		-- (Though existing C++ logic seems to handle cleanup at end of frame)
		
		Game.score = Game.score + 1
		print("Collected Coin! Score: " .. Game.score .. "/" .. (Game.total_coins or "?"))
		
		if Game.total_coins and Game.score >= Game.total_coins then
			print("***************************************")
			print("CONGRATULATIONS! YOU FOUND ALL COINS!")
			print("***************************************")
		end
	else
		print("Collected Coin! (Game logic missing)")
	end
	
	world:destroy_entity(self)
    -- Move it far away prevents further collision this frame?
    local loc = self.location
    loc.x = -99999
    self.location = loc
end

function Coin.on_update(self, dt)
	local loc = self.location
	loc.z = loc.z + dt * 3 -- Spin
	self.location = loc
end

return Coin
