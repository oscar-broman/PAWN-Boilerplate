/*!
 * Player/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 0
 */

{
	SetTimer(#this.CheckForNewAdmins, 0, false);
	
	Class_AddForGroup(
		GROUP_GLOBAL, 0,                 // group, skin id
		1134.8075, -2037.0220, 69.0078,  // x, y, z
		270.0,                           // r
		WEAPON_MP5, 500,                 // weapon, ammo
		WEAPON_ARMOUR, 50                // weapon, ammo, etc.
	);
}