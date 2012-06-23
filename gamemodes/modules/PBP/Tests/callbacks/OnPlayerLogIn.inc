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
	
	printf("Tests.PlayerInt = %d", this.PlayerInt[playerid]);
	printf("Tests.PlayerFloat = %f", this.PlayerFloat[playerid]);
	printf("Tests.PlayerString = \"%s\"", this.PlayerString[playerid]);
	printf("Tests.PlayerMallocString = \"%s\"", mstr(this.PlayerMallocString[playerid]));
	printf("Tests.PlayerBit = \"%d\"", Bit_Get(this.PlayerBit, playerid));
	printf("Tests.PlayerChar = \"%d\"", this.PlayerChar{playerid});
}