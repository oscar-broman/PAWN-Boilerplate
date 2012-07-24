/*!
 * PBP/Admin/header.inc
 *
 > Priority: 0
 */

const AdminLevel:this.INVALID_LEVEL = AdminLevel:-1;

#if !defined CUSTOM_ADMIN_LEVELS
	enum AdminLevel:{
		this.LEVEL_NONE,
		this.LEVEL_MOD,
		this.LEVEL_ADMIN,
		this.LEVEL_MANAGEMENT,
	
		this.NUM_LEVELS
	};
#endif


new
	AdminLevel:this.PlayerLevel[MAX_PLAYERS],
	           this.LevelNames[this.NUM_LEVELS][1]
;

#if !defined CUSTOM_ADMIN_LEVELS
new
	StaticGroup<GROUP_MOD>        = @"Moderator",
	StaticGroup<GROUP_ADMIN>      = @"Administrator",
	StaticGroup<GROUP_MANAGEMENT> = @"Management",
	
	StaticGroup<GROUP_RCON>       = @"RCON Administrator"
;
#endif

RegisterUserVariable: "admin-level" => this.PlayerLevel;


forward AdminLevel:GetPlayerAdminLevel(playerid);
forward            SetPlayerAdminLevel(playerid, AdminLevel:level, by_player = INVALID_PLAYER_ID);
forward       bool:Hook_IsPlayerAdmin(playerid);
forward       bool:IsPlayerManagement(playerid);
forward       bool:IsPlayerMod(playerid);
forward       bool:IsPlayerRconAdmin(playerid);