/*!
 * PBP/Config/functions.inc
 *
 > Priority: 15
 */

this.LoadEntries(bool:initial_load = false) {
	if (fexist(this.FILE)) {
		new
			File:fp = fopen(this.FILE, io_read),
			pos,
			start,
			key[64 char],
			line_buffer[512 char]
		;
		
		if (!fp) {
			printf(!"(Config) Error: Unable to open the file \"%S\" for reading.", this.FILE);

			return;
		}
		
		while (fread(fp, line_buffer, _, true)) {
			if (line_buffer{0} == '#')
				continue;
			
			if (-1 != (pos = strfind(line_buffer, !"="))) {
				pos--;
				
				while (pos > 1 && line_buffer{pos} <= ' ')
					pos--;
			
				if (!pos)
					continue;
			
				start = 0;
			
				while (start < pos && line_buffer{start} <= ' ')
					start++;
				
				pos -= start;
				
				strdel(line_buffer, 0, start);
				
				strmid(key, line_buffer, 0, pos + 1);
				
				if (-1 == (pos = strfind(line_buffer, !"=")))
					continue;
				
				pos++;
				
				while (0 < line_buffer{pos} <= ' ')
					pos++;
				
				strdel(line_buffer, 0, pos);
				
				pos = strlen(line_buffer);
				
				if (pos)
					pos--;
				
				while (pos && line_buffer{pos} <= ' ')
					line_buffer{pos--} = '\0';
				
				if (line_buffer{0} == '"' && line_buffer{pos} == '"')
					line_buffer{pos} = '\0', strdel(line_buffer, 0, 1);
				
				new key_hash = YHash(key, .pack = true);
				static key_unpacked[64];
				
				for (new i = 0; i < this.NumConfigEntries; i++) {
					if (key_hash == this.ConfigEntries[i][KeyHash] && !strcmp(key, mstr(this.ConfigEntries[i][Key]))) {
						switch (this.ConfigEntries[i][Type] & 0xFF) {
							case this.TYPE_INT: {
								new value = strval(line_buffer);
								
								if (@ptr[this.ConfigEntries[i][Address]] != value) {
									@ptr[this.ConfigEntries[i][Address]] = value;
									
									if (!initial_load) {
										strunpack(key_unpacked, mstr(this.ConfigEntries[i][Key]));
									
										CallFunction(this.CB_OnConfigValueChange, ref(key_unpacked), this.ConfigEntries[i][KeyHash]);
									}
								}
							}

							case this.TYPE_FLOAT: {
								new value = _:floatstr(line_buffer);

								if (@ptr[this.ConfigEntries[i][Address]] != value) {
									@ptr[this.ConfigEntries[i][Address]] = value;
									
									if (!initial_load) {
										strunpack(key_unpacked, mstr(this.ConfigEntries[i][Key]));
									
										CallFunction(this.CB_OnConfigValueChange, ref(key_unpacked), this.ConfigEntries[i][KeyHash]);
									}
								}
							}

							case this.TYPE_ARRAY: {
								new bool:modified = false;
								new old_val;
								new bool:zerofill = false;
								new type = this.ConfigEntries[i][Type] >>> 8;
								
								for (new j = 0; j < this.ConfigEntries[i][Size]; j++) {
									if (zerofill) {
										@ptr[this.ConfigEntries[i][Address]][j] = 0;
										
										continue;
									}
									
									if (-1 != (pos = strfind(line_buffer, !",")))
										line_buffer{pos} = 0;
									
									if (!modified)
										old_val = @ptr[this.ConfigEntries[i][Address]][j];
									
									if (type == this.TYPE_FLOAT)
										@ptr[this.ConfigEntries[i][Address]][j] = _:floatstr(line_buffer);
									else
										@ptr[this.ConfigEntries[i][Address]][j] = strval(line_buffer);
									
									if (!modified && old_val != @ptr[this.ConfigEntries[i][Address]][j])
										modified = true;
									
									if (pos == -1) {
										zerofill = true;
										
										continue;
									} else {
										line_buffer{pos++} = ',';
									}
									
									while (0 < line_buffer{pos} <= ' ')
										pos++;
									
									strdel(line_buffer, 0, pos);
								}
								
								if (modified && !initial_load) {
									strunpack(key_unpacked, mstr(this.ConfigEntries[i][Key]));

									CallFunction(this.CB_OnConfigValueChange, ref(key_unpacked), this.ConfigEntries[i][KeyHash]);
								}
							}
							
							case this.TYPE_STRING: {
								if (strcmp(@ptr[this.ConfigEntries[i][Address]], line_buffer, false) || isnull(@ptr[this.ConfigEntries[i][Address]]) != isnull(line_buffer)) {
									if (this.ConfigEntries[i][Size] & (1 << 31))
										strpack(@ptr[this.ConfigEntries[i][Address]], line_buffer, this.ConfigEntries[i][Size] & (-1 >>> 1));
									else
										strunpack(@ptr[this.ConfigEntries[i][Address]], line_buffer, this.ConfigEntries[i][Size]);
									
									if (!initial_load) {
										strunpack(key_unpacked, mstr(this.ConfigEntries[i][Key]));

										CallFunction(this.CB_OnConfigValueChange, ref(key_unpacked), this.ConfigEntries[i][KeyHash]);
									}
								}
							}
							
							case this.TYPE_MALLOC_STRING: {
								if (!@ptr[this.ConfigEntries[i][Address]] || strcmp(mstr(Alloc:@ptr[this.ConfigEntries[i][Address]]), line_buffer, false) || IsNull(mstr(Alloc:@ptr[this.ConfigEntries[i][Address]])) != IsNull(line_buffer)) {
									if (@ptr[this.ConfigEntries[i][Address]])
										free(Alloc:@ptr[this.ConfigEntries[i][Address]]);
									
									@ptr[this.ConfigEntries[i][Address]] = _:Malloc_NewS(line_buffer, !!(this.ConfigEntries[i][Size] & (1 << 31)));
									
									if (!initial_load) {
										strunpack(key_unpacked, mstr(this.ConfigEntries[i][Key]));

										CallFunction(this.CB_OnConfigValueChange, ref(key_unpacked), this.ConfigEntries[i][KeyHash]);
									}
								}
							}
						}
						
						break;
					}
				}
			}
		}
		
		fclose(fp);
	}
}

this.SaveEntries() {
	new
		File:fp = fopen(this.FILE, io_write),
		line_buffer[512]
	;
	
	if (!fp) {
		printf(!"(Config) Error: Unable to open the file \"%S\" for writing.", this.FILE);
		
		return;
	}
	
	fwrite(fp, "# This file will automatically get re-created when the game mode exits.\n");
	fwrite(fp, "# Use the RCON command \"reload_config\" to re-load the server's values.\n");
	fwrite(fp, "\n");
	
	for (new i = 0; i < this.NumConfigEntries; i++) {
		strunpack(line_buffer, mstr(this.ConfigEntries[i][Key]));
		strcat (line_buffer, !" = ");
		
		switch (this.ConfigEntries[i][Type] & 0xFF) {
			case this.TYPE_INT:
				strcatf(line_buffer, _, !"%d", @ptr[this.ConfigEntries[i][Address]]);
			
			case this.TYPE_FLOAT:
				strcatf(line_buffer, _, !"%f", @ptr[this.ConfigEntries[i][Address]]);
			
			case this.TYPE_ARRAY: {
				switch (this.ConfigEntries[i][Type] >>> 8) {
					case this.TYPE_FLOAT: {
						for (new j = 0; j < this.ConfigEntries[i][Size]; j++) {
							if (j)
								strcatf(line_buffer, _, !", %f", @ptr[this.ConfigEntries[i][Address]][j]);
							else
								strcatf(line_buffer, _, !"%f", @ptr[this.ConfigEntries[i][Address]][j]);
						}
					}
					
					default: {
						for (new j = 0; j < this.ConfigEntries[i][Size]; j++) {
							if (j)
								strcatf(line_buffer, _, !", %d", @ptr[this.ConfigEntries[i][Address]][j]);
							else
								strcatf(line_buffer, _, !"%d", @ptr[this.ConfigEntries[i][Address]][j]);
						}
					}
				}
			}
			
			case this.TYPE_STRING:
				strcat(line_buffer, "\""), strcat(line_buffer, @ptr[this.ConfigEntries[i][Address]]), strcat(line_buffer, "\"");
			
			case this.TYPE_MALLOC_STRING: {
				if (@ptr[this.ConfigEntries[i][Address]])
					strcat(line_buffer, "\""), strcat(line_buffer, mstr(Alloc:@ptr[this.ConfigEntries[i][Address]])), strcat(line_buffer, "\"");
				else
					strcat(line_buffer, "\"\"");
			}
				
		}
		
		strcat(line_buffer, !"\n");
		
		fwrite(fp, line_buffer);
	}
	
	fclose(fp);
}

stock this.RegisterVariableEntry(const _key[], tag, size, GLOBAL_TAG_TYPES:...) {
	new
		      address,
		      key[64 char],
		      type,
		      i,
		Alloc:key_ptr
	;
	
	strpack(key, _key);
	
	PBP.ResolveSymbolName(key);
	
	#emit LOAD.S.pri  24
	#emit STOR.S.pri  address
	
	if (tag == tagof(Float:))
		type = this.TYPE_FLOAT;
	else if (tag == tagof(Alloc:))
		type = this.TYPE_MALLOC_STRING;
	else
		type = this.TYPE_INT;
	
	if (size > 1) {
		if (numargs() > 4 && getarg(4))
			type = (type << 8) | this.TYPE_ARRAY;
		else
			type = this.TYPE_STRING;
	}
	
	key_ptr = malloc((strlen(key) + 1) char);
	
	strpack(mstr(key_ptr), key, Malloc_SlotSize(key_ptr));
	
	i = this.NumConfigEntries++;
	
	this.ConfigEntries[i][KeyHash] = YHash(key, .pack = true);
	this.ConfigEntries[i][Key]     = key_ptr;
	this.ConfigEntries[i][Address] = address;
	this.ConfigEntries[i][Type]    = type;
	this.ConfigEntries[i][Size]    = size;
	
	if (type == this.TYPE_STRING)
		this.ConfigEntries[i][Size] |= (_:ispacked(@ptr[address]) << 31);
	else if (type == this.TYPE_MALLOC_STRING && @ptr[address])
		this.ConfigEntries[i][Size] |= (_:ispacked(mstr(Alloc:@ptr[address])) << 31);
}