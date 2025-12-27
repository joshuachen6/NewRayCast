local Bullet = {}

function Bullet:on_start()
	self.mass = 1
	self.radius = 5
	self.model = "resources/models/bullet.csv"
end

function Bullet:on_collide(other)
    if other ~= GameData.player then
        game:get_world():destroy_entity(self)
        other:damage(100)
    end
end

return Bullet
