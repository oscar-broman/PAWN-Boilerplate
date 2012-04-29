/*!
 * Admin/functions.inc
 *
 > Priority: 0
 */

stock AdminLevel:this.GetPlayerLevel(playerid) {
	return this.PlayerLevel{playerid};
}

// This essentially creates a simple group inheritance
stock this.SetPlayerLevel(playerid, AdminLevel:level, by_player = INVALID_PLAYER_ID) {
	new AdminLevel:old_level = this.PlayerLevel{playerid};
	
	this.PlayerLevel{playerid} = level;
	
	Group_SetPlayer(GROUP_MOD       , playerid, (level >= this.LEVEL_MOD       ));
	Group_SetPlayer(GROUP_ADMIN     , playerid, (level >= this.LEVEL_ADMIN     ));
	Group_SetPlayer(GROUP_MANAGEMENT, playerid, (level >= this.LEVEL_MANAGEMENT));
	
	CallLocalFunctionEx(!"OnPlayerAdminLevelChanged", !"iiii", playerid, _:old_level, _:level, by_player);
}