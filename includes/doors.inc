stock LoadDoor(limit[] = "", bool:return_id = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_doors %s", limit));  
	cache_get_data(rows, fields);
	
	new d_id;
	
	for(new row = 0; row != rows; row++)
	{
		d_id = CreateDynamicPickup(cache_get_row_int(row, 17), 2, cache_get_row_float(row, 5), cache_get_row_float(row, 6), cache_get_row_float(row, 7), cache_get_row_int(row, 9), cache_get_row_int(row, 10), -1, 200.0);
		
		Iter_Add(Doors, d_id);	
		
		cache_get_row(row, 2, Door[d_id][door_name], mySQLconnection, 40);
		cache_get_row(row, 23, Door[d_id][door_audio], mySQLconnection, 100);
		
		Door[d_id][door_uid] = cache_get_row_int(row, 0);
		Door[d_id][door_type] = cache_get_row_int(row, 1);
		Door[d_id][door_owner_type] = cache_get_row_int(row, 3);
		Door[d_id][door_owner] = cache_get_row_int(row, 4);

		Door[d_id][door_objects_limit] = cache_get_row_int(row, 22);
		Door[d_id][door_auto_closing] = !!cache_get_row_int(row, 19);
		Door[d_id][door_car_crosing] = !!cache_get_row_int(row, 20);
		Door[d_id][door_payment] = cache_get_row_int(row, 21);

		Door[d_id][door_pos][0] = cache_get_row_float(row, 5);
		Door[d_id][door_pos][1] = cache_get_row_float(row, 6);
		Door[d_id][door_pos][2] = cache_get_row_float(row, 7);
		Door[d_id][door_pos][3] = cache_get_row_float(row, 8);
		Door[d_id][door_vw] = cache_get_row_int(row, 9);
		Door[d_id][door_int] = cache_get_row_int(row, 10);
		Door[d_id][door_spawn_pos][0] = cache_get_row_float(row, 11);
		Door[d_id][door_spawn_pos][1] = cache_get_row_float(row, 12);
		Door[d_id][door_spawn_pos][2] = cache_get_row_float(row, 13);
		Door[d_id][door_spawn_pos][3] = cache_get_row_float(row, 14);
		Door[d_id][door_spawn_vw] = cache_get_row_int(row, 15);
		if( Door[d_id][door_spawn_vw] == -1 ) Door[d_id][door_spawn_vw] = Door[d_id][door_uid];
		Door[d_id][door_spawn_int] = cache_get_row_int(row, 16);

		Door[d_id][door_rentable] = cache_get_row_int(row, 24);
		Door[d_id][door_rent] = cache_get_row_int(row, 25);
		
		Door[d_id][door_buyable] = cache_get_row_int(row, 26);
		Door[d_id][door_price] = cache_get_row_int(row, 27);

		Door[d_id][door_time] = cache_get_row_int(row, 28);
		
		Door[d_id][door_access] = cache_get_row_int(row, 29);
		Door[d_id][door_destroyed] = cache_get_row_int(row, 30);

		new map_model = cache_get_row_int(row, 18);
		if( map_model != -1 )
		{
			Door[d_id][door_map_icon] = CreateDynamicMapIcon(Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2], map_model, 0, Door[d_id][door_vw], Door[d_id][door_int]);
		}
		
		Door[d_id][door_closed] = Door[d_id][door_auto_closing];
	}
	
	if( return_id ) return d_id;
	return rows;
}

stock DeleteDoor(d_id, bool:from_mysql = true)
{	
	if( from_mysql ) mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_doors` WHERE `door_uid` = %d", Door[d_id][door_uid]));
	
	Iter_Remove(Doors, d_id);
	
	DestroyDynamicPickup(d_id);
	
	if( IsValidDynamicMapIcon(Door[d_id][door_map_icon]) ) DestroyDynamicMapIcon(Door[d_id][door_map_icon]);
	
	for(new z=0; e_doors:z != e_doors; z++)
    {
		Door[d_id][e_doors:z] = 0;
    }
}

stock GetDoorByUid(uid)
{
	foreach(new d_id : Doors)
	{
		if( Door[d_id][door_uid] == uid ) return d_id;
	}
	
	return -1;
}

stock CanPlayerUseDoor(playerid, d_id)
{
	if( HasCrewFlag(playerid, CREW_FLAG_DOORS) ) return 1;
	
	switch( Door[d_id][door_owner_type] )
	{
		case DOOR_OWNER_TYPE_PLAYER:
		{
			if( Door[d_id][door_owner] == pInfo[playerid][player_id] ) return 1;
			if( Door[d_id][door_uid] == pInfo[playerid][player_spawn]) return 1;
		}
		
		case DOOR_OWNER_TYPE_GROUP:
		{
			new gid = GetGroupByUid(Door[d_id][door_owner]);
			
			new slot = GetPlayerGroupSlot(playerid, gid);
			if( slot > -1 ) 
			{
				if( WorkerHasFlag(playerid, slot, WORKER_FLAG_DOORS) ) return 1;
			}
		}
	}
	
	return 0;
}

stock CanPlayerEditDoor(playerid, d_id)
{
	if(HasCrewFlag(playerid, CREW_FLAG_DOORS) ) return 1;
	if(d_id == -1) return 0;
	switch( Door[d_id][door_owner_type] )
	{
		case DOOR_OWNER_TYPE_PLAYER:
		{
			if( Door[d_id][door_owner] == pInfo[playerid][player_id] ) return 1;
		}
		
		case DOOR_OWNER_TYPE_GROUP:
		{
			new gid = GetGroupByUid(Door[d_id][door_owner]);
			
			new slot = GetPlayerGroupSlot(playerid, gid);
			if( slot > -1 ) 
			{
				if( WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return 1;
			}
		}
	}
	return 0;
}

stock CountDoorObjects(d_id)
{
	new count;
	foreach(new oid : Objects)
	{
		if( Object[oid][object_owner_type] == OBJECT_OWNER_TYPE_DOOR && Object[oid][object_owner] == Door[d_id][door_uid] ) count++;
	}
	
	return count;
}

stock CountDoorLabels(d_id)
{
	new count;
	foreach(new lid : Labels)
	{
		if( Label[Text3D:lid][label_owner_type] == LABEL_OWNER_TYPE_DOOR && Label[Text3D:lid][label_owner] == Door[d_id][door_uid] ) count++;
	}
	
	return count;
}

stock ShowPlayerDoorTextdraw(playerid, d_id)
{
	new header[80], all[256];
	if( !HasCrewFlag(playerid, CREW_FLAG_DOORS) )
	{
		if( Door[d_id][door_payment] > 0 ) format(header, sizeof(header), "%s (~g~$%d~w~)", Door[d_id][door_name], Door[d_id][door_payment]);
		else if( Door[d_id][door_payment] == 0 ) format(header, sizeof(header), "%s", Door[d_id][door_name]);
	}
	else
	{
		if( Door[d_id][door_payment] > 0 && !Door[d_id][door_closed] ) format(header, sizeof(header), "%s ~y~(%d)~w~ (~g~$%d~w~)", Door[d_id][door_name], d_id, Door[d_id][door_payment]);
		else if( Door[d_id][door_payment] == 0 ) format(header, sizeof(header), "%s (%d)", Door[d_id][door_name], d_id);
	}
	amount_players_in_door[playerid]=0;
	foreach(new p : Player)
	{
		if( GetPlayerVirtualWorld(p) == Door[d_id][door_spawn_vw] )
		{
			amount_players_in_door[playerid]++;
		}
	}
	if( Door[d_id][door_closed] ) format(all, sizeof(all), "%s~n~~n~~w~DOOR~r~ LOCKED~n~_", header);
	else format(all, sizeof(all), "%s~n~~w~People inside:_~y~%d~n~~w~To enter this building, press___~n~~g~[LALT+SPACE]", header, amount_players_in_door[playerid]);
	TextDrawSetString(DoorInfo[playerid], all);
	TextDrawShowForPlayer(playerid, DoorInfo[playerid]);
	TextDrawShowForPlayer(playerid, DoorInfo2[playerid]);
	defer HidePlayerDoorText[3000](playerid);
}

stock HidePlayerDoorTextdraw(playerid)
{
	TextDrawHideForPlayer(playerid, DoorInfo[playerid]);
	TextDrawHideForPlayer(playerid, DoorInfo2[playerid]);
}

stock DoorsDefaultInteriorsList(playerid, d_id, page=1)
{
	new rows, fields;
	mysql_query(mySQLconnection, "SELECT COUNT(*) as count FROM `ipb_default_interiors`");
	cache_get_data(rows, fields);

	new count = cache_get_row_int(0, 0);
	new all_rows = count;
	
	new header[64], Float:pp = all_rows / 20;
	format(header, sizeof(header), "Interiors list (%d/%d)", page, floatround(pp, floatround_ceil));
	
	pInfo[playerid][player_dialog_tmp1] = page;
	
	DynamicGui_Init(playerid);
	DynamicGui_SetDialogValue(playerid, d_id);
	
	new str[800];
	
	if( page > 1 )
	{
		format(str, sizeof(str), "%s"HEX_COLOR_SAMP"<<< Previous page\n  \n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_CHANGE_INTERIOR_PREV);
		
		DynamicGui_AddBlankRow(playerid);
	}
	
	if( page == 1 )
	{
		format(str, sizeof(str), "%s0.\tNone (custom interior)\n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_CHANGE_INTERIOR_ROW, -1);
	}
	
	mysql_query(mySQLconnection, sprintf("SELECT name, id FROM `ipb_default_interiors` LIMIT %d, 20", (page-1)*20));
	cache_get_data(rows, fields);
	
	new tmp_str[60];
	
	for(new row = 0; row != rows; row++)
	{
		cache_get_row(row, 0, tmp_str);
		
		format(str, sizeof(str), "%s%d.\t%s\n", str, (row+1)+((page-1)*20), tmp_str);
		DynamicGui_AddRow(playerid, DG_DRZWI_CHANGE_INTERIOR_ROW, cache_get_row_int(row, 1));
	}
	
	if( all_rows > (page*20) )
	{
		format(str, sizeof(str), "%s"HEX_COLOR_SAMP"\t\t\t\t\t\t\t   Next page >>>\n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_CHANGE_INTERIOR_NEXT);
	}
	
	ShowPlayerDialog(playerid, DIALOG_ADRZWI_CHANGE_INTERIOR, DIALOG_STYLE_LIST, header, str, "Choose", "Back");
	
	return 1;
}
