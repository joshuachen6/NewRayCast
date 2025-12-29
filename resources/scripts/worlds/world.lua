local World = {}

function World:on_start()
	self.friction = 2
	self.gravity = 1

	self.sky_texture = "resources/sprites/sky.jpg"
	self.ground_texture = "resources/sprites/grass.jpg"

	audio:play_music("resources/music/grasswalk.mp3")

	-- Initialize Game Data
	GameData.score = 0
	GameData.wave = 1
	GameData.game_over = false

	-- Spawn Player at origin
	self:spawn_entity("resources/scripts/entities/player.lua", Vector3(0, 0, 0))

	-- Spawn Spawner
	self:spawn_entity("resources/scripts/entities/spawner.lua", Vector3(0, 0, 0))
end

return World
