/*!
 * PBP/Player/callbacks/OnRconLoginAttempt.inc
 *
 * OnRconLoginAttempt(ip[], password[], success)
 *
 > Priority: 0
 */

{
	if (success)
		SetTimer(#this.CheckForNewAdmins, 0, false);
}