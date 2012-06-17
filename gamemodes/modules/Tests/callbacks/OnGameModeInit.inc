/*!
 * Tests/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 0
 */

{
	new weapon_set, spawn;
	
	weapon_set = CreateWeaponSet(
		WEAPON_DEAGLE,  307,
		WEAPON_SHOTGUN, 200,
		WEAPON_M4,      400,
		WEAPON_SNIPER,  150,
		WEAPON_ARMOUR,  50
	);
	
	spawn = CreateSpawnLocation(
		.interior   = 0,
		.world      = 0,
		.x          = 1958.3783,
		.y          = 1343.1572,
		.z          = 15.3746,
		.rot        = 269.1425,
		.weapon_set = weapon_set
	);
	
	foreach (new skin : Numbers({17, 57, 61, 76, 113, 120, 141, 153, 164, 166, 227, 267, 294, 295, 296}))
		CreateClass(skin, spawn);
}