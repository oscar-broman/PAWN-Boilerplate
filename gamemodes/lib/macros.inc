// Static variable initialized once with a non-constant value.
#define static%1=:%2;  static %1;{static _@init;if(!_@init){_@init=1;(%1)=%2;}}

// Direct access to y_malloc string
#assert !defined YSI_MALLOC_SECURE
#define mstr(%1)  (YSI_gMallocMemory[_:(%1)])

// XYZ0(ar) -> ar[0], ar[1], ar[2]
#define XYZ0(%1)  %1[0],%1[1],%1[2]
#define XYZR0(%1)  %1[0],%1[1],%1[2],%1[3]

// XYZ0(ar) -> ar[X], ar[Y], ar[Z]
#define XYZ(%1)  %1[X],%1[Y],%1[Z]
#define XYZR(%1)  %1[X],%1[Y],%1[Z],%1[Rot]