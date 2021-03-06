/*!
 * PBP/ClassSelection/callbacks/OnPlayerConnect.inc
 *
 * OnPlayerConnect(playerid)
 *
 > Priority: 10
 */

{
	Bit_Vet(this.PlayerHasRequestedClass, playerid);
	
	this.PreviousClassid{playerid} = 0;
	this.GroupSetOnSpawn[playerid] = GROUP_GLOBAL;
	this.PlayerClass[playerid] = INVALID_CLASS;
	
	SetFirstAvailableClass(playerid);
	
	this.LastClass[playerid] = -1;
	
	for (new id = 0; id < MAX_CLASSES; id++) {
		Bit_Vet(this.ClassAvailableForPlayer[playerid], id);
	}
}