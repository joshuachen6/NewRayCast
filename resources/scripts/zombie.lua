local Zombie = {}

function Zombie.on_start(self)
	self.mass = 100
	self.radius = 20
	self.model = "resources/models/zombie.csv"
end

function Zombie.on_update(self, dt)
	if Game.player then
		local vel = self.velocity
		local loc = self.location

		local target_loc = Game.player.location
		local diff = physics.squash(target_loc - loc)
		local direction = physics.normalize(diff)

		local accel = direction * (220 * dt)

		vel.x = vel.x + accel.x
		vel.y = vel.y + accel.y

		self.velocity = vel

		local target_angle = physics.direction(direction)

		loc.z = target_angle
		self.location = loc
	end
end

return Zombie
