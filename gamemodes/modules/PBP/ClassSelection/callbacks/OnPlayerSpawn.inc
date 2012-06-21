/*!
 * PBP/ClassSelection/callbacks/OnPlayerSpawn.inc
 *
 * OnPlayerSpawn(playerid)
 *
 > Priority: 9
 */

{
	SetPlayerInterior(playerid, 0);
	
	new class = this.RequestedClass[playerid];
	
	this.PlayerClass[playerid] = class;
	
	if (this.Classes[class][World] != 0)
		SetPlayerVirtualWorld(playerid, this.Classes[class][World]);
	else if (this.SpawnLocations[this.Classes[class][SpawnLocation]][World] != 0)
		SetPlayerVirtualWorld(playerid, this.SpawnLocations[this.Classes[class][SpawnLocation]][World]);
	else
		SetPlayerVirtualWorld(playerid, 0);
	
	if (this.Classes[class][Team] != INVALID_TEAM)
		SetPlayerTeam(playerid, this.Classes[class][Team]);
	else if (this.SpawnLocations[this.Classes[class][SpawnLocation]][Team] != INVALID_TEAM)
		SetPlayerTeam(playerid, this.SpawnLocations[this.Classes[class][SpawnLocation]][World]);
	else
		SetPlayerTeam(playerid, NO_TEAM);

	if (this.Classes[class][Group] != GROUP_GLOBAL) {
		if (!Group_GetPlayer(this.Classes[class][Group], playerid)) {
			Group_SetPlayer(this.Classes[class][Group], playerid, true);
			
			this.GroupSetOnSpawn[playerid] = this.Classes[class][Group];
		}
	} else if (this.SpawnLocations[this.Classes[class][SpawnLocation]][Group] != GROUP_GLOBAL) {
		if (!Group_GetPlayer(this.SpawnLocations[this.Classes[class][SpawnLocation]][Group], playerid)) {
			Group_SetPlayer(this.SpawnLocations[this.Classes[class][SpawnLocation]][Group], playerid, true);
			
			this.GroupSetOnSpawn[playerid] = this.SpawnLocations[this.Classes[class][SpawnLocation]][Group];
		}
	}
	
	new weapon_set = INVALID_WEAPON_SET;
	
	if (this.Classes[class][WeaponSet] != INVALID_WEAPON_SET)
		weapon_set = this.Classes[class][WeaponSet];
	else if (this.SpawnLocations[this.Classes[class][SpawnLocation]][WeaponSet] != INVALID_WEAPON_SET)
		weapon_set = this.SpawnLocations[this.Classes[class][SpawnLocation]][WeaponSet];
	
	if (weapon_set != INVALID_WEAPON_SET) {
		for (new slot = 0; slot < this.MAX_WEAPON_SLOT; slot++) {
			if (this.WeaponSets[weapon_set][slot][Weapon] == WEAPON_ARMOUR)
				SetPlayerArmour(playerid, float(this.WeaponSets[weapon_set][slot][Ammo]));
			else if (this.WeaponSets[weapon_set][slot][Weapon])
				GivePlayerWeapon(playerid, this.WeaponSets[weapon_set][slot][Weapon], this.WeaponSets[weapon_set][slot][Ammo]);
		}
	}
	
	SetPlayerArmedWeapon(playerid, 0);
}
