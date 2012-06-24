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
	    Text:this.TeleportTextDraw[MAX_PLAYERS] = {INVALID_TEXT_DRAW, ...},
	         this.TeleportStart[MAX_PLAYERS],
	BitArray:this.HasTeleported<MAX_PLAYERS>,
	   Float:this.TeleportDuration = 1500.0,
	         this.TeleportTimerStep = 40,
	         this.CB_OnPlayerPositionUpdate,
	         this.CB_OnPlayerVelocityUpdate,
	         this.CB_OnPlayerWeaponChange,
	         this.CB_OnPlayerAmmoChange,
	         this.CB_OnPlayerWeaponStateChg,
	         this.CB_OnPlayerMoneyChange,
	         this.CB_OnPlayerPingChange,
	         this.CB_OnPlayerSpecialActionChg,
	         this.CB_OnPlayerCameraModeChange,
	         this.CB_OnPlayerNameChange,
	    bool:this.UsePlayerPedAnimations = true
;

RegisterConfigVariable: this.UsePlayerPedAnimations;
RegisterConfigVariable: this.TeleportDuration;
RegisterConfigVariable: this.TeleportTimerStep;

// Function forward declarations
forward bool:this.IsInAnyOfGroups(playerid, Group:...);
forward this.CheckForNewAdmins();

forward this.Teleport(playerid, Float:x, Float:y, Float:z, Float:rot = 0.0, interior = -1, world = -1);