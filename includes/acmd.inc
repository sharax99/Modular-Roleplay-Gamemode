COMMAND:masked(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new list_players[256];

	foreach(new i: Player)
   	{
        if(strfind(pInfo[i][player_name], "Stranger", true) >= 0)
		{
			format(list_players, sizeof(list_players), "%s\n%d\t%s (%s)", list_players, i, pInfo[i][player_name], pGlobal[i][glo_name]);
		}
    }
	if(strlen(list_players))
	{
	    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, "Masked players:", list_players, "OK", "");
	}
	else
	{
	    SendGuiInformation(playerid, "Information", "There are no masked players.");
	}
    return 1;
}

COMMAND:limits(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new list_limits[256];
	format(list_limits, sizeof(list_limits), "Groups:\t%d/%d\nGates:\t%d/%d\nDoors:\t%d/%d\nAreas:\t%d/%d\nProducts:\t%d/%d\nGroup items:\t%d/%d", Iter_Count(Groups), MAX_GROUPS, Iter_Count(Gates), MAX_GATES, Iter_Count(Doors), MAX_DOORS, Iter_Count(Areas), MAX_AREAS, Iter_Count(Products), MAX_PRODUCTS, Iter_Count(Items), MAX_ITEMS);
	ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST, "Server limits", list_limits, "OK", "");
	return 1;
}

COMMAND:aproduct(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ITEMS) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new sub_cmd[20], rest[64], uid, cmd_value;
	if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct [reload/limit/resetlimit/extraid/value1/value2/type/price]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "load") )
		{
			if(sscanf(rest, "d", uid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct load [UID]");
			new prod = LoadProduct(uid);
			SendClientMessage(playerid, COLOR_LIGHTER_RED, sprintf("Loaded product %s (UID %d).", Product[prod][product_name], Product[prod][product_id]));
		}
		else if( !strcmp(sub_cmd, "reload") )
		{
			if(sscanf(rest, "d", uid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct reload [UID]");
			new prod = ReloadProduct(uid);
			SendClientMessage(playerid, COLOR_LIGHTER_RED, sprintf("Reloaded product %s (UID %d).", Product[prod][product_name], Product[prod][product_id]));
		}
		else if( !strcmp(sub_cmd, "limit") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct limit [uid] [limit]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct limit [uid] [limit]");
			Product[prodid][product_limit] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_limit_count = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Weekly limit has been edited.");
		}
		else if( !strcmp(sub_cmd, "resetlimit") )
		{
			if(sscanf(rest, "d", uid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct resetlimit [uid]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct resetlimit [uid]");
			Product[prodid][product_limit_used] = 0;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_limit_used = 0 WHERE product_uid = %d", Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Limit usage has been reseted.");
		}
		else if( !strcmp(sub_cmd, "type") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct type [uid] [type]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct type [uid] [type]");
			Product[prodid][product_type] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_type = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Product type edited.");
		}
		else if( !strcmp(sub_cmd, "value1") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct value1 [uid] [value]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct value1 [uid]");
			Product[prodid][product_value1] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_value1 = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Value1 of product has been edited.");
		}
		else if( !strcmp(sub_cmd, "value2") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct value2 [uid] [value]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct value2 [uid]");
			Product[prodid][product_value2] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_value2 = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Value2 of product has been edited.");
		}
		else if( !strcmp(sub_cmd, "price") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt price [uid] [price]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct price [uid]");
			Product[prodid][product_price] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_price = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Price of product has been edited.");
		}
		else if( !strcmp(sub_cmd, "extraid") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct extraid [uid] [value]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct extraid [uid]");
			Product[prodid][product_extra] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_extraid = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Extraid of product has been edited.");
		}
		else if( !strcmp(sub_cmd, "delete") )
		{
			if(sscanf(rest, "d", uid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aproduct delete [uid]");
			RemoveProduct(uid);
			SendClientMessage(playerid, COLOR_LIGHTER_RED, sprintf("Product deleted: UID %d.", uid));
		}

	}
	return 1;
}

COMMAND:event(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new sub_cmd[32];
	if( sscanf(params, "s[32]", sub_cmd) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /event [fire]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "fire") )
		{
			if(Fire[fire_is_burning]) return SendGuiInformation(playerid, "Information", "Fire is still burning.");
			if(!IsAnyFirefighterOnline()) return SendGuiInformation(playerid, "Information", "No firefighters on duty.");

			SetupFires();
			// defer spreadfire
			SendClientMessage(playerid, COLOR_GOLD, "Event stared, firefighters has been alarmed.");
		}
	}
	return 1;
}

COMMAND:setnick(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new id, nickname[MAX_PLAYER_NAME+1];
	if( sscanf(params, "ds[25]", id, nickname) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setnick [id] [nick]");
	if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
    if( !pInfo[id][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");

    strcopy(pInfo[id][player_name], nickname, MAX_PLAYER_NAME+1);
  
	UpdatePlayerLabel(id);
	return 1;
}

COMMAND:setstatus(playerid, params[])
{
    if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
    new targetid, status[32];
    if( sscanf(params, "us[32]", targetid, status) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setstatus [id] [text]");
    if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
    if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
    if( strlen(status) > 32 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setstatus [id] [text]");
   
  	if( !strcmp(status, "0", true) )
  	{
  		pInfo[targetid][player_extra_status] = EOS;
  		SendClientMessage(playerid, COLOR_LIGHTER_RED, "Status removed.");
  		RemovePlayerStatus(targetid, PLAYER_STATUS_EXTRA);
  		UpdatePlayerLabel(targetid);
  		mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_extra_status = '' WHERE char_uid = %d", pInfo[targetid][player_id]));
  		return 1;
  	}
    mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_extra_status` = '%s'  WHERE `char_uid` = %d", status, pInfo[targetid][player_id]));
    sscanf(status, "s[32]", pInfo[targetid][player_extra_status]);
   
    SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s has set up special status for you - %s.", pInfo[playerid][player_name], status);
    if( playerid != targetid ) SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "Status updated for %s (UID: %d, ID: %d) - %s.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, status);
    
    AddPlayerStatus(targetid, PLAYER_STATUS_EXTRA);
    UpdatePlayerLabel(targetid);
    return 1;
}

COMMAND:banip(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new pid;
	if(sscanf(params, "us[256]", pid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /banip [id] [reason]");
	Ban(pid);

	SendClientMessage(playerid, COLOR_GOLD, sprintf("Player banned: %s [GUID: %d]", pInfo[pid][player_name], pGlobal[pid][glo_id]));
	return 1;
}

COMMAND:jobrespawn(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new count;
	foreach(new vid : Vehicles)
	{
		if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_JOB )
		{
			if( Vehicle[vid][vehicle_occupants] > 0 ) continue;
			
			SetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]-0.3);
			SetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
			SetVehicleVirtualWorld(vid, Vehicle[vid][vehicle_park_world]);
			LinkVehicleToInterior(vid, Vehicle[vid][vehicle_park_interior]);
			SetVehicleHealth(vid, 1000);
			Vehicle[vid][vehicle_destroyed] = false; 
			Vehicle[vid][vehicle_damage][0] = 0;
			Vehicle[vid][vehicle_damage][1] = 0;
			Vehicle[vid][vehicle_damage][2] = 0;
			Vehicle[vid][vehicle_damage][3] = 0;
			Vehicle[vid][vehicle_fuel_current] = 50;
			
			UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_health` = 1000 WHERE `vehicle_uid` = %d", Vehicle[vid][vehicle_uid]));
		
			Vehicle[vid][vehicle_locked] = true;
			count++;
		}
	}
	
	SendGuiInformation(playerid, ""guiopis"Information", sprintf("Respawned %d vehicles from jobs.", count));
	return 1;
}

COMMAND:agmx(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return 1;

	SendClientMessageToAll(COLOR_LIGHTER_RED, "Society RP: server restart in 10 seconds.");
		
	foreach(new playeri: Player)
	{
		SavePlayer(playeri);
	}

	defer RestartServer[10000]();
	return 1;
}

COMMAND:apoint(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new seta[256], sets[256],
		Float:pX, Float:pY, Float:pZ;
	if(sscanf(params, "s[80]S()[80]", sets, seta)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /apoint [save, tp]]");
	if(!strcmp(sets, "save", true))
	{
		GetPlayerPos(playerid, pX, pY, pZ);
	    SetPVarFloat(playerid,"al_posx", pX);
	    SetPVarFloat(playerid,"al_posy", pY);
	    SetPVarFloat(playerid,"al_posz", pZ);
	    if(GetPVarInt(playerid, "al_pos") == 1)
		{
			SendClientMessage(playerid, COLOR_GOLD, sprintf("New position saved. [%f|%f|%f]", pX, pY, pZ));
		} else {
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Position saved. [%f:%f:%f]", pX, pY, pZ));
		}
		SetPVarInt(playerid, "al_pos", 1);
	}
	if(!strcmp(sets, "tp", true))
	{
		if(GetPVarInt(playerid, "al_pos") == 0)
		{
			SendGuiInformation(playerid, ""guiopis"Information", "You don't have any saved positions.");
		} else {
			if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
			{
				SetVehiclePos(GetPlayerVehicleID(playerid), GetPVarFloat(playerid, "al_posx"), GetPVarFloat(playerid, "al_posy"), GetPVarFloat(playerid, "al_posz"));
			} else {
				RP_PLUS_SetPlayerPos(playerid, GetPVarFloat(playerid, "al_posx"), GetPVarFloat(playerid, "al_posy"), GetPVarFloat(playerid, "al_posz"));
			}
			SendClientMessage(playerid, COLOR_GOLD, "Teleported to last saved position.");
		}
	}
	return 1;
}

COMMAND:ado(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new text[128], str[128];
	if(sscanf(params, "s[128]", text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ado [text]");
	format(str, sizeof(str), "** %s **", text);
	SendClientMessageToAll(0x9B91ECFF, str);
	return 1;
}

COMMAND:spec(playerid, params[])
{
	return cmd_rc(playerid, params);
}

COMMAND:rc(playerid, params[])
{
    if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
    new id;

    if(!sscanf(params, "d", id))
    {
    	if( !IsPlayerConnected(id) )
    	{
    		SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");
    		return 1;
    	}

    	if( pInfo[id][player_spec] != INVALID_PLAYER_ID )
    	{
    		SendClientMessage(playerid, COLOR_GREY, "This player is in spectate mode.");
    		return cmd_spec(playerid, sprintf("%d", Iter_Next(Player, id)));
    	}

	    SetPlayerInterior(playerid, GetPlayerInterior(id));
	    SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(id));

	    new string[340];

	    format(string, sizeof(string), "~n~~w~Global: %s		Health: ~p~%0.1f		~w~Cash: ~g~%d		~w~Hours: ~y~%d		Armour: ~w~%0.1f~n~~n~~n~~n~~w~Use ~r~LALT or ~r~SPACE ~w~to switch players.~n~~r~LSHIFT ~w~is refresh key.", pGlobal[id][glo_name],pInfo[id][player_health], GetPlayerMoney(id), pInfo[id][player_hours], pInfo[id][player_armour]);
	   	
	    TextDrawSetString(DashBoardPane2[playerid], string);
		TextDrawShowForPlayer(playerid, DashBoardPane2[playerid]);

	    if(IsPlayerInAnyVehicle(id))
	    {
            TogglePlayerSpectating(playerid, 1);
            PlayerSpectateVehicle(playerid, GetPlayerVehicleID(id));
	    } 
	    else 
	    {
            TogglePlayerSpectating(playerid, 1);
            PlayerSpectatePlayer(playerid, id);
	    }

	    pInfo[playerid][player_spec] = id;
	    pInfo[playerid][player_quit_vw] = GetPlayerVirtualWorld(playerid);
	}
	else
	{
		if(pInfo[playerid][player_spec] != INVALID_PLAYER_ID)
	    {
            TogglePlayerSpectating(playerid, 0);

			TextDrawHideForPlayer(playerid, DashBoardPane2[playerid]);

            SetPlayerSkin(playerid, pInfo[playerid][player_skin]);
            SetPlayerVirtualWorld(playerid, pInfo[playerid][player_quit_vw]);
            pInfo[playerid][player_spec] = INVALID_PLAYER_ID;
	    }
	    else
	    {
	    	SendClientMessage(playerid, COLOR_GREY, "Tip: /spec [player id]");
	    } 
	}
    return 1;
}

COMMAND:apos(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new Float:x, Float:y, Float:z;
    if(sscanf(params, "fff", x,y,z))
        return SendClientMessage(playerid, COLOR_GREY, "Tip: /apos [x, y, z]");
    
    RP_PLUS_SetPlayerPos(playerid, x, y, z);
	return 1;
}

COMMAND:freeze(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new id;
    if(sscanf(params, "u", id))
        return SendClientMessage(playerid, COLOR_GREY, "Tip: /freeze [id]");
    
    if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");

    if(pInfo[id][player_freezed]==false)
    {
	    TogglePlayerControllable(id, false);    
   		SendFormattedClientMessage(playerid, COLOR_GOLD, "Player %s (ID: %d, UID: %d) has been freezed.", pInfo[id][player_name], id, pInfo[id][player_id]);
   		SendFormattedClientMessage(id, COLOR_GOLD, "Administrator %s freezed you.", pInfo[playerid][player_name]);
   		pInfo[id][player_freezed]=true;
	}
	else
	{
	    TogglePlayerControllable(id, true);    
   		SendFormattedClientMessage(playerid, COLOR_GOLD, "Player %s (ID: %d, UID: %d) has been unfreezed.", pInfo[id][player_name], id, pInfo[id][player_id]);
   		SendFormattedClientMessage(id, COLOR_GOLD, "Administrator %s unfreezed you", pInfo[playerid][player_name]);
   		pInfo[id][player_freezed]=false;
	}
	return 1;
}

COMMAND:sethours(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) && !HasCrewFlag(playerid, CREW_FLAG_GM) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new targetid, hrs;
	if( sscanf(params, "ud", targetid, hrs) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /sethours [id] [hours]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	if( hrs < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /sethours [id] [hours]");
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_hours` = %d  WHERE `char_uid` = %d", hrs, pInfo[targetid][player_id]));
	pInfo[targetid][player_hours]=hrs;
	
	SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s has have you %d hours online.", pInfo[playerid][player_name], hrs);
	if( playerid != targetid ) SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "You've set %s (UID: %d, ID: %d) %d hours online.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, hrs);
	
	return 1;
}

COMMAND:slap(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new id, Float: PPos[3];
    if(sscanf(params, "u", id))
        return SendClientMessage(playerid, COLOR_GREY, "Tip: /slap [player id]");
    
    if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
    GetPlayerPos(id, PPos[0], PPos[1], PPos[2]);
    RP_PLUS_SetPlayerPos(id, PPos[0], PPos[1], PPos[2]+4);
    
   	SendFormattedClientMessage(playerid, COLOR_GOLD, "%s (ID: %d, UID: %d) has been slaped.", pInfo[id][player_name], id, pInfo[id][player_id]);
	return 1;
}

COMMAND:givecash(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, "Information", "Command only for head admins.");
	
	new targetid, skin;
	if( sscanf(params, "ud", targetid, skin) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /givecash [id] [amount]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	
	GivePlayerMoney(targetid, skin);
	SendFormattedClientMessage(playerid, COLOR_GOLD, "You gave cash for %s (UID: %d, ID: %d) - $%d.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, skin);
	
	return 1;
}

COMMAND:ban(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new playa,text[128];
	if(sscanf(params, "us[128]", playa, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ban [id] [reason]");
	if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
	if(strlen(text)<1 || strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long, max 47 chars.");

	EscapePL(text);
	
	BanAc(playa, playerid, text);

	SendFormattedClientMessage(playa, COLOR_LIGHTER_RED, "Administrator %s has banned you with reason: %s.", gInfo[playerid][global_name], text);
	Kick(playa);
	return 1;
}

COMMAND:block(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new sub_cmd[20], rest[64], playa, text[128], time;
	if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /block [char/run/dmg/veh/ooc]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "char") )
		{
			if(sscanf(rest, "us[128]", playa, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block char [id] [reason]");
			if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
			if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long or too short, min 1, max 47 chars.");

			Block(playa, playerid, text);
		}
		else if( !strcmp(sub_cmd, "ooc") )
		{
			if(sscanf(rest, "uds[128]", playa, time, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block ooc [id] [days] [reason]");
			if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
			if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long or too short, min 1, max 47 chars.");

			BlockOoc(playa, playerid, time, text);
		}
		else if( !strcmp(sub_cmd, "run") )
		{
			if(sscanf(rest, "uds[128]", playa, time, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block run [id] [days] [reason]");
			if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
			if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long or too short, min 1, max 47 chars.");

			BlockRun(playa, playerid, time, text);
		}
		else if( !strcmp(sub_cmd, "dmg") )
		{
			if(sscanf(rest, "uds[128]", playa, time, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block dmg [id] [days] [reason]");
			if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
			if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long or too short, min 1, max 47 chars.");

			BlockDmg(playa, playerid, time, text);
		}
		else if( !strcmp(sub_cmd, "veh") )
		{
			if(sscanf(rest, "uds[128]", playa, time, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block veh [id] [days] [reason]");
			if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
			if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long or too short, min 1, max 47 chars.");

			BlockVeh(playa, playerid, time, text);
		}
	}
	return 1;
}

COMMAND:fck(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new playa, text[128];
	
	if(sscanf(params, "us[128]", playa, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /fck [id] [reason]");
	if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
	if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long or too short, min 1, max 47 chars.");

	CharacterKill(playa, playerid, text);

	return 1;
}

COMMAND:aj(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new playa,text[128],time;
	if(sscanf(params, "uds[128]", playa, time, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aj [id] [time in minutes] [reason]");
	if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
	if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long or too short, min 1, max 47 chars.");
	if(time>1000) return SendClientMessage(playerid, COLOR_GREY, "Tip: Max time is 1000 minutes.");

	AdminJail(playa, playerid, text, time);
	return 1;
}

COMMAND:kick(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new playa,text[128];
	if(sscanf(params, "us[128]", playa, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /kick [id] [reason]");
	if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
	if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long or too short, min 1, max 47 chars.");

	KickAc(playa, playerid, text);
	return 1;
}

COMMAND:warn(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new playa,text[256];
	if(sscanf(params, "us[128]", playa, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /warn [id] [reason]");
	if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long or too short, min 1, max 47 chars.");

	Warn(playa, playerid, text);
	return 1;
}

COMMAND:gs(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new playa,text[256], score;
	if(sscanf(params, "uds[128]", playa, score, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /gs [id] [amount] [reason]");
	if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Reason too long or too short, min 1, max 47 chars.");

	Score(playa, playerid, score, text);
	return 1;
}

COMMAND:ab(playerid, params[])
{
	return cmd_abus(playerid, params);
}

COMMAND:abus(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_EDITOR) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");

	new type[32], varchar[32], string[128];
	if(sscanf(params, "s[32]S()[32]", type, varchar))
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /abus [create], [name], [pos], [delete]");
	    return 1;
	}
	if(!strcmp(type, "create", true))
	{
		new busstop_name[32];
		if(sscanf(varchar, "s[32]", busstop_name))
		{
		    SendClientMessage(playerid, COLOR_GREY, "Tip: /abus create [name]");
		    return 1;
		}
		new Float:PosX, Float:PosY, Float:PosZ;
		GetPlayerPos(playerid, PosX, PosY, PosZ);
		
		new busstop_id = CreateBusStop(busstop_name, PosX, PosY, PosZ + 0.25);
		
		format(string, sizeof(string), "New bus stop created.\nName: %s (SampID: %d, UID: %d).", busstop_name, busstop_id, BusStopData[busstop_id][bUID]);
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", string, "OK", "");
		return 1;
	}
	if(!strcmp(type, "name", true))
	{
	    new busstop_id, busstop_name[32];
	    if(sscanf(varchar, "ds[32]", busstop_id, busstop_name))
	    {
	        SendClientMessage(playerid, COLOR_GREY, "Tip: /abus name [bus stop id] [new name]");
	        return 1;
	    }
	    if(!BusStopData[busstop_id][bUID])
	    {
	    	SendGuiInformation(playerid, ""guiopis"Information", "Invalid bus stop id.");
	        return 1;
	    }
   		strmid(BusStopData[busstop_id][bName], busstop_name, 0, strlen(busstop_name), 32);

		SaveBusStop(busstop_id);

		format(string, sizeof(string), "Bus stop %d:\n%s.", busstop_id, busstop_name);
		UpdateDynamic3DTextLabelText(Text3D:BusStopData[busstop_id][bNameTag], LABEL_DESCRIPTION, string);

		format(string, sizeof(string), "Name of bus stop (SampID: %d, UID: %d) has been changed to %s.", busstop_id, BusStopData[busstop_id][bUID], busstop_name);
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", string, "OK", "");
	    return 1;
	}
	if(!strcmp(type, "pos", true))
	{
	    new busstop_id;
	    if(sscanf(varchar, "d", busstop_id))
	    {
	        SendClientMessage(playerid, COLOR_GREY, "Tip: /abus pos [bus stop id]");
	        return 1;
	    }
	    if(!BusStopData[busstop_id][bUID])
	    {
	        SendGuiInformation(playerid, ""guiopis"Information", "Invalid bus stop id.");
	        return 1;
	    }
   		new Float:PosX, Float:PosY, Float:PosZ;
		GetPlayerPos(playerid, PosX, PosY, PosZ);
		
		BusStopData[busstop_id][bPosX] = PosX;
		BusStopData[busstop_id][bPosY] = PosY;
		BusStopData[busstop_id][bPosZ] = PosZ + 0.25;
		
		SaveBusStop(busstop_id);
		
		Delete3DTextLabel(Text3D:BusStopData[busstop_id][bNameTag]);
		BusStopData[busstop_id][bNameTag] = Create3DTextLabel(string, LABEL_DESCRIPTION, BusStopData[busstop_id][bPosX], BusStopData[busstop_id][bPosY], BusStopData[busstop_id][bPosZ], 15.0, 0, 1);

		format(string, sizeof(string), "Position of bus stop (SampID: %d, UID: %d) has been changed.", busstop_id, BusStopData[busstop_id][bUID]);
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", string, "OK", "");
		return 1;
	}
	if(!strcmp(type, "delete", true))
	{
	    new busstop_id;
	    if(sscanf(varchar, "d", busstop_id))
	    {
	       	SendClientMessage(playerid, COLOR_GREY, "Tip: /abus delete [bus stop id]");
	        return 1;
	    }
	    if(!BusStopData[busstop_id][bUID])
	    {
	        SendGuiInformation(playerid, ""guiopis"Information", "Invalid bus stop id.");
	        return 1;
	    }
     	format(string, sizeof(string), "Bus stop (SampID: %d, UID: %d) has been deleted.", busstop_id, BusStopData[busstop_id][bUID]);

	    DeleteBusStop(busstop_id);

	    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", string, "OK", "");
	    return 1;
	}
	return 1;
}

COMMAND:pflags(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new targetid;
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pflags [id]");
	
	if( !IsPlayerConnected(targetid) || !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	
	DynamicGui_Init(playerid);
	DynamicGui_SetDialogValue(playerid, targetid);
	new str[256];
	
	format(str, sizeof(str), "%s%s01\tBMX\n", str, ((PlayerHasFlag(targetid, PLAYER_FLAG_BMX)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, PLAYER_FLAG_BMX);
	
	format(str, sizeof(str), "%s%s02\tOrdering\n", str, ((PlayerHasFlag(targetid, PLAYER_FLAG_ORDER)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, PLAYER_FLAG_ORDER);

	format(str, sizeof(str), "%s%s03\tMayor\n", str, ((PlayerHasFlag(targetid, PLAYER_FLAG_PRESIDENT)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, PLAYER_FLAG_PRESIDENT);

	format(str, sizeof(str), "%s%s04\tHelper\n", str, ((PlayerHasFlag(targetid, PLAYER_FLAG_HELPER)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, PLAYER_FLAG_HELPER);
	
	ShowPlayerDialog(playerid, DIALOG_GIVE_FLAG, DIALOG_STYLE_LIST, sprintf("Flags of %s [Global: %d, UID: %d, ID: %d]", pInfo[targetid][player_name], gInfo[targetid][global_id], pInfo[targetid][player_id], targetid), str, "Choose", "Close");
	return 1;
}

COMMAND:aflags(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new targetid;
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aflags [id]");
	
	if( !IsPlayerConnected(targetid) || !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	
	DynamicGui_Init(playerid);
	DynamicGui_SetDialogValue(playerid, targetid);
	new str[500];
	
	format(str, sizeof(str), "%s%s01\tGamemaster rank\n", str, ((HasCrewFlag(targetid, CREW_FLAG_GM)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_GM);
	
	format(str, sizeof(str), "%s%s02\tSupporter rank\n", str, ((HasCrewFlag(targetid, CREW_FLAG_ADMIN)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_ADMIN);
	
	format(str, sizeof(str), "%s%s03\tHead admin rank\n", str, ((HasCrewFlag(targetid, CREW_FLAG_ADMIN_ROOT)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_ADMIN_ROOT);
	
	format(str, sizeof(str), "%s%s04\tDoor managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_DOORS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_DOORS);
	
	format(str, sizeof(str), "%s%s05\tVehicle managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_VEHICLES)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_VEHICLES);
	
	format(str, sizeof(str), "%s%s06\tGroup managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_GROUPS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_GROUPS);
	
	format(str, sizeof(str), "%s%s07\tArea managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_AREAS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_AREAS);
	
	format(str, sizeof(str), "%s%s08\tObject managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_EDITOR)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_EDITOR);
	
	format(str, sizeof(str), "%s%s09\tItem managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_ITEMS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_ITEMS);
	
	ShowPlayerDialog(playerid, DIALOG_GIVE_CREW, DIALOG_STYLE_LIST, sprintf("Crew flags of %s [Global: %d, UID: %d, ID: %d]", pInfo[targetid][player_name], gInfo[targetid][global_id], pInfo[targetid][player_id], targetid), str, "Wybierz", "Zamknij");
	return 1;
}

COMMAND:duty(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	if( pInfo[playerid][player_admin_duty] )
	{
		strcopy(pInfo[playerid][player_name], pInfo[playerid][player_duty_tmp], MAX_PLAYER_NAME+1);
		pInfo[playerid][player_duty_tmp][0] = EOS;
		
		UpdatePlayerLabel(playerid);
		
		pInfo[playerid][player_admin_duty] = false;
		
		new duty_time = gettime() - pInfo[playerid][player_admin_duty_start] - pInfo[playerid][player_admin_duty_afk_time];
		
		SendClientMessage(playerid, 0xD7A064FF, sprintf("You've left admin duty. Time on duty: %d min %d sec.", floatround(duty_time/60, floatround_floor), duty_time%60));
		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_game_sessions` (session_uid, session_type, session_extraid, session_owner, session_start, session_end) VALUES (null, %d, %d, %d, %d, %d)", SESSION_TYPE_ADMIN_DUTY, 0, pInfo[playerid][player_id], pInfo[playerid][player_admin_duty_start], (gettime() - pInfo[playerid][player_admin_duty_afk_time])));
		
		pInfo[playerid][player_admin_duty_afk_time] = 0;
		pInfo[playerid][player_admin_duty_start] = 0;
	}
	else
	{
		if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASKA) > -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "You can't enter admin duty with mask on you.");
		new name[MAX_PLAYER_NAME+1];
   		GetPlayerName(playerid, name, sizeof(name));

		strcopy(pInfo[playerid][player_duty_tmp], name, MAX_PLAYER_NAME+1);
		strcopy(pInfo[playerid][player_name], pGlobal[playerid][glo_name], MAX_PLAYER_NAME+1);
		EscapePL(pInfo[playerid][player_name]);
		
		UpdatePlayerLabel(playerid);
		
		pInfo[playerid][player_admin_duty] = true;
		pInfo[playerid][player_admin_duty_start] = gettime();

		if( HasCrewFlag(playerid, CREW_FLAG_ITEMS) )
		{
			pInfo[playerid][player_capacity] = 50000;
		}
		
		SendClientMessage(playerid, 0xD7A064FF, "Admin duty enabled. To leave, use command again.");
	}
	
	return 1;
}

COMMAND:atime(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new time;
	if( sscanf(params, "d", time) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /atime [time]");
	SetWorldTime(time);
	return 1;
}

COMMAND:aweather(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new weather;
	if( sscanf(params, "d", weather) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aweather [weather id]");
	
	foreach(new pid: Player)
	{
		if(GetPlayerVirtualWorld(playerid) != 0) continue;
		SetPlayerWeather(pid, weather);
	}
	WorldWeather = weather;
	return 1;
}

COMMAND:setskin(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new targetid, skin;
	if( sscanf(params, "ud", targetid, skin) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setskin [id] [skin]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	
	SetPlayerSkin(targetid, skin);
	pInfo[targetid][player_skin]= skin;
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_skin` = %d WHERE `char_uid` = %d", skin, pInfo[targetid][player_id]));
	SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s gave you skin id %d.", pInfo[playerid][player_name], skin);
	if( playerid != targetid ) SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "Skin has been set up for %s (UID: %d, ID: %d), skin %d.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, skin);
	
	return 1;
}

COMMAND:acto(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new cmdParam;
	if( sscanf(params, "d", cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /acto [actor id]");

	new
		Float:x,
		Float:y,
		Float:z;
	GetActorPos(cmdParam, x, y, z);
	
	RP_PLUS_SetPlayerPos(playerid, x, y, z);
	SetPlayerVirtualWorld(playerid, GetActorVirtualWorld(cmdParam));
	
	FreezePlayer(playerid, 2000);
	
	SendFormattedClientMessage(playerid, COLOR_GOLD, "Teleported to actor ID: %d.", cmdParam);

	return 1;
}

COMMAND:res(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new cmdParam;
	if( sscanf(params, "u", cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /res [id]");
	if( (!IsPlayerConnected(cmdParam) || !pInfo[cmdParam][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");

	if(pInfo[cmdParam][player_spawn_type] == SPAWN_TYPE_GLOBAL)
	{
		new spawn;
		spawn = Iter_Random(GlobalSpawns);	
		SetPlayerPos(cmdParam,  GlobalSpawn[spawn][gspawn_pos][0], GlobalSpawn[spawn][gspawn_pos][1], GlobalSpawn[spawn][gspawn_pos][2]);	
	}
	else if(pInfo[cmdParam][player_spawn_type] == SPAWN_TYPE_GLOBAL_SC)
	{
		new spawn;
		spawn = Iter_Random(ScSpawns);
		SetPlayerPos(cmdParam,  GlobalSpawn[spawn][gspawn_pos][0], GlobalSpawn[spawn][gspawn_pos][1], GlobalSpawn[spawn][gspawn_pos][2]);	
	}
	else if(pInfo[cmdParam][player_spawn_type] == SPAWN_TYPE_HOUSE)
	{
		new spawn = pInfo[cmdParam][player_spawn];
		new d_id = GetDoorByUid(spawn);
		if(d_id != -1)
		{
			SetPlayerPos(cmdParam, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
			SetPlayerVirtualWorld(cmdParam, Door[d_id][door_spawn_vw]);
			SetPlayerInterior(cmdParam, Door[d_id][door_spawn_int]);
		}
	}
	else if(pInfo[cmdParam][player_spawn_type] == SPAWN_TYPE_HOTEL)
	{
		new spawn = pInfo[cmdParam][player_spawn];
		new d_id = GetDoorByUid(spawn);
		if(d_id != -1)
		{
			SetPlayerPos(cmdParam, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
			SetPlayerVirtualWorld(cmdParam, Door[d_id][door_spawn_vw]);
			SetPlayerInterior(cmdParam, Door[d_id][door_spawn_int]);
		}
	}

	FreezePlayer(cmdParam, 2000);

	SendFormattedClientMessage(playerid, COLOR_GOLD, "Player %s (ID: %d, UID: %d) has been reseted.", pInfo[cmdParam][player_name], cmdParam, pInfo[cmdParam][player_id]);

	return 1;
}

COMMAND:goto(playerid, params[])
{
	return cmd_to(playerid, params);
}

COMMAND:to(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new cmdParam;
	if( sscanf(params, "u", cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /to [id]");
	if( (!IsPlayerConnected(cmdParam) || !pInfo[cmdParam][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");

	new
		Float:x,
		Float:y,
		Float:z;
	GetPlayerPos(cmdParam, x, y, z);
	
	RP_PLUS_SetPlayerPos(playerid, x, y, z);
	SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(cmdParam));
	SetPlayerInterior(playerid, GetPlayerInterior(cmdParam));
	
	FreezePlayer(playerid, 2000);
	

	SendFormattedClientMessage(playerid, COLOR_GOLD, "Teleported to player %s (ID: %d, UID: %d).", pInfo[cmdParam][player_name], cmdParam, pInfo[cmdParam][player_id]);

	return 1;
}

COMMAND:gethere(playerid, params[])
{
	return cmd_tm(playerid, params);
}

COMMAND:takecash(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new cmdParam, cash;
	if( sscanf(params, "ud", cmdParam, cash) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /takecash [id] [amount]");
	if( (!IsPlayerConnected(cmdParam) || !pInfo[cmdParam][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");

	GivePlayerMoney(cmdParam, -cash);
	
	SendFormattedClientMessage(cmdParam, COLOR_GOLD, "Administrator %s has took $%d from you.", pInfo[playerid][player_name], cash);
	SendFormattedClientMessage(playerid, COLOR_GOLD, "You've took $%d from %s (ID: %d, UID: %d).", cash, pInfo[cmdParam][player_name], cmdParam, pInfo[cmdParam][player_id]);

	return 1;
}

COMMAND:tm(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new cmdParam;
	if( sscanf(params, "u", cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /tm [id]");
	if( (!IsPlayerConnected(cmdParam) || !pInfo[cmdParam][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");

	new
		Float:x,
		Float:y,
		Float:z;
	GetPlayerPos(playerid, x, y, z);
	
	RP_PLUS_SetPlayerPos(cmdParam, x, y, z);
	SetPlayerVirtualWorld(cmdParam, GetPlayerVirtualWorld(playerid));
	SetPlayerInterior(cmdParam, GetPlayerInterior(playerid));
	
	FreezePlayer(cmdParam, 2000);
	
	SendFormattedClientMessage(cmdParam, COLOR_GOLD, "Administrator %s teleported you to him.", pInfo[playerid][player_name]);
	SendFormattedClientMessage(playerid, COLOR_GOLD, "You've teleported %s (ID: %d, UID: %d) to you.", pInfo[cmdParam][player_name], cmdParam, pInfo[cmdParam][player_id]);

	return 1;
}

COMMAND:tp(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new cmdParam[2];
	if( sscanf(params, "a<i>[2]", cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /tp [who] [to who]");
	if( (!IsPlayerConnected(cmdParam[0]) || !pInfo[cmdParam[0]][player_logged]) || (!IsPlayerConnected(cmdParam[1]) || !pInfo[cmdParam[1]][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "One of players is not logged.");
	
	new
		Float:x,
		Float:y,
		Float:z;
	GetPlayerPos(cmdParam[1], x, y, z);
	
	RP_PLUS_SetPlayerPos(cmdParam[0], x, y, z);
	SetPlayerVirtualWorld(cmdParam[0], GetPlayerVirtualWorld(cmdParam[1]));
	SetPlayerInterior(cmdParam[0], GetPlayerInterior(cmdParam[1]));
	
	FreezePlayer(cmdParam[0], 2500);
	
	if( cmdParam[0] == playerid )
	{
		SendFormattedClientMessage(cmdParam[0], COLOR_GOLD, "Teleported you to %s (ID: %d, UID: %d).", pInfo[cmdParam[1]][player_name], cmdParam[1], pInfo[cmdParam[1]][player_id]);
	}				
	else
	{
		SendFormattedClientMessage(cmdParam[0], COLOR_GOLD, "%s teleported you to %s (ID: %d, UID: %d).", pInfo[playerid][player_name], pInfo[cmdParam[1]][player_name], cmdParam[1], pInfo[cmdParam[1]][player_id]);
	}

	return 1;
}

COMMAND:aspawn(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn [create/delete]");
	
	if( !strcmp(sub_cmd, "create", false) )
	{
		new type;
		if( sscanf(param, "d", type) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn create [type]");

		if(type <1 || type > 2) return SendClientMessage(playerid, COLOR_GREY, "Types: 1(Market), 2(Idlewood)");

		new Float:pPos[4];
		
		GetPlayerPos(playerid, pPos[0], pPos[1], pPos[2]);
		GetPlayerFacingAngle(playerid, pPos[3]);
		
		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_gspawns` (gspawn_uid, gspawn_posx, gspawn_posy, gspawn_posz, gspawn_posa, gspawn_type) VALUES (null, %f, %f, %f, %f, %d)", pPos[0], pPos[1], pPos[2], pPos[3], type));
		
		new id = cache_insert_id();
		new i;

		if(type == SPAWN_TYPE_GLOBAL)
		{
		 	i = Iter_Free(GlobalSpawns);
		}
		else
		{
			i = Iter_Free(ScSpawns);
		}

		GlobalSpawn[i][gspawn_id] = id;
		GlobalSpawn[i][gspawn_pos][0] = pPos[0];
		GlobalSpawn[i][gspawn_pos][1] = pPos[1];
		GlobalSpawn[i][gspawn_pos][2] = pPos[2];
		GlobalSpawn[i][gspawn_pos][3] = pPos[3];
		
		if(type == SPAWN_TYPE_GLOBAL)
		{
		 	Iter_Add(GlobalSpawns, i);
		 	SendFormattedClientMessage(playerid, COLOR_GOLD, "Added new global spawn (Market) (ID: %d).", i);
		}
		else
		{
			Iter_Add(ScSpawns, i);
			SendFormattedClientMessage(playerid, COLOR_GOLD, "Added new global spawn (South Central) (ID: %d).", i);
		}
	}
	else if( !strcmp(sub_cmd, "delete") )
	{
		new id;
		if( sscanf(param, "d", id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn usun [id spawnu]");
		if( id < 0 || id >= MAX_GLOBAL_SPAWNS ) return SendClientMessage(playerid, COLOR_GREY, "Invalid spawn id.");
		if( !Iter_Contains(GlobalSpawns, id) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid spawn id.");
		
		mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_gspawns` WHERE `gspawn_uid` = %d", GlobalSpawn[id][gspawn_id]));
		
		for(new z=0; e_global_spawns:z != e_global_spawns; z++)
		{
			GlobalSpawn[id][e_global_spawns:z] = 0;
		}
		
		Iter_Remove(GlobalSpawns, id);
		
		SendFormattedClientMessage(playerid, COLOR_GOLD, "Deleted global spawn (ID: %d).", id);
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn [create/delete]");
	
	return 1;
}

COMMAND:glob(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) && !HasCrewFlag(playerid, CREW_FLAG_GM) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new text[290];
	if( sscanf(params, "s[290]", text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /glob [text]");
	
	format(text, sizeof(text), "[[ %s: %s ]]", pInfo[playerid][player_name], BeautifyString(text));
	foreach(new p : Player)
	{
		if( !pInfo[p][player_logged] ) continue;
		SendClientMessage(p, COLOR_WHITE, text);
	}
	
	return 1;
}

COMMAND:ac(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ac [text]");
	
	new text[200];
	if( HasCrewFlag(playerid, CREW_FLAG_GM) ) format(text, sizeof(text), "(( [AC] %s%s{EFE7AD}: %s ))", HEX_COLOR_CREW_GM, pInfo[playerid][player_name], params);
	if( HasCrewFlag(playerid, CREW_FLAG_ADMIN) ) format(text, sizeof(text), "(( [AC] %s%s{EFE7AD}: %s ))", HEX_COLOR_CREW_ADMIN, pInfo[playerid][player_name], params);
	if( HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) format(text, sizeof(text), "(( [AC] %s%s{EFE7AD}: %s ))", HEX_COLOR_CREW_ADMIN_ROOT, pInfo[playerid][player_name], params);
	
	foreach(new p : Player)
	{
		if( !pInfo[p][player_logged] ) continue;
		if( !HasCrewFlag(p, CREW_FLAG_GM) && !HasCrewFlag(p, CREW_FLAG_ADMIN) && !HasCrewFlag(p, CREW_FLAG_ADMIN_ROOT) ) continue;
		
		SendClientMessage(p, 0xEFE7ADFF, text);
	}
	
	return 1;
}

COMMAND:hp(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	new targetid, health;
	if( sscanf(params, "ud", targetid, health) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /hp [player id] [hp]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id..");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	if( health < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /hp [player id] [hp]");
	
	SetPlayerHealth(targetid, health);
	SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s has gave you %d hp.", pInfo[playerid][player_name], health);
	if( playerid != targetid ) SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "You've gave %s (UID: %d, ID: %d) %d HP.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, health);
	
	return 1;
}

COMMAND:ad(playerid, params[])
{
	return cmd_adoor(playerid, params);
}

COMMAND:adoor(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_DOORS) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor [create/delete/pickup/rent/flat/mapicon/goto/assign/interior/enter/exitvw/objects/reload]");
	
	
	if( !strcmp(sub_cmd, "create", false) )
	{
		new name[50];
		if( sscanf(param, "s[50]", name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor create [name]");
		
		new Float:pPos[4];
		
		GetPlayerPos(playerid, pPos[0], pPos[1], pPos[2]);
		GetPlayerFacingAngle(playerid, pPos[3]);
		mysql_real_escape_string(name, name);

		new str[400];
		strcat(str, "INSERT INTO ipb_doors (door_uid, door_owner, door_ownertype, door_name, door_type, door_pickupid, door_enterx, door_entery, door_enterz, door_entera, door_entervw, door_enterint, door_exitx, door_exity, door_exitz, door_exita) ");
		strcat(str, sprintf("VALUES (null, 0, 0, '%s', 1, 1239, %f, %f, %f, %f, %d, %d, %f, %f, %f, %f)", name, pPos[0], pPos[1], pPos[2], pPos[3], GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), pPos[0], pPos[1], pPos[2], pPos[3]));
		
		mysql_query(mySQLconnection, str);
		new uid = cache_insert_id();
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);

		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Door created. [UID: %d, ID: %d]", uid, did));
	}
	else if( !strcmp(sub_cmd, "house", false) )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor house [door id]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor house [door id]");

		if(Door[d_id][door_buyable] == 0)
		{
			SendGuiInformation(playerid, ""guiopis"Information", "Building is now for sale.");
			Door[d_id][door_buyable]=1;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_buyable` = 1 WHERE `door_uid` = %d", Door[d_id][door_uid]));
		}
		else
		{
			SendGuiInformation(playerid, ""guiopis"Information", "Building cant be buyed now.");
			Door[d_id][door_buyable]=0;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_buyable` = 0 WHERE `door_uid` = %d", Door[d_id][door_uid]));
		}
	}
	else if( !strcmp(sub_cmd, "id", false) )
	{
		new d_uid;
		if( sscanf(param, "d", d_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor id [door uid]");
		new d_id = GetDoorByUid(d_uid);
		if(d_id == -1) return SendGuiInformation(playerid, "Information", "These doors are not exist.");
		SendClientMessage(playerid, COLOR_GOLD, sprintf("SAMPid of door UID %d is: %d", d_uid, d_id));
	}
	else if( !strcmp(sub_cmd, "price", false) )
	{
		new d_id, czynsz;
		if( sscanf(param, "dd", d_id, czynsz) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor price [door id] [price]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor price [door id] [price]");

		if(Door[d_id][door_buyable] == 1)
		{
			Door[d_id][door_price]=czynsz;
			SendGuiInformation(playerid, ""guiopis"Information", "Building has new price.");
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_price` = %d WHERE `door_uid` = %d", czynsz, Door[d_id][door_uid]));

		}
		else
		{
			SendGuiInformation(playerid, ""guiopis"Information", "You can't set up price, this building is not for sale.");
		}
	}
	else if( !strcmp(sub_cmd, "flat", false) )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor flat [door id]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor flat [door id]");

		if(Door[d_id][door_rentable] == 0)
		{
			SendGuiInformation(playerid, ""guiopis"Information", "This building is now flat and can be rented.");
			Door[d_id][door_rentable]=1;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_rentable` = 1 WHERE `door_uid` = %d", Door[d_id][door_uid]));
		}
		else
		{
			SendGuiInformation(playerid, ""guiopis"Information", "This buildings is no more flat.");
			Door[d_id][door_rentable]=0;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_rentable` = 0 WHERE `door_uid` = %d", Door[d_id][door_uid]));
		}
	}
	else if( !strcmp(sub_cmd, "rent", false) )
	{
		new d_id, czynsz;
		if( sscanf(param, "dd", d_id, czynsz) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor rent [door id] [price]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor rent [door id] [price]");

		if(Door[d_id][door_rentable] == 1)
		{
			Door[d_id][door_rent]=czynsz;
			SendGuiInformation(playerid, ""guiopis"Information", "Building has now new renting price.");
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_rent` = %d WHERE `door_uid` = %d", czynsz, Door[d_id][door_uid]));

		}
		else
		{
			SendGuiInformation(playerid, ""guiopis"Information", "You can't set up renting price for this building. It's not a flat.");
		}
	}
	else if( !strcmp(sub_cmd, "delete", false) )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor delete [door id]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor delete [door id]");

		mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_spawn = 0, char_spawn_type = 1 WHERE char_spawn_type = %d AND char_spawn = %d", pInfo[playerid][player_spawn_type] == SPAWN_TYPE_HOUSE, Door[d_id][door_uid]));
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Deleted door with UID %d.", Door[d_id][door_uid]));
		printf("[ADMIN] %s (UID %d) removed door UID %d. (%s)", pInfo[playerid][player_name], pInfo[playerid][player_id], Door[d_id][door_uid], Door[d_id][door_name]);
		DeleteDoor(d_id);
	}
	else if( !strcmp(sub_cmd, "pickup") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor pickup [door id]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor pickup [door id]");
		
		new str[512];
		
		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, d_id);
		
		format(str, sizeof(str), "%s01\tHeart (1240)\n", str);
		DynamicGui_AddRow(playerid, 0, 1240);
		
		format(str, sizeof(str), "%s02\tKevlar (1242)\n", str);
		DynamicGui_AddRow(playerid, 0, 1242);
		
		format(str, sizeof(str), "%s03\t'i' (1239)\n", str);
		DynamicGui_AddRow(playerid, 0, 1239);
		
		format(str, sizeof(str), "%s04\tBlue house (1272)\n", str);
		DynamicGui_AddRow(playerid, 0, 1272);
		
		format(str, sizeof(str), "%s05\tGreen house (1273)\n", str);
		DynamicGui_AddRow(playerid, 0, 1273);
		
		format(str, sizeof(str), "%s06\tCash (1212)\n", str);
		DynamicGui_AddRow(playerid, 0, 1212);
		
		format(str, sizeof(str), "%s07\tPill(1241)\n", str);
		DynamicGui_AddRow(playerid, 0, 1241);
		
		format(str, sizeof(str), "%s08\tBadge (1247)\n", str);
		DynamicGui_AddRow(playerid, 0, 1247);
		
		format(str, sizeof(str), "%s09\tDollar (1274)\n", str);
		DynamicGui_AddRow(playerid, 0, 1274);
		
		format(str, sizeof(str), "%s10\tBlue shirt (1275)\n", str);
		DynamicGui_AddRow(playerid, 0, 1275);

		format(str, sizeof(str), "%s11\tBench (3072)\n", str);
		DynamicGui_AddRow(playerid, 0, 3072);

		format(str, sizeof(str), "%s12\tHuman (1314)\n", str);
		DynamicGui_AddRow(playerid, 0, 1314);
		
		format(str, sizeof(str), "%s13\tFor sale (19470)\n", str);
		DynamicGui_AddRow(playerid, 0, 19470);

		format(str, sizeof(str), "%s14\tHammer (18635)\n", str);
		DynamicGui_AddRow(playerid, 0, 18635);

		format(str, sizeof(str), "%s15\tCD (1961)\n", str);
		DynamicGui_AddRow(playerid, 0, 1961);

		format(str, sizeof(str), "%s16\tRed house (19522)\n", str);
		DynamicGui_AddRow(playerid, 0, 19522);

		format(str, sizeof(str), "%s17\tYellow house (19524)\n", str);
		DynamicGui_AddRow(playerid, 0, 19524);

		format(str, sizeof(str), "%s18\tCase (19624)\n", str);
		DynamicGui_AddRow(playerid, 0, 19624);

		format(str, sizeof(str), "%s19\tDisc (1277)\n", str);
		DynamicGui_AddRow(playerid, 0, 1277);

		ShowPlayerDialog(playerid, DIALOG_ADRZWI_PICKUP, DIALOG_STYLE_LIST, sprintf("Door %d (%s; SampID: %d)", Door[d_id][door_uid], Door[d_id][door_name], d_id), str, "Choose", "Close");
	}
	else if( !strcmp(sub_cmd, "mapicon") )
	{
		new d_id, model;
		if( sscanf(param, "dd", d_id, model) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor mapicon [door id] [map icon model(-1 to delete)]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor mapicon [door id] [map icon model(-1 to delete)]");	
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_mapmodel` = %d WHERE `door_uid` = %d", model, Door[d_id][door_uid]));
			
		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Map icon has been changed! [ICON: %d, UID: %d, ID: %d]", model, uid, did));
	}
	else if( !strcmp(sub_cmd, "goto") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor goto [door id]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor goto [door id]");
		
		FreezePlayer(playerid, 2500);
				
		RP_PLUS_SetPlayerPos(playerid, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
		SetPlayerFacingAngle(playerid, Door[d_id][door_pos][3]);
		
		SetCameraBehindPlayer(playerid);
		
		SetPlayerVirtualWorld(playerid, Door[d_id][door_vw]);
		SetPlayerInterior(playerid, Door[d_id][door_int]);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Teleported to door [UID: %d, ID: %d].", Door[d_id][door_uid], d_id));
	}
	else if( !strcmp(sub_cmd, "assign") )
	{
		new d_id, action[32], who;
		if( sscanf(param, "ds[32]d", d_id, action, who) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor assign [door id] [group/player/global] [group uid/player id(not uid)]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor assign [door id] [group/player] [group uid/player id(not uid)]");
		
		if( !strcmp(action, "group") )
		{
			new gid = GetGroupByUid(who);
			if( gid == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Group with that uid is not exist.");
			
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_ownertype` = %d, `door_owner` = %d WHERE `door_uid` = %d", DOOR_OWNER_TYPE_GROUP, who, Door[d_id][door_uid]));
			
			Door[d_id][door_owner_type] = DOOR_OWNER_TYPE_GROUP;
			Door[d_id][door_owner] = who;
			
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Door [UID: %d, ID: %d] has been assigned to %s [UID: %d, ID: %d].", Door[d_id][door_uid], d_id, Group[gid][group_name], Group[gid][group_uid], gid));
		}
		else if( !strcmp(action, "player") )
		{
			if( !IsPlayerConnected(who) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id..");
			
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_ownertype` = %d, `door_owner` = %d WHERE `door_uid` = %d", DOOR_OWNER_TYPE_PLAYER, pInfo[who][player_id], Door[d_id][door_uid]));
			
			Door[d_id][door_owner_type] = DOOR_OWNER_TYPE_PLAYER;
			Door[d_id][door_owner] = pInfo[who][player_id];
			
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Door [UID: %d, ID: %d] has been assigned to %s [UID: %d, ID: %d].", Door[d_id][door_uid], d_id, pInfo[who][player_name], pInfo[who][player_id], who));
		}
		else if( !strcmp(action, "global") )
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_ownertype` = %d, `door_owner` = 0 WHERE `door_uid` = %d", DOOR_OWNER_TYPE_GLOBAL, Door[d_id][door_uid]));
			
			Door[d_id][door_owner_type] = DOOR_OWNER_TYPE_GLOBAL;
			Door[d_id][door_owner] = 0;
			
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Door [UID: %d, ID: %d] are global now.", Door[d_id][door_uid], d_id));
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor assign [door id] [group/player] [group uid/player id(not uid)]");	
	}
	else if( !strcmp(sub_cmd, "interior") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor interior [door id]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor interior [door id]");
		
		DoorsDefaultInteriorsList(playerid, d_id);
	}
	else if( !strcmp(sub_cmd, "enter") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor enter [door id]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor enter [door id]");
		
		GetPlayerPos(playerid, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
		GetPlayerFacingAngle(playerid, Door[d_id][door_pos][3]);
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_enterx` = %f, `door_entery` = %f, `door_enterz` = %f, `door_entera` = %f, door_entervw = %d WHERE `door_uid` = %d", Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2], Door[d_id][door_pos][3], GetPlayerVirtualWorld(playerid), Door[d_id][door_uid]));		
		
		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Entrace of door [UID: %d, ID: %d] has been changed to your current position.", Door[d_id][door_uid], did));
	}
	else if( !strcmp(sub_cmd, "exit") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor exit [door id]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor exit [door id]");
		
		GetPlayerPos(playerid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
		GetPlayerFacingAngle(playerid, Door[d_id][door_spawn_pos][3]);
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_exitx` = %f, `door_exity` = %f, `door_exitz` = %f, `door_exita` = %f WHERE `door_uid` = %d", Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], Door[d_id][door_spawn_pos][3], Door[d_id][door_uid]));		
		
		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Exit position of door [UID: %d, ID: %d] has been changed to your current.", Door[d_id][door_uid], did));
	}
	else if( !strcmp(sub_cmd, "objects") )
	{
		new d_id, limit;
		if( sscanf(param, "dd", d_id, limit) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor objects [door id] [new limit]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor objects [door id] [new limit]");
		if( limit < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor objects [door id] [new limit]");
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_objects` = %d WHERE `door_uid` = %d", limit, Door[d_id][door_uid]));
			
		Door[d_id][door_objects_limit] = limit;
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Objet limit of door [LIMIT: %d, UID: %d, ID: %d] has been changed.", limit, Door[d_id][door_uid], d_id));	
	}
	else if( !strcmp(sub_cmd, "reload") )
	{
		Code_ExTimer_Begin(DoorsReload);
		for(new d_id;d_id<MAX_DOORS;d_id++)
		{
			if( Iter_Contains(Doors, d_id) ) DeleteDoor(d_id, false);
		}
		
		new count = LoadDoor();

		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Reloaded all doors [amount: %d, time: %d ms].", count, Code_ExTimer_End(DoorsReload)));	
	}
	else if( !strcmp(sub_cmd, "exitvw") )
	{
		new d_id, vw;
		if( sscanf(param, "dd", d_id, vw) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor exitvw [door id] [vw]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor exitvw [door id] [vw]");
		if( vw < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor exitvw [door id] [vw]");
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_exitvw` = %d WHERE `door_uid` = %d", vw, Door[d_id][door_uid]));
		
		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Virtual world of door has been changed [EXITVW: %d, UID: %d, ID: %d].", vw, Door[did][door_uid], did));
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)oor [create/delete/pickup/rent/flat/mapicon/goto/assign/interior/enter/exitvw/objects/reload]");
	return 1;
}

COMMAND:area(playerid, params[])
{	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) )
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /area [audio]");
		}
		else
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /area [create/delete/assign/check/objects/flags]");
		}
		return 1;
	}
	
	if( !strcmp(sub_cmd, "create") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Information", "No access to this command..");
		if( pInfo[playerid][player_lookup_area] ) return SendClientMessage(playerid, COLOR_GREY, "You are currently looking up area.");
		if( pInfo[playerid][player_creating_area] )
		{
			if( pInfo[playerid][player_carea_type] == AREA_SHAPE_SQUARE )
			{
				if( pInfo[playerid][player_carea_point2][0] == 0.0 && pInfo[playerid][player_carea_point2][1] == 0.0 && pInfo[playerid][player_carea_point2][2] == 0.0 ) return ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "Aby stworzyæ prostok¹tn¹ strefê musisz wyznaczyæ jej dwa punkty tworz¹ce przek¹tn¹.", "OK", "");	
			
				new str[400];
				strcat(str, "INSERT INTO `ipb_areas` (area_uid, area_shape, area_type, area_ownertype, area_owner, area_objects, area_vw, area_size, area_point1, area_point2) ");
				strcat(str, sprintf("VALUES(null, %d, %d, %d, 0, 0, %d, 0.0, '%f|%f|%f', '%f|%f|%f')", AREA_SHAPE_SQUARE, AREA_TYPE_NORMAL, AREA_OWNER_TYPE_GLOBAL, GetPlayerVirtualWorld(playerid), pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2], pInfo[playerid][player_carea_point2][0], pInfo[playerid][player_carea_point2][1], pInfo[playerid][player_carea_point2][2]));
				mysql_query(mySQLconnection, str);
				
				new uid = cache_insert_id();
				new a_id = LoadArea(sprintf("WHERE `area_uid` = %d", uid), true);
				
				pInfo[playerid][player_carea_point1][0] = 0.0;
				pInfo[playerid][player_carea_point1][1] = 0.0;
				pInfo[playerid][player_carea_point1][2] = 0.0;
				
				pInfo[playerid][player_carea_point2][0] = 0.0;
				pInfo[playerid][player_carea_point2][1] = 0.0;
				pInfo[playerid][player_carea_point2][2] = 0.0;
				
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
				
				GangZoneDestroy(pInfo[playerid][player_carea_zone]);
				
				pInfo[playerid][player_creating_area] = false;
				SendPlayerInformation(playerid, "", 0);
				
				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Created rectangle area [UID: %d, ID: %d].", uid, a_id), "OK", "");
			}
			else if( pInfo[playerid][player_carea_type] == AREA_SHAPE_CIRCLE )
			{
				if( pInfo[playerid][player_carea_point1][0] == 0.0 && pInfo[playerid][player_carea_point1][1] == 0.0 && pInfo[playerid][player_carea_point1][2] == 0.0 ) return ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "Aby stworzyÃ¦ sferycznÂ¹ strefê musisz wyznaczyÃ¦ punkt srodka sfery.", "OK", "");	
			
				new str[400];
				strcat(str, "INSERT INTO `ipb_areas` (area_uid, area_shape, area_type, area_ownertype, area_owner, area_objects, area_vw, area_size, area_point1, area_point2) ");
				strcat(str, sprintf("VALUES(null, %d, %d, %d, 0, 0, %d, %f, '%f|%f|%f', '')", AREA_SHAPE_CIRCLE, AREA_TYPE_NORMAL, AREA_OWNER_TYPE_GLOBAL, GetPlayerVirtualWorld(playerid), pInfo[playerid][player_carea_size], pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2]));
				mysql_query(mySQLconnection, str);
			
				new uid = cache_insert_id();
				new a_id = LoadArea(sprintf("WHERE `area_uid` = %d", uid), true);
				
				pInfo[playerid][player_carea_point1][0] = 0.0;
				pInfo[playerid][player_carea_point1][1] = 0.0;
				pInfo[playerid][player_carea_point1][2] = 0.0;
				
				pInfo[playerid][player_carea_point2][0] = 0.0;
				pInfo[playerid][player_carea_point2][1] = 0.0;
				pInfo[playerid][player_carea_point2][2] = 0.0;
				
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
				
				GangZoneDestroy(pInfo[playerid][player_carea_zone]);
				
				pInfo[playerid][player_creating_area] = false;
				SendPlayerInformation(playerid, "", 0);
				
				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Created circle area [UID: %d, ID: %d].", uid, a_id), "OK", "");
			}
			
			
			return 1;
		}
		
		new type[32], Float:size;
		if( sscanf(param, "s[32]F(0.0)", type, size) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area create [rectangle/circle]");
		
		if( !strcmp(type, "circle") )
		{
			if( size == 0.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area create circle [size]");
			
			pInfo[playerid][player_carea_size] = size;
			pInfo[playerid][player_carea_type] = AREA_SHAPE_CIRCLE;
			pInfo[playerid][player_creating_area] = true;
			SendPlayerInformation(playerid, "To add area point press ~r~RMB~w~ or ~r~LMB ~w~to delete last point. ~n~To cancel press ~g~LALT + SPACE~w~.~n~~n~~y~To finish creator use /area create again.", 30000);
		}
		else if( !strcmp(type, "rectangle") )
		{
			pInfo[playerid][player_carea_type] = AREA_SHAPE_SQUARE;
			pInfo[playerid][player_creating_area] = true;
			SendPlayerInformation(playerid, "To add area point press ~r~RMB~w~ or ~r~LMB ~w~to delete last point. ~n~To cancel press ~g~LALT + SPACE~w~.~n~~n~~y~To finish creator use /area create again.", 30000);
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /area create [rectangle/circle]");
	}
	else if( !strcmp(sub_cmd, "audio") )
	{
		new audio_stream[100], a_id;
		if( sscanf(param, "s[100]d", audio_stream, a_id) )
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /area audio [area id] [link]");
			SendClientMessage(playerid, COLOR_GREY, "Tip: Type '-' to remove audio stream from area.");
			return 1;
		}
		if(!CanPlayerEditArea(playerid, a_id)) return SendGuiInformation(playerid, "Information", "No access to this area.");
		sscanf(audio_stream, "s[100]", Area[a_id][area_audio]);
		SendGuiInformation(playerid, "Information", "Audio stream changed.");
	}
	else if( !strcmp(sub_cmd, "mapicon") )
	{
		new a_id, model;
		if( sscanf(param, "dd", a_id, model) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area mapicon [area id] [map icon model(-1 to delete)]");	
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area mapicon [area id] [map icon model(-1 to delete)]");	
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_areas` SET `area_mapicon` = %d WHERE `area_uid` = %d", model, Area[a_id][area_uid]));
			
		new uid = Area[a_id][area_uid];
		DeleteArea(a_id, false);
		
		new aid = LoadArea(sprintf("WHERE `area_uid` = %d", uid), true);
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Map icon has been changed! [ICON: %d, UID: %d, ID: %d]", model, uid, aid));
	}
	else if( !strcmp(sub_cmd, "delete") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Information", "No access to this command..");
		new a_id;
		if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area delete [area id]");	
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area delete [area id]");	
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area delete [area id]");	
		
		foreach(new p : Player)
		{
			if( IsPlayerInDynamicArea(playerid, a_id) )
			{
				TextDrawHideForPlayer(playerid, AreaInfo[playerid]);
			}
		}
		
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Deleted area [UID: %d, ID: %d].", Area[a_id][area_uid], a_id), "OK", "");
		
		DeleteArea(a_id);
	}
	else if( !strcmp(sub_cmd, "assign") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Information", "No access to this command..");
		new a_id, action[32], who;
		if( sscanf(param, "ds[32]d", a_id, action, who) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area assign [area id] [group/player/global] [group uid/player id(not uid)]");
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area assign [area id] [group/player/global] [group uid/player id(not uid)]");
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area assign [area id] [group/player/global] [group uid/player id(not uid)]");
		
		if( !strcmp(action, "group") )
		{
			new gid = GetGroupByUid(who);
			if( gid == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Group with that uid is not exist.");
			
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_areas` SET `area_ownertype` = %d, `area_owner` = %d WHERE `area_uid` = %d", AREA_OWNER_TYPE_GROUP, who, Area[a_id][area_uid]));
			
			Area[a_id][area_owner_type] = AREA_OWNER_TYPE_GROUP;
			Area[a_id][area_owner] = who;
			
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Changed owner of area [UID: %d, ID: %d] to %s [UID: %d, ID: %d].", Area[a_id][area_uid], a_id, Group[gid][group_name], Group[gid][group_uid], gid), "OK", "");
		}
		else if( !strcmp(action, "player") )
		{
			if( !IsPlayerConnected(who) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");
			
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_areas` SET `area_ownertype` = %d, `area_owner` = %d WHERE `area_uid` = %d", AREA_OWNER_TYPE_PLAYER, pInfo[who][player_id], Area[a_id][area_uid]));
			
			Area[a_id][area_owner_type] = AREA_OWNER_TYPE_PLAYER;
			Area[a_id][area_owner] = pInfo[who][player_id];
			
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Changed owner of area [UID: %d, ID: %d] to %s [UID: %d, ID: %d].", Area[a_id][area_uid], a_id, pInfo[who][player_name], pInfo[who][player_id], who), "OK", "");
		}
		else if( !strcmp(action, "global") )
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_areas` SET `area_ownertype` = %d, `area_owner` = 0 WHERE `area_uid` = %d", AREA_OWNER_TYPE_GLOBAL, Area[a_id][area_uid]));
			
			Area[a_id][area_owner_type] = AREA_OWNER_TYPE_GLOBAL;
			Area[a_id][area_owner] = 0;
			
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Changed owner of area [UID: %d, ID: %d] to global.", Area[a_id][area_uid], a_id), "OK", "");
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /area assign [area id] [group/player/global] [group uid/player id(not uid)]");
	}
	else if( !strcmp(sub_cmd, "objects") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Information", "No access to this command.");
		new a_id, limit;
		if( sscanf(param, "dd", a_id, limit) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area objects [area id] [object limit]");	
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area objects [area id] [object limit]");	
		if( limit < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area objects [area id] [object limit]");	
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area objects [area id] [object limit]");	
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_areas` SET `area_objects` = %d WHERE `area_uid` = %d", limit, Area[a_id][area_uid]));
			
		Area[a_id][area_objects_limit] = limit;
		
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Object limit changed [LIMIT: %d, UID: %d, ID: %d].", limit, Area[a_id][area_uid], a_id), "OK", "");	
	}
	else if( !strcmp(sub_cmd, "flags") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Information", "No access to this command.");
		new a_id;
		if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area flags [area id]");
		
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area flags [area id]");
		
		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, a_id);
		new str[700];
		
		format(str, sizeof(str), "%s%s01\tDrug dealing\n", str, ((AreaHasFlag(a_id, AREA_FLAG_CORNER)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_CORNER);
		
		format(str, sizeof(str), "%s%s02\tBMX\n", str, ((AreaHasFlag(a_id, AREA_FLAG_BMX)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_BMX);
		
		format(str, sizeof(str), "%s%s03\tMonitored\n", str, ((AreaHasFlag(a_id, AREA_FLAG_MONITORING)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_MONITORING);

		format(str, sizeof(str), "%s%s04\tDrive-thru\n", str, ((AreaHasFlag(a_id, AREA_FLAG_DRIVE)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_DRIVE);

		format(str, sizeof(str), "%s%s05\tPlanting\n", str, ((AreaHasFlag(a_id, AREA_FLAG_PLANT)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_PLANT);

		format(str, sizeof(str), "%s%s06\tNo parking\n", str, ((AreaHasFlag(a_id, AREA_FLAG_PARKING)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_PARKING);

		format(str, sizeof(str), "%s%s07\tProducts offering\n", str, ((AreaHasFlag(a_id, AREA_FLAG_OFFER)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_OFFER);

		format(str, sizeof(str), "%s%s08\tCar fix\n", str, ((AreaHasFlag(a_id, AREA_FLAG_SERWIS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_SERWIS);

		format(str, sizeof(str), "%s%s09\tLumberjack\n", str, ((AreaHasFlag(a_id, AREA_FLAG_WORK)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_WORK);

		format(str, sizeof(str), "%s%s10\tFisher\n", str, ((AreaHasFlag(a_id, AREA_FLAG_WORK_FISH)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_WORK_FISH);

		format(str, sizeof(str), "%s%s11\tAnty Jobcar\n", str, ((AreaHasFlag(a_id, AREA_FLAG_LS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_LS);

		format(str, sizeof(str), "%s%s12\tGolf\n", str, ((AreaHasFlag(a_id, AREA_FLAG_GOLF)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_GOLF);

		format(str, sizeof(str), "%s%s13\tCar junk\n", str, ((AreaHasFlag(a_id, AREA_FLAG_ZLOM)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_ZLOM);
		
		ShowPlayerDialog(playerid, DIALOG_AREA_FLAGS, DIALOG_STYLE_LIST, sprintf("Flags of area [UID: %d, ID: %d]", Area[a_id][area_uid], a_id), str, "Choose", "Close");
	}
	else if( !strcmp(sub_cmd, "clear") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Information", "No access to this command..");
		new a_id, object_count;
		if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area clear [area id]");	
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area clear [area id]");	
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area clear [area id]");	
		
		foreach(new o_id : Objects)
		{
			if( Object[o_id][object_owner_type] != OBJECT_OWNER_TYPE_AREA ) continue;
			if( Object[o_id][object_owner] == Area[a_id][area_uid] )
			{
				mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_objects` WHERE `object_uid` = %d", Object[o_id][object_uid]));

				DestroyDynamicObject(o_id);

				for(new z=0; e_objects:z != e_objects; z++)
				{
			  		Object[o_id][e_objects:z] = 0;
			    }
			    object_count++;
			    
			    new next;
			    Iter_SafeRemove(Objects, o_id, next);
			    o_id = next;
			}
		}
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Deleted %d object from area. [UID: %d, ID: %d].", object_count, Area[a_id][area_uid], a_id), "OK", "");
	}
	else if( !strcmp(sub_cmd, "check") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Information", "No access to this command.");
		if( pInfo[playerid][player_lookup_area] )
		{
			pInfo[playerid][player_lookup_area] = false;
			GangZoneDestroy(pInfo[playerid][player_carea_zone]);
			
			if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
			if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
			
			TextDrawHideForPlayer(playerid, Dashboard[playerid]);
		}
		else
		{
			new a_id;
			if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /area check [area id]");	
			if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid area id.");
			if( pInfo[playerid][player_creating_area] ) return SendClientMessage(playerid, COLOR_GREY, "You cant check area when you using area creator.");
			if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "You can't edit this type of area.");
			
			pInfo[playerid][player_lookup_area] = true;
			new string[140];
		
			new owner_str[80];
			switch( Area[a_id][area_owner_type] )
			{
				case AREA_OWNER_TYPE_GLOBAL:
				{
					format(owner_str, sizeof(owner_str), "global; none");
				}
				
				case AREA_OWNER_TYPE_PLAYER:
				{			
					new name[MAX_PLAYER_NAME+1];
					GetPlayerNameByUid(Area[a_id][area_owner], name);
					
					format(owner_str, sizeof(owner_str), "private; %s", name);
				}
				
				case AREA_OWNER_TYPE_GROUP:
				{			
					new gid = GetGroupByUid(Area[a_id][area_owner]);
					format(owner_str, sizeof(owner_str), "group; %s", Group[gid][group_name]);
				}
			}
			
			format(string, sizeof(string), "%d. Area (SampID: %d)~n~~n~~p~Owner: ~w~%d:%d (%s)    ~w~Objects: %d/%d", Area[a_id][area_uid], a_id, Area[a_id][area_owner_type], Area[a_id][area_owner], owner_str, CountAreaObjects(a_id), Area[a_id][area_objects_limit]);
				
			TextDrawSetString(Dashboard[playerid], string);
			TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			
			new rows, fields;
			new Float:point1[3], Float:point2[3], shape;
			mysql_query(mySQLconnection, sprintf("SELECT area_shape, area_point1, area_point2 FROM `ipb_areas` WHERE `area_uid` = %d", Area[a_id][area_uid]));
			cache_get_data(rows, fields);
			
			new str[100];
			cache_get_row(0, 1, str);
			sscanf(str, "p<|>a<f>[3]", point1);
			
			new str2[100];
			cache_get_row(0, 2, str2);
			sscanf(str2, "p<|>a<f>[3]", point2);
			
			shape = cache_get_row_int(0, 0);
			
			if( shape == AREA_SHAPE_SQUARE  )
			{
				pInfo[playerid][player_carea_zone] = GangZoneCreate(Min(point1[0], point2[0]), Min(point1[1], point2[1]), Max(point1[0], point2[0]), Max(point1[1], point2[1]));
				GangZoneShowForPlayer(playerid, pInfo[playerid][player_carea_zone], 0xFF3C3C80);

				pInfo[playerid][player_carea_label][0] = CreateDynamic3DTextLabel(sprintf("First point\n(%f, %f, %f)", point1[0], point1[1], point1[2]), COLOR_LIGHTER_RED, point1[0], point1[1], point1[2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
				pInfo[playerid][player_carea_label][1] = CreateDynamic3DTextLabel(sprintf("Seccond point\n(%f, %f, %f)", point2[0], point2[1], point2[2]), COLOR_LIGHTER_RED, point2[0], point2[1], point2[2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
			}
			else if( shape == AREA_SHAPE_CIRCLE )
			{
				pInfo[playerid][player_carea_label][0] = CreateDynamic3DTextLabel(sprintf("First point\n(%f, %f, %f)", point1[0], point1[1], point1[2]), COLOR_LIGHTER_RED, point1[0], point1[1], point1[2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
			}
		}
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /area [create/delete/assign/check/objects/flags]");
	return 1;
}

COMMAND:bw(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new time, targetid;
	if( sscanf(params, "ud", targetid, time) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /bw [id] [time]");
	if( time < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /bw [id] [time]");
	if( !IsPlayerConnected(targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id..");
	if( !pInfo[targetid][player_logged] ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");
		
	if( time == 0 && pInfo[targetid][player_bw] > 0 )
	{
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Applied BW for %s [UID: %d, ID: %d], time: %d minutes.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, time), "OK", "");
		// koniec bw
		pInfo[targetid][player_bw] = 0;
		pInfo[targetid][player_bw_end_time] = 0;
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_bw` = 0 WHERE `char_uid` = %d", pInfo[targetid][player_id]));
		
		RemovePlayerStatus(targetid, PLAYER_STATUS_BW);
		SetPlayerHealth(targetid, 20);
		TogglePlayerControllable(targetid, 1);
		SetCameraBehindPlayer(targetid);
		ClearAnimations(targetid);
		new skin = GetPlayerSkin(playerid);
		SetPlayerSkin(playerid, skin);
		SetPlayerSpecialAction(targetid, SPECIAL_ACTION_NONE);
		SetPlayerChatBubble(playerid, "", -1, 7.0, 5);
		return 1;
	}
	else if( time == 0 )
	{
		return SendClientMessage(playerid, COLOR_GREY, "This player is not brutally wounded.");
	}

	ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Applied BW for %s [UID: %d, ID: %d], time: %d minutes.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, time), "OK", "");

	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	GetPlayerPos(targetid, x, y, z);
	GetPlayerFacingAngle(targetid, a);


	pInfo[targetid][player_bw] = 60 * time;
	pInfo[targetid][player_bw_end_time] = pInfo[targetid][player_bw] + gettime();
		
	
	RP_PLUS_SetPlayerPos(targetid, x, y, z);
	SetPlayerFacingAngle(targetid, a);

	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_bw`=%d, `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d WHERE `char_uid`=%d", pInfo[targetid][player_bw], x, y, z, a, GetPlayerVirtualWorld(targetid), GetPlayerInterior(targetid), pInfo[targetid][player_id]));
	
	pInfo[targetid][player_quit_pos][0] = x;
	pInfo[targetid][player_quit_pos][1] = y;
	pInfo[targetid][player_quit_pos][2] = z;
	pInfo[targetid][player_quit_pos][3] = a;
	pInfo[targetid][player_quit_vw] = GetPlayerVirtualWorld(targetid);
	pInfo[targetid][player_quit_int] = GetPlayerInterior(targetid);

	defer ApplyAnim[2000](targetid, ANIM_TYPE_BW);
	
	scrp_SpawnPlayer(targetid);
	return 1;
}

COMMAND:setvw(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new targetid, vw;
	if( sscanf(params, "ud", targetid, vw) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setvw [id] [vw]");
	if( vw < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setvw [id] [vw]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id..");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	
	SetPlayerVirtualWorld(targetid, vw);
	
	if( targetid != playerid ) SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s changed your VW to %d.", pInfo[playerid][player_name], vw);
	SendGuiInformation(playerid, ""guiopis"Information", sprintf("VW for player %s changed to %d.", pInfo[targetid][player_name], vw));
	
	return 1;
}

COMMAND:setint(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new targetid, vw;
	if( sscanf(params, "dd", targetid, vw) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setint [id gracza] [interior world]");
	if( vw < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setint[id gracza] [vw]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Gracz o podanym id nie jest podlaczony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
	
	SetPlayerInterior(targetid, vw);
	
	if( targetid != playerid ) SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s zmieni³ Twoj interior world na %d.", pInfo[playerid][player_name], vw);
	SendGuiInformation(playerid, ""guiopis"Information", sprintf("Zmieni³eœ interior world graczowi %s na %d.", pInfo[targetid][player_name], vw));
	
	return 1;
}

COMMAND:ag(playerid, params[])
{
	return cmd_agroup(playerid, params);
}

COMMAND:agroup(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GROUPS) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup [create/delete/flags/name/leader/info/type/kick/invite]");
	
	if( !strcmp(sub_cmd, "create") )
	{
		new name[50];
		if( sscanf(param, "s[50]", name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup create [name]");
		
		mysql_real_escape_string(name, name);
		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_game_groups` (group_uid, group_name, group_tag) VALUES (null, '%s', 'TAG')", name));
		
		if( cache_insert_id() )
		{
			new gid = Iter_Free(Groups);
			Iter_Add(Groups, gid);
			
			Group[gid][group_uid] = cache_insert_id();
			Group[gid][group_bank_money] = 0;
			
			strcopy(Group[gid][group_name], name);
			
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", sprintf("Created group %s (UID: %d, ID: %d).", Group[gid][group_name], Group[gid][group_uid], gid), "OK", "");
		}
	}
	else if( !strcmp(sub_cmd, "color") )
	{
		new color[16], uid;
		if( sscanf(param, "ds[16]", uid,  color) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ag color [group uid] [HEX]");
		if(!ishex(color))  return SendClientMessage(playerid, COLOR_GREY, "Tip: /ag color [group uid] [HEX]");
		
		format(color, sizeof(color), "0x%sDD", color);
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_game_groups` SET `group_color` = '%d' WHERE `group_uid` = %d", hexstring(color), uid));
		SendGuiInformation(playerid, "Information", "Group color has been changed.");
	}
	else if( !strcmp(sub_cmd, "delete") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup delete [group uid]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid group uid.");
		
		foreach(new d_id : Doors)
		{
			if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GROUP && Door[d_id][door_owner] == Group[gid][group_uid] ) return SendGuiInformation(playerid, ""guiopis"Information", "You can't delete group with assigned doors.");
		}
		
		foreach(new a_id : Areas)
		{
			if( Area[a_id][area_owner_type] == AREA_OWNER_TYPE_GROUP && Area[a_id][area_owner] == Group[gid][group_uid] ) return SendGuiInformation(playerid, ""guiopis"Information", "You can't delete group with assigned areas.");
		}
		
		foreach(new v_id: Vehicles)
		{
			if( Vehicle[v_id][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP && Vehicle[v_id][vehicle_owner] == Group[gid][group_uid] )
			{
				mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_vehicles` WHERE `vehicle_uid` = %d", Vehicle[v_id][vehicle_uid]));
				DestroyDynamic3DTextLabel(Vehicle[v_id][vehicle_state_label]);
				DestroyDynamic3DTextLabel(Vehicle[v_id][vehicle_description]);
				DestroyDynamic3DTextLabel(Vehicle[v_id][vehicle_sign]);

				if(Vehicle[v_id][vehicle_siren] == true)
				{
					DestroyDynamicObject(Vehicle[v_id][vehicle_siren_object]);
					Vehicle[v_id][vehicle_siren] = false;
				}

				for(new z=0; e_vehicles:z != e_vehicles; z++)
				{
					Vehicle[v_id][e_vehicles:z] = 0;
				}

				DestroyVehicle(v_id);

				new next;
			    Iter_SafeRemove(Vehicles, v_id, next);
			    v_id = next;
			}
		}

		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Deleted group %s [UID: %d, ID: %d] with all their vehicles.", Group[gid][group_name], Group[gid][group_uid], gid));
		
		// Na poczatku usuwamy z grupy wszystkich graczy online
		foreach(new p : Player)
		{
			new slot = GetPlayerGroupSlot(p, gid);
			if( slot > -1 ) 
			{
				if( pGroup[p][slot][pg_duty] ) PlayerTextDrawHide(p, GroupDutyTag);
				CleanPlayerGroup(playerid, slot);
			}
		}
		
		// Teraz usuwamy czlonkow z bazy danych
		mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_char_groups` WHERE `group_belongs` = %d", Group[gid][group_uid]));
		
		// Kolej na usuniecie grupy z bazy danych
		mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_game_groups` WHERE `group_uid` = %d", Group[gid][group_uid]));
		
		// Na koncu usuwamy grupe z serwera
		for(new z=0; e_groups:z != e_groups; z++)
		{
			Group[gid][e_groups:z] = 0;
		}
		
		Iter_Remove(Groups, gid);
	}
	else if( !strcmp(sub_cmd, "info") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup info [group uid]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid group uid.");
		
		new str[300];
		
		format(str, sizeof(str), "%sType:\t\t%s\n", str, GroupTypes[Group[gid][group_type]]);
		format(str, sizeof(str), "%sBudget:\t\t\t$%d\n", str, Group[gid][group_bank_money]);
		format(str, sizeof(str), "%sCapital:\t\t\t$%d\n", str, Group[gid][group_capital]);
		format(str, sizeof(str), "%sActivity points:\t%s%d\n", str, ((Group[gid][group_activity_points] >= 1000) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)), Group[gid][group_activity_points]);
		format(str, sizeof(str), "%sDotation:\t\t$%d\n", str, GetGroupDotation(gid));
		format(str, sizeof(str), "%sTag:\t\t\t%s\n", str, GetGroupTag(gid));
			
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) » Details", Group[gid][group_name], Group[gid][group_uid]), str, "OK", "");
	}
	else if( !strcmp(sub_cmd, "flags") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup info [group uid]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid group uid.");
		
		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, gid);
		new str[700];
		
		format(str, sizeof(str), "%s%s01\tRadio\n", str, ((GroupHasFlag(gid, GROUP_FLAG_IC)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_IC);
		
		format(str, sizeof(str), "%s%s02\tOOC chat\n", str, ((GroupHasFlag(gid, GROUP_FLAG_OOC)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_OOC);

		format(str, sizeof(str), "%s%s03\tColored names\n", str, ((GroupHasFlag(gid, GROUP_FLAG_COLORED)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_COLORED);

		format(str, sizeof(str), "%s%s04\tDepartment chat\n", str, ((GroupHasFlag(gid, GROUP_FLAG_DEP)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_DEP);

		format(str, sizeof(str), "%s%s05\tLimited duty\n", str, ((GroupHasFlag(gid, GROUP_FLAG_DUTY)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_DUTY);

		format(str, sizeof(str), "%s%s06\tArrest\n", str, ((GroupHasFlag(gid, GROUP_FLAG_KEEP)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_KEEP);

		format(str, sizeof(str), "%s%s07\tBlockades\n", str, ((GroupHasFlag(gid, GROUP_FLAG_BLOCKADE)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_BLOCKADE);

		format(str, sizeof(str), "%s%s08\tFlaged weapons\n", str, ((GroupHasFlag(gid, GROUP_FLAG_WEAPON_FLAG)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_WEAPON_FLAG);

		format(str, sizeof(str), "%s%s09\t911 access\n", str, ((GroupHasFlag(gid, GROUP_FLAG_911)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_911);

		format(str, sizeof(str), "%s%s10\tRace creator\n", str, ((GroupHasFlag(gid, GROUP_FLAG_RACES)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_RACES);

		format(str, sizeof(str), "%s%s11\tUsing masks\n", str, ((GroupHasFlag(gid, GROUP_FLAG_MASK)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_MASK);

		format(str, sizeof(str), "%s%s12\tSearching\n", str, ((GroupHasFlag(gid, GROUP_FLAG_SEARCH)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_SEARCH);

		format(str, sizeof(str), "%s%s13\tCuffing\n", str, ((GroupHasFlag(gid, GROUP_FLAG_CUFFS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_CUFFS);

		format(str, sizeof(str), "%s%s14\tDoor prying\n", str, ((GroupHasFlag(gid, GROUP_FLAG_PRYING)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_PRYING);

		format(str, sizeof(str), "%s%s15\tBot access\n", str, ((GroupHasFlag(gid, GROUP_FLAG_BOT)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_BOT);

		format(str, sizeof(str), "%s%s16\tTaking\n", str, ((GroupHasFlag(gid, GROUP_FLAG_TAKE)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_TAKE);

		format(str, sizeof(str), "%s%s17\tHealing\n", str, ((GroupHasFlag(gid, GROUP_FLAG_MEDIC)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_MEDIC);

		format(str, sizeof(str), "%s%s18\tPaintjobs\n", str, ((GroupHasFlag(gid, GROUP_FLAG_PAINTJOB)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_PAINTJOB);

		format(str, sizeof(str), "%s%s19\tBusiness\n", str, ((GroupHasFlag(gid, GROUP_FLAG_BUSINESS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_BUSINESS);
	
		ShowPlayerDialog(playerid, DIALOG_GROUP_FLAGS, DIALOG_STYLE_LIST, sprintf("Flags of group [UID: %d]", g_uid), str, "Choose", "Close");
	}
	else if( !strcmp(sub_cmd, "name") )
	{
		new g_uid, name[64];
		if( sscanf(param, "ds[64]", g_uid, name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup name [group uid] [name]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid group uid.");
		mysql_real_escape_string(name, name);	
		strcopy(Group[gid][group_name], name);
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_game_groups` SET `group_name` = '%s' WHERE `group_uid` = %d", Group[gid][group_name], Group[gid][group_uid]));
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Group name changed [UID: %d, ID: %d] to %s.", Group[gid][group_uid], gid, Group[gid][group_name]));
	}
	else if( !strcmp(sub_cmd, "type") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup type [group uid]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid group uid.");
		
		new str[400];
		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, gid);
		
		for(new i;i<sizeof(GroupTypes);i++)
		{
			format(str, sizeof(str), "%s%02d\t%s\n", str, i+1, GroupTypes[i]);
			DynamicGui_AddRow(playerid, 0, i);
		}
		
		ShowPlayerDialog(playerid, DIALOG_AGRUPA_TYP, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) » Type change", Group[gid][group_name], Group[gid][group_uid]), str, "Choose", "Close");
	}
	else if( !strcmp(sub_cmd, "leader") )
	{
		new g_uid, targetid;
		if( sscanf(param, "dd", g_uid, targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup leader [group uid] [player id]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid group uid.");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id..");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
		
		new slot = GetPlayerGroupSlot(targetid, gid);
		if( slot > -1 )
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_char_groups` SET `group_perm` = %d WHERE `char_uid` = %d AND `group_belongs` = %d", 63, pInfo[targetid][player_id], Group[gid][group_uid]));
			
			pGroup[targetid][slot][pg_rank_perm] = 63;
			
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Player %s (UID: %d, ID: %d) is now leader of %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
			if( targetid != playerid ) SendGuiInformation(targetid, "Information", sprintf("Administrator %s made you leader of %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
		}
		else
		{
			new free_slot = GetPlayerGroupFreeSlot(targetid);
			if( free_slot == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "This player has no free group slot.");
			
			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_char_groups` (char_uid, group_belongs, group_perm) VALUES (%d, %d, %d)", pInfo[targetid][player_id], Group[gid][group_uid], 63));
			
			pGroup[targetid][free_slot][pg_id] = gid;
			pGroup[targetid][free_slot][pg_rank_perm] = 63;
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Invited %s (UID: %d, ID: %d) and made him leader of %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
			if( targetid != playerid ) SendGuiInformation(targetid, "Information", sprintf("Administrator %s invited you to group %s (UID: %d) with leader privileges.", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
		}
	}
	else if( !strcmp(sub_cmd, "invite") )
	{
		new g_uid, targetid;
		if( sscanf(param, "dd", g_uid, targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup invite [group uid] [player id]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid group uid.");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id..");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
		if( GetPlayerGroupSlot(targetid, gid) > -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "This player is already in this group.");
		
		new free_slot = GetPlayerGroupFreeSlot(targetid);
		if( free_slot == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "This player has no free group slot.");
		
		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_char_groups` (char_uid, group_belongs, group_perm) VALUES (%d, %d, %d)", pInfo[targetid][player_id], Group[gid][group_uid], 31));
			
		pGroup[targetid][free_slot][pg_id] = gid;
		pGroup[targetid][free_slot][pg_rank_perm] = 31;
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Invited %s (UID: %d, ID: %d) to group %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
		if( targetid != playerid ) SendGuiInformation(targetid, "Information", sprintf("Administrator %s has invited you to group %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
	}
	else if( !strcmp(sub_cmd, "kick") )
	{
		new g_uid, targetid;
		if( sscanf(param, "dd", g_uid, targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup kick [group uid] [player id]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid group uid.");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id..");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
		
		new slot = GetPlayerGroupSlot(targetid, gid);
		if( slot == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "This player is not member of this group.");
		
		mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_char_groups` WHERE `char_uid` = %d AND `group_belongs` = %d", pInfo[targetid][player_id], Group[gid][group_uid]));
		
		if( pGroup[targetid][slot][pg_duty] ) PlayerTextDrawHide(targetid, GroupDutyTag);
		
		CleanPlayerGroup(targetid, slot);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Kicked %s (UID: %d, ID: %d) from %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
		if( targetid != playerid ) SendGuiInformation(targetid, "Information", sprintf("Administrator %s has kicked you from %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)roup [create/delete/flags/name/leader/info/type/kick/invite]");
	return 1;
}

COMMAND:av(playerid, params[])
{	
	return cmd_avehicle(playerid, params);
}

COMMAND:avehicle(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_VEHICLES) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle [create/delete/info/model/hp/visual/assign/id/park/resp/spawn/unspawn/fueltype/fuel/goto/gethere/color1/color2]");
	
	if( !strcmp(sub_cmd, "create") )
	{
		new carmodel[32], c1, c2;
		if( sscanf(param, "dds[32]", c1, c2, carmodel) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle create [color1] [color2] [name]");

		new model = GetVehicleModelIDFromName(carmodel);

		if( model < 400 || model > 611 ) return SendGuiInformation(playerid, ""guiopis"Information", "Wrong name of vehicle.");
		
		new Float:v_pos[4];
		GetPointInAngleOfPlayer(playerid, v_pos[0], v_pos[1], v_pos[2], 4.0, 0.0);
		
		GetPlayerFacingAngle(playerid, v_pos[3]);
		if( v_pos[3] > 90.0 ) v_pos[3] -= 90.0;
		else v_pos[3] += 90.0;
		
		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_vehicles` (vehicle_uid, vehicle_model, vehicle_posx, vehicle_posy, vehicle_posz, vehicle_posa, vehicle_world, vehicle_interior, vehicle_color1, vehicle_color2, vehicle_fuel) VALUES (null, %d, %f, %f, %f, %f, %d, %d, %d, %d, '20.0')", model, v_pos[0], v_pos[1], v_pos[2], v_pos[3], GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), c1, c2));
		new uid = cache_insert_id();
	
		new vid = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);
	
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Created vehicle %s [UID: %d, ID: %d].", VehicleNames[model-400], uid, vid));
	}
	else if( !strcmp(sub_cmd, "delete") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle delete [vehicle id]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");

		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Deleted vehicle %s [UID: %d, ID: %d]", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
		
		DeleteVehicle(vid, true);
	}
	else if( !strcmp(sub_cmd, "spawn") )
	{
		new v_uid;
		if( sscanf(param, "d", v_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle spawn [vehicle uid]");
		if( v_uid < 1 ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle spawn [vehicle uid]");
		
		new vid = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", v_uid), true);
		
		if( vid == INVALID_VEHICLE_ID ) SendGuiInformation(playerid, ""guiopis"Information", "Vehicle is spawned or doesnt exist.");
		else SendGuiInformation(playerid, ""guiopis"Information", sprintf("Spawned vehicle %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], v_uid, vid));
	}
	else if( !strcmp(sub_cmd, "tirestate") )
	{
		new vid, tstate;
		if( sscanf(param, "dd", vid, tstate) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle tirestate [vehicle id] [state]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed tire state of %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
		
		new Panels, Doors, Lights, Tires;
		GetVehicleDamageStatus(vid, Panels, Doors, Lights, Tires);
		UpdateVehicleDamageStatus(vid, Panels, Doors, Lights, tstate);
	}
	else if( !strcmp(sub_cmd, "unspawn") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle unspawn [vehicle id]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Unspawned vehicle %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
		
		DeleteVehicle(vid);
	}
	else if( !strcmp(sub_cmd, "reset") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle reset [vehicle id]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Reseted position of %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
		
		SetVehiclePos(vid, Vehicle[vid][vehicle_park][0],Vehicle[vid][vehicle_park][1],Vehicle[vid][vehicle_park][2]);
		SetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
		SetVehicleVirtualWorld(vid, Vehicle[vid][vehicle_park_world]);
		LinkVehicleToInterior(vid, Vehicle[vid][vehicle_park_interior]);	
	}
	else if( !strcmp(sub_cmd, "model") )
	{
		new vid, carmodel[32];
		if( sscanf(param, "ds[32]", vid, carmodel) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle model [vehicle id] [name]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");

		new model = GetVehicleModelIDFromName(carmodel);
		if( model < 400 || model > 611 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle name.");
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_model` = %d WHERE `vehicle_uid` = %d", model, Vehicle[vid][vehicle_uid]));
		
		new uid = Vehicle[vid][vehicle_uid];
		DeleteVehicle(vid);
		
		new v_id = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed vehicle model of %s [UID: %d, ID: %d]", VehicleNames[GetVehicleModel(v_id)-400], uid, v_id));
	}
	else if( !strcmp(sub_cmd, "id") )
	{
		new v_uid;
		if( sscanf(param, "d", v_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle id [vehicle uid]");
		if( v_uid < 1 ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle id [vehicle uid]");
		
		new vid = GetVehicleByUid(v_uid);
		
		if( vid == INVALID_VEHICLE_ID ) SendGuiInformation(playerid, ""guiopis"Information", "Vehicle with that uid is not spawned.");
		else SendGuiInformation(playerid, ""guiopis"Information", sprintf("ID of %s is: %d.", VehicleNames[GetVehicleModel(vid)-400], vid));
	}
	else if( !strcmp(sub_cmd, "info") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle info [vehicle id]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");

		new owner_str[80];
		switch( Vehicle[vid][vehicle_owner_type] )
		{
			case VEHICLE_OWNER_TYPE_PLAYER:
			{			
				new name[MAX_PLAYER_NAME+1];
				GetPlayerNameByUid(Vehicle[vid][vehicle_owner], name);
				
				format(owner_str, sizeof(owner_str), "player; %s", name);
			}
			
			case VEHICLE_OWNER_TYPE_GROUP:
			{			
				new gid = GetGroupByUid(Vehicle[vid][vehicle_owner]);
				format(owner_str, sizeof(owner_str), "group; %s", Group[gid][group_name]);
			}

			case VEHICLE_OWNER_TYPE_JOB:
			{			
				format(owner_str, sizeof(owner_str), "job; %d", Vehicle[vid][vehicle_owner]);
			}
		}
		
		new string[340];
		format(string, sizeof(string), "Vehicle UID %d - %s~n~~n~~y~Owner: ~w~%d:%d (%s)~n~~y~Colors:~w~ %d:%d~n~", Vehicle[vid][vehicle_uid], VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_owner_type], Vehicle[vid][vehicle_owner], owner_str, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);
		format(string, sizeof(string), "%s~n~~y~Fuel: ~w~%d/%d~n~~y~Plates: ~w~%s~n~~y~HP: ~w~%.1f", string, floatround(Vehicle[vid][vehicle_fuel_current], floatround_ceil), VehicleFuelMax[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_register], Vehicle[vid][vehicle_health]);
		format(string, sizeof(string), "%s~n~~n~~b~~h~~h~VW:~w~ %d~n~~b~~h~~h~INT: ~w~%d~n~~n~Key ~p~N~w~ to close this window.", string, Vehicle[vid][vehicle_park_world], Vehicle[vid][vehicle_park_interior]);

		
		TextDrawSetString(Tutorial[playerid], string);
		TextDrawShowForPlayer(playerid, Tutorial[playerid]);
	}
	else if( !strcmp(sub_cmd, "fix") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle fix [vehicle id]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		
		SetVehicleHealth(vid, 1000.0);

		Vehicle[vid][vehicle_destroyed] = false; 
		Vehicle[vid][vehicle_damage][0] = 0;
		Vehicle[vid][vehicle_damage][1] = 0;
		Vehicle[vid][vehicle_damage][2] = 0;
		Vehicle[vid][vehicle_damage][3] = 0;

		UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);

		SaveVehicle(vid);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Fixed vehicle %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
		
		logprintf(LOG_VEHICLE, "[/av fix] [VUID: %d, PUID: %d, PGID: %d]", Vehicle[vid][vehicle_uid], pInfo[playerid][player_id], gInfo[playerid][global_id]);
	}
	else if( !strcmp(sub_cmd, "hp") )
	{
		new vid, Float:hp;
		if( sscanf(param, "df", vid, hp) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle hp [vehicle id] [hp]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		if( hp < 0.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle hp [vehicle id] [hp]");
		
		SetVehicleHealth(vid, hp);
		Vehicle[vid][vehicle_destroyed] = false; 
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_health` = %f WHERE `vehicle_uid` = %d", hp, Vehicle[vid][vehicle_uid]));
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Set %.1f hp to vehicle %s [UID: %d, ID: %d].", hp, VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
		
		logprintf(LOG_VEHICLE, "[/av hp] [VUID: %d, PUID: %d, PGID: %d, VHP: %f]", Vehicle[vid][vehicle_uid], pInfo[playerid][player_id], gInfo[playerid][global_id], hp);
	}
	else if( !strcmp(sub_cmd, "visual") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle visual [vehicle id]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		
		Vehicle[vid][vehicle_damage][0] = 0;
		Vehicle[vid][vehicle_damage][1] = 0;
		Vehicle[vid][vehicle_damage][2] = 0;
		Vehicle[vid][vehicle_damage][3] = 0;
		
		UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);
		
		SaveVehicle(vid);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Fixed visual damages of %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else if( !strcmp(sub_cmd, "park") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle park [vehicle id]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		
		GetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]);
		GetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
		Vehicle[vid][vehicle_park_world] = GetVehicleVirtualWorld(vid);
		Vehicle[vid][vehicle_park_interior] = Vehicle[vid][vehicle_interior];

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_posx` = %f, `vehicle_posy` = %f, `vehicle_posz` = %f, `vehicle_posa` = %f, `vehicle_world` = %d, `vehicle_interior` = %d WHERE `vehicle_uid` = %d", Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2], Vehicle[vid][vehicle_park][3], Vehicle[vid][vehicle_park_world], Vehicle[vid][vehicle_park_interior] ,Vehicle[vid][vehicle_uid]));
	
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Parked vehicle %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else if( !strcmp(sub_cmd, "assign") )
	{
		new vid, action[10], id;
		if( sscanf(param, "ds[10]d", vid, action, id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /av assign [vehicle id] [player/group/job] [player id/group uid/job id]");
		
		if( !strcmp(action, "player") )
		{
			if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
			if( !pInfo[id][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid player id.");
			if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
			
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_PLAYER, pInfo[id][player_id], Vehicle[vid][vehicle_uid]));
			Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_PLAYER;
			Vehicle[vid][vehicle_owner] = pInfo[id][player_id];
			
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Assigned vehicle %s [UID: %d, ID: %d] to %s [ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, pInfo[id][player_name], id));
		}
		else if( !strcmp(action, "job") )
		{	
			if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_JOB, id, Vehicle[vid][vehicle_uid]));
			Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_JOB;
			Vehicle[vid][vehicle_owner] = id;
			
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Przypisano pojazd %s [UID: %d, ID: %d] pracy dorywczej [ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, id));
		}
		else if( !strcmp(action, "group") )
		{
			if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
			new gid = GetGroupByUid(id);
			if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid group uid.");
			
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_GROUP, id, Vehicle[vid][vehicle_uid]));
			Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_GROUP;
			Vehicle[vid][vehicle_owner] = id;
			
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Assigned vehicle %s [UID: %d, ID: %d] to group %s [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, Group[gid][group_name], id));
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /av assign [vehicle id] [player/group/job] [player id/group uid/job id]");
	}
	else if( !strcmp(sub_cmd, "fueltype") )
	{
		new vid, type;
		if( sscanf(param, "dd", vid, type) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle fueltype [vehicle id] [1 - gas, 2 - propane gas, 3 - diesel, 4 - plane fuel]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		if( type < 1 || type > 4 ) return SendGuiInformation(playerid, ""guiopis"Information", "Wrong fueltype.");
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_fueltype` = %d WHERE `vehicle_uid` = %d", type, Vehicle[vid][vehicle_uid]));
		
		Vehicle[vid][vehicle_fuel_type] = type;
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed fueltype of %s [UID: %d, ID: %d] to: %s.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, VehicleFuelTypes[type]));
	}
	else if( !strcmp(sub_cmd, "fuel") )
	{
		new vid, Float:amount;
		if( sscanf(param, "df", vid, amount) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle fuel [vehicle id] [amount]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		if( amount < 0.0 || amount > 3000.0 ) return  SendGuiInformation(playerid, ""guiopis"Information", "Invalid amount.");
		
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_fuel` = %f WHERE `vehicle_uid` = %d", amount, Vehicle[vid][vehicle_uid]));
		Vehicle[vid][vehicle_fuel_current] = amount;
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed fuel state of %s [UID: %d, ID: %d] to: %.1fL.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, amount));
	}
	else if( !strcmp(sub_cmd, "goto") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle goto [vehicle id]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		
		new Float:v_pos[3];
		GetVehiclePos(vid, v_pos[0], v_pos[1], v_pos[2]);
		
		RP_PLUS_SetPlayerPos(playerid, v_pos[0], v_pos[1], v_pos[2]);
		SetPlayerVirtualWorld(playerid, GetVehicleVirtualWorld(vid));
		SetPlayerInterior(playerid, Vehicle[vid][vehicle_interior]);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Teleportowales sie do pojazdu %s [UID: %d, ID: %d, VW: %d, INT: %d]", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, GetVehicleVirtualWorld(vid), Vehicle[vid][vehicle_interior]));
	}
	else if( !strcmp(sub_cmd, "gethere") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle gethere [vehicle id]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		
		new Float:v_pos[4];
		GetPointInAngleOfPlayer(playerid, v_pos[0], v_pos[1], v_pos[2], 4.0, 0.0);
		
		GetPlayerFacingAngle(playerid, v_pos[3]);
		if( v_pos[3] > 90.0 ) v_pos[3] -= 90.0;
		else v_pos[3] += 90.0;
		
		SetVehiclePos(vid, v_pos[0], v_pos[1], v_pos[2]);
		SetVehicleZAngle(vid, v_pos[3]);
		SetVehicleVirtualWorld(vid, GetPlayerVirtualWorld(playerid));
		LinkVehicleToInterior(vid, GetPlayerInterior(playerid));
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Teleported vehicle %s [UID: %d, ID: %d] to you.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else if( !strcmp(sub_cmd, "color1") )
	{
		new vid, color;
		if( sscanf(param, "dd", vid, color) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle color2 [vehicle id] [kolor]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		if( color < 0 || color > 255 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid color id");
		
		Vehicle[vid][vehicle_color][0] = color;
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_color1` = %d WHERE `vehicle_uid` = %d", color, Vehicle[vid][vehicle_uid]));
		
		ChangeVehicleColor(vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed color of %s [UID: %d, ID: %d] to %d:%d.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]));
	}
	else if( !strcmp(sub_cmd, "color2") )
	{
		new vid, color;
		if( sscanf(param, "dd", vid, color) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle color2 [vehicle id] [kolor]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		if( color < 0 || color > 255 ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid color id.");
		
		Vehicle[vid][vehicle_color][1] = color;
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_color2` = %d WHERE `vehicle_uid` = %d", color, Vehicle[vid][vehicle_uid]));
		
		ChangeVehicleColor(vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed color of %s [UID: %d, ID: %d] to %d:%d.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]));
	}
	else if( !strcmp(sub_cmd, "plates") )
	{
		new plate[32], vid;
		if( sscanf(param, "ds[32]", vid, plate) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle plates [vehicle id] [new plates]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		sscanf(plate, "s[32]", Vehicle[vid][vehicle_register]);
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_register` = '%s' WHERE `vehicle_uid` = %d", plate, Vehicle[vid][vehicle_uid]));
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed vehicle plates of %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else if( !strcmp(sub_cmd, "resp") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle resp [vehicle id]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "Invalid vehicle id.");
		
		SetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]);
		SetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
		SetVehicleVirtualWorld(vid, Vehicle[vid][vehicle_park_world]);
		LinkVehicleToInterior(vid, Vehicle[vid][vehicle_park_interior]);
		Vehicle[vid][vehicle_engine] = false;
		Vehicle[vid][vehicle_locked] = true;
		UpdateVehicleVisuals(vid);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Vehicle %s [UID: %d, ID: %d] is back on spawn position.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle [create/delete/info/model/hp/visual/assign/id/park/resp/spawn/unspawn/fueltype/fuel/goto/gethere/color1/color2]");
	return 1;
}

COMMAND:ai(playerid, params[])
{
	return cmd_aitem(playerid, params);
}

COMMAND:aitem(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ITEMS) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem [create/delete/value1/value2/type]");
	
	if( !strcmp(sub_cmd, "create") )
	{
		new type, model, val1, val2, name[40];
		if( sscanf(param, "dddd s[40]", type, model, val1, val2, name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem create [type] [object model] [value1] [value2] [name]");

		if(model == 0)
		{
			model = 327;
		}
		
		Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, type, model, val1, val2, name);
		
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Created item %s [TYPE: %d, MODEL: %d, V1: %d, V2: %d].", name, type, model, val1, val2));
	}
	else if( !strcmp(sub_cmd, "delete") )
	{
		new itemuid;
		if(	sscanf(param, "d", itemuid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem delete [item uid]");
		
		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			DeleteItem(itemid, true, playerid);
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Skasowa³eœ przedmiot o UID: %d.", itemuid));
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This item is not in your inventory or doesnt exist.");
		}
	}
	else if( !strcmp(sub_cmd, "value1") )
	{
		new itemuid, value1;
		if(	sscanf(param, "dd", itemuid, value1) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem value1 [item uid] [value1]");
		
		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			PlayerItem[playerid][itemid][player_item_value1] = value1;	
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value1` = %d WHERE `item_uid` = %d", value1, itemuid));
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed value1 of item %d to: %d.", itemuid, value1));
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This item is not in your inventory or doesnt exist.");
		}
	}
	else if( !strcmp(sub_cmd, "value2") )
	{
		new itemuid, value2;
		if(	sscanf(param, "dd", itemuid, value2) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem value2 [item uid] [value2]");
		
		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 ) 
		{
			PlayerItem[playerid][itemid][player_item_value2] = value2;
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed value2 of item %d to: %d.", itemuid, value2));
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value2` = %d WHERE `item_uid` = %d", value2, itemuid));
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This item is not in your inventory or doesnt exist.");
		}
	}
	else if( !strcmp(sub_cmd, "extraid") )
	{
		new itemuid, groupuid;
		if(	sscanf(param, "dd", itemuid, groupuid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem extraid [item uid] [extraid]");
		
		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 ) 
		{
			PlayerItem[playerid][itemid][player_item_extraid] = groupuid;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_extraid` = %d WHERE `item_uid` = %d", groupuid, itemuid));
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed extraid to %d for item uid: %d.", groupuid, itemuid));
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This item is not in your inventory or doesnt exist.");
		}
	}
	else if( !strcmp(sub_cmd, "flag") )
	{
		new itemuid, groupuid;
		if(	sscanf(param, "dd", itemuid, groupuid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem flag [item uid] [group uid]");
		
		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 ) 
		{
			PlayerItem[playerid][itemid][player_item_group] = groupuid;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_group` = %d WHERE `item_uid` = %d", groupuid, itemuid));
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed flag of item uid %d for group %d.", itemuid, groupuid));
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This item is not in your inventory or doesnt exist.");
		}
	}
	else if( !strcmp(sub_cmd, "type") )
	{
		new itemuid, typ;
		if(	sscanf(param, "dd", itemuid, typ) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem type [item uid] [type]");
		
		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			PlayerItem[playerid][itemid][player_item_type] = typ;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_type` = %d WHERE `item_uid` = %d", typ, itemuid));
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed type of item uid %d to %d.", itemuid, typ));
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This item is not in your inventory or doesnt exist.");
		}
	}
	else if( !strcmp(sub_cmd, "model") )
	{
		new itemuid, model;
		if(	sscanf(param, "dd", itemuid, model) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem model [item uid] [typ]");
		
		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			PlayerItem[playerid][itemid][player_item_model] = model;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_model` = %d WHERE `item_uid` = %d", model, itemuid));
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed model of item uid %d to %d.", itemuid, model));
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This item is not in your inventory or doesnt exist.");
		}
	}
	else if( !strcmp(sub_cmd, "name") )
	{
		new itemuid, itname[40];
		if(	sscanf(param, "ds[40]", itemuid, itname) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem name [item uid] [new name]");
		
		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			sscanf(itname, "s[40]", PlayerItem[playerid][itemid][player_item_name]);
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_name` = '%s' WHERE `item_uid` = %d", itname, itemuid));
			SendGuiInformation(playerid, ""guiopis"Information", sprintf("Changed name of item uid %d to %s.", itemuid, itname));
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This item is not in your inventory or doesnt exist.");
		}
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ai)tem [create/delete/value1/value2/type]");
	
	return 1;
}

/*COMMAND:attach(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ITEMS) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach [stworz/usun/edytuj/print]");
	
	if( !strcmp(sub_cmd, "stworz") )
	{
		new model, bone;
		if( sscanf(param, "dd", model, bone) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach stworz [model] [bone]");
		
		new freeid = GetPlayerFreeAttachSlot(playerid);
		if( freeid == -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "Nie mo¿esz przyczepiæ ju¿ wiecej obiektów.");
		
		SetPlayerAttachedObject(playerid, freeid, model, bone);
		SendFormattedClientMessage(playerid, COLOR_GOLD, "Utworzyles nowy obiekt przyczepialny [INDEX: %d]. Aby zapisaæ jego pozycje, zapisz edycje za pomoc¹ ikonky dyskietki.", freeid);
		
		ao[playerid][freeid][ao_model] = model;
        ao[playerid][freeid][ao_bone] = bone;
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach usun [index]");
		if( !IsPlayerAttachedObjectSlotUsed(playerid, index) ) return SendGuiInformation(playerid, ""guiopis"Information", "W tym slocie nie masz ¿adnego obiektu.");
		
		ao[playerid][index][ao_model] = -1;
        ao[playerid][index][ao_bone] = -1;
		ao[playerid][index][ao_x] = 0.0;
        ao[playerid][index][ao_y] = 0.0;
        ao[playerid][index][ao_z] = 0.0;
        ao[playerid][index][ao_rx] = 0.0;
        ao[playerid][index][ao_ry] = 0.0;
        ao[playerid][index][ao_rz] = 0.0;
        ao[playerid][index][ao_sx] = 0.0;
        ao[playerid][index][ao_sy] = 0.0;
        ao[playerid][index][ao_sz] = 0.0;
		
		RemovePlayerAttachedObject(playerid, index);
		SendGuiInformation(playerid, ""guiopis"Information", "Pomyœlnie usun¹³eœ obiekt przyczepialny");
	}
	else if( !strcmp(sub_cmd, "edytuj") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach edytuj [index]");
		if( !IsPlayerAttachedObjectSlotUsed(playerid, index) ) return SendGuiInformation(playerid, ""guiopis"Information", "W tym slocie nie masz ¿adnego obiektu.");
		
		EditAttachedObject(playerid, index);
	}
	else if( !strcmp(sub_cmd, "print") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach print [index]");
		if( !IsPlayerAttachedObjectSlotUsed(playerid, index) ) return SendGuiInformation(playerid, ""guiopis"Information", "W tym slocie nie masz ¿adnego obiektu.");
		
		printf("[model %d, bone: %d] %f, %f, %f, %f, %f, %f, %f, %f, %f", ao[playerid][index][ao_model], ao[playerid][index][ao_bone], ao[playerid][index][ao_x], ao[playerid][index][ao_y], ao[playerid][index][ao_z], ao[playerid][index][ao_rx], ao[playerid][index][ao_ry], ao[playerid][index][ao_rz], ao[playerid][index][ao_sx], ao[playerid][index][ao_sy], ao[playerid][index][ao_sz]);
	}
	return 1;
}*/

COMMAND:aplace(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_DOORS) ) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this command.");
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aplace [create/delete]");
	
	if( !strcmp(sub_cmd, "create") )
	{
		new type, Float:sx, Float:sy, Float:sz;
		if( sscanf(param, "d", type) )
		{
			TextDrawSetString(Tutorial[playerid], "~w~Place ~p~types~w~:~n~~n~1. Bank~n~2. Hotel~n~3. 24/7~n~4. Victim~n~5. Pharmacy~n~6. Dealership~n~7. Ammunation~n~8.Toolshop~n~Key ~g~N~w~ is closing this window.");
			TextDrawShowForPlayer(playerid, Tutorial[playerid]);
			SendClientMessage(playerid, COLOR_GREY, "Tip: /aplace stworz [typ]");
			return 1;
		}
		new svw = GetPlayerVirtualWorld(playerid);
		GetPlayerPos(playerid, sx, sy, sz);
		
		new puid = CreateSpecialPlace(type, Float:sx, Float:sy, Float:sz, svw);
		SendGuiInformation(playerid, ""guiopis"Information", sprintf("Place UID %d, type: %d sucessfully created.", puid, type));
	}
	else if( !strcmp(sub_cmd, "delete") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aplace delete [uid]");
		SendGuiInformation(playerid, ""guiopis"Information", "Place has been deleted.");
		new ib = GetPlaceByUid(index);
		if(ib != -1) Iter_Remove(Places, ib);
		DeleteSpecialPlace(index);
	}
	return 1;
}