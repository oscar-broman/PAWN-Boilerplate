/*!
 * Groups/header.inc
 *
 > Priority: 10
 */

// Make sure the YCMD define is what we expect it to be
#if !defined YCMD
	#error y_commands is required.
#endif

static stock const YCMD_DEF_TEST[] = #YCMD:;

#if sizeof(YCMD_DEF_TEST) != 25
	#error The YCMD macro has been modified.
#endif

// Re-define YCMD to support the new syntax, while keeping support for the normal one.
#undef YCMD
#define YCMD%1:  _pC@();_YCMD_0%1:_YCMD_1:_YCMD_2:
#define _pC@();_YCMD_0(%1):_YCMD_1:_YCMD_2:%2(  @pC_%2();@pC_%2()return _:_@pG:%1;_YCMD_0:_YCMD_1:_YCMD_2:%2(
#define _@pG:                  _@pG1:_@pG2:
#define _@pG1:_@pG2:%1,  %1 | _:_@pG:

enum e_STATIC_GROUPS {
	      Name[MAX_GROUP_NAME],
	Group:Group
};

// This enum must be in sync with the array below
enum (<<= 1) {
	SUPER_ADMIN = 1,
	ADMIN,
	MODERATOR,
	VIP,
	
	this.STATIC_GROUPS
};

new
	this.StaticGroups[][e_STATIC_GROUPS] = {
		{"Super Admin", Group:0},
		{"Admin"      , Group:0},
		{"Moderator"  , Group:0},
		{"VIP"        , Group:0}
	}
;

// If this fails, the above array is not in sync with the enum
#assert _:this.STATIC_GROUPS == 1 << sizeof(this.StaticGroups)