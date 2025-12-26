local World = {}

Game = {
	score = 0,
	total_coins = 0
}

function World.on_start(self)
	print("Starting Coin Hunt Game!")
	
	self.friction = 2
	self.gravity = 1

	self.sky_texture = "resources/sprites/sky.jpg"
	self.ground_texture = "resources/sprites/grass.jpg"

	-- Load map walls
	self:vertex_from_model("resources/models/world1.csv")

	-- Spawn Player at origin
	self:spawn_entity("resources/scripts/player.lua", Vector3(0, 0, 0))
	
	-- Spawn Coins
	local coin_pos = {
		{0, 100},
		{150, -50},
		{-200, 50},
		{100, 200},
		{-100, -150}
	}
	
	Game.total_coins = #coin_pos
	Game.score = 0
	print("Find all " .. Game.total_coins .. " coins!")
	
	for i, pos in ipairs(coin_pos) do
		self:spawn_entity("resources/scripts/coin.lua", Vector3(pos[1], pos[2], 0))
	end
end

function World.on_update(self, dt)
end

return World
