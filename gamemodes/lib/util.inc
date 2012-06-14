// Buffer used within this file
static stock
	gs_Buffer[256]
;

// Modified version from YSI/y_utils
stock GetIPFromString(const ip[])
{
	new
		ipv = strval(ip) << 24,
		pos = 0;
	while (pos < 15 && ip[pos++] != '.') {}
	ipv += strval(ip[pos]) << 16;
	while (pos < 15 && ip[pos++] != '.') {}
	ipv += strval(ip[pos]) << 8;
	while (pos < 15 && ip[pos++] != '.') {}
	ipv += strval(ip[pos]);
	return ipv;
}

// ispacked for arguments (starting at 0)
stock bool:isargpacked(arg) {
	#emit LOAD.S.pri  arg
	#emit SMUL.C      4
	#emit LOAD.S.alt  0
	#emit ADD
	#emit ADD.C       12
	#emit LOAD.I
	#emit PUSH.pri
	#emit PUSH.C      4
	#emit SYSREQ.C    ispacked
	#emit STACK       8
	#emit RETN
	
	return false;
}

// Change the address of a dimension in an array. The dimension can't be the last one.
stock RedirectArraySlot(array[][], slot, address) {
	#emit LOAD.S.pri  array
	#emit LOAD.S.alt  slot
	#emit SHL.C.alt   2
	#emit ADD
	#emit PUSH.pri
	
	#emit LOAD.S.pri  address
	#emit POP.alt
	#emit SUB
	#emit STOR.I
}

// Safer version, also for packed strings
stock IsNull(const string[]) {
	if (string[0] > 255)
		return string{0} == '\0' || (string[0] & 0xFFFF0000) == 0x01000000;
	else
		return string[0] == '\0' || string[0] == '\1' && string[1] == '\0';
}

#if defined formatex
	// Packed (or unpacked) string
	FormatSpecifier<'S'>(output[], string[])
		strunpack(output, string);
	
	// Lower-case string
	FormatSpecifier<'L'>(output[], string[]) {
		strunpack(output, string);
		
		for (new i = 0, l = strlen(output); i < l; i++)
			output[i] = tolower(output[i]);
	}
	
	// Allows use of formatex in y_va functions.
	#if defined va_args
		stock va_formatex(output[], size = sizeof(output), const fmat[], va_:STATIC_ARGS) {
			new
				num_args,
				arg_start,
				arg_end;
			// Get the pointer to the number of arguments to the last function.
			#emit LOAD.S.pri   0
			#emit ADD.C        8
			#emit MOVE.alt
			// Get the number of arguments.
			#emit LOAD.I
			#emit STOR.S.pri   num_args
			// Get the variable arguments (end).
			#emit ADD
			#emit STOR.S.pri   arg_end
			// Get the variable arguments (start).
			#emit LOAD.S.pri   STATIC_ARGS
			#emit SMUL.C       4
			#emit ADD
			#emit STOR.S.pri   arg_start
			// Using an assembly loop here screwed the code up as the labels added some
			// odd stack/frame manipulation code...
			while (arg_end != arg_start)
			{
				#emit MOVE.pri
				#emit LOAD.I
				#emit PUSH.pri
				#emit CONST.pri    4
				#emit SUB.alt
				#emit STOR.S.pri   arg_end
			}
			// Push the additional parameters.
			#emit PUSH.S       fmat
			#emit PUSH.S       size
			#emit PUSH.S       output
			// Push the argument count.
			#emit LOAD.S.pri   num_args
			#emit ADD.C        12
			#emit LOAD.S.alt   STATIC_ARGS
			#emit XCHG
			#emit SMUL.C       4
			#emit SUB.alt
			#emit PUSH.pri
			#emit MOVE.alt
			// Push the return address.
			#emit LCTRL        6
			#emit ADD.C        28
			#emit PUSH.pri
			// Call formatex
			#emit CONST.pri    formatex
			#emit SCTRL        6
		}
		
		stock strcatf(dest[], maxlength = sizeof(dest), const fmat[], va_args<>) {
			va_formatex(gs_Buffer, _, fmat, va_start<3>);
			
			strcat(dest, gs_Buffer, maxlength);
		}
	#endif
#endif

// Improved version of CallLocalFunction
#if defined GLOBAL_TAG_TYPES
	stock CallLocalFunctionEx(const szFunction[], const szFormat[] = "", GLOBAL_TAG_TYPES:...) {
#else
	stock CallLocalFunctionEx(const szFunction[], const szFormat[] = "", {Float, _}:...) {
#endif
    new
             iFunc,
             iIndex,
             iTemp,
             iNumArgs = strlen(szFormat),
             iArg = iNumArgs,
        bool:bPackedFormat = ispacked(szFormat)
    ;
    
    // Get the function's address
    if (-1 != (iIndex = funcidx(szFunction))) {
        // Load the offset to DAT from the prefix
        #emit LCTRL        1
        
        // Invert it so we have the offset to the prefix from DAT
        #emit NEG
        
        // Copy it to alt for use later
        #emit MOVE.alt
        
        // Add 32 to jump to the offset containing the public function's table
        #emit ADD.C        32
        
        // Read the value there; must be done using LREF because
        // it's outside of the DAT section
        #emit STOR.S.pri   iTemp
        #emit LREF.S.pri   iTemp
        
        // Add the value we just loaded to the prefix (that we stored in alt)
        #emit ADD
        
        // Add index * 8 (each entry contains 2 cells - a pointer to the function's name
        // and a pointer to the function itself, relative to COD).
        #emit LOAD.S.alt   iIndex
        #emit SHL.C.alt    3
        
        // Add that to the offset
        #emit ADD
        
        // Now get the address it's pointing to. This seems to only work
        // using LREF (as opposed to LOAD.I, for example).
        #emit STOR.S.pri   iTemp
        #emit LREF.S.pri   iTemp
        
        // Now store it
        #emit STOR.S.pri   iFunc
    } else {
        return 0;
    }
    
    while (--iArg >= 0) {
        switch (bPackedFormat ? szFormat{iArg} : szFormat[iArg]) {
            // String, array, and variables passed by reference
            case 's', 'a', 'v': {
                // Load the frame pointer
                #emit LCTRL 5
                
                // Add 12 + (2 + iArg) * 4 to get the argument we want
                #emit LOAD.S.alt  iArg
                #emit SHL.C.alt   2
                #emit ADD
                #emit ADD.C       20
                
                // Load the address there
                #emit LOAD.I
                
                // Push that address
                #emit PUSH.pri
            }
            
            // Single-cell arguments passed by value; I used "default"
            // here because it seems that's what CallLocalFunction does.
            default: {
                // Load the frame pointer
                #emit LCTRL 5
                
                // Add 12 + (2 + iArg) * 4 to get the argument we want
                #emit LOAD.S.alt  iArg
                #emit SHL.C.alt   2
                #emit ADD
                #emit ADD.C       20
                
                // Load the address there
                #emit LOAD.I
                
                // Load the value that address points to
                #emit LOAD.I
                
                // Push that value
                #emit PUSH.pri
            }
        }
    }
    
    // Push iNumArgs * 4
    #emit LOAD.S.pri  iNumArgs
    #emit SHL.C.pri   2
    #emit PUSH.pri
    
    // Push the return address
    #emit LCTRL       6
    #emit ADD.C       28
    #emit PUSH.pri
    
    // Call the function
    #emit LOAD.S.pri  iFunc
    #emit SCTRL       6
    
    // Restore the stack
    #emit STACK       24
    
    // Return (pri is the return value, which comes from iFunc)
    #emit RETN
    
    // Never actually happens
    return 0;
}



// compare string hashes first (if any), then the strings
stock bool:strequals(const str1[], const str2[], hash1 = 0, hash2 = 0, bool:ignorecase = false) {
	if (hash1 && hash2 && hash1 != hash2)
		return false;
	
	new
		c1 = (str1[0] > 255) ? str1{0} : str1[0],
		c2 = (str2[0] > 255) ? str2{0} : str2[0]
	;
	
	if (!c1 != !c2)
		return false;
	
	return !strcmp(str1, str2, ignorecase);
}

#define ispacked(%1) \
	((%1)[0] > 255)

stock firstchar(const string[])
	return ispacked(string) ? string{0} : string[0];

enum trim_edges {
	trim_left  = 1,
	trim_right = 2,
	trim_both = trim_left | trim_right
};

stock trim(string[], const chars[] = !"", trim_edges:edges = trim_both) {
	new bool:packed = ispacked(string);
	
	// If "chars" is empty, trim whitespace
	if (!firstchar(chars)) {
		if (edges & trim_left) {
			new i = 0;
			
			if (packed)
				while (0 < string{i} <= ' ') i++;
			else
				while (0 < string[i] <= ' ') i++;
			
			if (i)
				strdel(string, 0, i);
		}
		
		if (edges & trim_right) {
			new i = strlen(string);
			
			if (i) {
				if (packed) {
					while (--i && 0 < string{i} <= ' ') {}
					
					string{i + 1} = '\0';
				} else {
					while (--i && 0 < string[i] <= ' ') {}
					
					string[i + 1] = '\0';
				}
			}
		}
	} else {
		if (edges & trim_left) {
			new i = 0;
			new sub[2];
			
			if (packed) {
				while ((sub[0] = string{i})) {
					if (strfind(chars, sub) == -1)
						break;
					else
						i++;
				}
				
				if (i)
					strdel(string, 0, i);
			} else {
				while ((sub[0] = string[i])) {
					if (strfind(chars, sub) == -1)
						break;
					else
						i++;
				}
				
				if (i)
					strdel(string, 0, i);
			}
		}
		
		if (edges & trim_right) {
			new i = strlen(string);
			new sub[2];
			
			if (i >= 0) {
				if (packed) {
					while (i--) {
						sub[0] = string{i};
						
						if (strfind(chars, sub) == -1)
							break;
					}
					
					string{i + 1} = '\0';
				} else {
					while (i--) {
						sub[0] = string[i];
						
						if (strfind(chars, sub) == -1)
							break;
					}
					
					string[i + 1] = '\0';
				}
			}
		}
	}
}

stock strimplode(const glue[], output[], maxlength = sizeof(output), ...) {
	new args = numargs();
	
	output[0] = '\0';
	
	// Fix for compiler bug, this code is never executed.
	while (output[0]) strcat(output, "", 0);
	
	for (new arg = 3; arg < args; arg++) {
		if (arg != 3)
			strcat(output, glue, maxlength);
		
		// Wrap these in braces or they will be a part of the above if statement
		{
			// Get the address of argument no. <arg>
			#emit LCTRL       5
			#emit ADD.C       12
			#emit LOAD.S.alt  arg
			#emit SHL.C.alt   2
			#emit ADD
			#emit LOAD.I
			
			// Push the maxlength, arg address, and output address
			#emit PUSH.S      maxlength
			#emit PUSH.pri
			#emit PUSH.S      output
			
			// Push the argument count
			#emit PUSH.C      12
			
			// call strcat
			#emit SYSREQ.C    strcat
			
			// Restore the stack
			#emit STACK       16
		}
	}
}