local Spawner = {}
local spawn_timer = 0
local to_spawn = 0

function Spawner:on_start()
	self.is_static = true
	self.has_collision = false
	self:add_tag("spawner")
	
	GameData.wave = 1
	GameData.enemies_remaining = 0
	to_spawn = 0
	self:start_wave()
	print("SPAWNER_INITIALIZED: WAVE 1")
end

function Spawner:start_wave()
	spawn_timer = 2
	local count = 5 + (GameData.wave * 3)
	GameData.enemies_remaining = count
	to_spawn = count
	GameData.status_message = "THREAT_LEVEL_" .. tostring(GameData.wave) .. "_DETECTED"
	GameData.status_timer = 3
	print("STARTING_WAVE: " .. tostring(GameData.wave) .. " | THREATS: " .. tostring(count))
end

function Spawner:on_update(dt)
	if GameData.game_over then return end

	-- Check if wave is fully cleared
	if GameData.enemies_remaining <= 0 then
		print("SECTOR_CLEAR: PROCEEDING_TO_NEXT_WAVE")
		GameData.wave = GameData.wave + 1
		self:start_wave()
		return
	end

	-- Spawn logic
	if to_spawn > 0 then
		spawn_timer = spawn_timer - dt
		if spawn_timer <= 0 then
			local num_to_spawn = math.min(2, to_spawn)
			for i = 1, num_to_spawn do
				-- Spawn in a ring around the spawner
				local angle = math.random() * math.pi * 2
				local dist = 100 + math.random(0, 50)
				local offset_x = math.cos(angle) * dist
				local offset_y = math.sin(angle) * dist
				
				local spawn_loc = Vector3(self.location.x + offset_x, self.location.y + offset_y, 0)
				game:get_world():spawn_entity("resources/scripts/entities/zombie.lua", spawn_loc)
				to_spawn = to_spawn - 1
			end
			
			spawn_timer = 3 + math.random(0, 2)
			print("UNITS_DEPLOYED: REMAINING_TO_SPAWN: " .. tostring(to_spawn))
		end
	end
end

return Spawner
