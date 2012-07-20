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

		SetSpawnInfo(playerid, NO_TEAM, this.Classes[class][Skin], XYZR(this.SpawnLocations[this.Classes[class][SpawnLocation]]), 0, 0, 0, 0, 0, 0);

		ApplyAnimation(playerid, "ped", "Idle_Gang1", 1.0, true, false, false, true, 1, false);
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
	
	}
}