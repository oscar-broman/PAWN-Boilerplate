/*!
 * ClassSelection/callbacks/OnPlayerRequestClass.inc
 *
 * OnPlayerRequestClass(playerid, classid)
 *
 > Priority: 10
 */

{
	if (this.RequestedClass[playerid] == INVALID_CLASS)
		this.SetFirstAvailableClass(playerid);
	
	if (this.RequestedClass[playerid] != INVALID_CLASS) {
		new class = this.RequestedClass[playerid];
		
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
				new search_class = this.RequestedClass[playerid];
			
				do {
					search_class = (search_class + dir) % this.NumClasses;
				
					if (this.PlayerCanSelectClass(playerid, search_class))
						break;
				} while (search_class != this.RequestedClass[playerid]);
			
				if (search_class != this.RequestedClass[playerid] && this.PlayerCanSelectClass(playerid, search_class))
					class = this.RequestedClass[playerid] = search_class;
			}
		}

		SetPlayerVirtualWorld(playerid, WORLD_CLASS_SELECTION);
		SetPlayerInterior    (playerid, this.Interior);
		SetPlayerPos         (playerid, XYZ0(this.PlayerPos));
		SetPlayerFacingAngle (playerid, this.PlayerRot);
		SetPlayerCameraPos   (playerid, XYZ0(this.CameraPos));
		SetPlayerCameraLookAt(playerid, XYZ0(this.CameraLookAt));
		SetPlayerSkin        (playerid, this.Classes[this.RequestedClass[playerid]][Skin]);

		SetSpawnInfo(playerid, NO_TEAM, this.Classes[class][Skin], XYZR(this.SpawnLocations[this.Classes[class][SpawnLocation]]), 0, 0, 0, 0, 0, 0);

		ApplyAnimation(playerid, "ped", "Idle_Gang1", 1.0, true, false, false, true, 1, false);
	}
}