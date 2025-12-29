GameData = {
	player = nil,
	score = 0,
}

local Game = {}

function Game:on_start()
	self:switch_world("resources/scripts/worlds/world.lua")
end

function Game:on_render(renderer)
	local screen_size = renderer:get_size()
	local font = "resources/fonts/font.ttf"
	local white = Color(255, 255, 255, 255)
	local red = Color(255, 0, 0, 255)
	local green = Color(0, 255, 0, 255)

	if GameData.player then
		renderer:draw_text(
			Vector2(10, 10),
			font,
			"Health: " .. tostring(math.ceil(GameData.player.health)),
			screen_size.y / 20,
			white
		)
		renderer:draw_text(
			Vector2(10, 10 + screen_size.y / 15),
			font,
			"Score: " .. tostring(GameData.score),
			screen_size.y / 20,
			white
		)
		renderer:draw_text(
			Vector2(10, 10 + screen_size.y / 7.5),
			font,
			"Wave: " .. tostring(GameData.wave),
			screen_size.y / 20,
			white
		)
	end

	renderer:draw_text_wrapped(
		Vector2(0, 0),
		font,
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
		screen_size.y / 20,
		green,
		screen_size.x
	)

	if GameData.game_over then
		renderer:draw_text(
			Vector2(screen_size.x / 2 - 200, screen_size.y / 2),
			font,
			"GAME OVER",
			screen_size.y / 10,
			red
		)
	end
end

return Game
