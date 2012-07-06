/*!
 * PBP/Text/callbacks/OnPlayerConnect.inc
 *
 * OnPlayerConnect(playerid)
 *
 > Priority: 11
 */

{
	this.SetPlayerLanguage(playerid, this.DefaultLanguage);
	
	Bit_Vet(this.HasSelectedLanguage, playerid);
}