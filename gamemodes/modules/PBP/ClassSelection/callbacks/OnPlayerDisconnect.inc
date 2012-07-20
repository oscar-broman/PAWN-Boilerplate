/*!
 * PBP/ClassSelection/callbacks/OnPlayerDisconnect.inc
 *
 * OnPlayerDisconnect(playerid, reason)
 *
 > Priority: 0
 */

{
	TextDrawDestroyVar(this.WeaponsInfo[playerid]);
}