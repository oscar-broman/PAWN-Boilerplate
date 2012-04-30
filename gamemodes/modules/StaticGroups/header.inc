/*!
 * StaticGroups/header.inc
 *
 > Priority: 9
 */

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