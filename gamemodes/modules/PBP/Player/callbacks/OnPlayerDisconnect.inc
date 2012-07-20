/*!
 * PBP/Player/callbacks/OnPlayerDisconnect.inc
 *
 * OnPlayerDisconnect(playerid, reason)
 *
 > Priority: 0
 */

{
	new timer;
	
	if (-1 != (timer = GetPVarInt(playerid, #this.TeleportTimer)))
		KillTimer(timer);
	
	TextDrawDestroyVar(this.TeleportTextDraw[playerid]);
}