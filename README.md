# PAWN Boilerplate

Please note that this project is not yet officially released.

PAWN Boilerplate, or PBP, is a solid base for rapidly developing large gamemodes for SA-MP servers.
Everything is organized into modules, which are very easy to create and get started with.

To make this work, a pre-compiler has been created. The pre-compiler will automatically generate a main file linked to all the modules and their contents.

Have a look at the **[Wiki](https://github.com/oscar-broman/PAWN-Boilerplate/wiki)** for more information.

# Cool features

Let's be concise.

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

Very easily make variables load/saveable. Plain-text file in `scriptfiles/config.cfg` you can edit and reload via RCON.

```C++
new g_SomeVariable = 50; // 50 will be the default value

// g_SomeVariable will now automatically load and save when the gamemode inits/exits
RegisterConfigVariable<g_SomeVariable>;

// Same here, for the whole array
new g_SomeArray[20];
RegisterConfigArray<g_SomeArray>;
```

## LOTS OF MORE AWESOMENESS