local Spawner = {}
local timer = 0
local spawn_rate = 5 -- Seconds
local wave = 1
local started = false

local function spawn_wave()
	if GameData.player then
		-- Spawn random zombies around player
		local count = math.min(10, math.ceil(wave / 2))
		for i = 1, count do
			local angle = math.random() * math.pi * 2
			local dist = 300 + math.random() * 200
			local px = GameData.player.location.x
			local py = GameData.player.location.y
			local x = px + math.cos(angle) * dist
			local y = py + math.sin(angle) * dist

			game:get_world():spawn_entity("resources/scripts/entities/zombie.lua", Vector3(x, y, 0))
		end
	end
end

function Spawner:on_start()
	self.location = Vector3(0, 0, 0)
	self.is_static = true

	self:add_tag("spawner")
	self.id = "zombie_spawner"

	self:set_model("resources/models/zombie.csv")

	spawn_wave()
end

function Spawner:on_interact(entity)
	started = true
	self:set_model("")
end

function Spawner:on_update(dt)
	if started then
		timer = timer + dt
		if timer > spawn_rate then
			timer = 0
			spawn_rate = math.max(1, spawn_rate * 0.95) -- Increase difficulty
			wave = wave + 1
			GameData.wave = wave
			spawn_wave()
		end
	end
end

return Spawner
