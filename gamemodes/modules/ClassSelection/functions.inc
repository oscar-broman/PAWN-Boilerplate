/*!
 * ClassSelection/functions.inc
 *
 > Priority: 0
 */

stock this.AllowSpawning(playerid, bool:set = true) {
	Bit_Set(this.PlayerMaySpawn, playerid, set);
}