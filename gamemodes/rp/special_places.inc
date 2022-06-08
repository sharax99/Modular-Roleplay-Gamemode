stock LoadSpecialPlaces()
{
	Code_ExTimer_Begin(LoadSpecialPlaces);
	new rows, fields, p_id;
	mysql_query(mySQLconnection, "SELECT * FROM `ipb_special_places`");
	
	cache_get_data(rows, fields);

	for(new row = 0; row != rows; row++)
	{
		p_id = Iter_Free(Places);
		Place[p_id][place_uid] = cache_get_row_int(row, 0);
		Place[p_id][place_type] = cache_get_row_int(row, 1);
		Place[p_id][place_pos][0] = cache_get_row_float(row, 2);
		Place[p_id][place_pos][1] = cache_get_row_float(row, 3);
		Place[p_id][place_pos][2] = cache_get_row_float(row, 4);
		Place[p_id][place_world] = cache_get_row_int(row, 5);

		Iter_Add(Places, p_id);
	}
	
	printf("Loaded special places [count: %d] [time %d ms]", Iter_Count(Places), Code_ExTimer_End(LoadSpecialPlaces));
	return 1;
}

stock CreateSpecialPlace(type, Float:sx, Float:sy, Float:sz, svw)
{
	mysql_query_format("INSERT INTO `ipb_special_places` (`place_type`, `place_x`, `place_y`, `place_z`, place_world) VALUES ('%d', '%f', '%f', '%f', '%d')", type, sx, sy, sz, svw);

	new p_id = Iter_Free(Places);

	Place[p_id][place_uid] = place_uid;
	Place[p_id][place_type] = type;
	Place[p_id][place_pos][0] = sx;
	Place[p_id][place_pos][1] = sy;
	Place[p_id][place_pos][2] = sz;
	Place[p_id][place_world] = svw;

	Iter_Add(Places, p_id);

	return cache_insert_id();
}

stock GetPlaceByUid(uid)
{
	foreach(new ga_id : Places)
	{
		if( Place[ga_id][place_uid] == uid ) return ga_id;
	}
	return -1;
}

stock DeleteSpecialPlace(uid)
{
	mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_special_places` WHERE `place_uid` = %d", uid));

	new placeid = GetPlaceByUid(uid);

	if(placeid != -1)
	{
		for(new z=0; e_places:z != e_places; z++)
		{
			Place[placeid][e_places:z] = 0;
		}
	}
}
