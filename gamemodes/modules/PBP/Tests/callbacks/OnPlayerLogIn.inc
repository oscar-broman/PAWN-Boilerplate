/*!
 * PBP/Tests/callbacks/OnPlayerLogIn.inc
 *
 * OnPlayerLogIn(playerid, bool:autologin)
 *
 > Priority: 0
 */

{
	if (!this.PlayerMallocString[playerid])
		this.PlayerMallocString[playerid] = Malloc_NewS("");
	
	P:3(playerid, -1, "Tests.PlayerInt = %d", this.PlayerInt[playerid]);
	P:3(playerid, -1, "Tests.PlayerFloat = %f", this.PlayerFloat[playerid]);
	P:3(playerid, -1, "Tests.PlayerString = \"%s\"", this.PlayerString[playerid]);
	P:3(playerid, -1, "Tests.PlayerMallocString = \"%s\"", mstr(this.PlayerMallocString[playerid]));
}