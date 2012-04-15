/*!
 * Config/functions.inc
 *
 > Priority: 0
 */

this.LoadEntries() {
	new id, key[64], value[512];
	
	stmt_bind_result_field(this.stmt_load_entries, 0, DB::TYPE_INT   , id); // id
	stmt_bind_result_field(this.stmt_load_entries, 1, DB::TYPE_STRING, key, sizeof(key)); // key
	stmt_bind_result_field(this.stmt_load_entries, 2, DB::TYPE_STRING, value, sizeof(value)); // value
	
	if (stmt_execute(this.stmt_load_entries)) {
	    while (stmt_fetch_row(this.stmt_load_entries)) {
			new key_hash = YHash(key, .pack = false);
			
			for (new i = 0; i < this.NumConfigEntries; i++) {
				if (key_hash == this.ConfigEntries[i][KeyHash] && !strcmp(key, mget(this.ConfigEntries[i][Key], 0))) {
					this.ConfigEntries[i][Id] = id;
					
					if (this.ConfigEntries[i][Size] == 1) {
						switch (this.ConfigEntries[i][Type]) {
							case DB::TYPE_FLOAT:
								@ptr[this.ConfigEntries[i][Address]] = _:floatstr(value);
							
							default:
								@ptr[this.ConfigEntries[i][Address]] = strval(value);
						}
					} else {
						switch (this.ConfigEntries[i][Type]) {
							case DB::TYPE_STRING: {
								if (this.ConfigEntries[i][Size] & (1 << 31))
									strpack(@ptr[this.ConfigEntries[i][Address]], value, this.ConfigEntries[i][Size] & (-1 >>> 1));
								else
									strunpack(@ptr[this.ConfigEntries[i][Address]], value, this.ConfigEntries[i][Size]);
							}
							
							default: {
								// todo
							}
						}
					}
					
					break;
				}
			}
		}
	}
}

this.SaveEntries() {
	for (new i = 0; i < this.NumConfigEntries; i++) {
		if (this.ConfigEntries[i][Id] == -1)
			stmt_bind_value(this.stmt_save_entry, 0, DB::TYPE_NULL);
		else
			stmt_bind_value(this.stmt_save_entry, 0, DB::TYPE_INT, this.ConfigEntries[i][Id]);
		
		stmt_bind_value(this.stmt_save_entry, 1, DB::TYPE_STRING, mget(this.ConfigEntries[i][Key], 0));
		
		if (this.ConfigEntries[i][Size] == 1) {
			stmt_bind_value(this.stmt_save_entry, 2, this.ConfigEntries[i][Type], @ptr[this.ConfigEntries[i][Address]]);
		} else {
			switch (this.ConfigEntries[i][Type]) {
				case DB::TYPE_STRING:
					stmt_bind_value(this.stmt_save_entry, 2, DB::TYPE_STRING, @ptr[this.ConfigEntries[i][Address]]);
				
				default: {
					// todo
				}
			}
		}
		
		stmt_execute(this.stmt_save_entry);
	}
}

stock this.RegisterVariableEntry(const _key[], tag, size, GLOBAL_TAG_TYPES:...) {
	new
		           address,
		           key[64 char],
		DBDataType:type,
		           i,
		     Alloc:key_ptr
	;
	
	strpack(key, _key);
	
	PBP.ResolveSymbolName(key);
	
	#emit LOAD.S.pri  24
	#emit STOR.S.pri  address
	
	if (tag == tagof(String:))
		type = DB::TYPE_STRING;
	else if (tag == tagof(Float:))
		type = DB::TYPE_FLOAT;
	else
		type = DB::TYPE_INT;
	
	key_ptr = malloc((strlen(key) + 1) char);
	
	strpack(mget(key_ptr, 0), key, Malloc_SlotSize(key_ptr));
	
	i = this.NumConfigEntries++;
	
	this.ConfigEntries[i][Id]       = -1;
	this.ConfigEntries[i][KeyHash] = YHash(key, .pack = true);
	this.ConfigEntries[i][Key]     = key_ptr;
	this.ConfigEntries[i][Address] = address;
	this.ConfigEntries[i][Type]    = type;
	this.ConfigEntries[i][Size]    = size;
	
	if (type == DB::TYPE_STRING)
		this.ConfigEntries[i][Size] |= (_:ispacked(@ptr[address]) << 31);
}