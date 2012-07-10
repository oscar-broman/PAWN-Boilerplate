/*!
 * PBP/ClassSelection/callbacks/OnPlayerLogIn.inc
 *
 * OnPlayerLogIn(playerid, bool:autologin)
 *
 > Priority: 0
 */

{
	if (this.LastClass[playerid] != -1) {
		new class = this.LastClass[playerid];
		
		if (this.RequestedClass[playerid] != class && PlayerCanSelectClass(playerid, class) && this.Classes[class][Skin] == this.LastClassSkin[playerid]) {
			this.RequestedClass[playerid] = class;
			
			CallRemoteFunction(!"OnPlayerRequestClass", !"ii", playerid, this.PreviousClassid{playerid});
		}
	}
}