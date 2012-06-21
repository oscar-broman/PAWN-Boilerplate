/*!
 * PBP/Users/callbacks/OnPlayerDisconnect.inc
 *
 * OnPlayerDisconnect(playerid, reason)
 *
 > Priority: 0
 */

{
	if (this.IsLoggedIn(playerid)) {
		this.SaveAccount(playerid);
	}
}