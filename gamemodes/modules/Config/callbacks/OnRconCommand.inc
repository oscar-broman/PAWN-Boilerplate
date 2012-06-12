/*!
 * Config/callbacks/OnRconCommand.inc
 *
 * OnRconCommand(cmd[])
 *
 > Priority: 0
 */

{
	if (!isnull(cmd)) {
		if (!strcmp(cmd, !"config_reload", true)) {
			this.LoadEntries();
		
			printf(!"Reloaded the configuration from \"%S\".", this.FILE);
		} else if (!strcmp(cmd, !"config_set", true, 10)) {
			new key[32], value[256];
			
			if (!sscanf(cmd[10], !"s[32]s[256]", key, value)) {
				new key_hash = YHash(key), bool:found = false;
				
				for (new i = 0; i < this.NumConfigEntries; i++) {
					if (key_hash == this.ConfigEntries[i][KeyHash] && !strcmp(key, mget(this.ConfigEntries[i][Key], 0))) {
						switch (this.ConfigEntries[i][Type] & 0xFF) {
							case this.TYPE_INT: {
								new val = strval(value);
								
								if (@ptr[this.ConfigEntries[i][Address]] != val) {
									@ptr[this.ConfigEntries[i][Address]] = val;
									
									CallFunction(this.CB_OnConfigValueChange, ref(key), key_hash);
									
									printf(!"The config variable \"%S\" was set to \"%d\".", key, val);
								} else {
									print(!"The variable already has that value.");
								}
							}
							
							case this.TYPE_FLOAT: {
								new val = _:floatstr(value);
								
								if (@ptr[this.ConfigEntries[i][Address]] != val) {
									@ptr[this.ConfigEntries[i][Address]] = val;
									
									CallFunction(this.CB_OnConfigValueChange, ref(key), key_hash);
									
									printf(!"The config variable \"%S\" was set to \"%f\".", key, val);
								} else {
									print(!"The variable already has that value.");
								}
							}
							
							case this.TYPE_STRING: {
								new len = strlen(value);
								
								if (value[0] == '"' && value[len - 1] == '"')
									strdel(value, 0, 1), value[len - 2] = '\0';
								
								if (strcmp(@ptr[this.ConfigEntries[i][Address]], value, false) || isnull(@ptr[this.ConfigEntries[i][Address]]) != isnull(value)) {
									if (this.ConfigEntries[i][Size] & (1 << 31))
										strpack(@ptr[this.ConfigEntries[i][Address]], value, this.ConfigEntries[i][Size] & (-1 >>> 1));
									else
										strunpack(@ptr[this.ConfigEntries[i][Address]], value, this.ConfigEntries[i][Size]);
									
									CallFunction(this.CB_OnConfigValueChange, ref(key), key_hash);
									
									printf(!"The config variable \"%S\" was set to \"%s\".", key, value);
								} else {
									print(!"The variable already has that value.");
								}
							}
							
							case this.TYPE_MALLOC_STRING: {
								new len = strlen(value);
								
								if (value[0] == '"' && value[len - 1] == '"')
									strdel(value, 0, 1), value[len - 2] = '\0';
								
								if (!@ptr[this.ConfigEntries[i][Address]] || strcmp(mget(Alloc:@ptr[this.ConfigEntries[i][Address]], 0), value, false) || IsNull(mget(Alloc:@ptr[this.ConfigEntries[i][Address]], 0)) != IsNull(value)) {
									if (@ptr[this.ConfigEntries[i][Address]])
										free(Alloc:@ptr[this.ConfigEntries[i][Address]]);
									
									@ptr[this.ConfigEntries[i][Address]] = _:Malloc_NewS(value, !!(this.ConfigEntries[i][Size] & (1 << 31)));
									
									CallFunction(this.CB_OnConfigValueChange, ref(key), key_hash);
									
									printf(!"The config variable \"%S\" was set to \"%s\".", key, value);
								} else {
									print(!"The variable already has that value.");
								}
							}
							
							default: {
								print(!"You can only set the value of ints, floats, or strings.");
							}
						}
						
						found = true;
						
						break;
					}
				}
				
				if (!found)
					printf(!"Invalid key: \"%s\".", key);
				else
					Config.SaveEntries();
			} else {
				print(!"USAGE: config_set [key] [value]");
			}
		}
	}
}