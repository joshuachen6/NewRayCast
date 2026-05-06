local Coin = {}

function Coin:on_start()
	self.is_static = true
	self.radius = 15
	self:set_model("resources/models/coin.csv")
	self:add_tag("health_core")
end

function Coin:on_collide(other)
	if other:has_tag("player") then
		-- Restore health
		other.health = math.min(100, other.health + 20)
		GameData.status_message = "HEALTH_RECONSTITUTED"
		GameData.status_timer = 2
		
		audio:play_sound("resources/sounds/money.ogx")
		game:get_world():destroy_entity(self)
	end
end

return Coin
