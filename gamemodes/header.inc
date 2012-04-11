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

// YSI
#include <YSI\y_commands>
#include <YSI\y_utils>
#include <YSI\y_classes>
#include <YSI\y_colors>
#include <YSI\y_groups>
#include <YSI\y_amx>
#include <YSI\y_hooks>

// Misc.
#include <formatex>

#include "lib\util"
#include "lib\samp"
#include "lib\macros"

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
