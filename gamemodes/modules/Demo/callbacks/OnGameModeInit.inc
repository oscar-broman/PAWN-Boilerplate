/*!
 * Demo/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 0
 */

{
	// Set the gamemode's name to MODE_NAME and version information (from gamemodes/header.inc).
	SetGameModeText(MODE_NAME " v" #SCRIPT_VERSION_MAJOR "." #SCRIPT_VERSION_MINOR "." #SCRIPT_VERSION_BUILD);
	
	// Spawn locations
	new pizza_place, ammu_nation, container;
	
	// Outside of pizza place in Blueberry
	pizza_place = CreateSpawnLocation(
		.interior = 0,
		.x        = 203.7050,
		.y        = -205.3500,
		.z        = 1.5781,
		.rot      = 180.0
	);
	
	// Outside of Ammu-Nation in Blueberry
	ammu_nation = CreateSpawnLocation(
		.interior = 0,
		.x        = 240.0390,
		.y        = -178.2970,
		.z        = 1.5781,
		.rot      = 90.0
	);
	
	// On top of a container in Blueberry
	container = CreateSpawnLocation(
		.interior = 0,
		.x        = 164.0287,
		.y        = -239.0828,
		.z        = 13.4838,
		.rot      = 314.0858
	);
	
	// Pizza and clickin-bell skins
	foreach (new skin : Numbers({155, 167}))
		CreateClass(skin, pizza_place);
	
	// Ammu-nation guy skin
	CreateClass(179, ammu_nation);
	
	// Construction workers
	foreach (new skin : Numbers({27, 260}))
		CreateClass(skin, container);
	
}