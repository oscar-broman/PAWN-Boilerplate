/*!
 * Tests/callbacks/OnPlayerLogIn.inc
 *
 * OnPlayerLogIn(playerid, bool:autologin)
 *
 > Priority: 0
 */

{
	if (!this.PlayerMallocString[playerid])
		this.PlayerMallocString[playerid] = Malloc_NewS("");
	
	SendClientMessage(playerid, -1, "Tests.PlayerInt = %d", this.PlayerInt[playerid]);
	SendClientMessage(playerid, -1, "Tests.PlayerFloat = %f", this.PlayerFloat[playerid]);
	SendClientMessage(playerid, -1, "Tests.PlayerString = \"%s\"", this.PlayerString[playerid]);
	SendClientMessage(playerid, -1, "Tests.PlayerMallocString = \"%s\"", mstr(this.PlayerMallocString[playerid]));
}