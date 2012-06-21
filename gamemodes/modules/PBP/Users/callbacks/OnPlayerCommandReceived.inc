/*!
 * PBP/Users/callbacks/OnPlayerCommandReceived.inc
 *
 * OnPlayerCommandReceived(playerid, cmdtext[])
 *
 > Priority: 0
 */

{
	if (!this.IsLoggedIn(playerid)) {
		this.ShowLogInFirst(playerid);
		
		return 0;
	}
}