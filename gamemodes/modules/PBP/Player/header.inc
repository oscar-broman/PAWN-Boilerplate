/*!
 * PBP/Player/header.inc
 *
 > Priority: 0
 */

enum e_PLAYER_INFO {
	      IP,
	 bool:IsAdmin,
	      Name[MAX_PLAYER_NAME],
	Float:X, Float:Y, Float:Z,
	Float:Rot,
	Float:Health,
	Float:Armour,
	      Weapon,
	      Ammo,
	      WeaponState,
	      DrunkLevel,
	      Money,
	      Ping,
	      Keys,
	      SpecialAction,
	      CameraMode,
	Float:VX, Float:VY, Float:VZ,
	      SurfingVehicle,
	      SurfingObject
};

enum (<<= 1) {
	HP_HEALTH = 1,
	HP_ARMOUR
};

new
	     this.Info[MAX_PLAYERS][e_PLAYER_INFO],
	     this.CB_OnPlayerPositionUpdate,
	     this.CB_OnPlayerVelocityUpdate,
	     this.CB_OnPlayerWeaponChange,
	     this.CB_OnPlayerAmmoChange,
	     this.CB_OnPlayerWeaponStateChg,
	     this.CB_OnPlayerMoneyChange,
	     this.CB_OnPlayerPingChange,
	     this.CB_OnPlayerSpecialActionChg,
	     this.CB_OnPlayerCameraModeChange,
	bool:this.UsePlayerPedAnimations = true
;

RegisterConfigVariable: this.UsePlayerPedAnimations;

// Function forward declarations
forward bool:this.IsInAnyOfGroups(playerid, Group:...);
forward this.CheckForNewAdmins();