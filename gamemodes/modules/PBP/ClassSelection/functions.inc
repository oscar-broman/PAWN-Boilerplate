/*!
 * PBP/ClassSelection/functions.inc
 *
 > Priority: 0
 */

stock bool:IsValidWeaponSet(ws) {
	return (ws == INVALID_WEAPON_SET || 0 <= ws < this.NumWeaponSets);
}

stock bool:IsValidSpawnLocation(spawn) {
	return (spawn == INVALID_SPAWN_LOCATION || 0 <= spawn < this.NumSpawnLocations);
}

stock bool:IsValidClass(class) {
	return (class == INVALID_CLASS || 0 <= class < this.NumClasses);
}

stock CreateWeaponSet(...) {
	if (this.NumWeaponSets >= this.MAX_WEAPON_SETS) {
		printf("(ClassSelection.CreateWeaponSet) ERROR: Exceeded this.MAX_WEAPON_SETS.");
		
		return INVALID_WEAPON_SET;
	}
	
	new args = numargs();
	
	if (args & 0b1)
		printf("(ClassSelection.CreateWeaponSet) ERROR: Invalid number of arguments given to ClassSelection.CreateWeaponSet. Expecting (weapon,ammo,...).");
	else {
		new id = this.NumWeaponSets++;
		
		for (new arg = 0; arg < args; arg += 2) {
			new slot;
			
			if (@arg[arg] == WEAPON_ARMOUR)
				slot = this.MAX_WEAPON_SLOT - 1;
			else
			 	slot = GetWeaponSlot(@arg[arg]);
			
			if (slot == WEAPONSLOT_NONE)
				continue;
			
			this.WeaponSets[id][slot][Weapon] = @arg[arg];
			this.WeaponSets[id][slot][Ammo] = @arg[arg + 1];
		}
		
		return id;
	}
	
	return INVALID_WEAPON_SET;
}

stock CreateSpawnLocation(interior = 0, world = 0, Float:x, Float:y, Float:z, Float:rot, Group:group = GROUP_GLOBAL, team = INVALID_TEAM, weapon_set = INVALID_WEAPON_SET) {
	if (this.NumSpawnLocations >= this.MAX_SPAWN_LOCATIONS) {
		printf("(ClassSelection.CreateSpawnLocation) ERROR: Exceeded this.MAX_SPAWN_LOCATIONS.");
		
		return INVALID_SPAWN_LOCATION;
	}
	
	if (!IsValidWeaponSet(weapon_set)) {
		printf("(ClassSelection.CreateSpawnLocation) WARNING: Invalid weapon set given.");
		
		weapon_set = INVALID_WEAPON_SET;
	}
	
	new id = this.NumSpawnLocations++;

	this.SpawnLocations[id][Interior]  = interior;
	this.SpawnLocations[id][World]     = world;
	this.SpawnLocations[id][X]         = x;
	this.SpawnLocations[id][Y]         = y;
	this.SpawnLocations[id][Z]         = z;
	this.SpawnLocations[id][Rot]       = rot;
	this.SpawnLocations[id][Group]     = group;
	this.SpawnLocations[id][Team]      = team;
	this.SpawnLocations[id][WeaponSet] = weapon_set;
	
	return id;
}

stock CreateClass(skin, spawn_location, weapon_set = INVALID_WEAPON_SET, world = 0, Group:group = GROUP_GLOBAL, team = INVALID_TEAM) {
	if (this.NumClasses >= this.MAX_CLASSES) {
		printf("(ClassSelection.CreateClass) ERROR: Exceeded this.MAX_CLASSES.");
		
		return INVALID_CLASS;
	}
	
	if (!IsValidSpawnLocation(spawn_location) || spawn_location == INVALID_SPAWN_LOCATION) {
		printf("(ClassSelection.CreateClass) Invalid spawn location given.");
		
		return INVALID_CLASS;
	}

	if (!IsValidWeaponSet(weapon_set)) {
		printf("(ClassSelection.CreateClass) WARNING: Invalid weapon set given.");

		weapon_set = INVALID_WEAPON_SET;
	}
	
	new id = this.NumClasses++;
	
	this.Classes[id][Skin]          = skin;
	this.Classes[id][SpawnLocation] = spawn_location;
	this.Classes[id][WeaponSet]     = weapon_set;
	this.Classes[id][World]         = world;
	this.Classes[id][Group]         = group;
	this.Classes[id][Team]          = team;
	this.Classes[id][Available] 	= true;
	
	return id;
}

stock setClassAvailable(class, bool:available) {
	this.Classes[class][Available] = available;
}

stock bool:PlayerCanSelectClass(playerid, class) {
	#pragma unused playerid
	if(this.Classes[class][Available] == false) return this.Classes[class][Available];
	return (0 <= class < this.NumClasses);
}

stock bool:SetFirstAvailableClass(playerid) {
	this.RequestedClass[playerid] = INVALID_CLASS;
	
	for (new i = 0; i < this.NumClasses; i++) {
		if (PlayerCanSelectClass(playerid, i)) {
			this.RequestedClass[playerid] = i;
			
			break;
		}
	}
	
	if (this.RequestedClass[playerid] == INVALID_CLASS)
		printf("(ClassSelection.SetFirstAvailableClass) ERROR: No classes available for player %d.", playerid);
}

stock GetPlayerClass(playerid) {
	return this.PlayerClass[playerid];
}