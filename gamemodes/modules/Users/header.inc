/*!
 * Users/header.inc
 *
 > Priority: 0
 */

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
		{0, "passhash",    "blob", "NOT NULL",                           ""}
	}
;

new
	            this.DatabaseName[32 char] = !"users.db",
	         DB:this.DB,
	DBStatement:this.StmtTestAuth,
	DBStatement:this.StmtRegister,
	DBStatement:this.StmtRename,
	DBStatement:this.StmtChangePasshash,
	DBStatement:this.StmtFindByName,
	        UID:this.PlayerUID[MAX_PLAYERS],
	        UID:this.PlayerNameUID[MAX_PLAYERS],
	       bool:this.PlayerIsLoggedIn[MAX_PLAYERS char],
	            this.RegisterDate[MAX_PLAYERS],
	      Alloc:this.Email[MAX_PLAYERS]
;

RegisterConfigVariable: this.DatabaseName;

// Function forward-declarations
forward  UID:this.RegisterAccount(const name[], const password[]);
forward bool:this.RenameAccount({UID, _}:uid, const new_name[]);
forward bool:this.ChangeAccountPassword({UID, _}:uid, const new_password[]);
forward      this.SaveAccount({UID, _}:uid);
forward      this.SaveAccounts();
forward      this.DeleteAccount({UID, _}:uid);
forward bool:this.TestAccountAuth({UID, _}:uid, const password[]);
forward  UID:this.FindAccountByName(const name[]);
forward      this.FindPlayerByAccount(UID:uid);

forward bool:this.IsLoggedIn({UID, _}:playerid);
forward bool:this.IsRegistered({UID, _}:playerid);