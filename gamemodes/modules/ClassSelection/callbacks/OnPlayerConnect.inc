/*!
 * ClassSelection/callbacks/OnPlayerConnect.inc
 *
 * OnPlayerConnect(playerid)
 *
 > Priority: 10
 */

{
	this.AllowSpawning(playerid);
	
	Bit_Vet(this.PlayerHasRequestedClass, playerid);
	
	this.PreviousClassid{playerid} = 0;
	
	this.SetFirstAvailableClass(playerid);
}