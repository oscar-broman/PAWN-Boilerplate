/*!
 * Admin/header.inc
 *
 > Priority: 0
 */

enum AdminLevel:{
	this.INVALID_LEVEL = -1,
	
	this.LEVEL_NONE,
	this.LEVEL_MOD,
	this.LEVEL_ADMIN,
	this.LEVEL_MANAGEMENT,
	
	this.NUM_LEVELS
};

stock const
	this.LevelNames[this.NUM_LEVELS][] = {
		"Nothing",
		"Moderator",
		"Administrator",
		"Management"
	}
;

new
	AdminLevel:this.PlayerLevel[MAX_PLAYERS char],
	
	StaticGroup<GROUP_MOD>        = "Moderator",
	StaticGroup<GROUP_ADMIN>      = "Administrator",
	StaticGroup<GROUP_MANAGEMENT> = "Management"
;

// Forward-declarations
forward AdminLevel:this.GetPlayerLevel(playerid);
forward this.SetPlayerLevel(playerid, AdminLevel:level, by_player = INVALID_PLAYER_ID);