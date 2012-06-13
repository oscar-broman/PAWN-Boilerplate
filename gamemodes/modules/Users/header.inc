/*!
 * Users/header.inc
 *
 > Priority: 9
 */

// Get a syntax error instead of "unknown symbol" error when macros used incorrectly.
stock RegisterUserVariable;

// Entry points
#define RegisterUserVariable:%1; \
	@Pu_E1:@Pu_E2:%1;

// "alias" => variable
#define @Pu_E1:@Pu_E2:%1=>%2; \
	forward %2@Pu_(); \
	public %2@Pu_() { \
		Users.RegisterUserVariable(%1, tagof((%2)), sizeof((%2[])), _:%2); \
	}

// variable
#define @Pu_E2:%1; \
	@Pu_X(); \
	forward %1@Pu_(); \
	public %1@Pu_() { \
		Users.RegisterUserVariable(#%1, tagof((%1)), sizeof((%1[])), _:%1); \
	}

enum {
	this.DIALOG_REGISTER = this.OFFSET,
	this.DIALOG_REGISTER_QUIT_CONFIRM,
	this.DIALOG_LOG_IN,
	this.DIALOG_LOG_IN_QUIT_CONFIRM
};

enum this.e_COLUMN {
	Table,

	Name[32],
	Type[16],
	Constraint[64],
	Default[16]
};

enum this.e_TABLE {
	Name[16],
	AutoIncrement
};

enum {
	this.TYPE_INT,
	this.TYPE_FLOAT,
	this.TYPE_STRING,
	this.TYPE_MALLOC_STRING
};

enum this.E_USER_VARIABLE {
	Alloc:Key,
	      KeyHash,
	      Type,
	      Size,
	      Address,
	 bool:Packed
};

const UID:INVALID_UID = UID:-1;
const MINIMUM_UID     = 1000;

new const
	this.Tables[][this.e_TABLE] = {
		{"users"     , MINIMUM_UID},
		{"users_info", 0}
	},
	this.Columns[][this.e_COLUMN] = {
		{0,      "uid", "integer", "NOT NULL PRIMARY KEY AUTOINCREMENT", ""},
		{0,     "name",    "text", "UNIQUE NOT NULL COLLATE NOCASE",     ""},
		{0, "passhash",    "blob", "NOT NULL",                           ""},
		
		{1,      "idx", "integer", "NOT NULL PRIMARY KEY AUTOINCREMENT", ""},
		{1,      "uid", "integer", "NOT NULL",                           ""},
		{1,      "key",    "text", "NOT NULL COLLATE NOCASE",            ""},
		{1,    "value",        "", "NOT NULL",                           ""}
	}
;

new
	            this.DatabaseName[32 char] = !"users.db",
	            this.MaxLogInAttempts = 3,
	         DB:this.DB,
	        UID:this.PlayerUID[MAX_PLAYERS],
	        UID:this.PlayerNameUID[MAX_PLAYERS],
	       bool:this.PlayerIsLoggedIn[MAX_PLAYERS char],
	            this.LogInAttempts[MAX_PLAYERS],
	            this.NumUserVariables = 0,
	            this.UserVariables[1024][this.E_USER_VARIABLE]
;

RegisterConfigVariable: this.DatabaseName;
RegisterConfigVariable: this.MaxLogInAttempts;

// Function forward-declarations
forward  UID:this.RegisterAccount(const name[], const password[]);
forward bool:this.RenameAccount({UID, _}:uid, const new_name[]);
forward bool:this.ChangeAccountPassword({UID, _}:uid, const new_password[]);
forward      this.SaveAccount({UID, _}:_playerid);
forward      this.DeleteAccount({UID, _}:uid);
forward bool:this.LogPlayerIn(playerid, const password[]);
forward bool:this.TestAccountAuth({UID, _}:uid, const password[]);
forward  UID:this.FindAccountByName(const name[]);
forward      this.FindPlayerByAccount(UID:uid);

forward bool:this.IsLoggedIn({UID, _}:playerid);
forward bool:this.IsRegistered({UID, _}:playerid);
forward bool:this.RegisterPlayer(playerid, const password[]);

forward this.RegisterUserVariable(const _key[], tag, size, ...);