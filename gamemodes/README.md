# File structure

## `header.inc`

File inclusions, re-definitions of macros, script name and version information.

This file shouldn't need to be modified. If you want to change any macros
(such as colors), simply re-define them like this:

```cpp
#undef COLOR_COMMAND_HELP
#define COLOR_COMMAND_HELP 0x12345678
```

## `lib`

Generic helpers used throughout the gamemode is inside this folder.

There are 3 includes in this folder:

- `util.inc` - Generic helpers.
- `samp.inc` - Helpers related directly to SA-MP functions.
- `macros.inc` - Generic macros.

## `modules`

This is where the gamemode's code will be, split up into modules.

To create a module, simply add a folder inside `modules` and run the compiler.

There is more information about modules in this Wiki page: [Modules](https://github.com/oscar-broman/PAWN-Boilerplate/wiki/Modules).