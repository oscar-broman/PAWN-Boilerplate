/*!
 * Player/callbacks/OnPlayerRequestClass.inc
 *
 * OnPlayerRequestClass(playerid, classid)
 *
 > Priority: 0
 */

{
	SetPlayerVirtualWorld(playerid, this.OFFSET);
	SetPlayerInterior    (playerid, 12);
	SetPlayerPos         (playerid, 2324.0000, -1143.0000, /* XXX */ TRUE ? 1050.4922 : 1057.4922);
	SetPlayerFacingAngle (playerid, 148.7130);
	SetPlayerCameraPos   (playerid, 2322.5078, -1146.3310, 1050.7100);
	SetPlayerCameraLookAt(playerid, 2324.0000, -1143.0000, 1050.5000);
}