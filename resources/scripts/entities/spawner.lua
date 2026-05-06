local Spawner = {}
local spawn_timer = 0

function Spawner:on_start()
	self.is_static = true
	self.has_collision = false
	self:add_tag("spawner")
	
	GameData.wave = 1
	GameData.enemies_remaining = 0
	self:start_wave()
end

function Spawner:start_wave()
	spawn_timer = 2
	GameData.enemies_remaining = 5 + (GameData.wave * 3)
	GameData.status_message = "THREAT_LEVEL_" .. tostring(GameData.wave) .. "_DETECTED"
	GameData.status_timer = 3
end

function Spawner:on_update(dt)
	if GameData.game_over then return end

	-- Check if wave is clear
	if GameData.enemies_remaining <= 0 then
		GameData.wave = GameData.wave + 1
		self:start_wave()
		return
	end

	-- Spawn logic
	spawn_timer = spawn_timer - dt
	if spawn_timer <= 0 then
		local num_to_spawn = math.min(3, GameData.enemies_remaining)
		for i = 1, num_to_spawn do
			local offset_x = (math.random() - 0.5) * 100
			local offset_y = (math.random() - 0.5) * 100
			local spawn_loc = Vector3(self.location.x + offset_x, self.location.y + offset_y, 0)
			
			game:get_world():spawn_entity("resources/scripts/entities/zombie.lua", spawn_loc)
		end
		
		-- Slow down spawns as wave progresses
		spawn_timer = 5 + math.random(1, 3)
	end
end

return Spawner
