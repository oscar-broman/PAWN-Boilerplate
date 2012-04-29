/*!
 * Admin/commands.inc
 *
 > Priority: 0
 */

CommandDescription<setlevel> = "Set a player's admin level.";

YCMD(GROUP_MANAGEMENT):setlevel(playerid, params[], help) {
	if (help) {
		SendClientMessage(playerid, COLOR_COMMAND_HELP, "* Set a player's admin level.");

usage:
        SendClientMessage(playerid, COLOR_COMMAND_USAGE, "* Usage: /setlevel [player] [level]");
	} else {
		new
			           otherid,
			AdminLevel:level = this.INVALID_LEVEL,
			           level_name[32]
		;
		
		if (!sscanf(params, "ui", otherid, _:level)) {
			if (!(AdminLevel:0 <= level < this.NUM_LEVELS))
				level = this.INVALID_LEVEL;
		} else if (!sscanf(params, "us[32]", otherid, level_name)) {
			new AdminLevel:found_level = this.INVALID_LEVEL;
			
			for (level = AdminLevel:0; level < this.NUM_LEVELS; level++) {
				if (strfind(this.LevelNames[level], level_name, true) != -1) {
					if (found_level != this.INVALID_LEVEL) {
						// Multiple matches
						found_level = this.INVALID_LEVEL;
						
						break;
					} else {
						found_level = level;
					}
				}
			}
			
			level = found_level;
		} else
			goto usage;
		
		if (!IsPlayerAdmin(playerid) && this.GetPlayerLevel(otherid) >= this.GetPlayerLevel(playerid)) {
			SendClientMessage(playerid, COLOR_COMMAND_ERROR, "* You may only change the level of players with a level lower than yours.");
		} else if (level == this.INVALID_LEVEL) {
			SendClientMessage(playerid, COLOR_COMMAND_ERROR, "* Invalid level.");
		} else {
			if (this.GetPlayerLevel(otherid) == level)
				SendClientMessagef(playerid, COLOR_COMMAND_ERROR, "* %P%C already has that level.", otherid, COLOR_COMMAND_ERROR);
			else {
				this.SetPlayerLevel(otherid, level, playerid);
			
				SendClientMessagef(playerid, COLOR_COMMAND_INFO, "* You set the level of %P%C to %s (%d).", otherid, COLOR_COMMAND_INFO, this.LevelNames[level], _:level);
			}
		}
	}
	
	return 1;
}