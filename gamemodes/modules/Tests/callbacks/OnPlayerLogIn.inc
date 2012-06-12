/*!
 * Tests/callbacks/OnPlayerLogIn.inc
 *
 * OnPlayerLogIn(playerid, bool:autologin)
 *
 > Priority: 0
 */

{
	SendClientMessagef(playerid, -1, "Tests.PlayerInt = %d", this.PlayerInt[playerid]);
	SendClientMessagef(playerid, -1, "Tests.PlayerFloat = %f", this.PlayerFloat[playerid]);
	SendClientMessagef(playerid, -1, "Tests.PlayerString = \"%s\"", this.PlayerString[playerid]);
	SendClientMessagef(playerid, -1, "Tests.PlayerMallocString = \"%s\"", mstr(this.PlayerMallocString[playerid]));
}