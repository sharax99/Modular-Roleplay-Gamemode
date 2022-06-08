stock LoadGate(limit[] = "", bool:return_id = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_gates` %s", limit));  
	cache_get_data(rows, fields);
	
	new g_id;
	
	for(new row = 0; row != rows; row++)
	{
		g_id = Iter_Free(Gates);
		Iter_Add(Gates, g_id);
		
		Gate[g_id][gate_uid] 	= cache_get_row_int(row, 0);
		Gate[g_id][gate_object] = cache_get_row_int(row, 1);
		Gate[g_id][gate_pos][0] = cache_get_row_float(row, 2);
		Gate[g_id][gate_pos][1] = cache_get_row_float(row, 3);
		Gate[g_id][gate_pos][2] = cache_get_row_float(row, 4);
		Gate[g_id][gate_pos][3] = cache_get_row_float(row, 5);
		Gate[g_id][gate_pos][4] = cache_get_row_float(row, 6);
		Gate[g_id][gate_pos][5] = cache_get_row_float(row, 7);

		Gate[g_id][gate_opened] = false;
		Gate[g_id][gate_object_id] = GetObjectByUid(Gate[g_id][gate_object]);
	}
	
	if( return_id ) return g_id;
	return rows;
}

stock GetGateByUid(uid)
{
	foreach(new ga_id : Gates)
	{
		if( Gate[ga_id][gate_uid] == uid ) return ga_id;
	}
	return -1;
}

stock DeleteGate(gateid, bool:from_db = false)
{
	if( from_db ) mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_gates` WHERE `gate_uid` = %d", Gate[gateid][gate_uid]));

	Iter_Remove(Gates, gateid);

	for(new z=0; e_gates:z != e_gates; z++)
	{
		Gate[gateid][e_gates:z] = 0;
	}
}
