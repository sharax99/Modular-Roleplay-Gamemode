stock OnPlayerLoginHere(playerid)
{
	new rows, fields;

	mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_characters WHERE `char_uid` = %d LIMIT 1", pInfo[playerid][player_id]));
	cache_get_data(rows, fields);

	pInfo[playerid][player_hours] 		= cache_get_row_int(0, 3);
	pInfo[playerid][player_minutes] 	= cache_get_row_int(0, 4);
	pInfo[playerid][player_money] 		= cache_get_row_int(0, 5);
	pInfo[playerid][player_bank_money] 	= cache_get_row_int(0, 6);
	pInfo[playerid][player_bank_number] = cache_get_row_int(0, 7);
	pInfo[playerid][player_skin] 		= cache_get_row_int(0, 8);
	pInfo[playerid][player_health]		= cache_get_row_float(0, 9);
	pInfo[playerid][player_sex] 		= cache_get_row_int(0, 10);
	pInfo[playerid][player_age] 		= cache_get_row_int(0, 11);
	pInfo[playerid][player_quit_pos][0] = cache_get_row_float(0, 12);
	pInfo[playerid][player_quit_pos][1] = cache_get_row_float(0, 13);
	pInfo[playerid][player_quit_pos][2] = cache_get_row_float(0, 14);
	pInfo[playerid][player_quit_pos][3] = cache_get_row_float(0, 15);
	pInfo[playerid][player_quit_vw] 	= cache_get_row_int(0, 16);
	pInfo[playerid][player_quit_int] 	= cache_get_row_int(0, 17);
	pInfo[playerid][player_quit_time] 	= cache_get_row_int(0, 18);
	pInfo[playerid][player_strength] 	= cache_get_row_int(0, 19);
	pInfo[playerid][player_bw] 			= cache_get_row_int(0, 20);
	pInfo[playerid][player_aj] 			= cache_get_row_int(0, 21);
	pInfo[playerid][player_job] 		= cache_get_row_int(0, 22);
	pInfo[playerid][player_documents] 	= cache_get_row_int(0, 23);
	pInfo[playerid][player_fight_style] = cache_get_row_int(0, 24);
	pInfo[playerid][player_active] 		= cache_get_row_int(0, 25);
	pInfo[playerid][player_walking_anim]= cache_get_row_int(0, 26);
	cache_get_row(0, 27, pInfo[playerid][player_walking_lib], mySQLconnection, 16);
 	cache_get_row(0, 28, pInfo[playerid][player_walking_name], mySQLconnection, 24);
	pInfo[playerid][player_renting] 	= cache_get_row_int(0, 29);
	pInfo[playerid][player_spawn] 		= cache_get_row_int(0, 30);
	pInfo[playerid][player_spawn_type] 	= cache_get_row_int(0, 31);
	pInfo[playerid][player_weapon_skill]= cache_get_row_float(0, 32);
	cache_get_row(0, 33, pInfo[playerid][player_extra_status], mySQLconnection, 32);
   	pInfo[playerid][player_arrest] 		= cache_get_row_int(0, 34);
   	pInfo[playerid][player_last_payday] = cache_get_row_int(0, 35);
   	pInfo[playerid][player_gym_carnet]  = cache_get_row_int(0, 36);
   	pInfo[playerid][player_last_work]   = cache_get_row_int(0, 37);
   	pInfo[playerid][player_flags]       = cache_get_row_int(0, 38);

	pInfo[playerid][player_online_time] = pInfo[playerid][player_hours] * 3600 + pInfo[playerid][player_minutes] * 60;
	if( pInfo[playerid][player_bw] > 0 ) pInfo[playerid][player_bw_end_time] = pInfo[playerid][player_bw] + gettime();  
	
	if(pInfo[playerid][player_active] == 1)
	{
		for(new i;i<50;i++) SendClientMessage(playerid, -1, " ");
		SendGuiInformation(playerid, ""guiopis"Alert", "This characters is blocked.");
		Kick(playerid);
		return 1;
	}
	
	for(new w = 0; (w < 10) && (w != 7 && w != 8 && w != 9); w++) SetPlayerSkillLevel(playerid, w, 1);

	if(pInfo[playerid][player_weapon_skill] > 1)
	{
		new wlevel = floatround(pInfo[playerid][player_weapon_skill]);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_PISTOL_SILENCED, wlevel);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_DESERT_EAGLE, wlevel);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_MP5, wlevel);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_AK47, wlevel);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_M4, wlevel);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_SHOTGUN, wlevel);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_SNIPERRIFLE, wlevel);
	}

	new country[128];
	GetPlayerCountry(playerid, country);

	/*if( strfind(country, "Anonymous") != -1 )
	{
		//
	}
	else
	{
		BanAc(playerid, -1, "VPN/Proxy detected");
		return 1;
	}*/

	if( strfind(country, "Poland") != -1 )
	{
		//
	}
	else
	{
		foreach(new play: Player)
		{
			if(pInfo[play][player_admin_duty] == true)
			{
				SendClientMessage(play, COLOR_GOLD, sprintf("%s: %s.", pInfo[playerid][player_name], country));
			}
		}
	}

	if(pGlobal[playerid][glo_premium] != 0 && pGlobal[playerid][glo_premium] < gettime())
	{
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET member_premium_time = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
		SendClientMessage(playerid, COLOR_GOLD, "Your premium time has been ended. Thank you for donating our server.");
		pGlobal[playerid][glo_premium] = 0;
	}
	
	UpdatePlayerLabel(playerid);
	SetPlayerMoney(playerid, pInfo[playerid][player_money]);
	SetPlayerScore(playerid, pGlobal[playerid][glo_score]);
	SetPVarInt(playerid, "AnimHitPlayerGun", 0);

	Attach3DTextLabelToPlayer(pInfo[playerid][player_label], playerid, 0.0, 0.0, 0.2);
	AttachPlayerStatuses(playerid);

	pInfo[playerid][player_capacity] = floatround(pInfo[playerid][player_strength], floatround_floor)*400;
	pInfo[playerid][player_last_join] = gettime();
	pInfo[playerid][player_last_skin] = pInfo[playerid][player_skin];

	mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_logged_players` (char_uid, user_id, seen) VALUES (%d, %d, %d)", pInfo[playerid][player_id], pGlobal[playerid][glo_id], pInfo[playerid][player_last_join]));
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET char_online = 1 WHERE char_uid = %d", pInfo[playerid][player_id]));
	
	KillTimer(connect_timer[playerid]);

	scrp_SpawnPlayer(playerid, false);
	TogglePlayerSpectating(playerid, 0);
	
	pInfo[playerid][player_logged] = true;

	new groupsLoaded = LoadPlayerGroupData(playerid);
	new itemsLoaded  = LoadPlayerItemData(playerid);

	UpdatePlayerColor(playerid);

	LoadGlobalPenalties(playerid);

	SetPlayerFightingStyle(playerid, pInfo[playerid][player_fight_style]);

	SendFormattedClientMessage(playerid, -1, "Hello "HEX_COLOR_CREW_ADMIN"%s (GID: %d)"HEX_COLOR_WHITE". Logged in on "HEX_COLOR_CREW_ADMIN"%s (UID: %d)"HEX_COLOR_WHITE". Have fun!", pGlobal[playerid][glo_name], pGlobal[playerid][glo_id], pInfo[playerid][player_name], pInfo[playerid][player_id], playerid);  
	logprintf(LOG_PLAYER, "[load] Loaded character %s (UID: %d). [Groups: %d] [Items: %d]", pInfo[playerid][player_name], pInfo[playerid][player_id], groupsLoaded, itemsLoaded);
	return 1;
}

stock LoadPlayerItemData(playerid)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT item_uid, item_type, item_value1, item_value2, item_used, item_extraid, item_weight, item_model, item_name FROM ipb_items WHERE item_ownertype = %d AND item_owner = %d", ITEM_OWNER_TYPE_PLAYER, pInfo[playerid][player_id]));
	cache_get_data(rows, fields);

	new i_id = -1;
	
	for(new row = 0; row != rows; row++)
	{
		i_id = Iter_Free(PlayerItems[playerid]);
		Iter_Add(PlayerItems[playerid], i_id);
		
		PlayerItem[playerid][i_id][player_item_uid] = cache_get_row_int(row, 0);
		PlayerItem[playerid][i_id][player_item_type] = cache_get_row_int(row, 1);
		PlayerItem[playerid][i_id][player_item_value1] = cache_get_row_int(row, 2);
		PlayerItem[playerid][i_id][player_item_value2] = cache_get_row_int(row, 3);
		PlayerItem[playerid][i_id][player_item_used] = cache_get_row_int(row, 4);
		PlayerItem[playerid][i_id][player_item_extraid] = cache_get_row_int(row, 5);
		PlayerItem[playerid][i_id][player_item_weight] = cache_get_row_int(row, 6);
		PlayerItem[playerid][i_id][player_item_model] = cache_get_row_int(row, 7);
		cache_get_row(row, 8, PlayerItem[playerid][i_id][player_item_name], mySQLconnection, 40);
	}

	SetupPlayerCapacity(playerid);
	return rows;
}

stock EncountDamage(playerid, Float:amount, bodypart, weaponid)
{
	new damage_id = GetFreeDamageId(playerid);
	pInjury[playerid][damage_id][injury_id] = damage_id;
	pInjury[playerid][damage_id][injury_damage] = floatround(amount);
	pInjury[playerid][damage_id][injury_bodypart] = bodypart;
	pInjury[playerid][damage_id][injury_weapon] = weaponid;
	pInjury[playerid][damage_id][injury_time] = gettime();
}

stock GetFreeDamageId(playerid)
{
	new damage_id;
	for(new b = 1; b < 10; b++)
	{
		if(pInjury[playerid][b][injury_id] == 0)
		{
			damage_id = b;
			break;
		}
	}
	return damage_id;
}

stock SetupPlayerCapacity(playerid)
{
	pInfo[playerid][player_capacity] = floatround(pInfo[playerid][player_strength], floatround_round) * 600;

	foreach (new item : PlayerItems[playerid])
	{
		pInfo[playerid][player_capacity] -= PlayerItem[playerid][item][player_item_weight];
		
	}
}

stock AttachPlayerStatuses(playerid)
{
	if(pGlobal[playerid][glo_score] < 30)
	{
		AddPlayerStatus(playerid, PLAYER_STATUS_NEW);
	}

	if(pInfo[playerid][player_strength] > 60)
	{
		AddPlayerStatus(playerid, PLAYER_STATUS_SPORT);
	}

	if(pInfo[playerid][player_strength] > 80)
	{
		AddPlayerStatus(playerid, PLAYER_STATUS_MUSCULAR);
		RemovePlayerStatus(playerid, PLAYER_STATUS_SPORT);
	}

	if(strlen(pInfo[playerid][player_extra_status]) > 1)
	{
		AddPlayerStatus(playerid, PLAYER_STATUS_EXTRA);
	}
}

stock SetPlayerLookAt(playerid, Float:x, Float:y)
{
	new Float:Px, Float:Py, Float: Pa;
	GetPlayerPos(playerid, Px, Py, Pa);
	Pa = floatabs(atan((y-Py)/(x-Px)));
	if (x <= Px && y >= Py) Pa = floatsub(180, Pa);
	else if (x < Px && y < Py) Pa = floatadd(Pa, 180);
	else if (x >= Px && y <= Py) Pa = floatsub(360.0, Pa);
	Pa = floatsub(Pa, 90.0);
	if (Pa >= 360.0) Pa = floatsub(Pa, 360.0);
	SetPlayerFacingAngle(playerid, Pa);
}

stock GetPlayerByUid(p_uid)
{
	foreach(new p : Player)
	{
		if( pInfo[p][player_id] == p_uid ) return p;
	}
	
	return -1;
}

stock ShowPlayerPayment(playerid)
{	
	ShowPlayerDialog(playerid, DIALOG_PAYMENT, DIALOG_STYLE_LIST, "Payment method", "Cash\nCredit card", "Choose", "Cancel");
}

stock OnPlayerPaymentResponse(playerid, type, response)
{
	OnPlayerOfferFinish(playerid, type, response);
	return 1;
}

stock AddPlayerBankMoney(playerid, amount)
{
	pInfo[playerid][player_bank_money] += amount;
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_bankcash` = %d WHERE `char_uid` = %d", pInfo[playerid][player_bank_money], pInfo[playerid][player_id]));
}

stock RefreshPlayer(playerid)
{
	new Float:pos[3];
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	RP_PLUS_SetPlayerPos(playerid, pos[0]+0.05, pos[1]+0.05, pos[2]);
}

stock LoadPlayerGroupData(playerid)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_char_groups WHERE `char_uid` = %d LIMIT 5", pInfo[playerid][player_id]));
	cache_get_data(rows, fields);
	
	for(new row = 0; row != rows; row++)
	{		
		pGroup[playerid][row][pg_id] = GetGroupByUid(cache_get_row_int(row, 2));
		
		cache_get_row(row, 4, pGroup[playerid][row][pg_rank_title], mySQLconnection, 32);
		pGroup[playerid][row][pg_rank_skin] = cache_get_row_int(row, 6);
		pGroup[playerid][row][pg_rank_payment] = cache_get_row_int(row, 5);
		pGroup[playerid][row][pg_rank_perm] = cache_get_row_int(row, 3);
	}
	return rows;
}

stock SendPlayerInformation(playerid, text[], showTime = 2000)
{
	pInfo[playerid][player_info_td_id]++;

	TextDrawHideForPlayer(playerid, Tutorial[playerid]);

	if( showTime > 0 )
	{
		new str[300];
		format(str, sizeof(str), "~w~%s", text);
		
		TextDrawSetString(Tutorial[playerid], str);
		TextDrawShowForPlayer(playerid, Tutorial[playerid]);

		pInfo[playerid][player_info_td_id] = showTime/1000;
	}
	else HideInformationTextdraw(playerid, pInfo[playerid][player_info_td_id]);
	
	return 1;
}

stock IsPlayerVip(playerid)
{
	if( pGlobal[playerid][glo_premium] > 0 ) return 1;
	
	return 0;
}

stock GetPlayerNameByUid(uid, string[25])
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT `char_name` FROM `ipb_characters` WHERE `char_uid` = %d", uid));
	cache_get_data(rows, fields);
	
	cache_get_row(0, 0, string);

	return true;
}

stock static Callback_IsVehicleInPoint(vehicleid, Float:range, Float:x, Float:y, Float:z)
{
    new
        Float:fX,
        Float:fY,
        Float:fZ
    ;
 
    GetVehiclePos(vehicleid, fX, fY, fZ);
 
    fX -= x;
    fY -= y;
    fZ -= z;
 
    return ((fX * fX) + (fY * fY) + (fZ * fZ)) < (range * range);
}

stock IsPlayerSprayingVehicle(playerid, vehicleid)
{
	new
	    index = GetPlayerAnimationIndex(playerid),
		Float:x,
		Float:y,
		Float:z,
		Float:a
	;

	if(IsVehicleStreamedIn(vehicleid, playerid))
	{
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);

		x += 2.0 * floatsin(-a, degrees);
		y += 2.0 * floatcos(-a, degrees);

		if(Callback_IsVehicleInPoint(vehicleid, 2.0, x, y, z))
		{
			if(pInfo[playerid][player_can_spray] == false) return 0;
			return (1160 <= index <= 1163) || index == 1167 || index == 640;
		}
	}

	return 0;
}

stock GetPlayerGender(playerid)
{
	new gender[10];
	if(pInfo[playerid][player_sex] == 0) format(gender, sizeof(gender), "Female");
	else if(pInfo[playerid][player_sex] == 1) format(gender, sizeof(gender), "Male");
	return gender;
}

stock GetPlayerJob(playerid)
{
	new praca[20];
	if(pInfo[playerid][player_job] == 0) format(praca, sizeof(praca), "None");
	else if(pInfo[playerid][player_job] == 1) format(praca, sizeof(praca), "Lumberjack");
	else if(pInfo[playerid][player_job] == 2) format(praca, sizeof(praca), "Fisher");
    else if(pInfo[playerid][player_job] == 3) format(praca, sizeof(praca), "Truck driver");
    return praca;
}

stock OnPlayerVirtualWorldChange(playerid, vw)
{
	new d_id = GetDoorByUid(vw);
	
	if( d_id == -1 ) StopAudioStreamForPlayer(playerid);
	else
	{
		if( isnull(Door[d_id][door_audio]) ) StopAudioStreamForPlayer(playerid);
		else
		{
			PlayAudioStreamForPlayer(playerid, Door[d_id][door_audio], 0);
		}
	}
}

stock scrp_SpawnPlayer(playerid, bool:withSpawn = true)
{
	if( pInfo[playerid][player_bw] > 0 ) return scrp_SpawnPlayerWithBW(playerid, withSpawn);

	if( pInfo[playerid][player_aj] > 0) return scrp_SpawnPlayerInAJ(playerid, withSpawn);

	new time_away = gettime() - pInfo[playerid][player_quit_time];
	if( time_away < QS_TIME )
	{
		SetSpawnInfo(playerid, 255, pInfo[playerid][player_last_skin], pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2], pInfo[playerid][player_quit_pos][3], 0, 0, 0, 0, 0, 0);
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_posx`='0.0', `char_posy`='0.0', `char_posz`='0.0', `char_posa`='0.0', `char_world`=0, `char_interior`=0, `char_quittime`=0 WHERE `char_uid`=%d", pInfo[playerid][player_id]));
		SendFormattedClientMessage(playerid, COLOR_GREEN, "You position has been restored [time since logout: %d min %d s].", floatround(time_away/60, floatround_floor), time_away%60);
		SetPlayerVirtualWorld(playerid, pInfo[playerid][player_quit_vw]);
		SetPlayerInterior(playerid, pInfo[playerid][player_quit_int]);
	}
	else
	{
		if( pInfo[playerid][player_spawn_type] == SPAWN_TYPE_HOUSE) return scrp_SpawnPlayerInHouse(playerid, withSpawn);
		if( pInfo[playerid][player_spawn_type] == SPAWN_TYPE_HOTEL) return scrp_SpawnPlayerInHouse(playerid, withSpawn);
		
		if( !Iter_Count(GlobalSpawns) )
		{
			SendGuiInformation(playerid, ""guiopis"Information", "Server didnt loaded global spawns, call Raydex.");
			Kick(playerid);
		}
		else
		{
			new r_gspawn;

			if( pInfo[playerid][player_spawn_type] == SPAWN_TYPE_GLOBAL_SC)
			{
				r_gspawn = Iter_Random(ScSpawns);
			}
			else
			{
				r_gspawn = Iter_Random(GlobalSpawns);
			}

			SetSpawnInfo(playerid, 255, pInfo[playerid][player_skin], GlobalSpawn[r_gspawn][gspawn_pos][0], GlobalSpawn[r_gspawn][gspawn_pos][1], GlobalSpawn[r_gspawn][gspawn_pos][2], GlobalSpawn[r_gspawn][gspawn_pos][3], 0, 0, 0, 0, 0, 0 );
			SetPlayerVirtualWorld(playerid, 0);
			SetPlayerInterior(playerid, 0);
		}	
	}
	SetCameraBehindPlayer(playerid);
	if( withSpawn ) SpawnPlayer(playerid);
	
	return 1;
}

stock scrp_SpawnPlayerInHouse(playerid, bool:withSpawn = true)
{
	new d_id = GetDoorByUid(pInfo[playerid][player_spawn]);
	if(d_id == -1)
	{
		SendClientMessage(playerid, COLOR_GOLD, "Door where you live has been deleted. Restoring global spawn.");
		new gspawns = Iter_Random(GlobalSpawns);
		SetSpawnInfo(playerid, 255, pInfo[playerid][player_skin], GlobalSpawn[gspawns][gspawn_pos][0], GlobalSpawn[gspawns][gspawn_pos][1], GlobalSpawn[gspawns][gspawn_pos][2], 0, 0, 0, 0, 0, 0, 0 );
		SetPlayerVirtualWorld(playerid, 0);
		SetPlayerInterior(playerid, 0);
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_spawn = 0, char_spawn_type = 1 WHERE char_uid = %d", pInfo[playerid][player_id]));

		if( withSpawn ) SpawnPlayer(playerid);	
		return 1;
	}

	SetSpawnInfo(playerid, 255, pInfo[playerid][player_skin], Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], 0, 0, 0, 0, 0, 0, 0 );
	SetPlayerVirtualWorld(playerid, Door[d_id][door_spawn_vw]);
	SetPlayerInterior(playerid, Door[d_id][door_spawn_int]);
	if( withSpawn ) SpawnPlayer(playerid);	
	return 1;
}

stock scrp_SpawnPlayerWithBW(playerid, bool:withSpawn = true)
{
	AddPlayerStatus(playerid, PLAYER_STATUS_BW);
	// Spawn player
	SetSpawnInfo(playerid, 1, pInfo[playerid][player_last_skin], pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2]-0.2, pInfo[playerid][player_quit_pos][3], 0, 0, 0, 0, 0, 0);
	SetPlayerHealth(playerid, 19);
	TogglePlayerControllable(playerid, 0);
	
	if( withSpawn ) SpawnPlayer(playerid);	
	return 1;
}

stock scrp_SpawnPlayerInAJ(playerid, bool:withSpawn = true)
{
	SetSpawnInfo(playerid, 1, pInfo[playerid][player_skin], 154.0880,-1951.6383,47.8750, 0, 0, 0, 0, 0, 0, 0);
	SetPlayerVirtualWorld(playerid, 1000+playerid);
	if( withSpawn ) SpawnPlayer(playerid);	
	return 1;
}

stock SavePlayer(playerid)
{
	if( pInfo[playerid][player_bw] > 0 && pInfo[playerid][player_bw_end_time] > gettime() )
	{
		pInfo[playerid][player_bw] = pInfo[playerid][player_bw_end_time] - gettime();
	}
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_game_points` = %d WHERE `member_id` = %d", pGlobal[playerid][glo_score], pGlobal[playerid][glo_id]));
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_hours` = %d, `char_minutes` = %d, `char_health` = '%f', `char_bw` = %d, `char_strength` = '%f', `char_skill` = '%f' WHERE `char_uid` = %d", pInfo[playerid][player_hours], pInfo[playerid][player_minutes], pInfo[playerid][player_health], pInfo[playerid][player_bw], pInfo[playerid][player_strength], pInfo[playerid][player_weapon_skill], pInfo[playerid][player_id]));
	mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_logged_players` WHERE `char_uid` = %d", pInfo[playerid][player_id]));
	
	logprintf(LOG_PLAYER, "[save] Saving player %s (UID: %d)", pInfo[playerid][player_name], pInfo[playerid][player_id]);
}

stock UpdatePlayerColor(playerid)
{	
	if( !pInfo[playerid][player_logged] )
	{
		SetPlayerColor(playerid, 0x00000000);
		return;
	}
	
	if( IsPlayerVip(playerid) ) SetPlayerColor(playerid, 0xFFFFFF80);
	else SetPlayerColor(playerid, 0xADADAD80);
}

stock PreloadAnimLib(playerid, animlib[])
{
	ApplyAnimation(playerid,animlib,"null",0.0,0,0,0,0,0);
}

stock UpdatePlayerLabel(playerid)
{
	new nick_part[40], status_part[256], f_label[256];
	
	if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASKA) > -1 ) format(nick_part, sizeof(nick_part), "%s", pInfo[playerid][player_name]);	
	else format(nick_part, sizeof(nick_part), "%s (%d)", pInfo[playerid][player_name], playerid);
	SetPlayerName(playerid, pInfo[playerid][player_name]);
	strreplace(pInfo[playerid][player_name], '_', ' ');	
	strreplace(nick_part, '_', ' ');	
	
	// Format status
	sortIntegers( pInfo[playerid][player_status], 0, 5 );
	new comma[5], status_count, last_status;
	for(new s; s < 5; s++)
	{
		if( pInfo[playerid][player_status][s] > 0 )
		{
			status_count += 1;
			last_status = s;
		}
	}
	for(new i; i < 5; i++)
	{
		if( pInfo[playerid][player_status][i] > 0 )
		{
			format(comma, 5, "");
			if( status_count > 1 && last_status != i ) format(comma, 5, ", ");
						
			switch( pInfo[playerid][player_status][i] )
			{
				case PLAYER_STATUS_AFK:
				{
					new afkHour, afkMinute, temp;
					TimestampToDate(pInfo[playerid][player_last_activity], temp, temp, temp, afkHour, afkMinute, temp, 1);
					
					if( afkHour == 24 ) afkHour = 0;
					format(status_part, sizeof(status_part), "%sAFK since %02d:%02d%s", status_part, afkHour+1, afkMinute, comma);
				}
				
				case PLAYER_STATUS_BELT:
				{
					format(status_part, sizeof(status_part), "%sseatbelts%s", status_part, comma);
				}
				case PLAYER_STATUS_JAIL:
    			{
    				if(pInfo[playerid][player_sex] == 0)
    				{
     					format(status_part, sizeof(status_part), "%sprisoned%s", status_part, comma);
     				}
     				else
     				{
     					format(status_part, sizeof(status_part), "%sprisoned%s", status_part, comma);
     				}
    			}
				case PLAYER_STATUS_EXTRA:
    			{
     				format(status_part, sizeof(status_part), "%s%s%s", status_part, pInfo[playerid][player_extra_status], comma);
    			}	
               case PLAYER_STATUS_NEW:
				{
					format(status_part, sizeof(status_part), "%snew player%s", status_part, comma);
				}
				
				case PLAYER_STATUS_BW:
				{
					if(pInfo[playerid][player_sex] == 0)
					{
						format(status_part, sizeof(status_part), "%sbrutally wounded%s", status_part, comma);
					}
					else
					{
						format(status_part, sizeof(status_part), "%sbrutally wounded%s", status_part, comma);
					}
				}
				
				case PLAYER_STATUS_DEALING:
				{
					format(status_part, sizeof(status_part), "%shandluje%s", status_part, comma);
				}
				case PLAYER_STATUS_DRUNK:
				{
					if(pInfo[playerid][player_sex] == 0)
					{
						format(status_part, sizeof(status_part), "%sdrunk%s", status_part, comma);
					}
					else
					{
						format(status_part, sizeof(status_part), "%sdrunk%s", status_part, comma);
					}
				}
				case PLAYER_STATUS_SPORT:
				{
					if(pInfo[playerid][player_sex] == 0)
					{
						format(status_part, sizeof(status_part), "%sathletic%s", status_part, comma);
					}
					else
					{
						format(status_part, sizeof(status_part), "%sathletic%s", status_part, comma);
					}
				}
				case PLAYER_STATUS_MUSCULAR:
				{
					if(pInfo[playerid][player_sex] == 0)
					{
						format(status_part, sizeof(status_part), "%smuscular%s", status_part, comma);
					}
					else
					{
						format(status_part, sizeof(status_part), "%smuscular%s", status_part, comma);
					}
				}
				case PLAYER_STATUS_DRUGS:
				{
					if(pInfo[playerid][player_sex] == 0)
					{
						format(status_part, sizeof(status_part), "%sstoned%s", status_part, comma);
					}
					else
					{
						format(status_part, sizeof(status_part), "%sstoned%s", status_part, comma);
					}
				}
				case PLAYER_STATUS_WEED:
				{
					if(pInfo[playerid][player_sex] == 0)
					{
						format(status_part, sizeof(status_part), "%sstoned%s", status_part, comma);
					}
					else
					{
						format(status_part, sizeof(status_part), "%sstoned%s", status_part, comma);
					}
				}
				case PLAYER_STATUS_REKAWICZKI:
				{
					format(status_part, sizeof(status_part), "%sgloves%s", status_part, comma);
				}
				case PLAYER_STATUS_ARMOUR:
				{
					format(status_part, sizeof(status_part), "%sarmour%s", status_part, comma);
				}
			}
		}
	}
	
	if( status_count > 0 ) format(status_part, sizeof(status_part), "(%s)", status_part);

	if( isnull(status_part) ) format(f_label, sizeof(f_label), "%s", nick_part);
	else format(f_label, sizeof(f_label), "%s\n%s", nick_part, status_part);
	
	if( pInfo[playerid][player_taken_damage] > 0 ) Update3DTextLabelText(pInfo[playerid][player_label], LABEL_NICK_DAMAGED_COLOR, f_label);
	else
	{
		new slot = GetPlayerDutySlot(playerid);
		if( slot > -1 )
		{
			new gid = pGroup[playerid][slot][pg_id];
			if( GroupHasFlag(gid, GROUP_FLAG_COLORED) )
			{
				new first = GetGroupColor(gid);
				new color = (first & 0xFFFFFFDD);
				Update3DTextLabelText(pInfo[playerid][player_label], color, f_label);
			}
			else if(pGlobal[playerid][glo_premium] == 1){Update3DTextLabelText(pInfo[playerid][player_label], LABEL_NICK_PREMIUM_COLOR, f_label);}else{Update3DTextLabelText(pInfo[playerid][player_label], LABEL_NICK_NORMAL_COLOR, f_label);}
		}
		else if(pGlobal[playerid][glo_premium] == 1){Update3DTextLabelText(pInfo[playerid][player_label], LABEL_NICK_PREMIUM_COLOR, f_label);}else{Update3DTextLabelText(pInfo[playerid][player_label], LABEL_NICK_NORMAL_COLOR, f_label);}
	}
}

stock AddPlayerStatus(playerid, type)
{
	new free_slot = -1;
	for(new s; s < 5; s++)
	{
		if( pInfo[playerid][player_status][s] == 0 && free_slot == -1 ) free_slot = s;
		if( pInfo[playerid][player_status][s] == type ) return;
	}
	
	if( free_slot == -1 ) return;
	
	pInfo[playerid][player_status][free_slot] = type;
	UpdatePlayerLabel(playerid);
}

stock RemovePlayerStatus(playerid, type)
{
	for(new s; s < 5; s++)
	{
		if( pInfo[playerid][player_status][s] == 0 ) continue;	
		if( pInfo[playerid][player_status][s] == type ) pInfo[playerid][player_status][s] = 0;
	}	
	UpdatePlayerLabel(playerid);
}

stock ApplyCommandAnim(playerid, alias[])
{
	if( !strcmp(alias, "/", true) )		return SendClientMessage(playerid, COLOR_GREY, "Detected invalid symbols.");
	if( !strcmp(alias, "%", true) )		return SendClientMessage(playerid, COLOR_GREY, "Detected invalid symbols.");
	if( !strcmp(alias, "$", true) )		return SendClientMessage(playerid, COLOR_GREY, "Detected invalid symbols.");
	if( !strcmp(alias, "#", true) )		return SendClientMessage(playerid, COLOR_GREY, "Detected invalid symbols.");
	if( !strcmp(alias, "!", true) )		return SendClientMessage(playerid, COLOR_GREY, "Detected invalid symbols.");
	if( !strcmp(alias, "@", true) )		return SendClientMessage(playerid, COLOR_GREY, "Detected invalid symbols.");
	if( !strcmp(alias, "*", true) )		return SendClientMessage(playerid, COLOR_GREY, "Detected invalid symbols.");
	if( !strcmp(alias, "(", true) )		return SendClientMessage(playerid, COLOR_GREY, "Detected invalid symbols.");
	if( !strcmp(alias, ")", true) )		return SendClientMessage(playerid, COLOR_GREY, "Detected invalid symbols.");

	if(pInfo[playerid][player_bw] == 0)
	{
		mysql_query(sprintf("SELECT * FROM `ipb_anim` WHERE `anim_command` = '%s'", alias));
		mysql_store_result();
		
		if( mysql_num_rows() == 1 )
		{
			new anim_action = mysql_fetch_field_int("anim_action");
			
			if( anim_action > 0 )
			{
				SetPlayerSpecialAction(playerid, anim_action);
			}
			else
			{
				new anim_lib[20], anim_name[20];
				mysql_fetch_field("anim_lib", anim_lib);
				mysql_fetch_field("anim_name", anim_name);
				
				pInfo[playerid][player_looped_anim] = true;
				
				ApplyAnimation(playerid, anim_lib, anim_name, mysql_fetch_field_float("anim_speed"), mysql_fetch_field_int("anim_opt1"), mysql_fetch_field_int("anim_opt2"), mysql_fetch_field_int("anim_opt3"), mysql_fetch_field_int("anim_opt4"), mysql_fetch_field_int("anim_opt5"), 1);	
			}
		}
		else
		{
			PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
		}
		
		mysql_free_result();
	}
	else
	{
		SendGuiInformation(playerid, "Information", "You can't use animation when you are brutally wounded.");
	}
	return 1;
}

stock FreezePlayer(playerid, time = 1000)
{
	TogglePlayerControllable(playerid, 0);
	pInfo[playerid][player_freeze] = time/1000;
}


stock CleanGlobalData(playerid)
{
	for(new z=0; e_global_account_info:z != e_global_account_info; z++)
    {
		gInfo[playerid][e_global_account_info:z] = 0;
    }
    for(new z=0; e_player_global:z != e_player_global; z++)
    {
		pGlobal[playerid][e_player_global:z] = 0;
    }
    for(new z=0; e_session_info:z != e_session_info; z++)
    {
		sInfo[playerid][e_session_info:z] = 0;
    }
}

stock GetPlayerFreeAttachSlot(playerid)
{
	if(IsPlayerVip(playerid))
	{
	    for(new i;i<3;i++)
	    {
			if( !IsPlayerAttachedObjectSlotUsed(playerid, i) ) return i;
	    }
	}
	else
	{
		for(new i;i<4;i++)
	    {
			if( !IsPlayerAttachedObjectSlotUsed(playerid, i) ) return i;
	    }
	}
	
    return -1;
}

stock CleanPlayerDamages(playerid)
{
	for(new y;y<10;y++)
	{
		pInjury[playerid][y][injury_id] = 0;
	}
}

stock CleanPlayerData(playerid)
{
	new Text3D:tmp_label = pInfo[playerid][player_label];
	new Text3D:tmp_label2 = pInfo[playerid][player_description_label];
	
	for(new z=0; e_players_info:z != e_players_info; z++)
    {
		pInfo[playerid][e_players_info:z] = 0;
    }
	
	for(new x=0; e_player_offer:x != e_player_offer; x++)
	{
		pOffer[playerid][e_player_offer:x] = 0;
	}
	
	for(new y;y<13;y++)
	{
		pWeapon[playerid][y][pw_itemid] = -1;
		pWeapon[playerid][y][pw_id] = -1;
		pWeapon[playerid][y][pw_ammo] = -1;
		pWeapon[playerid][y][pw_object_index] = -1;
	}

	CleanPlayerDamages(playerid);
	
	pInfo[playerid][player_gym_carnet] = 10;
	pInfo[playerid][player_label] = tmp_label;
	pInfo[playerid][player_description_label] = tmp_label2;
	pInfo[playerid][player_entering_vehicle] = -1;
	pInfo[playerid][player_occupied_vehicle] = -1;
	pInfo[playerid][player_vehicle_target] = -1;
	pInfo[playerid][player_last_pm_playerid] = INVALID_PLAYER_ID;
	pInfo[playerid][player_last_group_slot_chat] = -1;
	pInfo[playerid][player_duty_gid] = -1;
	pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;
	pInfo[playerid][player_phone_receiver] = INVALID_PLAYER_ID;
	pInfo[playerid][player_phone_object_index] = -1;
	pInfo[playerid][player_spec] = INVALID_PLAYER_ID;
	pInfo[playerid][player_taxi_veh] = INVALID_VEHICLE_ID;
	pInfo[playerid][player_interview] = -1;

	pInfo[playerid][player_spraying_car] = INVALID_VEHICLE_ID;
	pInfo[playerid][player_repair_car] = INVALID_VEHICLE_ID;
	pInfo[playerid][player_montage_car] = INVALID_VEHICLE_ID;
	pInfo[playerid][player_bomb_car] = INVALID_VEHICLE_ID;
	pInfo[playerid][player_phone_model] = 330;
	
	for(new i;i<5;i++) CleanPlayerGroup(playerid, i, false);
}

stock CleanPlayerGroup(playerid, slot, bool:do_reload = true)
{
	for(new z=0; e_player_groups:z != e_player_groups; z++)
    {
		pGroup[playerid][slot][e_player_groups:z] = 0;
    }
	
	pGroup[playerid][slot][pg_id] = -1;
	pGroup[playerid][slot][pg_rank_skin] = -1;
	
	if( do_reload )
	{
		ReloadPlayerGroupData(playerid);
	}
}

stock ReloadPlayerGroupData(playerid)
{
	for(new i;i<4;i++)
	{
		if( pGroup[playerid][i][pg_id] == -1 )
		{
			if( pGroup[playerid][i+1][pg_id] != -1 )
			{
				pGroup[playerid][i][pg_id] = pGroup[playerid][i+1][pg_id];
				
				pGroup[playerid][i][pg_rank_skin] = pGroup[playerid][i+1][pg_rank_skin];
				pGroup[playerid][i][pg_rank_payment] = pGroup[playerid][i+1][pg_rank_payment];
				pGroup[playerid][i][pg_rank_perm] = pGroup[playerid][i+1][pg_rank_perm];
				pGroup[playerid][i][pg_duty_start] = pGroup[playerid][i+1][pg_duty_start];
				pGroup[playerid][i][pg_duty] = pGroup[playerid][i+1][pg_duty];
				
				strcopy(pGroup[playerid][i][pg_rank_title], pGroup[playerid][i+1][pg_rank_title], 32);
				
				CleanPlayerGroup(playerid, i+1, false);
			}
		}
	}
}

stock HidePlayerDialog(playerid)
{
	ShowPlayerDialog(playerid,-1,0,"","","","");
}

stock HourPayday(playerid)
{
	SendPlayerInformation(playerid, "~w~You get ~p~$50~w~ for hour online.", 5000);
	GivePlayerMoney(playerid, 50);
}

stock SellDrug(playerid, price, itemid)
{
	if(PlayerItem[playerid][itemid][player_item_value2] == 1)
	{
		DeleteItem(itemid, true, playerid);
		GivePlayerMoney(playerid, price);

		new str[64];
		format(str, sizeof(str), "~w~Drugs sold. You get ~p~$%d~w~.", price);
		SendPlayerInformation(playerid, str, 5000);

		RemovePlayerStatus(playerid, PLAYER_STATUS_DEALING);
		ApplyAnimation(playerid, "DEALER", "DEALER_DEAL", 4, 0, 0, 0, 0, 0, 0);
		ProxMessage(playerid, "deals with client.", PROX_ME);
	}
	else if(PlayerItem[playerid][itemid][player_item_value2] > 1)
	{
		PlayerItem[playerid][itemid][player_item_value2]--;
		PlayerItem[playerid][itemid][player_item_weight] = PlayerItem[playerid][itemid][player_item_value2];

		pInfo[playerid][player_capacity] += PlayerItem[playerid][itemid][player_item_weight];
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_value2 = %d, item_weight = %d WHERE item_uid = %d AND item_owner = %d", PlayerItem[playerid][itemid][player_item_value2], PlayerItem[playerid][itemid][player_item_value2], PlayerItem[playerid][itemid][player_item_uid], pInfo[playerid][player_id]));
		
		if(cache_affected_rows(mySQLconnection))
		{
			GivePlayerMoney(playerid, price);
			
			new str[64];
			format(str, sizeof(str), "~w~Drugs sold. You get ~p~$%d~w~.", price);
			SendPlayerInformation(playerid, str, 5000);

			RemovePlayerStatus(playerid, PLAYER_STATUS_DEALING);
			ApplyAnimation(playerid, "DEALER", "DEALER_DEAL", 4, 0, 0, 0, 0, 0, 0);
			ProxMessage(playerid, "deals with client.", PROX_ME);
		}
	}
	return 1;
}

stock UpdatePlayerBWTextdraw(playerid)
{
	new bstring[50], Float:TimeInMin = floatdiv((pInfo[playerid][player_bw_end_time] - gettime()), 60);
	format(bstring, sizeof(bstring), "~w~end of ~y~bw~w~: ~y~%d ~w~min.", floatround(TimeInMin, floatround_ceil));	
	GameTextForPlayer(playerid,  bstring, 5000, 1);
}

task PlayerLoop[1000]()
{
	if(DeletePenalty != -1)
    {
        DeletePenalty--;
	  	if(DeletePenalty < 1)
 		{
   			TextDrawHideForAll(TextPenaltyType);
   			TextDrawHideForAll(TextPenalty);
   			TextDrawHideForAll(TextPenaltySprite);
		   	DeletePenalty = -1;
	  	}
	}

	foreach(new playerid : Player)
	{
		if(pInfo[playerid][player_race_count]==4)
		{
		    PlayerPlaySound(playerid, 1056, 0.0, 0.0, 0.0);
			GameTextForPlayer(playerid, "~r~3!", 5000, 3);
		    pInfo[playerid][player_race_count]--;
		}
		if(pInfo[playerid][player_race_count]==3)
		{
		    PlayerPlaySound(playerid, 1056, 0.0, 0.0, 0.0);
		    GameTextForPlayer(playerid, "~y~2", 5000, 3);
		    pInfo[playerid][player_race_count]--;
		}
		if(pInfo[playerid][player_race_count]==2)
		{
		    PlayerPlaySound(playerid, 1056, 0.0, 0.0, 0.0);
		    GameTextForPlayer(playerid, "~w~1", 5000, 3);
		    pInfo[playerid][player_race_count]--;
		}
		if(pInfo[playerid][player_race_count]==1)
		{
		    PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
		    GameTextForPlayer(playerid, "~g~START", 5000, 3);
   			TogglePlayerControllable(playerid, 1);
   			pInfo[playerid][player_race_count]--;
		}

		if( pInfo[playerid][player_logged] )
		{
			if( gettime() - pInfo[playerid][player_last_activity] >= AFK_TIME && !pInfo[playerid][player_afk] )
			{
				AddPlayerStatus(playerid, PLAYER_STATUS_AFK);
				pInfo[playerid][player_afk] = true;
			}
			
			new zone[MAX_ZONE_NAME];
			GetPlayer2DZone(playerid, zone, MAX_ZONE_NAME);
			if(strlen(zone) > 2)
			{
				PlayerTextDrawSetString(playerid, ZoneName, zone);
			}

			if(pInfo[playerid][player_info_td_id] > 0)
			{
				pInfo[playerid][player_info_td_id]--;

				if(pInfo[playerid][player_info_td_id] == 0)
				{
					TextDrawHideForPlayer(playerid, Tutorial[playerid]);
					pInfo[playerid][player_info_td_id] = 0;
				}
			}

			if( pInfo[playerid][player_freeze] > 0)
			{
				pInfo[playerid][player_freeze]--;
				if(pInfo[playerid][player_freeze] == 0)
				{
					TogglePlayerControllable(playerid, 1);
					pInfo[playerid][player_freeze]=0;
				}
			}

			if(GetPlayerSurfingVehicleID(playerid) != INVALID_VEHICLE_ID)
			{
				new surf = GetPlayerSurfingVehicleID(playerid);
				if(Vehicle[surf][vehicle_model] == 481 || Vehicle[surf][vehicle_model] == 509 || Vehicle[surf][vehicle_model] == 510)
				{
					if(pInfo[playerid][player_admin_duty] == false)
					{
						KickAc(playerid, -1, "Riding on bike (no seat)");
						continue;
					}
				}
			}

			if(!pInfo[playerid][player_drunk] && GetPlayerDrunkLevel(playerid) >= 5000)
			{
				AddPlayerStatus(playerid, PLAYER_STATUS_DRUNK);
				pInfo[playerid][player_drunk] = true;
			}

			if(pInfo[playerid][player_gps] > 0)
			{
				foreach(new police: Player)
				{
					if(pInfo[police][player_gps] > 0)
					{
						new Float:x, Float:y, Float:z;
						GetPlayerPos(police, x, y, z);
						SetPlayerMapIcon(playerid, police, x, y, z, 30, 0);
					}
				}
			}

			if(pInfo[playerid][player_dealing] > 0)
			{
				if(!pInfo[playerid][player_afk])
				{
					pInfo[playerid][player_dealing] --;
					TextDrawSetString(Tutorial[playerid], sprintf("~w~Waiting for client.~n~Remaining time: ~y~%ds", pInfo[playerid][player_dealing]));
					TextDrawShowForPlayer(playerid, Tutorial[playerid]);

					if(pInfo[playerid][player_dealing] < 1)
					{
						new type, itemid;
						itemid = pInfo[playerid][player_dialog_tmp4];
						type = PlayerItem[playerid][itemid][player_item_value1];
						new drug_price;

						switch(type)
						{
							case DRUG_TYPE_COCAINE:
							{
								drug_price = 250 + random(35);
							}
							case DRUG_TYPE_CRACK:
							{
								drug_price = 105 + random(15);
							}
							case DRUG_TYPE_HEROINE:
							{
								drug_price = 140 + random(35);
							}
							case DRUG_TYPE_AMFA:
							{
								drug_price = 120 + random(20);
							}
							case DRUG_TYPE_WEED:
							{
								drug_price = 75 + random(15);
							}
							case DRUG_TYPE_METH:
							{
								drug_price = 180 + random(20);
							}
							case DRUG_TYPE_EXTASY:
							{
								drug_price = 85 + random(15);
							}
						}

						pInfo[playerid][player_dialog_tmp4] = 0;
						TextDrawHideForPlayer(playerid, Tutorial[playerid]);
						
						SellDrug(playerid, drug_price, itemid);
						pInfo[playerid][player_dealing] = false;
					}
				}
			}

			if(pInfo[playerid][player_taxi_veh] != INVALID_VEHICLE_ID && pInfo[playerid][player_taxi_drive] == true)
			{
				new passenger = pInfo[playerid][player_taxi_passenger];

				if(pInfo[passenger][player_taxi_veh] == pInfo[playerid][player_taxi_veh] && pInfo[passenger][player_money] > 0)
				{
					if(pInfo[passenger][player_money]-pInfo[playerid][player_taxi_price] <= pInfo[playerid][player_taxi_cost])
					{
		 				RemovePlayerFromVehicle(passenger);
		 				ClearAnimations(passenger);
					}

					new vid = pInfo[playerid][player_taxi_veh], str[64];
					new Float:distance = GetVehicleDistanceFromPoint(vid, Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2]);

					if( distance > 0 )
					{
						pInfo[playerid][player_taxi_distance] += distance/150;
					}

					pInfo[playerid][player_taxi_cost] = pInfo[playerid][player_taxi_price]*floatround(pInfo[playerid][player_taxi_distance]);
					format(str, sizeof(str), "~n~~g~Taxi: ~w~$%d.", pInfo[playerid][player_taxi_cost]);
					GameTextForPlayer(passenger, str, 1000, 3);
					GameTextForPlayer(playerid, str, 1000, 3);
				}
			}

			if(pInfo[playerid][player_bomb_car] != INVALID_VEHICLE_ID && pInfo[playerid][player_bomb_planting] != 0 )
			{
	  			new vehid = pInfo[playerid][player_bomb_car], Float:VehX, Float:VehY, Float:VehZ, str[64];
		    	GetVehiclePos(vehid, VehX, VehY, VehZ);
				if(IsPlayerInRangeOfPoint(playerid, 4.0, VehX, VehY, VehZ))
				{
	   				pInfo[playerid][player_bomb] --;

					format(str, sizeof(str), "~n~~n~~n~~n~~n~~n~~n~~g~Time left: ~w~%d seconds.", pInfo[playerid][player_bomb]);
					GameTextForPlayer(playerid, str, 1000, 3);

					if(pInfo[playerid][player_bomb] <= 0)
					{
					    SendGuiInformation(playerid, "Information", "Bomb has been planted!\nYou can detonate it by using /bomb detonate.");
	  					pInfo[playerid][player_bomb] = 0;
	  					pInfo[playerid][player_bomb_planting] = 0;
					}
				}
			}

			if(pInfo[playerid][player_repair_car] != INVALID_VEHICLE_ID && pInfo[playerid][player_repairing] == 1)
			{
				new vehid = pInfo[playerid][player_repair_car], Float:VehX, Float:VehY, Float:VehZ;
				new string[64], str[32];
			    GetVehiclePos(vehid, VehX, VehY, VehZ);
			    if(IsPlayerInRangeOfPoint(playerid, 4.0, VehX, VehY, VehZ))
			    {
					if(!Vehicle[vehid][vehicle_engine])
					{
						format(str, sizeof(str), "Vehicle is being fixed (%ds)", pInfo[playerid][player_repair_time]);
						format(string, sizeof(string), "~w~Time left: ~y~%d ~w~seconds.", pInfo[playerid][player_repair_time]);

				        pInfo[playerid][player_repair_time] --;

				        UpdateDynamic3DTextLabelText(Vehicle[vehid][vehicle_state_label], COLOR_CAR_STATE, str);
				        TextDrawSetString(Tutorial[playerid], string);
				        TextDrawShowForPlayer(playerid, Tutorial[playerid]);

	  					if(pInfo[playerid][player_repair_time] <= 0)
	  					{
	  						new price = pInfo[playerid][player_fixprice];
	  						new panels, doors, lights, tires;

	  						GetVehicleDamageStatus(vehid, panels, doors, lights, tires);
	  						
	  					   	Vehicle[vehid][vehicle_damage][0] = 0;
	  					    Vehicle[vehid][vehicle_damage][1] = 0;
	  					    Vehicle[vehid][vehicle_damage][2] = 0;
	  					    Vehicle[vehid][vehicle_damage][3] = tires;

  					    	Vehicle[vehid][vehicle_health] = 1000.00;
  					    	SetVehicleHealth(vehid, 1000);
	  					    
	  					    Vehicle[vehid][vehicle_destroyed] = false;

	  					    UpdateVehicleDamageStatus(vehid, Vehicle[vehid][vehicle_damage][0], Vehicle[vehid][vehicle_damage][1], Vehicle[vehid][vehicle_damage][2], Vehicle[vehid][vehicle_damage][3]);

	  					    new visual_damage[32];
							format(visual_damage, sizeof(visual_damage), "%d %d %d %d", Vehicle[vehid][vehicle_damage][0], Vehicle[vehid][vehicle_damage][1], Vehicle[vehid][vehicle_damage][2], Vehicle[vehid][vehicle_damage][3]);
	  					    mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_health = '%f', vehicle_visual= '%s' WHERE vehicle_uid = %d", Vehicle[vehid][vehicle_health], visual_damage, Vehicle[vehid][vehicle_uid]));

	  					    SendGuiInformation(playerid,"Information", "Vehicle is fixed!");

	  					    new gid = pInfo[playerid][player_duty_gid];
	  					    if(gid != -1)
	  					    {
	  					    	if(Group[gid][group_type] == GROUP_TYPE_WORKSHOP)
	  					    	{
	  					    		if(price >= 20)
	  					    		{
	  					    			GiveGroupMoney(gid, price-20);
	  					    			GivePlayerMoney(playerid, 20);
	  					    		}
	  					    		else
	  					    		{
	  					    			GiveGroupMoney(gid, price);
	  					    		}
	  					    	}
	  					    }
	  					    else
	  					    {
	  					    	GivePlayerMoney(playerid, price);
	  					    }

							pInfo[playerid][player_repair_car] = INVALID_VEHICLE_ID;
							pInfo[playerid][player_repair_time] = 0;
							TextDrawHideForPlayer(playerid, Tutorial[playerid]);
							UpdateDynamic3DTextLabelText(Vehicle[vehid][vehicle_state_label], COLOR_CAR_STATE, "");
							pInfo[playerid][player_repairing]=0;
						}
					}
					else
					{
					    GameTextForPlayer(playerid, "~w~Engine must be ~y~off~w~!", 1000, 3);
					}
			    }
			}

			if(pInfo[playerid][player_montage_car] != INVALID_VEHICLE_ID && pInfo[playerid][player_tuning] == 2)
            {
                new vehid = pInfo[playerid][player_montage_car], Float:VehX, Float:VehY, Float:VehZ;
                new string[64], str[32];
                GetVehiclePos(vehid, VehX, VehY, VehZ);
                if(IsPlayerInRangeOfPoint(playerid, 4.0, VehX, VehY, VehZ))
                {
                    if(!Vehicle[vehid][vehicle_engine])
                    {
                        format(str, sizeof(str), "Installing component (%ds)", pInfo[playerid][player_montage_time]);
                        format(string, sizeof(string), "~w~Time left: ~y~%d ~w~.", pInfo[playerid][player_montage_time]);
 
                        pInfo[playerid][player_montage_time] --;
 
                        UpdateDynamic3DTextLabelText(Vehicle[vehid][vehicle_state_label], COLOR_CAR_STATE, str);
                        TextDrawSetString(Tutorial[playerid], string);
                        TextDrawShowForPlayer(playerid, Tutorial[playerid]);
 
                        if(pInfo[playerid][player_montage_time] <= 0)
                        {
                            new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_ACCESS);
                            new compid = pInfo[playerid][player_montage_comp];
 
                            Vehicle[vehid][vehicle_accessories] += compid;
                           
                            mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_access = '%d' WHERE vehicle_uid = %d", Vehicle[vehid][vehicle_accessories], Vehicle[vehid][vehicle_uid]));
 
                            SendGuiInformation(playerid,"Information", "Component has been installed.");
 
                            pInfo[playerid][player_montage_car] = INVALID_VEHICLE_ID;
                            pInfo[playerid][player_montage_time] = 0;
 
                            DeleteItem(itemid, true, playerid);
 
                            TextDrawHideForPlayer(playerid, Tutorial[playerid]);
                            UpdateDynamic3DTextLabelText(Vehicle[vehid][vehicle_state_label], COLOR_CAR_STATE, "");
                            pInfo[playerid][player_tuning] = 0;
                        }
                    }
                    else
                    {
                        GameTextForPlayer(playerid, "~w~Engine must be ~y~off~w~!", 1000, 3);
                    }
                }
            }

			if(pInfo[playerid][player_montage_car] != INVALID_VEHICLE_ID && pInfo[playerid][player_tuning] == 1)
			{
				new vehid = pInfo[playerid][player_montage_car], Float:VehX, Float:VehY, Float:VehZ;
				new string[64], str[32];
			    GetVehiclePos(vehid, VehX, VehY, VehZ);
			    if(IsPlayerInRangeOfPoint(playerid, 4.0, VehX, VehY, VehZ))
			    {
					if(!Vehicle[vehid][vehicle_engine])
					{
						format(str, sizeof(str), "Installing tuning (%ds)", pInfo[playerid][player_montage_time]);
						format(string, sizeof(string), "~w~Time left: ~y~%d ~w~.", pInfo[playerid][player_montage_time]);

				        pInfo[playerid][player_montage_time] --;

				        UpdateDynamic3DTextLabelText(Vehicle[vehid][vehicle_state_label], COLOR_CAR_STATE, str);
				        TextDrawSetString(Tutorial[playerid], string);
				        TextDrawShowForPlayer(playerid, Tutorial[playerid]);

	  					if(pInfo[playerid][player_montage_time] <= 0)
	  					{
	  						new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_TUNING);
	  						new compid = pInfo[playerid][player_montage_comp];

	  						srp_AddVehicleComponent(vehid, compid);

	  						new comp0 = Vehicle[vehid][vehicle_component][0];
	  						new comp1 = Vehicle[vehid][vehicle_component][1];
	  						new comp2 = Vehicle[vehid][vehicle_component][2];
	  						new comp3 = Vehicle[vehid][vehicle_component][3];
	  						new comp4 = Vehicle[vehid][vehicle_component][4];
	  						new comp5 = Vehicle[vehid][vehicle_component][5];
	  						new comp6 = Vehicle[vehid][vehicle_component][6];
	  						new comp7 = Vehicle[vehid][vehicle_component][7];
	  						new comp8 = Vehicle[vehid][vehicle_component][8];
	  						new comp9 = Vehicle[vehid][vehicle_component][9];
	  						new comp10 = Vehicle[vehid][vehicle_component][10];
	  						new comp11 = Vehicle[vehid][vehicle_component][11];
	  						new comp12 = Vehicle[vehid][vehicle_component][12];
	  						new comp13 = Vehicle[vehid][vehicle_component][13];
	  						
	  					    new visual_tuning[128];
							format(visual_tuning, sizeof(visual_tuning), "%d %d %d %d %d %d %d %d %d %d %d %d %d %d", comp0, comp1, comp2, comp3, comp4, comp5, comp6, comp7,comp8, comp9, comp10, comp11, comp12, comp13);
	  					    mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_component = '%s' WHERE vehicle_uid = %d", visual_tuning, Vehicle[vehid][vehicle_uid]));

	  					    SendGuiInformation(playerid,"Information", "Component has been installed.");

							pInfo[playerid][player_montage_car] = INVALID_VEHICLE_ID;
							pInfo[playerid][player_montage_time] = 0;
							DeleteItem(itemid, true, playerid);
							TextDrawHideForPlayer(playerid, Tutorial[playerid]);
							UpdateDynamic3DTextLabelText(Vehicle[vehid][vehicle_state_label], COLOR_CAR_STATE, "");
							pInfo[playerid][player_tuning] = 0;
						}
					}
					else
					{
					    GameTextForPlayer(playerid, "~w~Engine must be ~y~off~w~!", 1000, 3);
					}
			    }
			}

			if(pInfo[playerid][player_spraying_car] != INVALID_VEHICLE_ID && pInfo[playerid][player_spraying] == 1)
			{
				new vehid = pInfo[playerid][player_spraying_car], Float:VehX, Float:VehY, Float:VehZ;
				new str[42];
			    GetVehiclePos(vehid, VehX, VehY, VehZ);
			    if(IsPlayerInRangeOfPoint(playerid, 4.0, VehX, VehY, VehZ))
			    {
					if(!Vehicle[vehid][vehicle_engine])
					{
						if(IsPlayerSprayingVehicle(playerid, vehid))
						{
							new spraytime = pInfo[playerid][player_spraying_time];
							format(str, sizeof(str), "Vehicle is being painted (%d%%)", spraytime);

							new tmp, ammo;
							GetPlayerWeaponData(playerid, 9, tmp, ammo);
							if(ammo != pInfo[playerid][player_last_spray])
							{
								pInfo[playerid][player_last_spray] = ammo;
						        pInfo[playerid][player_spraying_time] += 2;

						        UpdateDynamic3DTextLabelText(Vehicle[vehid][vehicle_state_label], COLOR_CAR_STATE, str);

			  					if(pInfo[playerid][player_spraying_time] >= 100)
			  					{
			  						new color1 = pInfo[playerid][player_spraying_color];
			  						new color2 = pInfo[playerid][player_spraying_color2];

			  						ChangeVehicleColor(vehid, color1, color2);

			  					    mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_color1 = '%d', vehicle_color2 = '%d' WHERE vehicle_uid = %d", color1, color2, Vehicle[vehid][vehicle_uid]));

			  					    SendGuiInformation(playerid,"Information", "Vehicle has been painted.");

									pInfo[playerid][player_spraying_car] = INVALID_VEHICLE_ID;
									pInfo[playerid][player_spraying_time] = 0;

									UpdateDynamic3DTextLabelText(Vehicle[vehid][vehicle_state_label], COLOR_CAR_STATE, "");
									pInfo[playerid][player_spraying] = 0;
								}
							}
						}
					}
					else
					{
					    GameTextForPlayer(playerid, "~w~Engine must be ~y~off~w~!", 1000, 3);
					}
			    }
			}

			if(pInfo[playerid][player_spraying_car] != INVALID_VEHICLE_ID && pInfo[playerid][player_spraying] == 2)
			{
				new vehid = pInfo[playerid][player_spraying_car], Float:VehX, Float:VehY, Float:VehZ;
				new str[42], sc;
			    GetVehiclePos(vehid, VehX, VehY, VehZ);
			    if(IsPlayerInRangeOfPoint(playerid, 4.0, VehX, VehY, VehZ))
			    {
					if(!Vehicle[vehid][vehicle_engine])
					{
						if(IsPlayerSprayingVehicle(playerid, vehid))
						{
							new spraytime = pInfo[playerid][player_spraying_time];
							format(str, sizeof(str), "Vehicle is being painted (%d%%)", spraytime);

							sc++;

							if(sc==2)
							{
					        	pInfo[playerid][player_spraying_time] += 2;
					        	sc=0;
					        }

					        UpdateDynamic3DTextLabelText(Vehicle[vehid][vehicle_state_label], COLOR_CAR_STATE, str);

		  					if(pInfo[playerid][player_spraying_time] >= 100)
		  					{
		  						new color1 = pInfo[playerid][player_spraying_color];

		  						ChangeVehiclePaintjob(vehid, color1);

		  					    mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_paintjob = '%d' WHERE vehicle_uid = '%d'", color1, Vehicle[vehid][vehicle_uid]));

		  					    SendGuiInformation(playerid,"Information", "Paintjob finished.");

								pInfo[playerid][player_spraying_car] = INVALID_VEHICLE_ID;
								pInfo[playerid][player_spraying_time] = 0;

								UpdateDynamic3DTextLabelText(Vehicle[vehid][vehicle_state_label], COLOR_CAR_STATE, "");
								pInfo[playerid][player_spraying] = 0;
							}
						}
					}
					else
					{
					    GameTextForPlayer(playerid, "~w~Engine must be ~y~off~w~!", 1000, 3);
					}
			    }
			}
			
			if( pInfo[playerid][player_taken_damage] > 0 && gettime() - pInfo[playerid][player_taken_damage] >= 2 )
			{
				pInfo[playerid][player_taken_damage] = 0;
				UpdatePlayerLabel(playerid);
			}

			if(pInfo[playerid][player_fight_training])
			{
				new object_id = pInfo[playerid][player_gym_object];
				new Float:x, Float:y, Float:z;
				new anim_id = GetPlayerAnimationIndex(playerid);

				GetDynamicObjectPos(object_id, x, y, z);

				if(IsPlayerInRangeOfPoint(playerid, 3.0, x, y, z) && object_id > 0)
				{
					new Keys,ud,lr;
		    		GetPlayerKeys(playerid,Keys,ud,lr);

		    		if(Keys == 144)
		    		{
		    			new fight_style = GetPlayerFightingStyle(playerid);

		    			switch(fight_style)
		    			{
		    				case FIGHT_STYLE_BOXING:
		    				{
		    					if(anim_id == 473 || anim_id == 474)
			    				{
			    					if(pInfo[playerid][player_fight_progress] >= 100)
			    					{
			    						pInfo[playerid][player_fight_style] = FIGHT_STYLE_BOXING;
			    						mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_fightstyle = %d WHERE char_uid = %d", FIGHT_STYLE_BOXING, pInfo[playerid][player_id]));
			    						SendClientMessage(playerid, COLOR_GOLD, "Congratulations, training done. New fighting style has been added.");
			    						pInfo[playerid][player_gym_object] = 0;
			    						pInfo[playerid][player_fight_training] = false;
			    					}
			    					else
			    					{
			    						pInfo[playerid][player_fight_progress] += 2;
				    					TextDrawSetString(Tutorial[playerid], sprintf("Training process: ~y~%d~w~%%", pInfo[playerid][player_fight_progress]));
				    					TextDrawShowForPlayer(playerid, Tutorial[playerid]);
			    					}
			    				}
		    				}
		    				case FIGHT_STYLE_KUNGFU:
		    				{
		    					if(anim_id == 483 || anim_id == 484)
		    					{
		    						if(pInfo[playerid][player_fight_progress] >= 100)
			    					{
			    						pInfo[playerid][player_fight_style] = FIGHT_STYLE_KUNGFU;
			    						mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_fightstyle = %d WHERE char_uid = %d", FIGHT_STYLE_KUNGFU, pInfo[playerid][player_id]));
			    						SendClientMessage(playerid, COLOR_GOLD, "Congratulations, training done. New fighting style has been added.");
			    						pInfo[playerid][player_gym_object] = 0;
			    						pInfo[playerid][player_fight_training] = false;
			    					}
			    					else
			    					{
			    						pInfo[playerid][player_fight_progress] += 2;
				    					TextDrawSetString(Tutorial[playerid], sprintf("Training process: ~y~%d~w~%%", pInfo[playerid][player_fight_progress]));
				    					TextDrawShowForPlayer(playerid, Tutorial[playerid]);
			    					}
		    					}
		    				}
		    				case FIGHT_STYLE_KNEEHEAD:
		    				{
		    					if(anim_id == 495)
		    					{
		    						if(pInfo[playerid][player_fight_progress] >= 100)
			    					{
			    						pInfo[playerid][player_fight_style] = FIGHT_STYLE_KNEEHEAD;
			    						mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_fightstyle = %d WHERE char_uid = %d", FIGHT_STYLE_KNEEHEAD, pInfo[playerid][player_id]));
			    						SendClientMessage(playerid, COLOR_GOLD, "Congratulations, training done. New fighting style has been added.");
			    						pInfo[playerid][player_gym_object] = 0;
			    						pInfo[playerid][player_fight_training] = false;
			    					}
			    					else
			    					{
		    							pInfo[playerid][player_fight_progress] += 2;
				    					TextDrawSetString(Tutorial[playerid], sprintf("Training process: ~y~%d~w~%%", pInfo[playerid][player_fight_progress]));
				    					TextDrawShowForPlayer(playerid, Tutorial[playerid]);
		    						}
		    					}
		    				}
		    			}
		    		}
				}
			}
			
			if( pInfo[playerid][player_bw] > 0 )
			{
				if( pInfo[playerid][player_bw_end_time] <= gettime() )
				{
					// koniec bw
					pInfo[playerid][player_bw] = 0;
					pInfo[playerid][player_bw_end_time] = 0;
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_bw` = 0 WHERE `char_uid` = %d", pInfo[playerid][player_id]));
					
					RemovePlayerStatus(playerid, PLAYER_STATUS_BW);
					SetPlayerHealth(playerid, 20);
					TogglePlayerControllable(playerid, 1);
					SetCameraBehindPlayer(playerid);
					ClearAnimations(playerid);
					new skin = GetPlayerSkin(playerid);
					SetPlayerSkin(playerid, skin);
					SetPlayerChatBubble(playerid, "", -1, 7.0, 5);
				}
				else
				{
					// update textdrawu
					UpdatePlayerBWTextdraw(playerid);
				}
			}
			
			if(pInfo[playerid][player_bw] > 0)
			{
				if(GetPlayerSpeed(playerid) > 2)
				{
					pInfo[playerid][player_cheat_warns]++;
				}
			}

			if((GetPlayerWeapon(playerid) ==38 ||GetPlayerWeapon(playerid) ==37 || GetPlayerWeapon(playerid) ==36 || GetPlayerWeapon(playerid) ==39 || GetPlayerWeapon(playerid) ==35 || GetPlayerWeapon(playerid) ==16 || GetPlayerWeapon(playerid) ==18) && pInfo[playerid][player_admin_duty] == false)
			{
				new String[64];
				format(String, sizeof(String), "Restricted weap (w: %d)", GetPlayerWeapon(playerid));
	    		BanAc(playerid, -1, String);
	    		continue;
			}

			if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK)
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
				KickAc(playerid, -1, "Jetpack");
				continue;
			}

			if( pInfo[playerid][player_cheat_warns]>3)
			{
				KickAc(playerid, -1, "Unfreeze");
				continue;
			}
			
			if( pInfo[playerid][player_health_changed] ) pInfo[playerid][player_health_changed] = false;
			else
			{
				new Float:health;
				GetPlayerHealth(playerid, health);
				if( health > pInfo[playerid][player_health] ) 
				{
					SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
				}
			}

			if( pInfo[playerid][player_armour_changed] ) pInfo[playerid][player_armour_changed] = false;
			else
			{
				new Float:armour;
				GetPlayerArmour(playerid, armour);
				if( armour > pInfo[playerid][player_armour] ) 
				{
					SetPlayerArmour(playerid, floatround(pInfo[playerid][player_armour]));
				}
			}

			if(pInfo[playerid][player_bus_ride])
			{
				pInfo[playerid][player_bus_time] --;
					
			    if(pInfo[playerid][player_bus_time] <= 0)
			    {
					new pointid = pInfo[playerid][player_bus_travel];
					
					pInfo[playerid][player_bus_start] = 0;
					pInfo[playerid][player_bus_travel] = 0;
						
					pInfo[playerid][player_bus_time] = 0;
					pInfo[playerid][player_bus_price] = 0;
					pInfo[playerid][player_bus_ride] = false;			

					RP_PLUS_SetPlayerPos(playerid, BusStopData[pointid][bPosX], BusStopData[pointid][bPosY], BusStopData[pointid][bPosZ]);

					SetPlayerCameraPos(playerid, BusStopData[pointid][bPosX], BusStopData[pointid][bPosY], BusStopData[pointid][bPosZ]);
					SetPlayerCameraLookAt(playerid, BusStopData[pointid][bPosX], BusStopData[pointid][bPosY], BusStopData[pointid][bPosZ]);

					SetPlayerInterior(playerid, 0);
					SetPlayerVirtualWorld(playerid, 0);
					
					TogglePlayerControllable(playerid, 1);

					SetCameraBehindPlayer(playerid);

					PlayerPlaySound(playerid, 1098, 0.0, 0.0, 0.0);
					PlayerPlaySound(playerid, 1147, 0.0, 0.0, 0.0);
				}
			}

			//Speed hack
			if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER )
			{
				new speeds = GetVehicleSpeed(GetPlayerVehicleID(playerid));
				if(speeds > 300)
				{
					KickAc(playerid, -1, "Speedhack");
					continue;
				}
			}
		}
	}
}