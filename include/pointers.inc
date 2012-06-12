// pointers.inc by Slice

#if !defined MAX_POINTER_INDEX
	#define MAX_POINTER_INDEX  1024
#endif

stock
	g_aaiPointers[1][MAX_POINTER_INDEX]
;

stock PointerByAddress(...) {
	new args = numargs();
	
	if (args == 1) {
		#emit CONST.alt   g_aaiPointers
		#emit LCTRL       5
		#emit ADD.C       12
		#emit LOAD.I
		#emit LOAD.I
		#emit SUB
		#emit STOR.I
	} else {
		new address;
		
		#emit LCTRL       5
		#emit ADD.C       12
		#emit LOAD.I
		#emit LOAD.I
		#emit STOR.S.pri  address
		
		for (new arg = 1; arg < args; arg++) {
			if (arg == 1) {
				#emit LCTRL       5
				#emit ADD.C       16
				#emit LOAD.I
				#emit LOAD.I
				#emit SHL.C.pri   2
				#emit MOVE.alt
				
				#emit LOAD.S.pri  address
				#emit ADD
				#emit PUSH.pri
				#emit LOAD.I
				#emit POP.alt
				#emit ADD
				#emit STOR.S.pri  address
			} else {
				#emit LCTRL       5
				#emit ADD.C       12
				#emit LOAD.S.alt  arg
				#emit SHL.C.alt   2
				#emit ADD
				#emit LOAD.I
				#emit LOAD.I
				#emit SHL.C.pri   2
				#emit LOAD.S.alt  address
				#emit ADD
				#emit STOR.S.pri  address
			}
		}
		
		#emit CONST.alt   g_aaiPointers
		#emit LOAD.S.pri  address
		#emit SUB
		#emit STOR.I
	}
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