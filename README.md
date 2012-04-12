#<a name="pawn-boilerplate">PAWN Boilerplate</a>

Please note that this project is not yet officially released.

PAWN Boilerplate, or PBP, is a solid base for rapidly developing large gamemodes for SA-MP servers.
Everything is organized into modules, which are very easy to create and get started with.

To make this work, a pre-compiler has been created. The pre-compiler will automatically generate a main file linked to all the modules and their contents.

#<a name="compiling">Compiling</a>

To compile the whole shebang, simply run `compiler/compile.bat` (or `compiler/compile` if you're on *NIX).

Compiling works both on Windows, Linux, and OS X. **Note** that [Wine](http://www.winehq.org/) and [PHP](http://php.net/) is required on *NIX systems (with vcrun2005/2008).

##<a name="wine">Wine</a>
First off you need to install Wine itself. If you're on OS X, the easiest way to do it is by [installing MacPorts](http://www.macports.org/install.php).
Secondly, you'll need vcrun2005 and vcrun2008 to run the compiler. You get these from the command-line tool `winetricks`. Google is your friend. ;)

#<a name="code-structure">Code structure</a>

This will seem quite complex at first, having a look through some of the example modules will hopefully clear things up a little!

The main difference from a plain PAWN script is PBP consist of a module structure.

##<a name="modules">Modules</a>

###<a name="files">Files</a>

The module structure looks like this:

- `ModuleName/`
    - `header.inc`
    - `callbacks.inc`
    - `functions.inc`
    - `commands.inc`
    - `callbacks/`
        - `OnGameModeInit.inc`
        - `OnPlayerConnect.inc`
        - `OnSomethingSomething.inc`

###<a name="header-inc">`header.inc`</a>
This file should contain variable declarations, macros, and such.

###<a name="callbacks-inc">`callbacks.inc`</a>
This file should contain forwards for callbacks (do **not** put public functions here, only forward them). The pre-compiler will scan this file in all modules and add support for the forwarded functions inside.

###<a name="functions-inc">`functions.inc`</a>
This file should contain functions related to the class (creating forwarded, public functions for timers is acceptable).

###<a name="commands-inc">`commands.inc`</a>
This file should contain YCMD commands. You can read about those [here](http://forum.sa-mp.com/showthread.php?t=169029).

###<a name="callbacks-">`callbacks/`</a>
This folder should contain code to be executed inside callbacks. For example, if you create a file called `OnGameModeInit.inc`, any code inside that file will be executed when OnGameModeInit is called by the server.

###<a name="prefixes">Prefixes</a>
Each module will get a prefix with its name followed by a dot. Each module will also have an alias for their prefix - **`this`**.

The main purpose of these prefixes is to allow modules to use the same variable/function names. If Module1 has a global variable called `this.someVariable`, another module could have another global variable also referred to as `this.someVariable`.

Say we have 2 modules, one called ExampleModule and one called AnotherModule. We could do this to communicate between them.

```C++
// In ExampleModule/header.inc
new this.someVariable = 20;

// In AnotherModule/functions.inc
stock this.PrintSomeOtherVariable() {
    printf("%d", ExampleModule.someVariable);
}
```


###<a name="creating-a-module">Creating a module</a>
All you need to do is create a folder inside `gamemodes/modules/`. The name of that folder will be the name of the module. You can rename and delete folders without having to change things anywhere else (unless, ofcourse, other modules need them).


##<a name="callbacks">Callbacks</a>

As you can see in the list above, there's a folder called `callbacks`, containing files named after public functions.

To add code for a callback, simply add a file with the callback's name into the folder.

The PBP compiler will scan the `include` directory for callbacks, as well as all your `callbacks.inc` files. You must have the callback declared in one of those places to get it working

##<a name="file-headers">File headers</a>
When you run the PBP compiler, it will automatically add headers to all module files. These headers are not only informative.

Lines in the header starting with `>` are essentially variables that the PBP compiler will read. 

###<a name="priority">Priority</a>
One of those variables is called `Priority`. If you change its value, it will affect in what order the pre-compiler will include the file.

For example, if you create a callback file called `OnPlayerConnect.inc`, run the PBP compiler, change the priority in the header to `10` then the code inside that file will be the first to execute when a player connects (unless another `OnPlayerConnect.inc` has a higher priority).

You can also use negative values to have the file loaded after other ones.

The recommended range of priority values is -10 - 10.

###<a name="requires">Requires</a>
You can specify which other modules a file depends on (preferably `header.inc`). To do this, simply add a new line in the file header (below ` > Prefix: x`) containing a comma-separated list of other module names (case-sensitive).

##<a name="example-file-header">Example file header</a>
```C++
/*!
 * Groups/header.inc
 *
 > Priority: 10
 > Requires: Core, Admin, Player
 */
```

##<a name="default-modules">Default modules</a>
PBP comes with a few modules by default. If you don't want them, you can simply remove them - the core system does not depend on any of the modules.

###<a name="staticgroups">StaticGroups</a>
The StaticGroups module extends the functionality of y\_commands and y\_groups.

####<a name="creating-static-groups">Creating static groups</a>
You can now declare groups as global variables with the following syntax:
```C++
new StaticGroup<MY\_GROUP> = "My Group";

// In functions and such:
	// Add "playerid" to the group
	Group\_SetPlayer(MY\_GROUP, playerid, true);
```

####<a name="creating-commands-exclusively-for-static-groups">Creating commands exclusively for static groups</a>
Here's how you'd create a command only accessible for players in the `ADMIN` group:
```C++
// In your module's header.inc
new StaticGroup<ADMIN> = "Administrator";

// In your module's commands.inc
YCMD(ADMIN):kick(playerid, params[], help) {
	// ...
}
```