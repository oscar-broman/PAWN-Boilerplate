#PAWN Boilerplate

Please note that this project is not yet officially released.

PAWN Boilerplate, or PBP, is a solid base for rapidly developing large gamemodes for SA-MP servers.
Everything is organized into modules, which are very easy to create and get started with.

To make this work, a pre-compiler has been created. The pre-compiler will automatically generate a main file linked to all the modules and their contents.

#Compiling

To compile the whole shebang, simply run `compiler/compile.bat` (or `compiler/compile` if you're on *NIX).

Compiling works both on Windows, Linux, and OS X. **Note** that [Wine](http://www.winehq.org/) and [PHP](http://php.net/) is required on *NIX systems (with vcrun2005/2008).

##Wine
First off you need to install Wine itself. If you're on OS X, the easiest way to do it is by [installing MacPorts](http://www.macports.org/install.php).
Secondly, you'll need vcrun2005 and vcrun2008 to run the compiler. You get these from the command-line tool `winetricks`. Google is your friend. ;)

#Code structure

This will seem quite complex at first, having a look through some of the example modules will hopefully clear things up a little!

The main difference from a plain PAWN script is PBP consist of a module structure.

##Modules

###Files

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

###`header.inc`
This file should contain variable declarations, macros, and such.

###`callbacks.inc`
This file should contain forwards for callbacks (do **not** put public functions here, only forward them). The pre-compiler will scan this file in all modules and add support for the forwarded functions inside.

###`functions.inc`
This file should contain functions related to the class (creating forwarded, public functions for timers is acceptable).

###`commands.inc`
This file should contain YCMD commands. You can read about those [here](http://forum.sa-mp.com/showthread.php?t=169029).

###`callbacks/`
This folder should contain code to be executed inside callbacks. For example, if you create a file called `OnGameModeInit.inc`, any code inside that file will be executed when OnGameModeInit is called by the server.

###Prefixes
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


###Creating a module
All you need to do is create a folder inside `gamemodes/modules/`. The name of that folder will be the name of the module. You can rename and delete folders without having to change things anywhere else (unless, ofcourse, other modules need them).


##Callbacks

As you can see in the list above, there's a folder called `callbacks`, containing files named after public functions.

To add code for a callback, simply add a file with the callback's name into the folder.

The PBP compiler will scan the `include` directory for callbacks, as well as all your `callbacks.inc` files. You must have the callback declared in one of those places to get it working

##File headers
When you run the PBP compiler, it will automatically add headers to all module files. These headers are not only informative.

Lines in the header starting with `>` are essentially variables that the PBP compiler will read. 

###Priority
One of those variables is called `Priority`. If you change its value, it will affect in what order the pre-compiler will include the file.

For example, if you create a callback file called `OnPlayerConnect.inc`, run the PBP compiler, change the priority in the header to `10` then the code inside that file will be the first to execute when a player connects (unless another `OnPlayerConnect.inc` has a higher priority).

You can also use negative values to have the file loaded after other ones.
