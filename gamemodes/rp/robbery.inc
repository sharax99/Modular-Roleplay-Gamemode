IsPlayerCop(playerid)
{
	if(IsPlayerConnected(playerid))
	{
		if(GetPlayerSkin(playerid)== 281 || 
		GetPlayerSkin(playerid)== 265 ||
		GetPlayerSkin(playerid)== 266 ||
		GetPlayerSkin(playerid)== 267 ||
		GetPlayerSkin(playerid)== 280 ||
		GetPlayerSkin(playerid)== 282 ||
		GetPlayerSkin(playerid)== 283 ||
		GetPlayerSkin(playerid)== 284 ||
		GetPlayerSkin(playerid)== 285 ||
		GetPlayerSkin(playerid)== 286 ||
		GetPlayerSkin(playerid)== 288) 
		{
			return 1;
		}
 	}
	return 0;
}
