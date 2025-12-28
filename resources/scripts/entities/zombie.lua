local Zombie = {}
local cooldown = 0
local soundCooldown = 0

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

	if soundCooldown > 0 then
		soundCooldown = soundCooldown - dt
	end

	if soundCooldown <= 0 then
		if math.random() > 0.5 then
			audio:play_sound_at("resources/sounds/zombie.ogx", physics.squash(self.location))
		end
		soundCooldown = 2
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
	if other.script == "resources/scripts/entities/player.lua" then
		if cooldown <= 0 then
			other:damage(10)
			cooldown = 1
			audio:play_sound("resources/sounds/eat.ogx")
		end
	end
end

function Zombie:on_damage(amount)
	self.health = self.health - amount
	if self.health <= 0 then
		game:get_world():destroy_entity(self)
	end
end

function Zombie:on_death()
	game:get_world():spawn_entity("resources/scripts/entities/coin.lua", self.location)
	GameData.score = GameData.score + 10
end

return Zombie
