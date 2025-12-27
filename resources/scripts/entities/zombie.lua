local Zombie = {}
local cooldown = 0

function Zombie:on_start()
	self.mass = 100
	self.radius = 20
	self.model = "resources/models/zombie.csv"
	cooldown = 0
end

function Zombie:on_update(dt)
	if cooldown > 0 then
		cooldown = cooldown - dt
	end

	if GameData.player then
		local target_loc = GameData.player.location
		local diff = physics.squash(target_loc - self.location)
		local direction = physics.normalize(diff)

		self.velocity = self.velocity + (direction * (220 * dt))

		local loc = self.location
		loc.z = physics.direction(direction)
		self.location = loc
	end
end

function Zombie:on_collide(other)
	if cooldown <= 0 then
		other:damage(10)
		cooldown = 1
	end
end

return Zombie
