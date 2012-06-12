// Static variable initialized once with a non-constant value.
#define static%1:=%2;  static %1;{static _@init;if(!_@init){_@init=1;%1=%2;}}

// Direct access to y_malloc string
#assert !defined YSI_MALLOC_SECURE
#define mstr(%1)  (YSI_gMallocMemory[_:(%1)])