/*!
 * StaticGroups/header.inc
 *
 > Priority: 9
 */

// Make sure the YCMD define is what we expect it to be.
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
#define _pC@();_YCMD_0(%1):_YCMD_1:_YCMD_2:%2(  @pC_%2();@pC_%2()StaticGroups.SetCommandPermissions(#%2, %1);_YCMD_0:_YCMD_1:_YCMD_2:%2(

// Macros for creation declaration of static groups
#define StaticGroup<%1>%2=%3"%4" \
	Group:%1; \
	@pG_%1(name[], Group:group); \
	@pG_%1(name[], Group:group) { \
		strpack(name, #%4, MAX_GROUP_NAME char); \
		(%1 = group) && %1; \
	}_@pG_more_2:_@pG_more()

// Replace a trailing comma, if any, with another "new" keyword.
#define _@pG_more()%1, _@pG_nomore(); new 
#define _@pG_more_2:_@pG_more();