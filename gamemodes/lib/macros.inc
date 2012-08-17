/*
 * Static variable initialized once with a non-constant value.
 *
 * Example:
 *   static slots =: GetMaxPlayers();
 */
#define static%1=:%2;  static %1;{static _@init;if(!_@init){_@init=1;(%1)=%2;}}

/*
 * Direct access to an y_malloc string.
 *
 * Example:
 *   new Alloc:alloc = <SOMETHING>;
 *   printf("%s", mstr(alloc));
 */
#assert !defined YSI_MALLOC_SECURE
#define mstr(%1)  (YSI_gMallocMemory[_:(%1)])

/*
 * XYZ0(ar) -> ar[0], ar[1], ar[2]
 *
 * Example:
 *   new Float:pos[3];
 *   GetPlayerPos(playerid, XYZ0(pos));
 */
#define XYZ0(%1)  %1[0],%1[1],%1[2]
#define XYZR0(%1)  %1[0],%1[1],%1[2],%1[3]

/*
 * XYZ0(ar) -> ar[X], ar[Y], ar[Z]
 *
 * Same as above, but for enums with keys X, Y, Z, and optionally Rot.
 */
#define XYZ(%1)  %1[X],%1[Y],%1[Z]
#define XYZR(%1)  %1[X],%1[Y],%1[Z],%1[Rot]

/*
 * PUBLIC_ADDRESS("name")
 *
 * Get the address of a public function, using cache.
 */
#define PUBLIC_FUNC(%1)  GetFunctionAddressWithCache(!"\0\0\0\0" %1)

/*
 * Compile-time duration.
 *
 * Example:
 *   DURATION(1 hour, 20 minutes) -> 4800
 */
#define DURATION_MS(%1)     (DURATION(%1) * 1000)
#define DURATION(%1)        (DURATION_PT:%1,0)
#define DURATION_PT:%1,     (%1:DURATION)+_:DURATION_PT:

#define second%1:DURATION
#define seconds%1:DURATION
#define minute%1:DURATION   * DURATION_MINUTE
#define minutes%1:DURATION  * DURATION_MINUTE
#define hour%1:DURATION     * DURATION_HOUR
#define hours%1:DURATION    * DURATION_HOUR
#define day%1:DURATION      * DURATION_DAY
#define days%1:DURATION     * DURATION_DAY
#define week%1:DURATION     * DURATION_WEEK
#define weeks%1:DURATION    * DURATION_WEEK
#define month%1:DURATION    * DURATION_MONTH
#define months%1:DURATION   * DURATION_MONTH
#define year%1:DURATION     * DURATION_YEAR
#define years%1:DURATION    * DURATION_YEAR