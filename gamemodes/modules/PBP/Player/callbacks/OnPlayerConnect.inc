/*!
 * Player/callbacks/OnPlayerConnect.inc
 *
 * OnPlayerConnect(playerid)
 *
 > Priority: 9
 */

{
	
	// Reset the player info
	memset(Player.Info[playerid], sizeof(Player.Info[]), 0);
	
	Player.Info[playerid][IP] = GetIP(playerid);
	
	GetPlayerName(playerid, Player.Info[playerid][Name], MAX_PLAYER_NAME);
}