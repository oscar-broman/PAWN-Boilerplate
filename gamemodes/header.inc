/* ----------------------------------------------------------------------------
    General
   --------------------------------------------------------------------------*/

// Set the stack/heap size to 1mb
#pragma dynamic 262144

// Version info
#define MODE_NAME            "PBP Test Script"
#define SCRIPT_VERSION_MAJOR  0
#define SCRIPT_VERSION_MINOR  3
#define SCRIPT_VERSION_BUILD  0

// Default return values
const PBP.DEFAULT_RETURN<OnPlayerCommandText> = 0;

// Colors used throughout the script
#define COLOR_COMMAND_HELP   0x9DDDDCFF
#define COLOR_COMMAND_ERROR  0xCC2A41FF
#define COLOR_COMMAND_INFO   0xF4FCE8FF
#define COLOR_COMMAND_USAGE  0xFCFEF5FF
#define COLOR_GENERIC_INFO   0x7CB3CDFF

/* ----------------------------------------------------------------------------
    Include files
   --------------------------------------------------------------------------*/

// SA-MP
#include <a_samp>
#include <a_http>

// Set MAX_PLAYERS to the value in server.cfg
#if defined CFG_MAX_PLAYERS
	#undef MAX_PLAYERS
	#define MAX_PLAYERS CFG_MAX_PLAYERS
#endif

// sscanf
#include <sscanf2>

// amx_assembly
#include <amx_assembly\amx_header>
#include <amx_assembly\amx_memory>
#include <amx_assembly\dynamic_call>
#include <amx_assembly\phys_memory>
#include <amx_assembly\profiler>
#include <amx_assembly\stack_dump>

// These macros collide with ones in YSI
#undef r@
#undef w@

// fixes.inc
#include <..\YSI\pawno\include\fixes>

// YSI
#define YSI_NO_X11

#include <..\YSI\pawno\include\YSI\y_commands>
#include <..\YSI\pawno\include\YSI\y_utils>
#include <..\YSI\pawno\include\YSI\y_classes>
#include <..\YSI\pawno\include\YSI\y_colours>
#include <..\YSI\pawno\include\YSI\y_groups>
#include <..\YSI\pawno\include\YSI\y_amx>
#include <..\YSI\pawno\include\YSI\y_debug>
#include <..\YSI\pawno\include\YSI\y_malloc>
#include <..\YSI\pawno\include\YSI\y_iterate>
#include <..\YSI\pawno\include\YSI\y_hooks>

// Misc.
#include <pointers>
#include <sqlitei>
#include <formatex>

#include "lib\macros"
#include "lib\util"
#include "lib\samp"

/* ----------------------------------------------------------------------------
    Add tags to the constants below, without changing their values
   --------------------------------------------------------------------------*/

// Store the values in symbolic constants and add tags
const Text3D:@INVALID_3DTEXT_ID = Text3D:INVALID_3DTEXT_ID;
const   Text:@INVALID_TEXT_DRAW = Text:INVALID_TEXT_DRAW;
const   Menu:@INVALID_MENU      = Menu:INVALID_MENU;

// Undefine the macros then make them refer to the symbolic constants above
#undef  INVALID_3DTEXT_ID
#define INVALID_3DTEXT_ID @INVALID_3DTEXT_ID

#undef  INVALID_TEXT_DRAW
#define INVALID_TEXT_DRAW @INVALID_TEXT_DRAW

#undef  INVALID_MENU
#define INVALID_MENU @INVALID_MENU
