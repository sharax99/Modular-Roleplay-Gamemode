stock GetClosestObjectType(playerid, object_mod)
{
	new object_id,
    	Float:prevdist = 5.0, ObjectID = INVALID_OBJECT_ID, Float:PosX, Float:PosY, Float:PosZ, Float:dist;
     
	GetPlayerPos(playerid, PosX, PosY, PosZ);
 	for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
 	{
    	if(IsValidPlayerObject(playerid, player_object))
     	{
			object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);

        	if(GetObjectModel(object_id) == object_mod)
       		{
    			Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, dist);
    			if((dist < prevdist))
    			{
        			prevdist = dist;
     				ObjectID = object_id;
    			}
         	}
    	}
	}
	return ObjectID;
}

stock GetItemObject(playerid, itemuid)
{
	new object_id,
    	Float:prevdist = 5.0, ObjectID = INVALID_OBJECT_ID, Float:PosX, Float:PosY, Float:PosZ, Float:dist;
     
	GetPlayerPos(playerid, PosX, PosY, PosZ);
 	for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
 	{
    	if(IsValidPlayerObject(playerid, player_object))
     	{
			object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);

        	if(Streamer_GetIntData(STREAMER_TYPE_OBJECT, object_id, E_STREAMER_EXTRA_ID) == itemuid*-1)
       		{
    			Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, dist);
    			if((dist < prevdist))
    			{
        			prevdist = dist;
     				ObjectID = object_id;
    			}
         	}
    	}
	}
	return ObjectID;
}

stock LoadObject(limit[] = "", bool:return_id = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_objects %s", limit));  
	cache_get_data(rows, fields);

	new oid;
	
	for(new row = 0; row != rows; row++)
	{
		new model = cache_get_row_int(row, 6),
			vw = cache_get_row_int(row, 7),
			owner = cache_get_row_int(row, 3),
			owner_type = cache_get_row_int(row, 2);
			
		if( owner_type == 0 ) continue;
			
		oid = CreateDynamicObject(model, cache_get_row_float(row, 8), cache_get_row_float(row, 9), cache_get_row_float(row, 10), cache_get_row_float(row, 11), cache_get_row_float(row, 12), cache_get_row_float(row, 13), vw, -1, -1, 200.0);
		
		for(new z=0; e_objects:z != e_objects; z++)
		{
			Object[oid][e_objects:z] = 0;
		}
		
		Object[oid][object_uid] = cache_get_row_int(row, 0);
		Object[oid][object_owner_type] = owner_type;
		Object[oid][object_owner] = owner;
		
		switch( Object[oid][object_owner_type] )
		{
			case OBJECT_OWNER_TYPE_AREA:
			{
				new a_id = GetAreaByUid(owner);
				if( a_id == -1 )
				{
					DeleteObject(oid, false);
					continue;
				}
			}
			
			case OBJECT_OWNER_TYPE_DOOR:
			{
				new d_id = GetDoorByUid(owner);
				if( d_id == -1 )
				{
					DeleteObject(oid, false);
					continue;
				}
			}
		}

		Streamer_SetIntData(STREAMER_TYPE_OBJECT, oid, E_STREAMER_EXTRA_ID, Object[oid][object_uid]);

		Object[oid][object_vw] = vw;
		Object[oid][object_model] = model;
		Object[oid][object_stream_distance] = 300.0;
		Object[oid][object_type] = cache_get_row_int(row, 1);
		Object[oid][object_gate_opened] = false;
		Object[oid][object_gate] = cache_get_row_int(row, 20);

		Object[oid][object_gate_pos][0] = cache_get_row_float(row, 14);
		Object[oid][object_gate_pos][1] = cache_get_row_float(row, 15);
		Object[oid][object_gate_pos][2] = cache_get_row_float(row, 16);
		Object[oid][object_gate_pos][3] = cache_get_row_float(row, 17);
		Object[oid][object_gate_pos][4] = cache_get_row_float(row, 18);
		Object[oid][object_gate_pos][5] = cache_get_row_float(row, 19);

		Object[oid][object_pos][0] = cache_get_row_float(row, 8);
		Object[oid][object_pos][1] = cache_get_row_float(row, 9);
		Object[oid][object_pos][2] = cache_get_row_float(row, 10);

		Object[oid][object_pos][3] = cache_get_row_float(row, 11);
		Object[oid][object_pos][4] = cache_get_row_float(row, 12);
		Object[oid][object_pos][5] = cache_get_row_float(row, 13);

		Object[oid][object_textured] = cache_get_row_int(row, 24);

		Object[oid][object_logs] = 5;

		if(Object[oid][object_gate] > 0)
		{
			foreach(new g : Gates)
			{
				if(Object[oid][object_gate] == Gate[g][gate_uid])
				{
					Gate[g][gate_object_id] = oid;
				}
			}
		}

		Iter_Add(Objects, oid);
	}
	
	LoadMaterial(sprintf("WHERE `material_owner` = %d", Object[oid][object_uid]));
	
	if( return_id ) return oid;
	return rows;
}

stock DeleteObject(o_id, bool:from_db = true)
{
	if( from_db )
	{ 
		mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_objects` WHERE `object_uid` = %d", Object[o_id][object_uid]));
		mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_materials` WHERE `material_owner` = %d", Object[o_id][object_uid]));
		
		if(Object[o_id][object_textured] > 0)
		{
			mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_materials WHERE material_owner = %d", Object[o_id][object_uid]));
		}

		if(Object[o_id][object_gate] > 0)
		{
			new g_id = GetGateByUid(Object[o_id][object_gate]);
			DeleteGate(g_id, true);
		}
	}

	DestroyDynamicObject(o_id);
	Iter_Remove(Objects, o_id);

	for(new z=0; e_objects:z != e_objects; z++)
	{
  		Object[o_id][e_objects:z] = 0;
    }
 	return 1;
}

stock IsValidBlockadeObject(o_id)
{
	if(o_id == INVALID_OBJECT_ID) return 0;
	if(Object[o_id][object_type] != OBJECT_TYPE_BLOCKADE) return 0;

	switch(Object[o_id][object_model])
	{
		case 19834: return 1;
		case 1949: return 1;
		case 1427: return 1;
		case 1424: return 1;
		case 1423: return 1;
		case 1459: return 1;
		case 1237: return 1;
		case 3091: return 1;
		case 3578: return 1;
		case 1425: return 1;
		case 18728: return 1;
		case 1997: return 1;
	}

	return 0;
}

stock IsValidBlockadeModel(model)
{
	switch(model)
	{
		case 19834: return 1;
		case 1949: return 1;
		case 1427: return 1;
		case 1424: return 1;
		case 1423: return 1;
		case 1459: return 1;
		case 1237: return 1;
		case 3091: return 1;
		case 3578: return 1;
		case 1425: return 1;
		case 18728: return 1;
		case 1997: return 1;
	}
	return 0;
}

stock IsObjectEdited(o_id)
{
	if( Object[o_id][object_is_edited] )
	{
		foreach(new p : Player)
		{
			if( pInfo[p][player_edited_object] == o_id ) return 1;
		}
	}
	
	return 0;
}

stock GetObjectDataForPlayer(playerid, &owner, &owner_type)
{
	if( GetPlayerVirtualWorld(playerid) == 0 )
	{
		new a_id = GetPlayerArea(playerid, AREA_TYPE_NORMAL);
		if( a_id != -1 )
		{
			if( CanPlayerEditArea(playerid, a_id) )
			{
				owner = Area[a_id][area_uid];
				owner_type = OBJECT_OWNER_TYPE_AREA;
			}
		}
		else if( HasCrewFlag(playerid, CREW_FLAG_EDITOR) )
		{
			owner = 0;
			owner_type = OBJECT_OWNER_TYPE_GLOBAL;
			
			return 1;
		}
	}
	else
	{
		new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
		
		if( d_id == -1 ) return 1;
		if( Door[d_id][door_type] != DOOR_TYPE_NORMAL ) return 1;
		
		if( HasCrewFlag(playerid, CREW_FLAG_EDITOR) )
		{
			owner = Door[d_id][door_uid];
			owner_type = OBJECT_OWNER_TYPE_DOOR;
			
			return 1;
		}
		
		if( CanPlayerEditDoor(playerid, d_id) )
		{
			owner = Door[d_id][door_uid];
			owner_type = OBJECT_OWNER_TYPE_DOOR;
			
			return 1;
		}
	}
	return 1;
}

stock CanPlayerEditObject(playerid, o_id)
{
	if( HasCrewFlag(playerid, CREW_FLAG_EDITOR) ) return 1;
	
	switch( Object[o_id][object_owner_type] )
	{	
		case OBJECT_OWNER_TYPE_GLOBAL:
		{
			if(IsValidBlockadeObject(o_id) )
			{
				new gid = pInfo[playerid][player_duty_gid];
				if(gid == -1) return 0;
				if(Group[gid][group_flags] & GROUP_FLAG_BLOCKADE) return 1;
			}
		}

		case OBJECT_OWNER_TYPE_DOOR:
		{
			if( CanPlayerEditDoor(playerid, GetDoorByUid(Object[o_id][object_owner])) ) return 1;
		}
		
		case OBJECT_OWNER_TYPE_AREA:
		{
			if( CanPlayerEditArea(playerid, GetAreaByUid(Object[o_id][object_owner])) ) return 1;
		}
	}
	return 0;
}

stock CanPlayerUseObject(playerid, o_id)
{
	if( HasCrewFlag(playerid, CREW_FLAG_EDITOR) ) return 1;
	
	switch( Object[o_id][object_owner_type] )
	{	
		case OBJECT_OWNER_TYPE_DOOR:
		{
			new gid = pInfo[playerid][player_duty_gid];
			if(gid != -1)
			{
				if(GroupHasFlag(gid, GROUP_FLAG_911)) return 1;
			}
			
			if( CanPlayerUseDoor(playerid, GetDoorByUid(Object[o_id][object_owner])) ) return 1;
		}
		
		case OBJECT_OWNER_TYPE_AREA:
		{
			new areaa = GetAreaByUid(Object[o_id][object_owner]);
			if (areaa == -1) return 0;

			if(Area[areaa][area_owner_type] == AREA_OWNER_TYPE_GROUP)
			{
				new gid = GetGroupByUid(Area[areaa][area_owner]);
				if(gid == -1) return 0;

				if( GetPlayerGroupSlot(playerid, gid) != -1 ) return 1;
			}
			else if(Area[areaa][area_owner_type] == AREA_OWNER_TYPE_PLAYER)
			{
				if(Area[areaa][area_owner] == pInfo[playerid][player_id]) return 1;
			}
		}
	}
	return 0;
}

stock Float:GetPlayerDistanceToObject(playerid, object_id)
{
	if( !IsValidDynamicObject(object_id) ) return 0.0;
	
	new Float:p_pos[3], Float:distance;
	GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
	
	Streamer_GetDistanceToItem(p_pos[0], p_pos[1], p_pos[2], STREAMER_TYPE_OBJECT, object_id, distance);
	return distance;
}

stock UpdateObjectInfoTextdraw(playerid, o_id)
{
	new string[200];

	switch( Object[o_id][object_owner_type] )
	{
		case OBJECT_OWNER_TYPE_GLOBAL: format(string, sizeof(string), "%d. Global object (ID: %d)", Object[o_id][object_uid], o_id);
		case OBJECT_OWNER_TYPE_DOOR: format(string, sizeof(string), "%d. Door object (ID: %d)", Object[o_id][object_uid], o_id);
		case OBJECT_OWNER_TYPE_AREA: format(string, sizeof(string), "%d. Area object (ID: %d)", Object[o_id][object_uid], o_id);
	}

	format(string, sizeof(string), "%s~n~~n~~p~Owner: ~w~%d:%d    ~b~Type: ~w~%d    Model: %d", string, Object[o_id][object_owner_type], Object[o_id][object_owner], Object[o_id][object_type], Object[o_id][object_model]);	
	format(string, sizeof(string), "%s~n~~r~Position: ~w~%.2f, %.2f, %.2f~n~~r~Rotation: ~w~%.2f, %.2f, %.2f", string, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);

	TextDrawSetString(Dashboard[playerid], string);
}

stock GetObjectByUid(uid)
{
	foreach(new o_id : Objects)
	{
		if( Object[o_id][object_uid] == uid ) return o_id;
	}
	return -1;
}

stock RemoveBuildingsForPlayer(playerid)
{
	RemoveBuildingForPlayer(playerid, 5408, 1873.7422, -1682.4766, 34.7969, 0.25);
	
	//Dystrybutory na stacji idlewood
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1778.4531, 14.1406, 0.25);
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1774.3125, 14.1406, 0.25);
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1771.3438, 14.1406, 0.25);
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1767.2891, 14.1406, 0.25);

	//Leav
	RemoveBuildingForPlayer(playerid, 13890, 790.1328, -963.3281, 59.4766, 0.25);
	RemoveBuildingForPlayer(playerid, 1267, 776.7813, -1029.9063, 37.3438, 0.25);

	RemoveBuildingForPlayer(playerid, 1283, 1690.2813, -1607.8438, 15.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 1283, 1664.9063, -1593.1250, 15.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 1283, 1646.6016, -1591.6875, 15.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 1283, 1658.5313, -1583.3203, 15.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 1283, 1676.7813, -1591.6094, 15.5859, 0.25);
	RemoveBuildingForPlayer(playerid, 1283, 1703.9063, -1593.6719, 15.6250, 0.25);

	//Corona
	RemoveBuildingForPlayer(playerid, 4850, 1940.1875, -2007.3047, 15.33594, 49.453796); 
	RemoveBuildingForPlayer(playerid, 4978, 1940.1875, -2007.3047, 15.33594, 49.453796); 
	RemoveBuildingForPlayer(playerid, 3625, 1941.9844, -1970.7031, 14.98438, 13.033258); 
	RemoveBuildingForPlayer(playerid, 4857, 1942.6797, -1986.75, 14.89844, 13.08914); 
	RemoveBuildingForPlayer(playerid, 4979, 1942.6797, -1986.75, 14.89844, 13.08914);
	RemoveBuildingForPlayer(playerid, 1226, 1937.7833, -1999.1813, 18.90706, 6.6149273);


	//Santa molo 
	RemoveBuildingForPlayer(playerid, 6463, 389.7734, -2028.4688, 19.8047, 0.5);
	RemoveBuildingForPlayer(playerid, 3751, 389.8750, -2035.3828, 29.9531, 50);
	RemoveBuildingForPlayer(playerid, 6298, 389.7734, -2028.4688, 19.8047, 0.5);
	RemoveBuildingForPlayer(playerid, 6461, 389.7734, -2028.5000, 20.1094, 0.5);
	RemoveBuildingForPlayer(playerid, 3752, 389.8750, -2028.5000, 32.2266, 50);

	/*Centrum here 
	RemoveBuildingForPlayer(playerid, 6068, 1247.9141, -1429.9688, 15.1250, 0.25);
	RemoveBuildingForPlayer(playerid, 792, 1238.9844, -1481.9531, 12.7188, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1249.9844, -1481.4609, 9.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 1231, 1247.1953, -1483.4531, 15.2344, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1258.1797, -1478.0000, 9.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 792, 1238.9844, -1468.0156, 12.7188, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1250.6563, -1466.0000, 9.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1260.1797, -1454.1016, 9.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 1231, 1247.1953, -1451.8047, 15.2344, 0.25);
	RemoveBuildingForPlayer(playerid, 792, 1256.9844, -1443.0313, 12.7188, 0.25);
	RemoveBuildingForPlayer(playerid, 792, 1236.2813, -1443.0313, 12.7188, 0.25);
	RemoveBuildingForPlayer(playerid, 1231, 1265.6484, -1451.8047, 15.2344, 0.25);
	RemoveBuildingForPlayer(playerid, 6052, 1247.9141, -1429.9688, 15.1250, 0.25);
	RemoveBuildingForPlayer(playerid, 792, 1256.9844, -1417.7031, 12.7188, 0.25);
	RemoveBuildingForPlayer(playerid, 792, 1236.2813, -1417.7031, 12.7188, 0.25);*/

	//Alhambra
	RemoveBuildingForPlayer(playerid, 1524, 1837.6641, -1640.3828, 13.7578, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1855.7188, -1741.5391, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1879.5078, -1741.4844, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1908.2188, -1741.4844, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 712, 1929.5781, -1736.9063, 21.3906, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1931.0391, -1726.3281, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1832.3828, -1694.3125, 9.7188, 0.25);
	RemoveBuildingForPlayer(playerid, 1537, 1837.4375, -1683.9688, 12.3047, 0.25);
	RemoveBuildingForPlayer(playerid, 1533, 1837.4375, -1683.9531, 12.3047, 0.25);
	RemoveBuildingForPlayer(playerid, 1537, 1837.4375, -1686.9844, 12.3125, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1832.8984, -1670.7656, 9.7188, 0.25);
	RemoveBuildingForPlayer(playerid, 1533, 1837.4375, -1677.9219, 12.2969, 0.25);
	RemoveBuildingForPlayer(playerid, 1537, 1837.4375, -1680.9531, 12.2969, 0.25);
	RemoveBuildingForPlayer(playerid, 1533, 1837.4375, -1680.9375, 12.2969, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1931.0391, -1702.2891, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 712, 1929.5781, -1694.4609, 21.3906, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1931.0391, -1667.0313, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1931.0391, -1637.8984, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1855.7188, -1623.2813, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1879.5078, -1623.1016, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1908.2188, -1622.9844, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 712, 1929.5781, -1627.6250, 21.3906, 0.25);

	// Pole Golfowe
	RemoveBuildingForPlayer(playerid, 955, 2060.1172, -1897.6406, 12.9297, 0.25);
	RemoveBuildingForPlayer(playerid, 6491, 717.4844, -1357.3047, 20.2969, 0.25);
	RemoveBuildingForPlayer(playerid, 1689, 650.8359, -1377.6641, 21.7578, 0.25);
	RemoveBuildingForPlayer(playerid, 1689, 650.8359, -1356.5859, 21.7578, 0.25);
	RemoveBuildingForPlayer(playerid, 1635, 677.1953, -1328.8750, 25.1094, 0.25);
	RemoveBuildingForPlayer(playerid, 1689, 745.5859, -1381.1094, 25.8750, 0.25);
	RemoveBuildingForPlayer(playerid, 1689, 751.3359, -1368.0313, 25.8750, 0.25);
	RemoveBuildingForPlayer(playerid, 6490, 717.4844, -1357.3047, 20.2969, 0.25);
	RemoveBuildingForPlayer(playerid, 6516, 717.6875, -1357.2813, 18.0469, 0.25);
	RemoveBuildingForPlayer(playerid, 1415, 732.8516, -1332.8984, 12.6875, 0.25);
	RemoveBuildingForPlayer(playerid, 1439, 732.7266, -1341.7734, 12.6328, 0.25);
	RemoveBuildingForPlayer(playerid, 1689, 650.8359, -1338.2422, 21.7578, 0.25);
	RemoveBuildingForPlayer(playerid, 1635, 677.1953, -1328.8750, 25.1094, 0.25);
	RemoveBuildingForPlayer(playerid, 1635, 696.1953, -1328.8750, 25.1094, 0.25);
	RemoveBuildingForPlayer(playerid, 1635, 702.2344, -1328.8750, 25.1094, 0.25);
	RemoveBuildingForPlayer(playerid, 1635, 721.2109, -1328.8750, 25.1094, 0.25);
	RemoveBuildingForPlayer(playerid, 1226, 642.0938, -1334.8516, 16.2734, 0.25);
}
