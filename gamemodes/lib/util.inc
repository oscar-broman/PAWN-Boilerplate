// Functions in this file, not including format specifiers.

/*
 * Get an IP as an integer.
 * 
 * Modified version from YSI/y_utils
 */
stock GetIPFromString(const ip[]);

/*
 * Change the address of a dimension in an array.
 * 
 * The 2nd last dimension will be modified.
 *
 * RedirectArraySlot(array[][], slot, address)
 */
stock RedirectArraySlot(...);

/*
 * Get the address to an alloc slot's memory.
 */
stock Malloc_GetAddress(Alloc:slot);

/*
 * Get an alloc slot from a memory address.
 *
 * Returns NO_ALLOC if the address isn't within the y_malloc space.
 */
stock Alloc:Malloc_FromAddress(address);

/*
 * Safer version, also for packed strings.
 */
stock IsNull(const string[]);

/*
 * formatex for y_va functions.
 */
stock va_formatex(output[], size = sizeof(output), const fmat[], va_:STATIC_ARGS);

/*
 * strcat with formatex functionality.
 */
stock strcatf(dest[], maxlength = sizeof(dest), const fmat[], va_args<>);

/*
 * Improved version of CallLocalFunction.
 *
 * Supports strings and arrays better. Variables can be passed by reference (format specifier "v").
 */
stock CallLocalFunctionEx(const function[], const fmat[] = "", GLOBAL_TAG_TYPES:...);

/*
 * Iterate an array of numbers.
 *
 * Can contain any numbers but -1.
 */
stock Numbers@YSII_Ag(const numbers[], start, size = sizeof(numbers));

/*
 * Destroy a text draw and set the variable to INVALID_TEXT_DRAW.
 */
stock TextDrawDestroyVar(&Text:text);

/*
 * Get the address of a function, using the first 4 bytes of the input string as cache.
 *
 * The input string must be "\x00000000;CallbackName".
 */
stock GetFunctionAddressWithCache(const funcname[]);

/*
 * Convert a human-readable duration into seconds.
 * Examples of valid input:
 *   "2 weeks 5 days"
 *   "1 month and 3 weeks"
 *   "1 year -2 months" (10 months)
 *   "1y2m"
 *   "1hr20min"
 */
stock GetDurationFromString(const input[]);

/*
 * Opposite of the above.
 */
stock GetStringFromDuration(duration);

// Buffer used within this file
static stock
	gs_Buffer[256]
;

stock GetIPFromString(const ip[]) {
	new
		ipv = strval(ip) << 24,
		pos = 0
	;
	
	while (pos < 15 && ip[pos] != '.') pos++;
	ipv += strval(ip[pos]) << 16;
	
	while (pos < 15 && ip[pos] != '.') pos++;
	ipv += strval(ip[pos]) << 8;
	
	while (pos < 15 && ip[pos] != '.') pos++;
	ipv += strval(ip[pos]);
	
	return ipv;
}

stock RedirectArraySlot(...) {
	#emit LOAD.S.pri  12
	#emit LREF.S.alt  16
	#emit SHL.C.alt   2
	#emit ADD
	#emit PUSH.pri
	
	#emit LREF.S.pri  20
	#emit POP.alt
	#emit SUB
	#emit STOR.I
}

stock Malloc_GetAddress(Alloc:slot) {
	new address = 0;
	
	if (0 <= _:slot < sizeof(YSI_gMallocMemory)) {
		#emit CONST.pri   YSI_gMallocMemory
		#emit LOAD.S.alt  slot
		#emit SHL.C.alt   2
		#emit ADD
		#emit STOR.S.pri  address
	}
	
	return address;
}

stock Alloc:Malloc_FromAddress(address) {
	new Alloc:slot = NO_ALLOC;
	
	static min_address =: GetVariableAddress(YSI_gMallocMemory);
	static max_address =: GetVariableAddress(YSI_gMallocMemory) + (sizeof(YSI_gMallocMemory) - 1) * 4;
	
	if (min_address < address <= max_address)
		slot = Alloc:((address - min_address) / 4);
	
	return slot;
}

stock IsNull(const string[]) {
	if (string[0] > 255)
		return string{0} == '\0' || (string[0] & 0xFFFF0000) == 0x01000000;
	else
		return string[0] == '\0' || string[0] == '\1' && string[1] == '\0';
}

stock va_formatex(output[], size = sizeof(output), const fmat[], va_:STATIC_ARGS) {
	new
		num_args,
		arg_start,
		arg_end
	;
	
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

stock CallLocalFunctionEx(const function[], const fmat[] = "", GLOBAL_TAG_TYPES:...) {
    new
             func,
             idx,
             temp,
             args = strlen(fmat),
             arg = args,
        bool:packed_format = ispacked(fmat)
    ;
    
    // Get the function's address
    if (-1 != (idx = funcidx(function))) {
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
        #emit STOR.S.pri   temp
        #emit LREF.S.pri   temp
        
        // Add the value we just loaded to the prefix (that we stored in alt)
        #emit ADD
        
        // Add index * 8 (each entry contains 2 cells - a pointer to the function's name
        // and a pointer to the function itself, relative to COD).
        #emit LOAD.S.alt   idx
        #emit SHL.C.alt    3
        
        // Add that to the offset
        #emit ADD
        
        // Now get the address it's pointing to. This seems to only work
        // using LREF (as opposed to LOAD.I, for example).
        #emit STOR.S.pri   temp
        #emit LREF.S.pri   temp
        
        // Now store it
        #emit STOR.S.pri   func
    } else {
        return 0;
    }
    
    while (--arg >= 0) {
        switch (packed_format ? fmat{arg} : fmat[arg]) {
            // String, array, and variables passed by reference
            case 's', 'a', 'v': {
                // Load the frame pointer
                #emit LCTRL 5
                
                // Add 12 + (2 + arg) * 4 to get the argument we want
                #emit LOAD.S.alt  arg
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
                
                // Add 12 + (2 + arg) * 4 to get the argument we want
                #emit LOAD.S.alt  arg
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
    
    // Push args * 4
    #emit LOAD.S.pri  args
    #emit SHL.C.pri   2
    #emit PUSH.pri
    
    // Push the return address
    #emit LCTRL       6
    #emit ADD.C       28
    #emit PUSH.pri
    
    // Call the function
    #emit LOAD.S.pri  func
    #emit SCTRL       6
    
    // Restore the stack
    #emit STACK       24
    
    // Return (pri is the return value, which comes from func)
    #emit RETN
    
    // Never actually happens
    return 0;
}

stock Numbers@YSII_Ag(const numbers[], start, size = sizeof(numbers)) {
	if (!size)
		return -1;
	
	if (start == -1)
		return numbers[0];
	
	if (start == numbers[size - 1])
		return -1;
	
	for (new i = 0; i < size; i++) {
		if (numbers[i] == start)
			return numbers[i + 1];
	}
	
	return -1;
}

stock TextDrawDestroyVar(&Text:text) {
	if (_:text != _:INVALID_TEXT_DRAW) {
		TextDrawDestroy(text);
		
		text = INVALID_TEXT_DRAW;
	}
}

stock GetFunctionAddressWithCache(const funcname[]) {
	new address;
	
	address = funcname[0];
	
	if (address == 0) {
		new idx;
		
		if (-1 != (idx = funcidx(funcname[1]))) {
			#emit LCTRL        1
			#emit NEG
			#emit MOVE.alt
			#emit ADD.C        32
			#emit STOR.S.pri   address
			#emit LREF.S.pri   address
			#emit ADD
			#emit LOAD.S.alt   idx
			#emit SHL.C.alt    3
			#emit ADD
			#emit STOR.S.pri   address
			#emit LREF.S.pri   address
			#emit STOR.S.pri   address
			#emit LOAD.S.alt   funcname
			#emit STOR.I
		}
	}
	
	return address;
}

enum {
	DURATION_SECOND = 1,
	DURATION_MINUTE = 60 * DURATION_SECOND,
	DURATION_HOUR   = 60 * DURATION_MINUTE,
	DURATION_DAY    = 24 * DURATION_HOUR,
	DURATION_WEEK   = 7 * DURATION_DAY,
	DURATION_MONTH  = 30 * DURATION_DAY,
	DURATION_YEAR   = 365 * DURATION_DAY,
	
	DURATION_PERMANENT = 0,
	DURATION_EXPIRED   = -1
};

enum E_DURATION_MULTIPLIER {
	Word[11 char],
	Multiplier
};

stock const
	g_DurationMultipliers[][E_DURATION_MULTIPLIER] = {
		{!"s",      DURATION_SECOND},
		{!"sec",    DURATION_SECOND},
		{!"second", DURATION_SECOND},
		
		{!"m",      DURATION_MINUTE},
		{!"min",    DURATION_MINUTE},
		{!"minute", DURATION_MINUTE},

		{!"h",      DURATION_HOUR},
		{!"hr",     DURATION_HOUR},
		{!"hour",   DURATION_HOUR},

		{!"d",      DURATION_DAY},
		{!"day",    DURATION_DAY},

		{!"w",      DURATION_WEEK},
		{!"week",   DURATION_WEEK},

		{!"month",  DURATION_MONTH},

		{!"y",      DURATION_YEAR},
		{!"yr",     DURATION_YEAR},
		{!"year",   DURATION_YEAR},
		
		{!"decade",     10 * DURATION_YEAR},
		{!"century",    100 * DURATION_YEAR},
		{!"millennium", 1000 * DURATION_YEAR}
	}
;

stock GetDurationFromString(const input[]) {
	static
		buf[256]
	;
	
	new
		     pos,
		     duration = 0,
		     value,
		     word[8],
		     i,
		     length,
		bool:multiplier_found,
		     temp
	;
	
	// Zero-out the buffer and use strcat to copy the contents of input onto it
	buf[0] = 0;
	
	strcat(buf, input);
	
	// Remove "and" words
	while (-1 != (pos = strfind(buf, "and", true, pos++)))
	strdel(buf, pos, pos + 3);
	
	// Convert commas to spaces
	while (-1 != (pos = strfind(buf, ",", true, pos++)))
	buf[pos] = ' ';
	
	// Convert ampersands to spaced
	while (-1 != (pos = strfind(buf, "&", true, pos++)))
	buf[pos] = ' ';
	
	pos = 0;
	
	// Fix for strval ignoring long strings
	if (strlen(buf[pos]) > 32) {
		temp = buf[pos + 32];
	
		buf[pos + 32] = 0;
	}
	
	// The current position in the string (pos) should now be at the first number
	while (0 != (value = strval(buf[pos]))) {
		if (temp) {
			buf[pos + 32] = temp;
	
			temp = 0;
		}
	
		// Skip spaces (no need to do this before strval as it does it as well)
		while (0 < buf[pos] <= ' ') pos++;
	
		// EOS check
		if (!buf[pos]) return 0;
	
		// Allow a leading minus sign
		if (buf[pos] == '-') pos++;
	
		// Skip the number we just put into strval
		while ('0' <= buf[pos] <= '9') pos++;
	
		// Skip spaces
		while (0 < buf[pos] <= ' ') pos++;
	
		// EOS check
		if (!buf[pos]) return 0;
	
		// Start of the multiplier (hour, second, etc.)
		i = pos;
	
		// Loop until the next space or number
		while (buf[i] > ' ' && !('0' <= buf[i] <= '9') && buf[i] != '-') i++;
	
		// No word? Invalid format
		if (i == pos) return 0;
	
		// Zero-out word and use strcat to copy the fragment we just found from buf
		word[0] = 0;
	
		strcat(word, buf[pos], i - pos + 1);
	
		// Jump the current parsing position to the end of the word
		pos = i;
	
		// Null check
		if (!(length = strlen(word))) return 0;
	
		// Remove trailing s so we won't have to search after, for example, both "hour" and "hours"
		if (length > 1 && tolower(word[length - 1]) == 's')
		word[length - 1] = 0;
	
		// Loop through the duration multiplier list
	
		multiplier_found = false;
	
		for (i = 0; i < sizeof(g_DurationMultipliers); i++) {
			if (!strcmp(word, g_DurationMultipliers[i][Word], true)) {
				multiplier_found = true;
	
				// Multiply the number we got from strval with whatever this multiplier has
				duration += value * g_DurationMultipliers[i][Multiplier];
	
				break;
			}
		}
	
		// Give a warning  if a multiplier wasn't found
		if (!multiplier_found)
			printf("WARNING: Unknown multiplier in GetDurationFromString (\"%s\").", word);
	
		// Fix for strval ignoring long strings
		if (strlen(buf[pos]) > 32) {
			temp = buf[pos + 32];
	
			buf[pos + 32] = 0;
		}
	}
	
	return duration;
}

stock GetStringFromDuration(duration) {
	new
		minutes = 0,
		hours   = 0,
		days    = 0,
		weeks   = 0,
		months  = 0,
		years   = 0,

		length,
		pos,
		last_pos
	;

	static
		buf[256]
	;

	buf[0] = 0;

	years = duration / DURATION_YEAR;
	duration = duration % DURATION_YEAR;

	if (years) {
		format(buf, sizeof(buf), years == 1 ? ("%d year, ") : ("%d years, "), years);
	}

	months   = duration / DURATION_MONTH;
	duration = duration % DURATION_MONTH;

	if (months) {
		format(buf[(length = strlen(buf))], sizeof(buf) - length, months == 1 ? ("%d month, ") : ("%d months, "), months);
	}

	weeks    = duration / DURATION_WEEK;
	duration = duration % DURATION_WEEK;

	if (weeks) {
		format(buf[(length = strlen(buf))], sizeof(buf) - length, weeks == 1 ? ("%d week, ") : ("%d weeks, "), weeks);
	}

	days     = duration / DURATION_DAY;
	duration = duration % DURATION_DAY;

	if (days) {
		format(buf[(length = strlen(buf))], sizeof(buf) - length, days == 1 ? ("%d day, ") : ("%d days, "), days);
	}

	hours    = duration / DURATION_HOUR;
	duration = duration % DURATION_HOUR;

	if (hours) {
		format(buf[(length = strlen(buf))], sizeof(buf) - length, hours == 1 ? ("%d hour, ") : ("%d hours, "), hours);
	}

	minutes = duration / DURATION_MINUTE;

	if (minutes) {
		format(buf[(length = strlen(buf))], sizeof(buf) - length, minutes == 1 ? ("%d minute, ") : ("%d minutes, "), minutes);
	}

	if (!buf[0]) {
		format(buf[(length = strlen(buf))], sizeof(buf) - length, duration == 1 ? ("%d second") : ("%d seconds"), duration);
	} else {
		length = strlen(buf);

		buf[length - 2] = 0;
	}

	last_pos = -1;

	while (-1 != (pos = strfind(buf, ",", _, pos)))
		last_pos = pos++;

	if (last_pos != -1) {
		buf[last_pos] = ' ';
	
		strins(buf, "and", last_pos + 1);
	}

	return buf;
}

// Packed (or unpacked) strings.
FormatSpecifier<'S'>(output[], string[]) {
	strunpack(output, string);
}

// Lower-case string.
FormatSpecifier<'L'>(output[], string[]) {
	strunpack(output, string);
	
	for (new i = 0, l = strlen(output); i < l; i++)
		output[i] = tolower(output[i]);
}

// Duration
FormatSpecifier<'D'>(output[], duration) {
	strunpack(output, GetStringFromDuration(duration));
}