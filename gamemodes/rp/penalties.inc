new DeletePenalty = 0;

stock LoadGlobalPenalties(playerid)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT member_block_run, member_block_dmg, member_block_ooc, member_block_veh FROM ipb_members WHERE member_id = %d", pGlobal[playerid][glo_id]));
	cache_get_data(rows, fields);

	pGlobal[playerid][glo_run] = cache_get_row_int(0, 0);
	pGlobal[playerid][glo_dmg] = cache_get_row_int(0, 1);
	pGlobal[playerid][glo_ooc] = cache_get_row_int(0, 2);
	pGlobal[playerid][glo_veh] = cache_get_row_int(0, 3);

	if(pGlobal[playerid][glo_run] != 0 && pGlobal[playerid][glo_run] < gettime())
	{
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET member_block_run = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
		SendClientMessage(playerid, COLOR_GOLD, "Time of your blockade has passed.");
		pGlobal[playerid][glo_run] = 0;
	}

	if(pGlobal[playerid][glo_dmg] != 0 && pGlobal[playerid][glo_dmg] < gettime())
	{
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET member_block_dmg = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
		SendClientMessage(playerid, COLOR_GOLD, "Time of your blockade has passed.");
		pGlobal[playerid][glo_dmg] = 0;
	}

	if(pGlobal[playerid][glo_ooc] != 0 && pGlobal[playerid][glo_ooc] < gettime())
	{
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET member_block_ooc = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
		SendClientMessage(playerid, COLOR_GOLD, "Time of your blockade has passed.");
		pGlobal[playerid][glo_ooc] = 0;
	}

	if(pGlobal[playerid][glo_veh] != 0 && pGlobal[playerid][glo_veh] < gettime())
	{
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET member_block_veh = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
		SendClientMessage(playerid, COLOR_GOLD, "Time of your blockade has passed.");
		pGlobal[playerid][glo_veh] = 0;
	}
	return 1;
}

stock AddPlayerPunishLog(playerid, giveplayer_id, punish_type, reason[], extraid = 0, time = 0)
{
	if(!pInfo[playerid][player_logged]) return 1;
    mysql_escape_string(reason, reason, mySQLconnection, 128);
	if(giveplayer_id == -1)
 	{
 	   	mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_penalties` (`penalty_user_char_id`, `penalty_user_global_id`, `penalty_admin_global_id`, `penalty_expire`, `penalty_type`, `penalty_dateline`, `penalty_reason`) VALUES ('%d', '%d', '-1', '0', '%d', %d, '%s')", pInfo[playerid][player_id], pGlobal[playerid][glo_id], punish_type, gettime(), reason));
	}
	else
	{
		if(extraid == 0)
		{
 			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_penalties` (`penalty_user_char_id`, `penalty_user_global_id`, `penalty_admin_global_id`, `penalty_expire`, `penalty_type`, `penalty_dateline`, `penalty_reason`) VALUES ('%d', '%d', '%d', '%d', '%d', %d, '%s')", pInfo[playerid][player_id], pGlobal[playerid][glo_id], pGlobal[giveplayer_id][glo_id], time, punish_type, gettime(), reason));
 		}
 		else
 		{
 			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_penalties` (`penalty_user_char_id`, `penalty_user_global_id`, `penalty_admin_global_id`, `penalty_expire`, `penalty_type`, `penalty_dateline`, `penalty_reason`, `penalty_extraid`) VALUES ('%d', '%d', '%d', '0', '%d', %d, '%s', '%d')", pInfo[playerid][player_id], pGlobal[playerid][glo_id], pGlobal[giveplayer_id][glo_id], punish_type, gettime(), reason, extraid));
 		}
	}
	return 1;
}

stock KickAc(playerid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[200], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, "Kick");

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);

	Kick(playerid);
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_KICK, reason);
	DeletePenalty = 8;
	return 1;
}

stock CarUnspawn(playerid, vehicleid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(vehicleid == INVALID_VEHICLE_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[200], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, "Vehicle unspawn");

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);

	new uid = Vehicle[vehicleid][vehicle_uid];
	ClearAnimations(playerid);
	DeleteVehicle(vehicleid, false);
	LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);
	
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_WARN, reason);
	DeletePenalty = 8;
	return 1;
}

stock Warn(playerid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[200], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);

	SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "%s warned you with reason: %s.", giver_name, reason);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, "Warn");

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_WARN, reason);
	DeletePenalty = 8;
	return 1;
}

stock Block(playerid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, "Char block");

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_active` = 1 WHERE `char_uid` = %d", pInfo[playerid][player_id]));
	DeletePenalty = 8;
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK, reason);
	Kick(playerid);
	return 1;
}

stock BlockVeh(playerid, giveplayer_id, time, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(time == 0)
	{
		if(pGlobal[playerid][glo_veh])
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_veh` = 0 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
			SendClientMessage(giveplayer_id, COLOR_GOLD, sprintf("Blokada prowadzenia pojazdÛw gracza %s zosta≥a pomyúlnie zdjÍta.", pInfo[playerid][player_name]));
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Administrator %s zdjπ≥ twojπ blokade prowadzenia pojazdÛw.", pInfo[giveplayer_id][player_name]));
			pGlobal[playerid][glo_veh] = 0;
		}
		return 1;
	}
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, sprintf("Vehicles blockade (%d days)", time));

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);

	DeletePenalty = 8;
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK_VEH, reason, 0, gettime() + time*86400);
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_veh` = %d WHERE `member_id` = %d", gettime()+time*86400, pGlobal[playerid][glo_id]));
	pGlobal[playerid][glo_veh] = gettime() + time*86400;
	return 1;
}

stock BlockRun(playerid, giveplayer_id, time, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(time == 0)
	{
		if(pGlobal[playerid][glo_run])
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_run` = 0 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
			SendClientMessage(giveplayer_id, COLOR_GOLD, sprintf("Run blockade of %s has been removed.", pInfo[playerid][player_name]));
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Administrator %s deleted your run blockade.", pInfo[giveplayer_id][player_name]));
			pGlobal[playerid][glo_run] = 0;
		}
		return 1;
	}
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, sprintf("Run blockade (%d days)", time));

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);

	DeletePenalty = 8;
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK_RUN, reason, 0, gettime() + time*86400);
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_run` = %d WHERE `member_id` = %d", gettime()+time*86400, pGlobal[playerid][glo_id]));
	pGlobal[playerid][glo_run] = gettime() + time*86400;
	return 1;
}

stock BlockDmg(playerid, giveplayer_id, time, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(time == 0)
	{
		if(pGlobal[playerid][glo_dmg])
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_dmg` = 0 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
			SendClientMessage(giveplayer_id, COLOR_GOLD, sprintf("DMG blockade for %s has been removed.", pInfo[playerid][player_name]));
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Administrator %s has deleted your DMG blockade.", pInfo[giveplayer_id][player_name]));
			pGlobal[playerid][glo_dmg] = 0;
		}
		return 1;
	}
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, sprintf("DMG blockade (%d days)", time));

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);

	DeletePenalty = 8;
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK_DMG, reason, 0, gettime() + time*86400);
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_dmg` = %d WHERE `member_id` = %d", gettime()+time*86400, pGlobal[playerid][glo_id]));
	pGlobal[playerid][glo_dmg] = gettime() + time*86400;
	return 1;
}

stock BlockOoc(playerid, giveplayer_id, time, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(time == 0)
	{
		if(pGlobal[playerid][glo_ooc])
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_ooc` = 0 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
			SendClientMessage(giveplayer_id, COLOR_GOLD, sprintf("OOC blockade for %s has been removed.", pInfo[playerid][player_name]));
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Administrator %s has deleted your OOC blockade.", pInfo[giveplayer_id][player_name]));
			pGlobal[playerid][glo_ooc] = 0;
		}
		return 1;
	}
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, sprintf("OOC blockade (%d days)", time));

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);

	DeletePenalty = 8;
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK_OOC, reason, 0, gettime() + time*86400);
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_ooc` = %d WHERE `member_id` = %d", gettime()+time*86400, pGlobal[playerid][glo_id]));
	pGlobal[playerid][glo_ooc] = gettime() + time*86400;
	return 1;
}

stock Score(playerid, giveplayer_id, score, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24], scoreheader[32];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);
	format(scoreheader, sizeof(scoreheader), "~b~~h~~h~cPoints ~w~(%d)", score);
	TextDrawSetString(TextPenaltyType, scoreheader);

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);

	pGlobal[playerid][glo_score] += score;
	SetPlayerScore(playerid, pGlobal[playerid][glo_score]);

	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_GAME_SCORE, reason, score);
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_game_points` = %d WHERE `member_id` = %d", pGlobal[playerid][glo_score], pGlobal[playerid][glo_id]));
	DeletePenalty = 8;
	return 1;
}

stock CharacterKill(playerid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, "Character Kill");

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_active` = 1 WHERE `char_uid` = %d", pInfo[playerid][player_id]));
	DeletePenalty = 8;

	for(new item;item<MAX_PLAYER_ITEMS;item++)
	{
		if(PlayerItem[playerid][item][player_item_uid] > 0 && !PlayerItem[playerid][item][player_item_used])
		{
			Item_Drop(item, playerid);
		}
	}

	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	z = floatadd(z, -0.973339);

	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK, reason);

	new i_name[40];
    format(i_name, sizeof(i_name), "%s", pInfo[playerid][player_name]);

	new iid = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_CORPSE, 19944, pInfo[playerid][player_killer], pInfo[playerid][player_death], i_name);
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_ownertype` = '1', `item_world` = '%d', `item_posx` = '%f', `item_posy` = '%f', `item_posz` = '%f' WHERE `item_uid` = %d", GetPlayerVirtualWorld(playerid), x, y, z, PlayerItem[playerid][iid][player_item_uid]));
	
	new uid  = PlayerItem[playerid][iid][player_item_uid];
	DeleteItem(iid, false, playerid);

	LoadItem(sprintf("WHERE `item_uid` = %d", uid), false, true);

	Kick(playerid);
	return 1;
}

stock BanAc(playerid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[200], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, "Ban");

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_game_ban` = 1 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
	DeletePenalty = 8;

	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BAN, reason);
	Kick(playerid);
	return 1;
}

stock AdminJail(playerid, giveplayer_id, reason[], time)
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[200], ajstring[32], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(time==0)
	{
	    SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "%s released you from adminjail with reason: %s.", giver_name, reason);
	    SendFormattedClientMessage(giveplayer_id, COLOR_LIGHTER_RED, "You've released %s from AJ.", pInfo[playerid][player_name]);
	    SetPlayerPos(playerid, 1721.98, -1719.12, 13.5349);
	    mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_aj = 0 WHERE char_uid = %d", pInfo[playerid][player_id]));
	    pInfo[playerid][player_aj] = 0;
	    SetPlayerVirtualWorld(playerid, 0);
		return 1;
	}
	
	format(string, sizeof(string), "            Admin: %s~n~            Receiver: %s~n~            Reason: %s", giver_name, pInfo[playerid][player_name], reason);
	format(ajstring, sizeof(ajstring), "AdminJail (%d min)", time);
	
	TextDrawSetString(TextPenalty, string);
	TextDrawSetString(TextPenaltyType, ajstring);

	TextDrawShowForAll(TextPenalty);
	TextDrawShowForAll(TextPenaltySprite);
	TextDrawShowForAll(TextPenaltyType);
	
	pInfo[playerid][player_aj]=time;

	mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_aj = %d WHERE char_uid = %d", pInfo[playerid][player_aj], pInfo[playerid][player_id]));
	SetPlayerPos(playerid, 154.0880,-1951.6383,47.8750);
	SetPlayerVirtualWorld(playerid, pInfo[playerid][player_id]);
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_AJ, reason);
	DeletePenalty = 8;
	return 1;
}

stock EscapePL(name[])
{
    for(new i = 0; name[i] != 0; i++)
    {
	    if(name[i] == 'ú') name[i] = 's';
	    else if(name[i] == 'Í') name[i] = 'e';
	    else if(name[i] == 'Û') name[i] = 'o';
	    else if(name[i] == 'a') name[i] = 'a';
	    else if(name[i] == '≥') name[i] = 'l';
	    else if(name[i] == 'ø') name[i] = 'z';
	    else if(name[i] == 'ü') name[i] = 'z';
	    else if(name[i] == 'Ê') name[i] = 'c';
	    else if(name[i] == 'Ò') name[i] = 'n';
	    else if(name[i] == 'å') name[i] = 'S';
	    else if(name[i] == ' ') name[i] = 'E';
	    else if(name[i] == '”') name[i] = 'O';
	    else if(name[i] == '•') name[i] = 'A';
	    else if(name[i] == '£') name[i] = 'L';
	    else if(name[i] == 'Ø') name[i] = 'Z';
	    else if(name[i] == 'è') name[i] = 'Z';
	    else if(name[i] == '∆') name[i] = 'C';
	    else if(name[i] == '—') name[i] = 'N';
	    //else if(name[i] == ' ') name[i] = '_';
    }
}
