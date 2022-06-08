forward CreateBusStop(BusName[32], Float:BusPosX, Float:BusPosY, Float:BusPosZ);
public CreateBusStop(BusName[32], Float:BusPosX, Float:BusPosY, Float:BusPosZ)
{
	new string[64];
	mysql_real_escape_string(BusName, BusName);
	mysql_query_format("INSERT INTO `ipb_busstops` (`busstop_name`, `busstop_posx`, `busstop_posy`, `busstop_posz`) VALUES ('%s', '%f', '%f', '%f')", BusName, BusPosX, BusPosY, BusPosZ);

	new busstop_uid = cache_insert_id();
	new busstop_id = GetFreeBusStopID();

	BusStopData[busstop_id][bUID] = busstop_uid;
	strmid(BusStopData[busstop_id][bName], BusName, 0, strlen(BusName), 32);
	BusStopData[busstop_id][bPosX] = BusPosX;
	BusStopData[busstop_id][bPosY] = BusPosY;
	BusStopData[busstop_id][bPosZ] = BusPosZ;

	format(string, sizeof(string), "Bus stop %d:\n%s.", busstop_id, BusName);
	BusStopData[busstop_id][bNameTag] = CreateDynamic3DTextLabel(string, LABEL_DESCRIPTION, BusStopData[busstop_id][bPosX], BusStopData[busstop_id][bPosY], BusStopData[busstop_id][bPosZ], 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID);

	Iter_Add(Buses, busstop_id);

	return busstop_id;
}

forward LoadBusStops();
public LoadBusStops()
{
	Code_ExTimer_Begin(LoadBusStops);
	new rows, fields, string[512], busstop_id = 0;
	mysql_query(mySQLconnection, "SELECT * FROM `ipb_busstops`");
	cache_get_data(rows, fields);

	for(new row = 0; row != rows; row++)
	{
		busstop_id++;

		BusStopData[busstop_id][bUID] = cache_get_row_int(row, 0);
		cache_get_row(row, 1, BusStopData[busstop_id][bName], mySQLconnection, 32);
		
		BusStopData[busstop_id][bPosX] = cache_get_row_float(row, 2);
		BusStopData[busstop_id][bPosY] = cache_get_row_float(row, 3);
		BusStopData[busstop_id][bPosZ] = cache_get_row_float(row, 4);

		format(string, sizeof(string), "Bus stop %d:\n%s.", busstop_id, BusStopData[busstop_id][bName]);
		BusStopData[busstop_id][bNameTag] = CreateDynamic3DTextLabel(string, LABEL_DESCRIPTION, BusStopData[busstop_id][bPosX], BusStopData[busstop_id][bPosY], BusStopData[busstop_id][bPosZ], 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID);

		Iter_Add(Buses, busstop_id);
	}
	printf("Loaded bus stops [count: %d] [time %d ms]", busstop_id, Code_ExTimer_End(LoadBusStops));
	return 1;
}

forward SaveBusStop(busstop_id);
public SaveBusStop(busstop_id)
{
    mysql_real_escape_string(BusStopData[busstop_id][bName], BusStopData[busstop_id][bName]);
	mysql_query_format("UPDATE `ipb_busstops` SET busstop_name = '%s', busstop_posx = '%f', busstop_posy = '%f', busstop_posz = '%f' WHERE busstop_uid = '%d' LIMIT 1",
	BusStopData[busstop_id][bName],
	
	BusStopData[busstop_id][bPosX],
	BusStopData[busstop_id][bPosY],
	BusStopData[busstop_id][bPosZ],
	
	BusStopData[busstop_id][bUID]);
	return 1;
}

forward DeleteBusStop(busstop_id);
public DeleteBusStop(busstop_id)
{
	mysql_query_format("DELETE FROM `ipb_busstops` WHERE busstop_uid = '%d' LIMIT 1", BusStopData[busstop_id][bUID]);

	BusStopData[busstop_id][bUID] = 0;
	BusStopData[busstop_id][bPosX] = 0;
	BusStopData[busstop_id][bPosY] = 0;
	BusStopData[busstop_id][bPosZ] = 0;

	DestroyDynamic3DTextLabel(Text3D:BusStopData[busstop_id][bNameTag]);
	return 1;
}

stock GetFreeBusStopID()
{
	new busstop_id;
	for(new b = 1; b < MAX_BUSES; b++)
	{
		if(BusStopData[b][bUID] == 0)
		{
			busstop_id = b;
			break;
		}
	}
	return busstop_id;
}