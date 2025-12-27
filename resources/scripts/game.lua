GameData = {
	player = nil,
	score = 0,
}

local Game = {}

function Game:on_start()
	self:switch_world("resources/scripts/worlds/world.lua")
end

return Game
