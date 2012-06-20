/*!
 * Commands/header.inc
 *
 > Priority: 0
 > Requires: StaticGroups
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
#define YCMD%1:  _pc@();_pC@();_YCMD_0%1:_YCMD_1:_YCMD_2:
#define _pc@();_pC@();_YCMD_0%1:_YCMD_1:_YCMD_2:%2(  public @pc_%2=this.INDEX;stock Pc@_%2(){@pc_%2=@pc_%2;}_pC@();_YCMD_0%1:_YCMD_1:_YCMD_2:%2(
#define _pC@();_YCMD_0(%1):_YCMD_1:_YCMD_2:%2(  @pC_%2();@pC_%2()Commands.SetupPermissions(#%2, %1);_YCMD_0:_YCMD_1:_YCMD_2:%2(

// Allows easily specifying information about a command, later on accessible in this.Description.
#define CommandDescription<%1>%2=%3; \
	@pD_%1();   \
	@pD_%1() {  \
		return ref(%3); \
	}

new
	this.Description[MAX_COMMANDS],
	this.Module[MAX_COMMANDS] = {-1, ...}
;