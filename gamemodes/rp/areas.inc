stock LoadArea(limit[] = "", bool:return_id = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_areas` %s", limit));
	cache_get_data(rows, fields);
	
	new area_id;
	for(new row = 0; row != rows; row++)
	{
		area_id = -1;
		new shape = cache_get_row_int(row, 1);
		
		new Float:point1[3], Float:point2[3];
		
		new str[100];
		cache_get_row(row, 8, str);
		sscanf(str, "p<|>a<f>[3]", point1);
		
		new str2[100];
		cache_get_row(row, 9, str2);
		sscanf(str2, "p<|>a<f>[3]", point2);
		
		if( shape == AREA_SHAPE_SQUARE )
		{
			// Tworzymy prostokatna strefe
			new Float:pointes[8];
			pointes[0] = point1[0];
			pointes[1] = point1[1];
			pointes[2] = point1[0];
			pointes[3] = point2[1];
			pointes[4] = point2[0];
			pointes[5] = point2[1];
			pointes[6] = point2[0];
			pointes[7] = point1[1];
			
			area_id = CreateDynamicPolygon(pointes, -FLOAT_INFINITY, FLOAT_INFINITY, 8, cache_get_row_int(row, 6));
		
          	Area[area_id][area_objects_limit] = cache_get_row_int(row, 5);
		}
		else if( shape == AREA_SHAPE_CIRCLE )
		{
			// Tworzymy okragla strefe
			area_id = CreateDynamicSphere(point1[0], point1[1], point1[2], cache_get_row_float(row, 7), cache_get_row_int(row, 6));
		}
		
		if( area_id == -1 )
		{
			printf("Error while loading area: %d", cache_get_row_int(row, 0));
			continue;
		}
		
		Area[area_id][area_uid] = cache_get_row_int(row, 0);
		
		Area[area_id][area_type] = cache_get_row_int(row, 2);
		Area[area_id][area_owner_type] = cache_get_row_int(row, 3);
		Area[area_id][area_owner] = cache_get_row_int(row, 4);
		Area[area_id][area_flags] = cache_get_row_int(row, 15);
		cache_get_row(row, 16, Area[area_id][area_audio], mySQLconnection, 100);

		new map_model = cache_get_row_int(row, 17);
		if( map_model != -1 )
		{
			Area[area_id][area_map_icon] = CreateDynamicMapIcon(point1[0], point1[1], point1[2], map_model, 0, 0, 0);
		}
		
		Iter_Add(Areas, area_id);
	}
	
	if( return_id ) return area_id;
	return rows;
}

stock DeleteArea(a_id, bool:from_database = true)
{
	if( from_database ) mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_areas` WHERE `area_uid` = %d", Area[a_id][area_uid]));
	
	DestroyDynamicArea(a_id);
		
	Iter_Remove(Areas, a_id);
	
	for(new z=0; e_areas:z != e_areas; z++)
    {
		Area[a_id][e_areas:z] = 0;
    }
}

stock GetPlayerArea(playerid, type, owner_type = -1)
{
	foreach(new a_id : Areas)
	{
		if( Area[a_id][area_type] == type && IsPlayerInDynamicArea(playerid, a_id) )
		{
			if( owner_type == -1 ) return a_id;
			else if( Area[a_id][area_owner_type] == owner_type ) return a_id;
		}
	}
	return -1;
}

stock GetAreaByUid(a_uid)
{
	foreach(new a_id : Areas)
	{
		if( Area[a_id][area_uid] == a_uid ) return a_id;
	}
	
	return -1;
}

stock CountAreaObjects(a_id)
{
	new count;
	foreach(new o_id : Objects)
	{
		if( Object[o_id][object_owner_type] == OBJECT_OWNER_TYPE_AREA && Object[o_id][object_owner] == Area[a_id][area_uid] ) count++;
	}
	
	return count;
}

stock CountAreaLabels(a_id)
{
	new count;
	foreach(new lid : Labels)
	{
		if( Label[Text3D:lid][label_owner_type] == LABEL_OWNER_TYPE_AREA && Label[Text3D:lid][label_owner] == Area[a_id][area_uid] ) count++;
	}
	
	return count;
}

stock CanPlayerEditArea(playerid, a_id)
{
	if( HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return 1;
	
	switch( Area[a_id][area_owner_type] )
	{
		case AREA_OWNER_TYPE_PLAYER:
		{
			if( Area[a_id][area_owner] == pInfo[playerid][player_id] ) return 1;
		}
		
		case AREA_OWNER_TYPE_GROUP:
		{
			new slot = GetPlayerDutySlot(playerid);
			if(slot == -1) return 0;
			if( WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return 1;
		}
	}
	return 0;
}

stock ShowAreaFlags(playerid, a_id)
{
	new flags_list[256];
	new last_line[4];

	if( AreaHasFlag(a_id, AREA_FLAG_CORNER) )
	{
		format(flags_list, sizeof(flags_list), "%s~b~~h~~>~~w~drug corner~n~", flags_list);
	}
	if( AreaHasFlag(a_id, AREA_FLAG_BMX) )
	{
		format(flags_list, sizeof(flags_list), "%s~b~~h~~>~~w~bmx~n~", flags_list);
	}
	if( AreaHasFlag(a_id, AREA_FLAG_MONITORING) )
	{
		format(flags_list, sizeof(flags_list), "%s~b~~h~~>~~w~monitored~n~", flags_list);
	}
	if( AreaHasFlag(a_id, AREA_FLAG_DRIVE) )
	{
		format(flags_list, sizeof(flags_list), "%s~b~~h~~>~~w~drive-thru~n~", flags_list);
	}
	if( AreaHasFlag(a_id, AREA_FLAG_PLANT) )
	{
		format(flags_list, sizeof(flags_list), "%s~b~~h~~>~~w~planting~n~", flags_list);
	}
	if( AreaHasFlag(a_id, AREA_FLAG_PARKING) )
	{
		format(flags_list, sizeof(flags_list), "%s~b~~h~~>~~w~no parking~n~", flags_list);
	}
	if( AreaHasFlag(a_id, AREA_FLAG_OFFER) )
	{
		format(flags_list, sizeof(flags_list), "%s~b~~h~~>~~w~offering products~n~", flags_list);
	}
	if( AreaHasFlag(a_id, AREA_FLAG_SERWIS) )
	{
		format(flags_list, sizeof(flags_list), "%s~b~~h~~>~~w~car fix~n~", flags_list);
	}
	if( AreaHasFlag(a_id, AREA_FLAG_WORK) )
	{
		format(flags_list, sizeof(flags_list), "%s~b~~h~~>~~w~lumberjack~n~", flags_list);
	}
	if( AreaHasFlag(a_id, AREA_FLAG_WORK_FISH) )
	{
		format(flags_list, sizeof(flags_list), "%s~b~~h~~>~~w~fisher~n~", flags_list);
	}
	format(last_line, sizeof(last_line), "");
	format(flags_list, sizeof(flags_list), "%s %s", flags_list, last_line);
	TextDrawSetString(AreaFlags[playerid], flags_list);
	TextDrawShowForPlayer(playerid, AreaFlags[playerid]);
}
