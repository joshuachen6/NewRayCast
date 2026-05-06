GameData = {
	player = nil,
	score = 0,
	wave = 1,
	enemies_remaining = 0,
	game_over = false,
	status_message = "INITIATING_SYSTEMS...",
	status_timer = 3,
}

local Game = {}

function Game:on_start()
	self:switch_world("resources/scripts/worlds/world.lua")
end

function Game:on_render(renderer)
	local screen_size = renderer:get_size()
	local font = "resources/fonts/font.ttf"
	local white = Color(255, 255, 255, 255)
	local cyan = Color(0, 242, 255, 255)
	local red = Color(255, 50, 50, 255)
	local dim_white = Color(200, 200, 200, 150)

	-- Overlay Technical Details
	renderer:draw_text(Vector2(20, 20), font, "PROTOCOL: OMEGA_SURVIVAL", screen_size.y / 40, dim_white)
	
	if GameData.player then
		-- Health Bar Background
		renderer:draw_rectangle(Vector2(20, 60), Vector2(200, 20), Color(50, 50, 50, 150))
		-- Health Bar Foreground
		local health_width = math.max(0, (GameData.player.health / 100) * 200)
		renderer:draw_rectangle(Vector2(20, 60), Vector2(health_width, 20), GameData.player.health > 30 and cyan or red)
		
		renderer:draw_text(Vector2(230, 55), font, "HP_" .. tostring(math.ceil(GameData.player.health)) .. "%", screen_size.y / 30, white)
		
		-- Stats
		renderer:draw_text(Vector2(20, 100), font, "CORE_SCORE: " .. string.format("%06d", GameData.score), screen_size.y / 25, white)
		renderer:draw_text(Vector2(20, 140), font, "SECTOR_WAVE: " .. tostring(GameData.wave), screen_size.y / 25, cyan)
		renderer:draw_text(Vector2(20, 180), font, "THREATS_DETECTED: " .. tostring(GameData.enemies_remaining), screen_size.y / 35, red)
	end

	-- Status Messages
	if GameData.status_timer > 0 then
		renderer:draw_text(
			Vector2(screen_size.x / 2 - 150, screen_size.y / 4),
			font,
			GameData.status_message,
			screen_size.y / 25,
			cyan
		)
	end

	if GameData.game_over then
		renderer:draw_rectangle(Vector2(0, 0), screen_size, Color(0, 0, 0, 180))
		renderer:draw_text(
			Vector2(screen_size.x / 2 - 250, screen_size.y / 2 - 50),
			font,
			"CRITICAL_FAILURE",
			screen_size.y / 10,
			red
		)
		renderer:draw_text(
			Vector2(screen_size.x / 2 - 180, screen_size.y / 2 + 50),
			font,
			"PRESS ESC TO TERMINATE",
			screen_size.y / 30,
			white
		)

		if controls:key_triggered(Key.Escape) then
			self.running = false
		end
	end
end

function Game:on_update(dt)
	if GameData.status_timer > 0 then
		GameData.status_timer = GameData.status_timer - dt
	end
end

return Game
