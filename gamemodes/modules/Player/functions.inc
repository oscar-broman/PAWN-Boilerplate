/*!
 * Player/functions.inc
 *
 > Priority: 0
 */

forward this.CheckForNewAdmins();
public this.CheckForNewAdmins() {
	foreach (new playerid : Player) {
		if (!this.Info[playerid][IsAdmin] && IsPlayerAdmin(playerid)) {
			this.Info[playerid][IsAdmin] = true;
			
			CallRemoteFunction(#OnPlayerRconLogin, "i", playerid);
		}
	}
}