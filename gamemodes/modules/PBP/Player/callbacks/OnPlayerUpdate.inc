/*!
 * Player/callbacks/OnPlayerUpdate.inc
 *
 * OnPlayerUpdate(playerid)
 *
 > Priority: 5
 */

{
	static
		Float:x, Float:y, Float:z,
		vehicleid,
		Float:vx, Float:vy, Float:vz,
		weapon,
		ammo,
		weaponstate,
		money,
		ping,
		special_action,
		camera_mode
	;
	
	GetPlayerPos(playerid, x, y, z);
	(vehicleid = GetPlayerVehicleID(playerid)) && GetVehicleVelocity(vehicleid, vx, vy, vz) || GetPlayerVelocity(playerid, vx, vy, vz);
	GetPlayerFacingAngle(playerid, Player.Info[playerid][Rot]);
	GetPlayerHealth(playerid, Player.Info[playerid][Health]);
	GetPlayerArmour(playerid, Player.Info[playerid][Armour]);
	
	weapon = GetPlayerWeapon(playerid);
	ammo = GetPlayerAmmo(playerid);
	weaponstate = GetPlayerWeaponState(playerid);
	
	Player.Info[playerid][DrunkLevel] = GetPlayerDrunkLevel(playerid);
	money = GetPlayerMoney(playerid);
	
	ping = GetPlayerPing(playerid);
	
	special_action = GetPlayerSpecialAction(playerid);
	
	camera_mode = GetPlayerCameraMode(playerid);
	
	if (x != Player.Info[playerid][X] ||
	    y != Player.Info[playerid][Y] ||
	    z != Player.Info[playerid][Z]) {
	    Player.Info[playerid][X] = x;
	    Player.Info[playerid][Y] = y;
	    Player.Info[playerid][Z] = z;
		
		if (this.CB_OnPlayerPositionUpdate)
			CallFunction(this.CB_OnPlayerPositionUpdate, playerid, x, y, z);
	}
	
	if (vx != Player.Info[playerid][VX] ||
	    vy != Player.Info[playerid][VY] ||
	    vz != Player.Info[playerid][VZ]) {
	    Player.Info[playerid][VX] = vx;
	    Player.Info[playerid][VY] = vy;
	    Player.Info[playerid][VZ] = vz;
		
		if (this.CB_OnPlayerVelocityUpdate)
			CallFunction(this.CB_OnPlayerVelocityUpdate, playerid, vx, vy, vz);
	}
	
	if (weapon != Player.Info[playerid][Weapon]) {
		new previous_weapon = weapon;
		
		Player.Info[playerid][Weapon] = weapon;
		Player.Info[playerid][Ammo  ] = ammo;
		
		if (this.CB_OnPlayerWeaponChange)
			CallFunction(this.CB_OnPlayerWeaponChange, playerid, weapon, previous_weapon);
	} else if (ammo != Player.Info[playerid][Ammo]) {
		new previous_ammo = ammo;
		
		Player.Info[playerid][Ammo] = ammo;
		
		if (this.CB_OnPlayerAmmoChange)
			CallFunction(this.CB_OnPlayerAmmoChange, playerid, ammo, previous_ammo);
	}
	
	if (weaponstate != Player.Info[playerid][WeaponState]) {
		new previous_weaponstate = Player.Info[playerid][WeaponState];
		
		Player.Info[playerid][WeaponState] = weaponstate;
		
		if (this.CB_OnPlayerWeaponStateChg)
			CallFunction(this.CB_OnPlayerWeaponStateChg, playerid, weaponstate, previous_weaponstate);
	}
	
	if (money != Player.Info[playerid][Money]) {
		new previous_money = Player.Info[playerid][Money];
		
		Player.Info[playerid][Money] = money;
		
		if (this.CB_OnPlayerMoneyChange)
			CallFunction(this.CB_OnPlayerMoneyChange, playerid, money, previous_money);
	}
	
	if (ping != Player.Info[playerid][Ping]) {
		new previous_ping = Player.Info[playerid][Ping];
		
		Player.Info[playerid][Ping] = ping;
		
		if (this.CB_OnPlayerPingChange)
			CallFunction(this.CB_OnPlayerPingChange, playerid, ping, previous_ping);
	}
	
	if (special_action != Player.Info[playerid][SpecialAction]) {
		new previous_special_action = Player.Info[playerid][SpecialAction];
		
		Player.Info[playerid][SpecialAction] = special_action;
		
		if (this.CB_OnPlayerSpecialActionChg)
			CallFunction(this.CB_OnPlayerSpecialActionChg, playerid, special_action, previous_special_action);
	}
	
	if (camera_mode != Player.Info[playerid][CameraMode]) {
		new previous_camera_mode = Player.Info[playerid][CameraMode];
		
		Player.Info[playerid][CameraMode] = camera_mode;
		
		if (this.CB_OnPlayerCameraModeChange)
			CallFunction(this.CB_OnPlayerCameraModeChange, playerid, camera_mode, previous_camera_mode);
	}
}