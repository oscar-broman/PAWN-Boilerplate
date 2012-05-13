/*!
 * ClassSelection/callbacks/OnPlayerRequestSpawn.inc
 *
 * OnPlayerRequestSpawn(playerid)
 *
 > Priority: 10
 */

{
	if (!Bit_Get(this.PlayerMaySpawn, playerid)) {
		CallLocalFunction(!"OnPlayerSpawnDenied", !"i", playerid);
		
		return 0;
	}
}