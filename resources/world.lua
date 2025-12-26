local World = {}

function World.on_start(self)
	print("started world")

	self.friction = 2
	self.gravity = 1

	self.sky_texture = "resources/sprites/sky.jpg"
	self.ground_texture = "resources/sprites/grass.jpg"

	self:vertex_from_model("resources/models/world1.csv", Vector3(0, 0, 0))
	self:spawn_entity("resources/scripts/player.lua", Vector3(0, 0, 0))
end

function World.on_update(self, dt) end

return World
