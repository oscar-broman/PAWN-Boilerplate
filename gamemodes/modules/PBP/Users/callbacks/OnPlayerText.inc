/*!
 * Users/callbacks/OnPlayerText.inc
 *
 * OnPlayerText(playerid, text[])
 *
 > Priority: 0
 */

{
	if (!this.IsLoggedIn(playerid)) {
		this.ShowLogInFirst(playerid);
		
		return 0;
	}
}