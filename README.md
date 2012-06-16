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

## User module

Seamlessly integrates with the rest of your gamemode. Automatically loaded/saved user variables:

*Auto-magically!*

Here's how would keep in track how many times a player has logged in:

```C++
new g_NumTimesConnected[MAX_PLAYERS];

RegisterUserVariable: g_NumTimesConnected;

// OnPlayerLogIn
g_NumTimesConnected[playerid]++;

SendClientMessage(playerid, COLOR_GENERIC_INFO, "* Welcome. You have logged in %d times.", g_NumTimesConnected[playerid]);

// ..that's it!
```

## Text module

### formatex integration

The Text module adds formatex to many native text functions.

Examples of what now can be done:

```C++
new dogs = 2, cats = 3, name[] = "John Doe";

SendClientMessage(playerid, color, "You have %d dogs and %d cats. Your name is %s.", dogs, cats, name);
// Output: You have 2 dogs and 3 cats. Your name is John Doe.

new weapon = GetPlayerWeapon(playerid), modelid = GetVehicleModel(GetPlayerVehicleID(playerid));
SendClientMessage(playerid, color, "You're currently holding %w and driving a vehicle called %v.", weapon, modelid);
// Output: You're currently holding an m4 and driving a vehicle called Infernus.

// OnPlayerDeath:
SendClientMessage(playerid, color, "You were killed by %p using %w.", killerid, reason);
// Output: You were killed by [ABC]SomeGuy using a minigun.
```

### Translations

The Text module also brings an **amazing** system for translating text. All you need to do is use an at-sign before strings (`@"my string"`) and create files in `scriptfiles/languages/XX.lang.inc`.

#### Example

**Your code:**

```C++
SendClientMessage(playerid, color, @"Welcome to the server!");
```

The file **`scriptfiles/languages/sv.lang.inc`:**

When you create this file and run the compile script, it will now look like this:

```C++
"Welcome to the server!" = "Welcome to the server!"
```

Now, simply change the **right part** of the assignment, such as this:

```C++
"Welcome to the server!" = "Välkommen till servern!"
```

If a player has his language set to Swedish, then he will see `Välkommen till servern!` when that client message is sent. 

As always, *auto-magically!*