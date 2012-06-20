/*!
 * ClassSelection/callbacks/OnPlayerRequestSpawn.inc
 *
 * OnPlayerRequestSpawn(playerid)
 *
 > Priority: 10
 */

{
	if (!Bit_Get(this.PlayerHasRequestedClass, playerid)) {
		CallRemoteFunction(!"OnPlayerRequestClass", !"ii", playerid, 0);
		
		return 0;
	}
	
	if (!Bit_Get(this.PlayerMaySpawn, playerid)) {
		CallRemoteFunction(!"OnPlayerSpawnDenied", !"i", playerid);
		
		return 0;
	}
}