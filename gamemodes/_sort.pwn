#include <a_samp>

#define IsPlayerConnected(%1) \
	(((%1) / 5) % 2)

#include "..\include\md-sort\md-sort"

#undef MAX_PLAYERS
#define MAX_PLAYERS 20

enum E_TEST_DATA {
	      SomeInt,
	Float:SomeFloat
};

main() {
	new start = GetTickCount();
	
	for (new i = 0; i < 100000; i++) {
		new
			test_data[MAX_PLAYERS][E_TEST_DATA] = {
				{0,  -4448.310},
				{1,   8968.814},
				{2,  -7118.409},
				{3,  -8093.300},
				{4,   7057.550},
				{5,  -6370.720},
				{6,  -2655.949},
				{7,   2999.330},
				{8,   5157.266},
				{9,  -9994.338},
				{10,  9897.915},
				{11,  5025.210},
				{12, -8302.658},
				{13,  9612.310},
				{14,  8035.475},
				{15, -9574.874},
				{16,  -244.525},
				{17, -8121.103},
				{18,  1188.960},
				{19,  9251.628}
			}
		;
	
		//new sorted_into[MAX_PLAYERS];
	
		SortArrayUsingComparator(test_data, CompareTest);// => sorted_into;
	}
	
	printf("took %dms", GetTickCount() - start);
}

Comparator:CompareTest(left[E_TEST_DATA], right[E_TEST_DATA]) {
	//return right[SomeInt] - left[SomeInt];
    return floatcmp(right[SomeFloat], left[SomeFloat]);
}