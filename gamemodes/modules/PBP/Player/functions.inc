/*!
 * Player/functions.inc
 *
 > Priority: 0
 */

// Test whether a player is in any of the groups given
stock bool:this.IsInAnyOfGroups(playerid, Group:...) {
	for (new i = numargs(); --i; ) {
		if (Group_GetPlayer(playerid, Group:getarg(i)))
			return true;
	}

	return false;
}

// Used internally from a timer
public this.CheckForNewAdmins() {
	foreach (new playerid : Player) {
		if (!this.Info[playerid][IsAdmin] && IsPlayerAdmin(playerid)) {
			this.Info[playerid][IsAdmin] = true;
			
			CallRemoteFunction(#OnPlayerRconLogin, !"i", playerid);
		}
	}
}

// Custom sscanf specifier for searching through player names
// The default one doesn't find "def" in [ABC]Def.
#if defined SSCANF
	SSCANF:player_name(string[]) {
		new foundplayer = INVALID_PLAYER_ID;
		new bool:numeric = true;
		
		for (new i = 0, c; (c = string[i]); i++) {
			if (c < '0' || c > '9') {
				numeric = false;
				
				break;
			}
		}
		
		if (numeric) {
			foundplayer = strval(string);
			
			if (IsPlayerConnected(foundplayer))
				return foundplayer;
			else
				foundplayer = INVALID_PLAYER_ID;
		}
		
		foreach(new playerid : Player) {
			if (strfind(this.Info[playerid][Name], string, true) != -1) {
				// Multiple matches
				if (foundplayer != INVALID_PLAYER_ID)
					return INVALID_PLAYER_ID;
				else
					foundplayer = playerid;
			}
		}
		
		return foundplayer;
	}
#endif