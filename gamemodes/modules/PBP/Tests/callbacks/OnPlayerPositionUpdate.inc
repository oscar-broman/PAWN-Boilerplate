/*!
 * PBP/Tests/callbacks/OnPlayerPositionUpdate.inc
 *
 * OnPlayerPositionUpdate(playerid, Float:x, Float:y, Float:z)
 *
 > Priority: 0
 */

{
	P:7("%s changed position to: %5.2f, %5.2f, %5.2f", Player.Info[playerid][Name], x, y, z);
}