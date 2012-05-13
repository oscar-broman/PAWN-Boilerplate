/*!
 * ClassSelection/callbacks/OnPlayerSpawn.inc
 *
 * OnPlayerSpawn(playerid)
 *
 > Priority: 9
 */

{
	if (!Bit_Get(this.PlayerMaySpawn, playerid)) {
		ForceClassSelection(playerid);
		SetPlayerHealth(playerid, 0.0);
		TogglePlayerSpectating(playerid, false);
		
		CallLocalFunction(!"OnPlayerSpawnDenied", !"i", playerid);
		
		return 0;
	}
}
