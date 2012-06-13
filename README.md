# PAWN Boilerplate

Please note that this project is not yet officially released.

PAWN Boilerplate, or PBP, is a solid base for rapidly developing large gamemodes for SA-MP servers.
Everything is organized into modules, which are very easy to create and get started with.

To make this work, a pre-compiler has been created. The pre-compiler will automatically generate a main file linked to all the modules and their contents.

Have a look at the **[Wiki](https://github.com/oscar-broman/PAWN-Boilerplate/wiki)** for more information.

# Cool features

## Scripting made easy

PBP contains many mature, well-tested libraries and plugins, which are used extensively throughout the system.

### Libraries bundled

* **YSI** by Y_Less
* **fixes.inc** by Y_Less
* **amx_assembly** by Zeex
* **formatex** by Slice
* **pointers.inc** by Slice
* **SQLite Improved** by Slice

### Plugins bundled

* **sscanf** by Y_Less
* **whirlpool** by Y_Less
* **crashdetect** by Zeex

## Commands

PBP uses YCMD, though with some extensions:

```C++
// This will appear in /commands
CommandDescription<quit_race> = "Quit the race you're currently in.";

// This command is only available for players in the group GROUP_RACERS
YCMD(GROUP_RACERS):quit_race(playerid, params[], help) {
	// <quit race code here>
}
```

## Configuration variables

Very easily make variables save/load with the gamemode.

```C++
new g_SomeVariable = 50; // 50 will be the default value
new g_SomeArray[20];

// These variables now automatically loads and saves when the gamemode inits/exits
RegisterConfigVariable: g_SomeVariable;
RegisterConfigArray: g_SomeArray;
```

Config values can be changed while the server is running with the following RCON commands:

* Reload all config data from `scriptfiles/config.cfg`: `/rcon config_reload`
* Change the value of a specific config variable: `/rcon config_set g_SomeVariable 123`

## User system

Seamlessly integrates with the rest of your gamemode. Automatically loaded/saved user variables:

This code will make `g_SomePlayerVariable` load from the database when a player logs in, then save the new value when the player disconnects.

*Auto-magically!*

```C++
new g_SomePlayerVariable[MAX_PLAYERS];

RegisterUserVariable: g_SomePlayerVariable;
```