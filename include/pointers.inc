// pointers.inc by Slice

#if !defined MAX_POINTER_INDEX
	#define MAX_POINTER_INDEX  1024
#endif

stock
	g_aaiPointers[1][MAX_POINTER_INDEX]
;

stock PointerByAddress(...) {
	#emit CONST.alt   g_aaiPointers
	#emit LREF.S.pri  12
	#emit SUB
	#emit STOR.I
}

stock PointerByArgument(iArg) {
	#emit LOAD.S.pri  iArg
	#emit SHL.C.pri   2
	#emit LOAD.S.alt  0
	#emit ADD
	#emit ADD.C       12
	#emit LOAD.I
	#emit CONST.alt   g_aaiPointers
	#emit SUB
	#emit STOR.I
}

stock GetVariableAddress(...) {
	#emit LOAD.S.pri  12
	#emit RETN
	
	return 0;
}

stock GetArgumentAddress(iArg) {
	#emit LOAD.S.pri  iArg
	#emit SHL.C.pri   2
	#emit LOAD.S.alt  0
	#emit ADD
	#emit ADD.C       12
	#emit LOAD.I
	#emit RETN
	
	return 0;
}

#define  @ptr[%1] (PointerByAddress (%1), g_aaiPointers[0][0])
#define  @arg[%1] (PointerByArgument(%1), g_aaiPointers[0][0])