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
	
	if (this.TeleportTextDraw[playerid] != INVALID_TEXT_DRAW) {
		TextDrawDestroy(this.TeleportTextDraw[playerid]);
		
		this.TeleportTextDraw[playerid] = INVALID_TEXT_DRAW;
	}
}