/*!
 * PBP/Users/callbacks/OnPlayerRequestSpawn.inc
 *
 * OnPlayerRequestSpawn(playerid)
 *
 > Priority: 9
 */

{
	if (!this.IsLoggedIn(playerid)) {
		this.ShowLogInFirst(playerid);
		
		return 0;
	}
}