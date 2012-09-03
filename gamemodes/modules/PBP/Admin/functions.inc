/*!
 * PBP/Admin/functions.inc
 *
 > Priority: 0
 */

stock AdminLevel:GetPlayerAdminLevel(playerid) {
	return this.PlayerLevel[playerid];
}

stock this.UpdateGroups(playerid) {
	new AdminLevel:level = this.PlayerLevel[playerid];
	
	// This essentially creates a simple group inheritance
	Group_SetPlayer(GROUP_MOD       , playerid, (level >= this.LEVEL_MOD       ));
	Group_SetPlayer(GROUP_ADMIN     , playerid, (level >= this.LEVEL_ADMIN     ));
	Group_SetPlayer(GROUP_MANAGEMENT, playerid, (level >= this.LEVEL_MANAGEMENT));
}

stock SetPlayerAdminLevel(playerid, AdminLevel:level, by_player = INVALID_PLAYER_ID) {
	new AdminLevel:old_level = this.PlayerLevel[playerid];
	
	this.PlayerLevel[playerid] = level;
	
	this.UpdateGroups(playerid);
	
	CallLocalFunctionEx(!"OnPlayerAdminLevelChanged", !"iiii", playerid, _:old_level, _:level, by_player);
}

stock bool:IsPlayerMod(playerid) {
	return (this.PlayerLevel[playerid] >= this.LEVEL_MOD);
}

stock bool:Hook_IsPlayerAdmin(playerid) {
	return (this.PlayerLevel[playerid] >= this.LEVEL_ADMIN);
}

stock bool:IsPlayerManagement(playerid) {
	return (this.PlayerLevel[playerid] >= this.LEVEL_MANAGEMENT);
}

stock bool:IsPlayerRconAdmin(playerid) {
	return IsPlayerAdmin(playerid) || Group_GetPlayer(GROUP_RCON, playerid);
}

#if defined _ALS_IsPlayerAdmin
	#undef IsPlayerAdmin
#else
	#define _ALS_IsPlayerAdmin
#endif
#define IsPlayerAdmin Hook_IsPlayerAdmin