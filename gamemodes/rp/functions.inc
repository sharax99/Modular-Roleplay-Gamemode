stock LoadConfiguration()
{
	Code_ExTimer_Begin(LoadConfiguration);
	
	if( !dini_Exists(CONFIG_FILEPATH) ) 
	{
		dini_Create(CONFIG_FILEPATH);
		
		dini_Set(CONFIG_FILEPATH, "mysql_hostname", "127.0.0.1");
		dini_Set(CONFIG_FILEPATH, "mysql_username", "root");
		dini_Set(CONFIG_FILEPATH, "mysql_password", "");
		dini_Set(CONFIG_FILEPATH, "mysql_database", "testgta");
		
		dini_Set(CONFIG_FILEPATH, "global_spawn", "0.0|0.0|3.5|0.0");
	}
	
	dini_Get(CONFIG_FILEPATH, "mysql_hostname", Setting[setting_mysql_hostname]);
	dini_Get(CONFIG_FILEPATH, "mysql_username", Setting[setting_mysql_username]);
	dini_Get(CONFIG_FILEPATH, "mysql_password", Setting[setting_mysql_password]);
	dini_Get(CONFIG_FILEPATH, "mysql_database", Setting[setting_mysql_database]);
	
	SetGameModeText("Society RP");
	
	printf("Loaded MySQL configuration [time: %d ms] haha", Code_ExTimer_End(LoadConfiguration));
}

new LogPrintfStr[200];
#define logprintf(%0,%1,%2) \
						format(LogPrintfStr, 200, %1, %2); \
						_logprintf(%0, LogPrintfStr)

stock _logprintf(const file_name[], const string[])
{
	new File:log = fopen(file_name, io_append);
    if(log)
    {
		new date_time[60], Year, Month, Day, Hour, Minute, Second;
		gettime(Hour, Minute, Second);
		getdate(Year, Month, Day);	
		format(date_time, sizeof(date_time), "[%02d.%02d.%02d, %02d:%02d:%02d] ", Day, Month, Year, Hour, Minute, Second);
		
		fwrite(log, date_time);
        fwrite(log, string);
		fwrite(log, "\r\n");
        fclose(log);
    }
}

stock LoadAnims()
{
	Code_ExTimer_Begin(LoadAnims);
	new rows, fields, anim_id;
	mysql_query(mySQLconnection, "SELECT * FROM `ipb_anim` ORDER BY `anim_command` ASC");
	cache_get_data(rows, fields);

	for(new row = 0; row != rows; row++)
	{
		anim_id = Iter_Free(Anims);
		
		AnimInfo[anim_id][aUID] = cache_get_row_int(row, 0);
		cache_get_row(row, 1, AnimInfo[anim_id][aCommand], mySQLconnection, 32);
		cache_get_row(row, 2, AnimInfo[anim_id][aLib], mySQLconnection, 32);
		cache_get_row(row, 3, AnimInfo[anim_id][aName], mySQLconnection, 32);
		AnimInfo[anim_id][aSpeed] = cache_get_row_float(row, 4);
		AnimInfo[anim_id][aOpt1] = cache_get_row_int(row, 5);
		AnimInfo[anim_id][aOpt2] = cache_get_row_int(row, 6);
		AnimInfo[anim_id][aOpt3] = cache_get_row_int(row, 7);
		AnimInfo[anim_id][aOpt4] = cache_get_row_int(row, 8);
		AnimInfo[anim_id][aOpt5] = cache_get_row_int(row, 9);
		AnimInfo[anim_id][aAction] = cache_get_row_int(row, 10);

		Iter_Add(Anims, anim_id);
	}
	printf("Loaded animations [count: %d] [time %d ms]", Iter_Count(Anims), Code_ExTimer_End(LoadAnims));
	return 1;
}

stock ConnectMysql()
{
	Code_ExTimer_Begin(ConnectMysql);
	
	mySQLconnection = mysql_connect(Setting[setting_mysql_hostname], Setting[setting_mysql_username], Setting[setting_mysql_database], Setting[setting_mysql_password]);
	mysql_log(LOG_ERROR);

	if(mysql_errno(mySQLconnection) == 0)
    {
		printf("[MySQL] Connected to %s@%s [time: %d ms]", Setting[setting_mysql_username], Setting[setting_mysql_hostname], Code_ExTimer_End(ConnectMysql));
		return true;
	}
	else
	{
		printf("[MySQL] MySQL error [time: %d ms]", Code_ExTimer_End(ConnectMysql));
		return false;
	}
}

stock LoadGlobalSpawns()
{
	Code_ExTimer_Begin(LoadGlobalSpawns);
	
	new rows, fields;
	mysql_query(mySQLconnection, "SELECT * FROM ipb_gspawns");
	cache_get_data(rows, fields);

 	for(new row = 0; row != rows; row++)
	{
		if( row >= MAX_GLOBAL_SPAWNS ) break;
				
		GlobalSpawn[row][gspawn_id] = cache_get_row_int(row, 0);
		GlobalSpawn[row][gspawn_pos][0] = cache_get_row_float(row, 1);
		GlobalSpawn[row][gspawn_pos][1] = cache_get_row_float(row, 2);
		GlobalSpawn[row][gspawn_pos][2] = cache_get_row_float(row, 3);
		GlobalSpawn[row][gspawn_pos][3] = cache_get_row_float(row, 4);
		GlobalSpawn[row][gspawn_type] = cache_get_row_int(row, 5);
		
		if(GlobalSpawn[row][gspawn_type] == SPAWN_TYPE_GLOBAL_SC)
		{
			Iter_Add(ScSpawns, row);
		}
		else
		{
			Iter_Add(GlobalSpawns, row);
		}
	}
	
	printf("Loaded global spawns [count: %d] [time: %d ms]", Iter_Count(GlobalSpawns)+Iter_Count(ScSpawns), Code_ExTimer_End(LoadGlobalSpawns));
}

stock LoadObjects()
{
	Code_ExTimer_Begin(LoadObjects);

	mysql_query(mySQLconnection, "DELETE FROM `ipb_objects` WHERE `object_type` = '2' "); 
	
	new loaded = LoadObject();
	
	printf("Loaded objects [count: %d] [time: %d ms]", loaded, Code_ExTimer_End(LoadObjects));
}

stock IsBoat(carid)
{
	new Boats[] = { 446, 452, 453, 454, 472, 473, 484, 493 };
  	for(new i = 0; i < sizeof(Boats); i++)
  	{
    	if(GetVehicleModel(carid) == Boats[i]) return 1;
  	}
  	return 0;
}

stock LoadGroups()
{
	Code_ExTimer_Begin(LoadGroups);
	new rows, fields;
	mysql_query(mySQLconnection, "SELECT * FROM `ipb_game_groups` ORDER BY `group_owner` ASC");  
	cache_get_data(rows, fields);

	for(new row = 0; row != rows; row++)
	{
		new i = Iter_Free(Groups);

		Group[i][group_uid] = cache_get_row_int(row, 0);
		Group[i][group_type] = cache_get_row_int(row, 4);

		Group[i][group_flags] = cache_get_row_int(row, 12);
		
		Group[i][group_bank_money] = cache_get_row_int(row, 2);
		Group[i][group_capital] = cache_get_row_int(row, 11);
		Group[i][group_tribute] = cache_get_row_int(row, 6);
		Group[i][group_tributers] = cache_get_row_int(row, 7);
		cache_get_row(row, 1, Group[i][group_name], mySQLconnection, 64);
		
		Group[i][group_activity_points] = cache_get_row_int(row, 9);
		Group[i][group_last_tribute] = cache_get_row_int(row, 15);
		
		Iter_Add(Groups, i);
	}
	
	printf("Loaded groups [count: %d] [time: %d ms]", rows, Code_ExTimer_End(LoadGroups));
}

stock LoadDoors()
{
	Code_ExTimer_Begin(LoadDoors);
	
	new loaded = LoadDoor();
	
	printf("Loaded doors [count: %d] [time: %d ms]", loaded, Code_ExTimer_End(LoadDoors));
}

stock LoadAreas()
{
	Code_ExTimer_Begin(LoadAreas);
	
	new loaded = LoadArea();
	
	printf("Loaded areas [count: %d] [time: %d ms]", loaded, Code_ExTimer_End(LoadAreas));
}

stock LoadLabels()
{
	Code_ExTimer_Begin(LoadLabels);
	
	new loaded = LoadLabel();
	
	printf("Loaded 3d labels [count: %d] [time: %d ms]", loaded, Code_ExTimer_End(LoadLabels));
}

stock LoadVehicles()
{
	Code_ExTimer_Begin(LoadVehicles);

	new loaded = LoadVehicle(sprintf("WHERE `vehicle_ownertype` = %d OR  `vehicle_ownertype` = %d", VEHICLE_OWNER_TYPE_GROUP, VEHICLE_OWNER_TYPE_JOB));
	
	printf("Loaded vehicles [count: %d] [time: %d ms]", loaded, Code_ExTimer_End(LoadVehicles));
}

stock LoadItems()
{
	Code_ExTimer_Begin(LoadItems);

	new loaded = LoadItem(sprintf("WHERE `item_ownertype` = %d OR `item_ownertype` = %d", ITEM_OWNER_TYPE_GROUP, ITEM_OWNER_TYPE_GROUND));
	
	printf("Loaded items [count: %d] [time: %d ms]", loaded, Code_ExTimer_End(LoadItems));
}

stock LoadGates()
{
	Code_ExTimer_Begin(LoadGates);

	new loaded = LoadGate();
	
	printf("Loaded gates [count: %d] [time: %d ms]", loaded, Code_ExTimer_End(LoadGates));
}

stock LoadMaterials()
{
	Code_ExTimer_Begin(LoadMaterials);

	new loaded = LoadMaterial();
	
	printf("Loaded textures [count: %d] [time: %d ms]", loaded, Code_ExTimer_End(LoadMaterials));
}

stock LoadSkins()
{
	Code_ExTimer_Begin(LoadSkins);
	new skinid = 0, rows, fields;

	mysql_query(mySQLconnection, "SELECT * FROM `ipb_game_skins`");
	cache_get_data(rows, fields);

	for(new row = 0; row != rows; row++)
	{
	    if(skinid == MAX_SKINS) break;

        ClothSkin[skinid][skin_uid] = cache_get_row_int(row, 0);
        ClothSkin[skinid][skin_model] = cache_get_row_int(row, 1);
        cache_get_row(row, 2, ClothSkin[skinid][skin_name], mySQLconnection, 32);
		ClothSkin[skinid][skin_price] = cache_get_row_int(row, 3);
	    skinid++;
	}
	printf("Loaded skins [count: %d] [time: %d ms]", skinid-1, Code_ExTimer_End(LoadSkins));
}

stock LoadAccess()
{
	Code_ExTimer_Begin(LoadAccess);
	new accessid = 0, rows, fields;

	mysql_query(mySQLconnection, "SELECT * FROM ipb_attached_objects WHERE attach_category != 0");
	cache_get_data(rows, fields);

	for(new row = 0; row != rows; row++)
	{
	    if(accessid == MAX_ACCESS) break;

        ClothAccess[accessid][access_uid] = cache_get_row_int(row, 0);
        ClothAccess[accessid][access_model] = cache_get_row_int(row, 3);
        ClothAccess[accessid][access_price] = cache_get_row_int(row, 16);
        ClothAccess[accessid][access_bone] = cache_get_row_int(row, 4);
        ClothAccess[accessid][access_category] = cache_get_row_int(row, 14);
        ClothAccess[accessid][access_pos][0] = cache_get_row_float(row, 5);
        ClothAccess[accessid][access_pos][1] = cache_get_row_float(row, 6);
        ClothAccess[accessid][access_pos][2] = cache_get_row_float(row, 7);
        ClothAccess[accessid][access_pos][3] = cache_get_row_float(row, 8);
        ClothAccess[accessid][access_pos][4] = cache_get_row_float(row, 9);
        ClothAccess[accessid][access_pos][5] = cache_get_row_float(row, 10);

        cache_get_row(row, 15, ClothAccess[accessid][access_name], mySQLconnection, 40);

	    accessid++;
	}
	printf("Loaded accessories [count: %d] [time: %d ms]", accessid-1, Code_ExTimer_End(LoadAccess));
}
