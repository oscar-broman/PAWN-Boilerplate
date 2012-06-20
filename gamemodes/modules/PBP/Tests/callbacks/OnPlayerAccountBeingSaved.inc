/*!
 * Tests/callbacks/OnPlayerAccountBeingSaved.inc
 *
 * OnPlayerAccountBeingSaved(playerid)
 *
 > Priority: 0
 */

{
	this.PlayerInt[playerid]++;
	this.PlayerFloat[playerid] += 1.5;
	
	format(this.PlayerString[playerid], _, "time: %d", gettime());
	
	static temp[1000 char];
	
	if (this.PlayerMallocString[playerid] && !strlen(mstr(this.PlayerMallocString[playerid]))) {
		free(this.PlayerMallocString[playerid]);
		
		this.PlayerMallocString[playerid] = NO_ALLOC;
	}
	
	if (this.PlayerMallocString[playerid]) {
		strpack(temp, mstr(this.PlayerMallocString[playerid]));
		
		free(this.PlayerMallocString[playerid]);
		
		strcat(temp, !"n");
		
		this.PlayerMallocString[playerid] = Malloc_NewS(temp);
	} else {
		this.PlayerMallocString[playerid] = Malloc_NewS("yoyo! lege");
	}
}