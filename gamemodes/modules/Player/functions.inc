/*!
 * Player/functions.inc
 *
 > Priority: 0
 */

// Test whether a player is in any of the groups given
stock bool:this.IsInAnyGroup(playerid, Group:...) {
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