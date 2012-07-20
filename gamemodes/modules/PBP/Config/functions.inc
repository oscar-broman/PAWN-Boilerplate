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

stock this.RegisterVariableEntry(const _key[], tag, size, module, GLOBAL_TAG_TYPES:...) {
	new
		      address,
		      key[64 char],
		      type,
		      i,
		Alloc:key_ptr
	;
	
	strpack(key, _key);
	
	PBP.ResolveSymbolName(key);
	
	#emit LOAD.S.pri  28
	#emit STOR.S.pri  address
	
	if (tag == tagof(Float:))
		type = this.TYPE_FLOAT;
	else if (tag == tagof(Alloc:))
		type = this.TYPE_MALLOC_STRING;
	else
		type = this.TYPE_INT;
	
	if (size > 1) {
		if (numargs() > 5 && getarg(5))
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
	this.ConfigEntries[i][Module]  = module;
	
	if (module != -1)
		Bit_Let(this.ModulesWithEntries, module);
	
	if (type == this.TYPE_STRING)
		this.ConfigEntries[i][Size] |= (_:ispacked(@ptr[address]) << 31);
	else if (type == this.TYPE_MALLOC_STRING && @ptr[address])
		this.ConfigEntries[i][Size] |= (_:ispacked(mstr(Alloc:@ptr[address])) << 31);
}

this.GetValueAsString(entry) {
	static buffer[1024];
	
	buffer = "";
	
	switch (this.ConfigEntries[entry][Type] & 0xFF) {
		case this.TYPE_INT:
			strcatf(buffer, _, !"%d", @ptr[this.ConfigEntries[entry][Address]]);
		
		case this.TYPE_FLOAT:
			strcatf(buffer, _, !"%f", @ptr[this.ConfigEntries[entry][Address]]);
		
		case this.TYPE_ARRAY: {
			switch (this.ConfigEntries[entry][Type] >>> 8) {
				case this.TYPE_FLOAT: {
					for (new j = 0; j < this.ConfigEntries[entry][Size]; j++) {
						if (j)
							strcatf(buffer, _, !", %f", @ptr[this.ConfigEntries[entry][Address]][j]);
						else
							strcatf(buffer, _, !"%f", @ptr[this.ConfigEntries[entry][Address]][j]);
					}
				}
				
				default: {
					for (new j = 0; j < this.ConfigEntries[entry][Size]; j++) {
						if (j)
							strcatf(buffer, _, !", %d", @ptr[this.ConfigEntries[entry][Address]][j]);
						else
							strcatf(buffer, _, !"%d", @ptr[this.ConfigEntries[entry][Address]][j]);
					}
				}
			}
		}
		
		case this.TYPE_STRING:
			strcat(buffer, "\""), strcat(buffer, @ptr[this.ConfigEntries[entry][Address]]), strcat(buffer, "\"");
		
		case this.TYPE_MALLOC_STRING: {
			if (@ptr[this.ConfigEntries[entry][Address]])
				strcat(buffer, "\""), strcat(buffer, mstr(Alloc:@ptr[this.ConfigEntries[entry][Address]])), strcat(buffer, "\"");
			else
				strcat(buffer, "\"\"");
		}
	}
	
	return buffer;
}

this.SetValueFromString(entry, input[]) {
	new pos, key_unpacked[64];
	
	switch (this.ConfigEntries[entry][Type] & 0xFF) {
		case this.TYPE_INT: {
			new value = strval(input);
			
			if (@ptr[this.ConfigEntries[entry][Address]] != value) {
				@ptr[this.ConfigEntries[entry][Address]] = value;
				
				strunpack(key_unpacked, mstr(this.ConfigEntries[entry][Key]));
			
				CallFunction(this.CB_OnConfigValueChange, ref(key_unpacked), this.ConfigEntries[entry][KeyHash]);
			}
		}

		case this.TYPE_FLOAT: {
			new value = _:floatstr(input);

			if (@ptr[this.ConfigEntries[entry][Address]] != value) {
				@ptr[this.ConfigEntries[entry][Address]] = value;
				
				strunpack(key_unpacked, mstr(this.ConfigEntries[entry][Key]));
			
				CallFunction(this.CB_OnConfigValueChange, ref(key_unpacked), this.ConfigEntries[entry][KeyHash]);
			}
		}

		case this.TYPE_ARRAY: {
			new bool:modified = false;
			new old_val;
			new bool:zerofill = false;
			new type = this.ConfigEntries[entry][Type] >>> 8;
			
			for (new j = 0; j < this.ConfigEntries[entry][Size]; j++) {
				if (zerofill) {
					@ptr[this.ConfigEntries[entry][Address]][j] = 0;
					
					continue;
				}
				
				if (-1 != (pos = strfind(input, !",")))
					input[pos] = 0;
				
				if (!modified)
					old_val = @ptr[this.ConfigEntries[entry][Address]][j];
				
				if (type == this.TYPE_FLOAT)
					@ptr[this.ConfigEntries[entry][Address]][j] = _:floatstr(input);
				else
					@ptr[this.ConfigEntries[entry][Address]][j] = strval(input);
				
				if (!modified && old_val != @ptr[this.ConfigEntries[entry][Address]][j])
					modified = true;
				
				if (pos == -1) {
					zerofill = true;
					
					continue;
				} else {
					input[pos++] = ',';
				}
				
				while (0 < input[pos] <= ' ')
					pos++;
				
				strdel(input, 0, pos);
			}
			
			if (modified) {
				strunpack(key_unpacked, mstr(this.ConfigEntries[entry][Key]));

				CallFunction(this.CB_OnConfigValueChange, ref(key_unpacked), this.ConfigEntries[entry][KeyHash]);
			}
		}
		
		case this.TYPE_STRING: {
			if (strcmp(@ptr[this.ConfigEntries[entry][Address]], input, false) || isnull(@ptr[this.ConfigEntries[entry][Address]]) != isnull(input)) {
				if (this.ConfigEntries[entry][Size] & (1 << 31))
					strpack(@ptr[this.ConfigEntries[entry][Address]], input, this.ConfigEntries[entry][Size] & (-1 >>> 1));
				else
					strunpack(@ptr[this.ConfigEntries[entry][Address]], input, this.ConfigEntries[entry][Size]);
				
				strunpack(key_unpacked, mstr(this.ConfigEntries[entry][Key]));

				CallFunction(this.CB_OnConfigValueChange, ref(key_unpacked), this.ConfigEntries[entry][KeyHash]);
			}
		}
		
		case this.TYPE_MALLOC_STRING: {
			if (!@ptr[this.ConfigEntries[entry][Address]] || strcmp(mstr(Alloc:@ptr[this.ConfigEntries[entry][Address]]), input, false) || IsNull(mstr(Alloc:@ptr[this.ConfigEntries[entry][Address]])) != IsNull(input)) {
				if (@ptr[this.ConfigEntries[entry][Address]])
					free(Alloc:@ptr[this.ConfigEntries[entry][Address]]);
				
				@ptr[this.ConfigEntries[entry][Address]] = _:Malloc_NewS(input, !!(this.ConfigEntries[entry][Size] & (1 << 31)));
				
				strunpack(key_unpacked, mstr(this.ConfigEntries[entry][Key]));

				CallFunction(this.CB_OnConfigValueChange, ref(key_unpacked), this.ConfigEntries[entry][KeyHash]);
			}
		}
	}
}

this.ShowEditorDialog(playerid, selected_module = -1, selected_entry = -1) {
	static buffer[1024];
	
	if (selected_module == -1 && selected_entry == -1) {
		buffer = "";
		
		foreach (new module : Bits(this.ModulesWithEntries)) {
			strcat(buffer, PBP.Modules[module][Name]), strcat(buffer, "\n");
		}
		
		ShowPlayerDialog(playerid, this.DIALOG_CONFIG_MAIN, DIALOG_STYLE_LIST, @"Config editor - Select category", buffer, @"OK", @"Close");
	} else if (selected_entry == -1) {
		new module = 0, bool:first = true;
		
		buffer = "";
		
		if (selected_module & 0x80000000) {
			module = selected_module & 0x7FFFFFFF;
		} else {
			foreach (new i : Bits(this.ModulesWithEntries)) {
				if (selected_module == module++) {
					module = i;
				
					break;
				}
			}
		}
		
		for (new i = 0; i < this.NumConfigEntries; i++) {
			if (this.ConfigEntries[i][Module] != module)
				continue;
			
			if (first)
				first = false;
			else if (this.ShowValuesInEditor)
				strcat(buffer, " \n");
			
			if (this.ShowValuesInEditor) {
				strcatf(buffer, _, "%S\n\t%s\n", mstr(this.ConfigEntries[i][Key]), this.GetValueAsString(i));
			} else {
				strcatf(buffer, _, "%S\n", mstr(this.ConfigEntries[i][Key]));
			}
		}
		
		SetPVarInt(playerid, #this.InModule, module);
		
		ShowPlayerDialog(playerid, this.DIALOG_CONFIG_CATEGORY, DIALOG_STYLE_LIST, @"Config editor - Select entry", buffer, @"OK", @"Back");
	} else {
		new module = GetPVarInt(playerid, #this.InModule);
		
		new entry = 0;
		
		if (this.ShowValuesInEditor)
			selected_entry /= 3;
		
		for (new i = 0; i < this.NumConfigEntries; i++) {
			if (this.ConfigEntries[i][Module] != module)
				continue;
			
			if (selected_entry == entry++) {
				entry = i;
				
				break;
			}
		}
		
		SetPVarInt(playerid, #this.InEntry, entry);
		
		format(buffer, _, @"You are editing %S.\n\nCurrent value: %s", mstr(this.ConfigEntries[entry][Key]), this.GetValueAsString(entry));
		
		ShowPlayerDialog(playerid, this.DIALOG_CONFIG_EDITOR, DIALOG_STYLE_INPUT, @"Config editor - Edit entry", buffer, @"OK", @"Cancel");
	}
}