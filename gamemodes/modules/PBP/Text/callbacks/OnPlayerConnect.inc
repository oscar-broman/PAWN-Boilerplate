/*!
 * PBP/Text/callbacks/OnPlayerConnect.inc
 *
 * OnPlayerConnect(playerid)
 *
 > Priority: 11
 */

{
	this.SetPlayerLanguage(playerid, this.DEFAULT_LANGUAGE);
	
	Bit_Vet(this.HasSelectedLanguage, playerid);
}