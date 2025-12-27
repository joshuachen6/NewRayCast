local World = {}

function World:on_start()
	self.friction = 2
	self.gravity = 1

	self.sky_texture = "resources/sprites/sky.jpg"
	self.ground_texture = "resources/sprites/grass.jpg"

	-- Load map walls
	self:vertex_from_model("resources/models/world1.csv")

	-- Spawn Player at origin
	self:spawn_entity("resources/scripts/entities/player.lua", Vector3(0, 0, 0))

	local zombies = {
		{ 100, 100 },
	}

	for i, position in ipairs(zombies) do
		self:spawn_entity("resources/scripts/entities/zombie.lua", Vector3(position[1], position[2], 0))
	end
end

function World:on_render(renderer)
	if GameData.player then
		local screen_size = renderer:get_size()
		renderer:draw_text(
			Vector2(0, 0),
			"resources/fonts/font.ttf",
			tostring(GameData.player.health),
			screen_size.y / 20,
			Color(255, 255, 255, 255)
		)
	end
end

return World
