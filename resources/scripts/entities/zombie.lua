local Zombie = {}
local cooldown = 0
local soundCooldown = 0

function Zombie:on_start()
	self.mass = 150
	self.radius = 20
	self.health = 50 + (GameData.wave * 10) -- Scale health with wave
	self:set_model("resources/models/zombie.csv")

	self:add_tag("drone")
	self.id = "drone"

	cooldown = 0
end

function Zombie:on_update(dt)
	if cooldown > 0 then
		cooldown = cooldown - dt
	end

	if GameData.player then
		local target_loc = GameData.player.location
		local diff = physics.squash(target_loc - self.location)
		local dist = physics.mag(diff)
		local direction = physics.normalize(diff)

		-- Speed increases slightly each wave
		local speed = (250 + (GameData.wave * 15)) * dt
		self.velocity = self.velocity + (direction * speed)

		local loc = self.location
		loc.z = physics.direction(direction)
		self.location = loc
	end
end

function Zombie:on_collide(other)
	if other then
		if other:has_tag("player") then
			if cooldown <= 0 then
				other:damage(15)
				cooldown = 1.5
				audio:play_sound("resources/sounds/eat.ogx")
			end
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
	-- Spawn a health core occasionally
	if math.random() > 0.7 then
		game:get_world():spawn_entity("resources/scripts/entities/coin.lua", self.location)
	end
	
	GameData.score = GameData.score + 50
	GameData.enemies_remaining = GameData.enemies_remaining - 1
end

return Zombie
