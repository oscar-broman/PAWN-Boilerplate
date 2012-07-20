/*!
 * PBP/ClassSelection/callbacks/OnPlayerRequestClass.inc
 *
 * OnPlayerRequestClass(playerid, classid)
 *
 > Priority: 10
 */

{
	if (this.GroupSetOnSpawn[playerid] != GROUP_GLOBAL) {
		Group_SetPlayer(this.GroupSetOnSpawn[playerid], playerid, false);
		
		this.GroupSetOnSpawn[playerid] = GROUP_GLOBAL;
	}
	
	this.PlayerClass[playerid] = INVALID_CLASS;
	
	if (this.RequestedClass[playerid] == INVALID_CLASS)
		SetFirstAvailableClass(playerid);
	
	new class = this.RequestedClass[playerid];
	
	if (class != INVALID_CLASS) {
		Bit_Let(this.PlayerHasRequestedClass, playerid);
		
		if (0 <= classid < 3) {
			new dir;
		
			switch (this.PreviousClassid{playerid} << 4 | classid) {
				case 0x00, 0x11, 0x22: dir =  0; // No direction
				case 0x01, 0x12, 0x20: dir =  1;
				case 0x10, 0x21, 0x02: dir = -1;
			}
		
			this.PreviousClassid{playerid} = classid;
		
			if (dir) {
				new search_class = class;
			
				do {
					search_class = (search_class + dir) % this.NumClasses;
				
					if (PlayerCanSelectClass(playerid, search_class))
						break;
				} while (search_class != class);
			
				if (search_class != class && PlayerCanSelectClass(playerid, search_class))
					class = this.RequestedClass[playerid] = search_class;
			}
		}
		
		new spawn = this.Classes[class][SpawnLocation];

		SetPlayerVirtualWorld(playerid, WORLD_CLASS_SELECTION);
		SetPlayerSkin        (playerid, this.Classes[class][Skin]);

		SetSpawnInfo(playerid, NO_TEAM, this.Classes[class][Skin], XYZR(this.SpawnLocations[spawn]), 0, 0, 0, 0, 0, 0);
		
		if (this.ShowsSpawn) {
			new Float:x, Float:y, Float:rot;
			
			rot = this.SpawnLocations[spawn][Rot];
			
			x = this.SpawnLocations[spawn][X] + (this.CameraDistance * floatsin(-rot, degrees));
			y = this.SpawnLocations[spawn][Y] + (this.CameraDistance * floatcos(-rot, degrees));
			
			SetPlayerCameraPos   (playerid, x, y, this.SpawnLocations[spawn][Z] + this.CameraDistanceZ);
			SetPlayerCameraLookAt(playerid, XYZ(this.SpawnLocations[spawn]));
			SetPlayerPos         (playerid, XYZ(this.SpawnLocations[spawn]));
			SetPlayerInterior    (playerid, this.SpawnLocations[spawn][Interior]);
			SetPlayerFacingAngle (playerid, rot);
		} else {
			SetPlayerCameraPos   (playerid, XYZ0(this.CameraPos));
			SetPlayerCameraLookAt(playerid, XYZ0(this.CameraLookAt));
			SetPlayerPos         (playerid, XYZ0(this.PlayerPos));
			SetPlayerInterior    (playerid, this.Interior);
			SetPlayerFacingAngle (playerid, this.PlayerRot);
		}
	}
	
	TextDrawDestroyVar(this.WeaponsInfo[playerid]);
	
	new weapon_set = INVALID_WEAPON_SET;
	
	if (this.Classes[class][WeaponSet] != INVALID_WEAPON_SET)
		weapon_set = this.Classes[class][WeaponSet];
	else if (this.SpawnLocations[this.Classes[class][SpawnLocation]][WeaponSet] != INVALID_WEAPON_SET)
		weapon_set = this.SpawnLocations[this.Classes[class][SpawnLocation]][WeaponSet];
	
	if (weapon_set != INVALID_WEAPON_SET) {
		static weapon_text[256];
		
		weapon_text = "";
		
		new lines = this.MAX_WEAPON_SLOT;
		
		for (new slot = 0; slot < this.MAX_WEAPON_SLOT; slot++) {
			if (this.WeaponSets[weapon_set][slot][Weapon] == WEAPON_ARMOUR)
				strcatf(weapon_text, _, "%@~n~", @"Armour");
			else if (true||this.WeaponSets[weapon_set][slot][Weapon])
				strcatf(weapon_text, _, "%W~n~", this.WeaponSets[weapon_set][slot][Weapon]);
			else
				lines--;
		}
		
		if (!isnull(weapon_text)) {
			this.WeaponsInfo[playerid] = TextDrawCreate(132.0, 206.0, weapon_text);
		
			TextDrawSetShadow(this.WeaponsInfo[playerid], 0);
			TextDrawSetOutline(this.WeaponsInfo[playerid], 1);
			TextDrawBackgroundColor(this.WeaponsInfo[playerid], 0x00000022);
			TextDrawUseBox(this.WeaponsInfo[playerid], true);
			TextDrawBoxColor(this.WeaponsInfo[playerid], 0x00000055);
			TextDrawTextSize(this.WeaponsInfo[playerid], 270.0, 0.0);
			
			TextDrawShowForPlayer(playerid, this.WeaponsInfo[playerid]);
		}
	}
}