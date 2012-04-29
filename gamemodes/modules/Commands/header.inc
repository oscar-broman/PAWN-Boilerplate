/*!
 * Commands/header.inc
 *
 > Priority: 0
 */

#define CommandDescription<%1>%2=%3;      \
	Alloc:@pD_%1();                       \
	Alloc:@pD_%1() {                      \
		return Malloc_NewS(!"" %3, true); \
	}

new
	Alloc:this.Description[MAX_COMMANDS]
;