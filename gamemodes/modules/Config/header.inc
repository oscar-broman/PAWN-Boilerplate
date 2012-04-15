/*!
 * Config/header.inc
 *
 > Priority: 0
 */

#define RegisterConfigVariable<%1>; \
	forward %1@Pc_(); \
	public %1@Pc_() { \
		Config.RegisterVariableEntry(#%1, tagof((%1)), sizeof((%1)), %1); \
	}

#define RegisterConfigString<%1>; \
	forward %1@Pc_(); \
	public %1@Pc_() { \
		Config.RegisterVariableEntry(#%1, tagof(String:), sizeof((%1)), %1); \
	}

// Forward this since most RegisterConfigVariable will be in headers
this.RegisterVariableEntry(const _key[], tag, size, GLOBAL_TAG_TYPES:...);

enum this.e_CONFIG_ENTRY {
	           Id,
	           KeyHash,
	     Alloc:Key,
	           Address,
	DBDataType:Type,
	           Size
};

stock const
	this.DB_FILE[] = !"config.db"
;

new
	         DB:this.db,
	DBStatement:this.stmt_load_entries,
	DBStatement:this.stmt_save_entry,
	            this.NumConfigEntries = 0,
	            this.ConfigEntries[1024][this.e_CONFIG_ENTRY]
;