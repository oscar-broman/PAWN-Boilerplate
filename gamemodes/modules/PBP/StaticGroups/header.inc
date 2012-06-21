/*!
 * PBP/StaticGroups/header.inc
 *
 > Priority: 9
 */

#define StaticGroup<%1>%2= \
	Group:%1;@pG1:@pG2:%1;

#define @pG1:@pG2:%1;%2(%3) \
	@pG_%1(Group:g);@pG_%1(Group:g){%1=g; return ref((%3));}_@pG_more_2:_@pG_more()

#define @pG2:%1;%2"%3" \
	@pG_%1(Group:g);@pG1:@pG_%1(Group:g){%1=g; return @pG1:ref(!#%3);}_@pG_more_2:_@pG_more()

// Replace a trailing comma, if any, with another "new" keyword.
#define _@pG_more()%1, _@pG_nomore(); new 
#define _@pG_more_2:_@pG_more();