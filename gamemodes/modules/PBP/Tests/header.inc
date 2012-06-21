/*!
 * PBP/Tests/header.inc
 *
 > Priority: 0
 */

new
	      this.StringVariable[200] = "hello world",
	Alloc:this.MallocStringVariable
;

RegisterConfigVariable: this.StringVariable;
RegisterConfigVariable: this.MallocStringVariable;

new
	      this.PlayerInt[MAX_PLAYERS],
	Float:this.PlayerFloat[MAX_PLAYERS],
	      this.PlayerString[MAX_PLAYERS][50],
	Alloc:this.PlayerMallocString[MAX_PLAYERS]
;

RegisterUserVariable: "integer"       => this.PlayerInt;
RegisterUserVariable: "float"         => this.PlayerFloat;
RegisterUserVariable: "string"        => this.PlayerString;
RegisterUserVariable: "malloc-string" => this.PlayerMallocString;