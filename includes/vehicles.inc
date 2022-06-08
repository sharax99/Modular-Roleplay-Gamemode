new VehicleNames[212][] = {
   "Landstalker",  "Bravura",  "Buffalo", "Linerunner", "Perennial", "Sentinel",
   "Dumper",  "Firetruck" ,  "Trashmaster" ,  "Stretch",  "Manana",  "Infernus",
   "Voodoo", "Pony",  "Mule", "Cheetah", "Ambulance",  "Leviathan",  "Moonbeam",
   "Esperanto", "Taxi",  "Washington",  "Bobcat",  "Mr Whoopee", "BF Injection",
   "Hunter", "Premier",  "Enforcer",  "Securicar", "Banshee", "Predator", "Bus",
   "Rhino",  "Barracks",  "Hotknife",  "Trailer",  "Previon", "Coach", "Cabbie",
   "Stallion", "Rumpo", "RC Bandit",  "Romero", "Packer", "Monster",  "Admiral",
   "Squalo", "Seasparrow", "Pizzaboy", "Tram", "Trailer",  "Turismo", "Speeder",
   "Reefer", "Tropic", "Flatbed","Yankee", "Caddy", "Solair","Berkley's RC Van",
   "Skimmer", "PCJ-600", "Faggio", "Freeway", "RC Baron","RC Raider","Glendale",
   "Oceanic", "Sanchez", "Sparrow",  "Patriot", "Quad",  "Coastguard", "Dinghy",
   "Hermes", "Sabre", "Rustler", "ZR-350", "Walton",  "Regina",  "Comet", "BMX",
   "Burrito", "Camper", "Marquis", "Baggage", "Dozer","Maverick","News Chopper",
   "Rancher", "FBI Rancher", "Virgo", "Greenwood","Jetmax","Hotring","Sandking",
   "Blista Compact", "Police Maverick", "Boxville", "Benson","Mesa","RC Goblin",
   "Hotring Racer", "Hotring Racer", "Bloodring Banger", "Rancher",  "Super GT",
   "Elegant", "Journey", "Bike", "Mountain Bike", "Beagle", "Cropdust", "Stunt",
   "Tanker", "RoadTrain", "Nebula", "Majestic", "Buccaneer", "Shamal",  "Hydra",
   "FCR-900","NRG-500","HPV1000","Cement Truck","Tow Truck","Fortune","Cadrona",
   "FBI Truck", "Willard", "Forklift","Tractor","Combine","Feltzer","Remington",
   "Slamvan", "Blade", "Freight", "Streak","Vortex","Vincent","Bullet","Clover",
   "Sadler",  "Firetruck", "Hustler", "Intruder", "Primo", "Cargobob",  "Tampa",
   "Sunrise", "Merit",  "Utility Truck",  "Nevada", "Yosemite", "Windsor",  "Monster",
   "Monster","Uranus","Jester","Sultan","Stratum","Elegy","Raindance","RCTiger",
   "Flash","Tahoma","Savanna", "Bandito", "Freight", "Trailers", "Kart", "Mower",
   "Dune", "Sweeper", "Broadway", "Tornado", "AT-400",  "DFT-30", "Huntley",
   "Stafford", "BF-400", "Newsvan","Tug","Trailerk","Emperor","Wayfarer","Euros",
   "Hotdog", "Club", "Trailerb", "Trailere","Andromada","Dodo","RC Cam", "Launch",
   "Police Car (LSPD)", "Police Car (SFPD)","Police Car (LVPD)","Police Ranger",
   "Picador",   "S.W.A.T. Van",  "Alpha",   "Phoenix",   "Glendale Shit", "Stinger",
   "Luggage Trailere","Luggage Trailer","Stair Trailer", "Boxville", "Farm Plow",
   "Utility Trailer"
};

enum e_OffsetTypes {
	VEHICLE_OFFSET_BOOT,
	VEHICLE_OFFSET_HOOD,
	VEHICLE_OFFSET_ROOF
};

#if !defined IsValidVehicle
	native IsValidVehicle(vehicleid);
#endif

#define GetVehicleBoot(%0,%1,%2,%3) \
	(GetVehicleOffset((%0), VEHICLE_OFFSET_BOOT, %1, %2, %3))

/*new VehicleTrunkCapacity[212] = {
	25, 8, 12, 80, 15, 12,
	80, 40, 80, 25, 8, 12,
	8, 20, 25, 8, 25, 10, 12,
	8, 15, 15, 25, 15, 2,
	3, 15, 35, 30, 6, 5, 80,
	10, 40, 4, 100, 8, 65, 15,
	7, 30, 0, 10, 10, 5, 15,
	4, 5, 1, 0, 90, 8, 4,
	5, 10, 90, 70, 2, 15, 30,
	10, 0, 0, 15, 0, 3, 1, 7,
	8, 2, 6, 18, 20, 7, 0,
	13, 17, 9, 10, 4, 5, 8,
	7, 4, 65, 65, 10, 0,
	2, 2, 2, 13, 5,
	15, 16, 0, 0, 10, 1, 1,
	70, 0, 12, 8, 6, 12, 1,
	1, 0, 2, 60, 10, 8, 7,
	15, 8, 1, 1, 1, 8, 7,
	10, 7, 0, 0, 2, 11, 8, 5,
	10, 12, 15, 40, 20, 8, 6,
	6, 7, 7, 14, 17, 9, 18, 0,
	6, 13, 5, 0, 0, 0, 0,
	60, 1, 5, 7, 70, 40, 20,
	15, 1, 15, 1, 15, 12, 1, 8,
	10, 10, 15, 15, 80, 2, 0, 10,
	15, 15, 17, 20,
	20, 25, 8, 7, 10, 15,
	10, 10, 0, 60, 0, 5
};*/

new Float:VehicleEngineCapacity[212] = {
	3.6,  1.6,  3.8, 6.4, 1.9, 2.6,
	28.0,  6.6 ,  5.8 ,  3.0,  1.4,  8.3,
	2.8, 3.5,  4.0, 8.0, 5.8,  10.0,  3.0,
	2.4, 2.8,  3.3,  2.5,  2.5, 2.0,
	15.0, 2.8,  5.0,  4.5, 6.4, 4.4, 10.0,
	25.0,  10.0,  3.4,  0.0,  1.5, 10.0, 2.8,
	2.0, 3.5, 0.0,  2.5, 7.0, 8.5,  1.8,
	5.0, 9.0, 1.0, 0.0, 0.0, 8.2, 6.0,
	6.5, 6.5, 6.8, 4.5, 0.6, 2.2, 2.6,
	5.5, 1.0, 1.0, 1.4, 0.0, 0.0, 2.3,
	2.0, 0.8, 5.5, 4.2, 0.7,  5.5, 3.0,
	2.8, 4.0, 6.7, 6.2, 1.6,  1.7,  4.4, 0.0,
	5.6, 2.6, 4.6, 1.0, 5.6, 6.0, 6.0,
	5.2, 5.5, 2.4, 1.8, 6.0, 8.2, 6.4,
	2.4, 6.5, 4.8, 4.8, 3.2, 0.0,
	5.0, 5.0, 5.0, 5.2, 8.0,
	2.8, 4.5, 0.0, 0.0, 4.8, 3.8, 3.5,
	10.1, 11.4, 1.8, 2.0, 2.4, 15.0, 30.0,
	1.3, 1.6, 1.4, 8.0, 4.0, 1.6, 1.6,
	4.2, 1.8, 0.6, 0.8, 4.5, 2.0, 2.3,
	2.4, 2.4, 0.0, 0.0, 1.4, 1.8, 7.3, 2.0,
	1.8, 7.5, 2.0, 1.8, 1.6, 14.0, 2.2,
	1.8, 2.2, 3.2, 20.0, 3.6, 2.0, 7.0,
	7.0, 2.2, 4.6, 3.4, 2.4, 3.4, 11.0, 0.0,
	2.4, 1.8, 2.2, 1.2, 0.0, 0.0, 1.0, 1.0,
	7.0, 1.0, 2.2, 2.4, 18.0, 6.2, 4.2,
	2.8, 1.1, 3.0, 0.4, 0.0, 1.8, 1.3, 3.0,
	2.8, 1.9, 0.0, 0.0, 40.0, 9.0, 0.0, 8.0,
	4.6, 4.6, 4.6, 5.4,
	2.1, 6.0, 4.0, 4.2, 1.8, 1.6,
	0.0, 0.0, 0.0, 4.6, 0.0,
	0.0
};

new VehicleFuelMax[212] = {
55, 45, 65, 100, 45, 55,
70, 150, 125, 90, 45, 60,
50, 60, 60, 70, 75, 100, 60,
50, 55, 60, 50, 45, 40,
100, 60, 80, 75, 70, 60, 100,
100, 90, 65, 0, 45, 90, 55, 
55, 60, 0, 55, 90, 90, 50,
60, 60, 15, 0, 0, 65, 60,
60, 60, 80, 80, 25, 55, 70, 
60, 30, 10, 25, 0, 0, 55,
55, 45, 60, 70, 20, 60, 30,
55, 60, 80, 85, 55, 60, 70, 0,
60, 55, 60, 30, 75, 90, 90, 
65, 70, 55, 60, 50, 70, 75, 
55, 90, 75, 75, 55, 0,
70, 70, 70, 65, 75, 
60, 75, 0, 0, 250, 220, 220, 
90, 90, 55, 60, 60, 90, 90,
30, 35, 30, 60, 55, 50, 50,
60, 55, 25, 30, 60, 55, 50,
60, 55, 0, 0, 25, 55, 70, 50,
55, 150, 50, 55, 60, 80, 75,
60, 65, 65, 80, 60, 60, 80,
80, 60, 65, 70, 55, 60, 80, 0,
65, 55, 50, 40, 0, 0, 10, 10,
70, 20, 50, 50, 90, 80, 60,
60, 20, 65, 30, 0, 55, 25, 55,
60, 45, 0, 0, 90, 60, 0, 60,
72, 72, 72, 75,
60, 80, 70, 60, 60, 60,
0, 0, 0, 70, 0,
0,
};

stock ListDealership(playerid, category, header[40])
{
	new query[128], list_vehicles[1024], rows, fields;
	mysql_format(mySQLconnection, query, sizeof(query), "SELECT * FROM ipb_veh_dealer WHERE dealer_category = %d", category);
	mysql_query(mySQLconnection, query);
	cache_get_data(rows, fields);

	DynamicGui_Init(playerid);
	for(new row = 0; row != rows; row++)
	{
		new model = cache_get_row_int(row, 2);
		new price = cache_get_row_int(row, 3);
		format(list_vehicles, sizeof(list_vehicles), "%s %s\t$%d\n", list_vehicles, VehicleNames[model-400], price);
		DynamicGui_AddRow(playerid, DG_PRODS_SALON, model);  
	}

	if(strlen(list_vehicles))
	{
		ShowPlayerDialog(playerid, DIALOG_SALON_SELL, DIALOG_STYLE_TABLIST, header, list_vehicles, "Buy", "Exit");
	}
	else
	{
		SendGuiInformation(playerid, header, "No vehicles in this cattegory.");
	}
}

stock LoadVehicle(limit[] = "", bool:return_id = false)
{
	new rows, fields;
	Code_ExTimer_Begin(VehicleLoad);
	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_vehicles` %s", limit));
	cache_get_data(rows, fields);
	
	new vid = INVALID_VEHICLE_ID;
	
	for(new row = 0; row != rows; row++)
	{
		if( return_id && GetVehicleByUid(cache_get_row_int(row, 0)) != INVALID_VEHICLE_ID ) continue;
		
		new tmp_model, Float:tmp_park[4], tmp_color[2], tmp_owner;
		
		tmp_model 	= cache_get_row_int(row, 1);
		tmp_park[0] = cache_get_row_float(row, 2);
		tmp_park[1] = cache_get_row_float(row, 3);
		tmp_park[2] = cache_get_row_float(row, 4);
		tmp_park[3] = cache_get_row_float(row, 5);
		tmp_color[0] = cache_get_row_int(row, 8);
		tmp_color[1] = cache_get_row_int(row, 9);
		tmp_owner = cache_get_row_int(row, 22);

		if(tmp_owner == VEHICLE_OWNER_TYPE_PLAYER)
		{
			vid = CreateVehicle(tmp_model, tmp_park[0], tmp_park[1], tmp_park[2], tmp_park[3], tmp_color[0], tmp_color[1], 3600);
		}
		else if(tmp_owner == VEHICLE_OWNER_TYPE_JOB)
		{
			vid = CreateVehicle(tmp_model, tmp_park[0], tmp_park[1], tmp_park[2], tmp_park[3], tmp_color[0], tmp_color[1], 600);
		}
		else if(tmp_owner == VEHICLE_OWNER_TYPE_GROUP || tmp_owner == 0)
		{
			vid = CreateVehicle(tmp_model, tmp_park[0], tmp_park[1], tmp_park[2], tmp_park[3], tmp_color[0], tmp_color[1], -1);
		}

		LinkVehicleToInterior(vid, cache_get_row_int(row, 7));
		SetVehicleVirtualWorld(vid, cache_get_row_int(row, 6));
		
		Vehicle[vid][vehicle_park_world] = cache_get_row_int(row, 6);
		Vehicle[vid][vehicle_park_interior] = cache_get_row_int(row, 7);

		Vehicle[vid][vehicle_uid] = cache_get_row_int(row, 0);
		Vehicle[vid][vehicle_model] = tmp_model;
		Vehicle[vid][vehicle_owner_type] = cache_get_row_int(row, 22);
		Vehicle[vid][vehicle_owner] = cache_get_row_int(row, 21);
		Vehicle[vid][vehicle_color][0] = tmp_color[0];
		Vehicle[vid][vehicle_color][1] = tmp_color[1];
		Vehicle[vid][vehicle_paintjob] = cache_get_row_int(row, 16);
		Vehicle[vid][vehicle_park][0] = tmp_park[0];
		Vehicle[vid][vehicle_park][1] = tmp_park[1];
		Vehicle[vid][vehicle_park][2] = tmp_park[2];
		Vehicle[vid][vehicle_park][3] = tmp_park[3];
		Vehicle[vid][vehicle_health] = cache_get_row_float(row, 12);
		Vehicle[vid][vehicle_accessories] = cache_get_row_int(row, 17);
		
		Vehicle[vid][vehicle_mileage] = cache_get_row_float(row, 13);

		Vehicle[vid][vehicle_blocked] = cache_get_row_int(row, 18);

		Vehicle[vid][vehicle_registered] = cache_get_row_int(row, 19);
		
		Vehicle[vid][vehicle_last_pos][0] = tmp_park[0];
		Vehicle[vid][vehicle_last_pos][1] = tmp_park[1];
		Vehicle[vid][vehicle_last_pos][2] = tmp_park[2];
		
		Vehicle[vid][vehicle_map_icon] = CreateDynamicMapIcon(Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2], 55, 0, 0, 0, 0, 10000.0);
		Streamer_SetIntData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_STYLE, 3);
		Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_PLAYER_ID, 0);
		
		Vehicle[vid][vehicle_description] = CreateDynamic3DTextLabel("", LABEL_DESCRIPTION, 0.0, 0.0, 0.0, 6.0, INVALID_PLAYER_ID, vid);

		Vehicle[vid][vehicle_sign] = CreateDynamic3DTextLabel("", COLOR_WHITE, -0.9, -2.50, -0.46, 15.0, INVALID_PLAYER_ID, vid);
		
		cache_get_row(row, 23, Vehicle[vid][vehicle_block_reason], mySQLconnection, 32);	

		new tmp_dmg[80];
		cache_get_row(row, 15, tmp_dmg);	
		sscanf(tmp_dmg, "a<d>[4]", Vehicle[vid][vehicle_damage]);

		new tmp_comp[128];
		cache_get_row(row, 24, tmp_comp);	
		sscanf(tmp_comp, "a<d>[12]", Vehicle[vid][vehicle_component]);
		
		Vehicle[vid][vehicle_engine] = false;
		Vehicle[vid][vehicle_lights] = false;
		Vehicle[vid][vehicle_boot] = false;
		Vehicle[vid][vehicle_bonnet] = false;
		Vehicle[vid][vehicle_destroyed] = false;
		
		Vehicle[vid][vehicle_locked] = true;
		Vehicle[vid][vehicle_last_fuel_td] = gettime();
		Vehicle[vid][vehicle_driver] = INVALID_PLAYER_ID;
		
		// Wczytywanie paliwa
		Vehicle[vid][vehicle_fuel_current] = cache_get_row_float(row, 10);
		Vehicle[vid][vehicle_fuel_type] = 1;
		
		cache_get_row(row, 20, Vehicle[vid][vehicle_register], mySQLconnection, 12);

		if(Vehicle[vid][vehicle_registered] == 0)
		{
			new rstr[22];
			
			if(Vehicle[vid][vehicle_uid] < 10)
			{
				format(rstr, sizeof(rstr), "%d%c%c%c00%d", random(9), randomString(), randomString(), randomString(), Vehicle[vid][vehicle_uid]);
			}
			else if(Vehicle[vid][vehicle_uid] > 9 && Vehicle[vid][vehicle_uid] < 100)
			{
				format(rstr, sizeof(rstr), "%d%c%c%c0%d", random(9), randomString(), randomString(), randomString(), Vehicle[vid][vehicle_uid]);
			}
			else if(Vehicle[vid][vehicle_uid] > 99)
			{
				format(rstr, sizeof(rstr), "%d%c%c%c%d", random(9), randomString(), randomString(), randomString(), Vehicle[vid][vehicle_uid]);
			}

			SetVehicleNumberPlate(vid, rstr);
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_register = '%s', vehicle_registered = '1' WHERE vehicle_uid = %d", rstr, Vehicle[vid][vehicle_uid]));
		}
		else
		{
			SetVehicleNumberPlate(vid, Vehicle[vid][vehicle_register]);
		}

		Iter_Add(Vehicles, vid);

		new model = GetVehicleModel(vid);
  		if(model == 509 || model == 510 || model == 481)
	    {
	        Vehicle[vid][vehicle_engine] = true;
	    }
		
		// Wykrywanie zniszczonych pojazdow
		if( Vehicle[vid][vehicle_health] <= 250.0 )
		{
			Vehicle[vid][vehicle_health] = 300.0;
			Vehicle[vid][vehicle_destroyed] = true;
			SetVehicleHealth(vid, 300.0);
		}
		else SetVehicleHealth(vid, Vehicle[vid][vehicle_health]);
		
		if( Vehicle[vid][vehicle_paintjob] >= 0 && Vehicle[vid][vehicle_paintjob] < 3 )
		{
		 	ChangeVehiclePaintjob(vid, Vehicle[vid][vehicle_paintjob]);
		 	ChangeVehicleColor(vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);
		}
		
		Vehicle[vid][vehicle_win_driver] = true;
		Vehicle[vid][vehicle_win_pp] = true;
		Vehicle[vid][vehicle_win_pt] = true;
		Vehicle[vid][vehicle_win_lt] = true;

		UpdateVehicleVisuals(vid);
		UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);

		for(new s;s<14;s++)
		{
		    if(Vehicle[vid][vehicle_component][s] != 0)
		    {
				AddVehicleComponent(vid, Vehicle[vid][vehicle_component][s] + 999);
		    }
		}

		// Tworzenie labela statusow
		Vehicle[vid][vehicle_state_label] = CreateDynamic3DTextLabel("", COLOR_WHITE, 0.0, 0.0, 1.1, 10.0, INVALID_PLAYER_ID, vid, 0, -1, 0, -1, 50.0);
		
		if( rows == 1 ) logprintf(LOG_VEHICLE, "[load] Loaded vehicle UID %d w czasie %d ms", Vehicle[vid][vehicle_uid], Code_ExTimer_End(VehicleLoad));
	}
	
	if( return_id ) return vid;
	return rows;
}

stock DeleteVehicle(vehicleid, bool:from_db = false)
{
	if( from_db ) mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_vehicles` WHERE `vehicle_uid` = %d", Vehicle[vehicleid][vehicle_uid]));
	else SaveVehicle(vehicleid);
	
	DestroyDynamic3DTextLabel(Vehicle[vehicleid][vehicle_state_label]);
	DestroyDynamic3DTextLabel(Vehicle[vehicleid][vehicle_description]);
	DestroyDynamic3DTextLabel(Vehicle[vehicleid][vehicle_sign]);
	
	if(Vehicle[vehicleid][vehicle_siren] == true)
	{
		DestroyDynamicObject(Vehicle[vehicleid][vehicle_siren_object]);
		Vehicle[vehicleid][vehicle_siren] = false;
	}
	
	for(new z=0; e_vehicles:z != e_vehicles; z++)
	{
		Vehicle[vehicleid][e_vehicles:z] = 0;
	}

	Iter_Remove(Vehicles, vehicleid);
	DestroyVehicle(vehicleid);

}

stock UpdateVehicleVisuals(vehicleid)
{
	SetVehicleParamsEx(vehicleid, Vehicle[vehicleid][vehicle_engine], Vehicle[vehicleid][vehicle_lights], false, false, Vehicle[vehicleid][vehicle_bonnet], Vehicle[vehicleid][vehicle_boot], false);
}

stock UpdateWindowVisuals(vehicleid)
{
	SetVehicleParamsCarWindows(vehicleid, Vehicle[vehicleid][vehicle_win_driver], Vehicle[vehicleid][vehicle_win_pp], Vehicle[vehicleid][vehicle_win_lt], Vehicle[vehicleid][vehicle_win_pt]);
}

stock SaveVehicle(vehicleid)
{
	new visual_damage[32];
	format(visual_damage, sizeof(visual_damage), "%d %d %d %d", Vehicle[vehicleid][vehicle_damage][0], Vehicle[vehicleid][vehicle_damage][1], Vehicle[vehicleid][vehicle_damage][2], Vehicle[vehicleid][vehicle_damage][3]);

	new Float:saveHealth = Vehicle[vehicleid][vehicle_health];
	if( Vehicle[vehicleid][vehicle_destroyed] ) saveHealth = 0.0;
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_health` = '%f', `vehicle_visual` = '%s', `vehicle_mileage` = %f, `vehicle_fuel` = '%f' WHERE `vehicle_uid` = %d", saveHealth, visual_damage, Vehicle[vehicleid][vehicle_mileage], Vehicle[vehicleid][vehicle_fuel_current], Vehicle[vehicleid][vehicle_uid]));
}

stock GetVehicleByUid(v_uid)
{
	foreach(new vid : Vehicles)
	{
		if( Vehicle[vid][vehicle_uid] == v_uid ) return vid;
	}
	
	return INVALID_VEHICLE_ID;
}

stock GetVehicleModelIDFromName(vname[])
{
	for(new i = 0; i < 211; i++)
	{
		if(strfind(VehicleNames[i], vname, true) != -1)
		return i + 400;
	}
	return -1;
}

stock CanPlayerUseVehicle(playerid, vehicleid)
{
	if(playerid == INVALID_PLAYER_ID) return 0;
	if( HasCrewFlag(playerid, CREW_FLAG_VEHICLES) ) return 1;
	
	if( Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_JOB)
	{
		if( Vehicle[vehicleid][vehicle_owner] == pInfo[playerid][player_job]) return 1;
	}
	if( Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_PLAYER )
	{
		if( Vehicle[vehicleid][vehicle_owner] == pInfo[playerid][player_id] ) return 1;
	}
	else if( Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP )
	{
		new gid = GetGroupByUid(Vehicle[vehicleid][vehicle_owner]), slot = GetPlayerGroupSlot(playerid, gid);
		if( slot > -1 )
		{
			if( WorkerHasFlag(playerid, slot, WORKER_FLAG_VEHICLES) ) return 1;
		}
	}
	
	return 0;
}

stock CanPlayerEditVehicle(playerid, vehicleid)
{
	if( HasCrewFlag(playerid, CREW_FLAG_VEHICLES) ) return 1;
	
	if( Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_PLAYER )
	{
		if( Vehicle[vehicleid][vehicle_owner] == pInfo[playerid][player_id] ) return 1;
	}
	else if( Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP )
	{
		new gid = GetGroupByUid(Vehicle[vehicleid][vehicle_owner]), slot = GetPlayerGroupSlot(playerid, gid);
		if( slot > -1 )
		{
			if( WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return 1;
		}
	}
	
	return 0;
}

stock GetNearestVehicle(playerid)
{
	new 
		Float:vehicle_pos[3],
		Float:pretender_distance = 10.0,
		pretender = -1;
		
	foreach(new v_id : Vehicles)
	{	
		GetVehiclePos(v_id, vehicle_pos[0], vehicle_pos[1], vehicle_pos[2]);
		new 
			Float:distance = GetPlayerDistanceFromPoint(playerid, vehicle_pos[0], vehicle_pos[1], vehicle_pos[2]);
		if( distance <= pretender_distance )
		{
			if( !VehicleHasAccessory(v_id, VEHICLE_ACCESSORY_ALARM) && distance > 4.0 ) continue;
			// TODO: suppport for alarm
			pretender_distance = distance;
			pretender = v_id;
		}
	}
	
	return pretender;
}

stock GetNearestPlayerVehicle(playerid)
{
	new 
		Float:vehicle_pos[3],
		Float:pretender_distance = 10.0,
		pretender = -1;
		
	foreach(new v_id : Vehicles)
	{
		if( !CanPlayerUseVehicle(playerid, v_id) ) continue;
		
		GetVehiclePos(v_id, vehicle_pos[0], vehicle_pos[1], vehicle_pos[2]);
		new 
			Float:distance = GetPlayerDistanceFromPoint(playerid, vehicle_pos[0], vehicle_pos[1], vehicle_pos[2]);
		if( distance <= pretender_distance )
		{
			if( !VehicleHasAccessory(v_id, VEHICLE_ACCESSORY_ALARM) && distance > 4.0 ) continue;
			// TODO: suppport for alarm
			pretender_distance = distance;
			pretender = v_id;
		}
	}
	
	return pretender;
}

stock IsVehicleOccupied(vehicleid)
{
    foreach(new i: Player)
    {
        if(IsPlayerInAnyVehicle(i))
        {
            if(GetPlayerVehicleID(i)==vehicleid)
            {
                return 1;
            }
        }
    }
    return 0;
}

stock GetVehicleDriver(vehicleid, checks = true)
{	
	if( IsPlayerConnected(Vehicle[vehicleid][vehicle_driver]) )
	{
		if( checks )
		{
			if( GetPlayerVehicleID(Vehicle[vehicleid][vehicle_driver]) == vehicleid && GetPlayerVehicleSeat(Vehicle[vehicleid][vehicle_driver]) == 0 ) return Vehicle[vehicleid][vehicle_driver];
		}
		else return Vehicle[vehicleid][vehicle_driver];
	}
	return INVALID_PLAYER_ID;
}


task VehiclesUpdate[1000]()
{
	foreach(new vid : Vehicles)
	{
		if(Vehicle[vid][vehicle_engine])
		{
			if( gettime() - Vehicle[vid][vehicle_last_fuel_td] > 40 )
			{
				new model = GetVehicleModel(vid);
		  		if(model != 509 && model != 510 && model != 481)
			    {
					new driver = GetVehicleDriver(vid);
					if( driver != INVALID_PLAYER_ID )
					{
						SendPlayerInformation(driver,sprintf("~w~Fuel: ~y~%d/%d~w~L", floatround(Vehicle[vid][vehicle_fuel_current], floatround_ceil), VehicleFuelMax[GetVehicleModel(vid)-400]),3000);
					}
					
					Vehicle[vid][vehicle_last_fuel_td] = gettime();
				}
			}
		}
		
		//NOP engine state
		if(GetVehicleSpeed(vid)>30 && Vehicle[vid][vehicle_engine] == false && Vehicle[vid][vehicle_occupants] > 0)
		{
			new driver = GetVehicleDriver(vid);
			if(driver != INVALID_PLAYER_ID)
			{
				if( !CanPlayerUseVehicle(driver, vid) )
				{
	        	   	KickAc(driver, -1, "NOP SetEngineState");
					//SetVehicleToRespawn(vid);
					continue;
				}
			}
		}

		//Ostatnio uzywany
		if(Vehicle[vid][vehicle_last_used] != 0)
		{
			if(gettime() > Vehicle[vid][vehicle_last_used] + 60000)
			{
				if(Vehicle[vid][vehicle_occupants] == 0)
				{
					Vehicle[vid][vehicle_last_used] = 0;
				}
			}
		}

		// Sprawdzamy hp pojazdu
		new Float:vehicleHp;
		GetVehicleHealth(vid, vehicleHp);


		if( vehicleHp < 250 && Vehicle[vid][vehicle_destroyed] == false)
		{
			Vehicle[vid][vehicle_engine] = false;
			UpdateVehicleVisuals(vid);
			Vehicle[vid][vehicle_destroyed] = true;
			SetVehicleHealth(vid, 300);
			vehicleHp = 300;
		}

		if( vehicleHp > Vehicle[vid][vehicle_health] )
		{
			SetVehicleHealth(vid, Vehicle[vid][vehicle_health]);
		}

		// Sprawdzamy tuning pojazdu
		for (new j = 0; j < 14; ++j)
   		{
			new componentid = GetVehicleComponentInSlot(vid, j);
  			if (componentid != 0 && componentid != (Vehicle[vid][vehicle_component][j] + 999))
        	{
        		new str[50];
        		format(str, sizeof(str), "Force tune (component: %d, v: %d)", componentid, vid);
        		new driver = GetVehicleDriver(vid);
        		if(driver != INVALID_PLAYER_ID)
        		{
        	   		BanAc(driver, -1, str);
					DeleteVehicle(vid, false);
	            	continue;
	            }
	        }
	    }
		
		// Sprawdzamy zniszczenia wizualne
		new vehicleDmg[4];
		GetVehicleDamageStatus(vid, vehicleDmg[0], vehicleDmg[1], vehicleDmg[2], vehicleDmg[3]);
		
		if( vehicleDmg[0] < Vehicle[vid][vehicle_damage][0] || vehicleDmg[1] < Vehicle[vid][vehicle_damage][1] || vehicleDmg[2] < Vehicle[vid][vehicle_damage][2] || vehicleDmg[3] < Vehicle[vid][vehicle_damage][3] )
		{
			UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);
		}
		
		Vehicle[vid][vehicle_damage][0] = vehicleDmg[0];
		Vehicle[vid][vehicle_damage][1] = vehicleDmg[1];
		Vehicle[vid][vehicle_damage][2] = vehicleDmg[2];
		Vehicle[vid][vehicle_damage][3] = vehicleDmg[3];
		
		// Czas na przebieg
		new Float:distance = GetVehicleDistanceFromPoint(vid, Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2]);
		if( distance >= 0 )
		{
			if( distance > 0 ) Vehicle[vid][vehicle_mileage] += distance/1000;
			
			if( Vehicle[vid][vehicle_engine] )
			{
				// Czas na paliwko
				new Float:fuel_drain;
				
				// Pojemnosc silnika
				fuel_drain = floatdiv(VehicleEngineCapacity[Vehicle[vid][vehicle_model]-400], 10000);
				
				// Typ paliwa
				switch( Vehicle[vid][vehicle_fuel_type] )
				{
					case VEHICLE_FUEL_TYPE_BENZYNA:
					{
						fuel_drain *= BENZYNA_MULTIPLIER;
					}
					
					case VEHICLE_FUEL_TYPE_DIESEL:
					{
						fuel_drain *= DIESEL_MULTIPLIER;
					}
					
					case VEHICLE_FUEL_TYPE_GAZ:
					{
						fuel_drain *= GAZ_MULTIPLIER;
					}
					
					case VEHICLE_FUEL_TYPE_LOTNICZE:
					{
						fuel_drain *= LOTNICZE_MULTIPLIER;
					}
				}
				
				// Przebieg pojazdu
				if( Vehicle[vid][vehicle_mileage] > FUEL_DRAIN_INCREASE_MILEAGE_CAP )
				{
					fuel_drain *= floatdiv(Vehicle[vid][vehicle_mileage], FUEL_DRAIN_INCREASE_MILEAGE_CAP);
				}
				
				if( distance > 0 ) fuel_drain *= distance;
				else fuel_drain *= 1.4;
				

				if( floatcmp(Vehicle[vid][vehicle_fuel_current], fuel_drain) <= 0 )
				{
					Vehicle[vid][vehicle_fuel_current] = 0.0;
					Vehicle[vid][vehicle_engine] = false;
					UpdateVehicleVisuals(vid);
					
					new playerid = GetVehicleDriver(vid);
					
					if( playerid != INVALID_PLAYER_ID )
					{
						TextDrawShowForPlayer(playerid, vehicleInfo);
						SendPlayerInformation(playerid, "There is no more fuel in your vehicle.", 5000);
					}
				}
				else Vehicle[vid][vehicle_fuel_current] -= fuel_drain;
			}
		}

		// Ustawiamy ostatnia pozycje na aktualna
		GetVehiclePos(vid, Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2]);
		
		if( distance > 0 )
		{
			// Poprawiamy pozycje map icony
			Streamer_SetFloatData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_X, Vehicle[vid][vehicle_last_pos][0]);
			Streamer_SetFloatData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_Y, Vehicle[vid][vehicle_last_pos][1]);
			Streamer_SetFloatData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_Z, Vehicle[vid][vehicle_last_pos][2]);
		}

		Vehicle[vid][vehicle_health] = vehicleHp;
		Vehicle[vid][vehicle_damaged] = false;
	}
}

stock GetVehicleOffset(vehicleid, type, &Float:x, &Float:y, &Float:z)
{
    new Float:fPos[4], Float:fSize[3];

	if (!IsValidVehicle(vehicleid))
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;

		return 0;
	}
	else
	{
		GetVehiclePos(vehicleid, fPos[0], fPos[1], fPos[2]);
		GetVehicleZAngle(vehicleid, fPos[3]);
		GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_SIZE, fSize[0], fSize[1], fSize[2]);

		switch (type)
		{
		    case VEHICLE_OFFSET_BOOT:
		    {
				x = fPos[0] - (floatsqroot(fSize[1] + fSize[1]) * floatsin(-fPos[3], degrees));
				y = fPos[1] - (floatsqroot(fSize[1] + fSize[1]) * floatcos(-fPos[3], degrees));
 				z = fPos[2];
			}
			case VEHICLE_OFFSET_HOOD:
			{
			    x = fPos[0] + (floatsqroot(fSize[1] + fSize[1]) * floatsin(-fPos[3], degrees));
				y = fPos[1] + (floatsqroot(fSize[1] + fSize[1]) * floatcos(-fPos[3], degrees));
	 			z = fPos[2];
			}
			case VEHICLE_OFFSET_ROOF:
			{
			    x = fPos[0];
			    y = fPos[1];
			    z = fPos[2] + floatsqroot(fSize[2]);
			}
		}
	}
	return 1;
}

stock IsPlayerInInvalidNosVehicle(playerid)
{
	new vehicleid = GetPlayerVehicleID(playerid);
	#define MAX_INVALID_NOS_VEHICLES 52
	new InvalidNosVehicles[MAX_INVALID_NOS_VEHICLES] =
	{
		581,523,462,521,463,522,461,448,468,586,417,425,469,487,512,520,563,593,
		509,481,510,472,473,493,520,595,484,430,453,432,476,497,513,533,577,
		452,446,447,454,590,569,537,538,570,449,519,460,488,511,519,548,592
	};
 	if(IsPlayerInAnyVehicle(playerid))
  	{
   		for(new i = 0; i < MAX_INVALID_NOS_VEHICLES; i++)
     	{
      		if(GetVehicleModel(vehicleid) == InvalidNosVehicles[i]) return true;
       	}
   	}
   	return false;
}

stock IsComponentidCompatible(modelid, componentid)
{
    if(componentid == 1025 || componentid == 1073 || componentid == 1074 || componentid == 1075 || componentid == 1076 ||
         componentid == 1077 || componentid == 1078 || componentid == 1079 || componentid == 1080 || componentid == 1081 ||
         componentid == 1082 || componentid == 1083 || componentid == 1084 || componentid == 1085 || componentid == 1096 ||
         componentid == 1097 || componentid == 1098 || componentid == 1087 || componentid == 1086)
         return true;

    switch (modelid)
    {
        case 400: return (componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1018 || componentid == 1013 || componentid == 1024 || componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 401: return (componentid == 1005 || componentid == 1004 || componentid == 1142 || componentid == 1143 || componentid == 1144 || componentid == 114 || componentid == 1020 || componentid == 1019 || componentid == 1013 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1001 || componentid == 1003 || componentid == 1017 || componentid == 1007);
        case 402: return (componentid == 1009 || componentid == 1009 || componentid == 1010);
        case 404: return (componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1013 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1002 || componentid == 1016 || componentid == 1000 || componentid == 1017 || componentid == 1007);
        case 405: return (componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1001 || componentid == 1014 || componentid == 1023 || componentid == 1000);
        case 409: return (componentid == 1009);
        case 410: return (componentid == 1019 || componentid == 1021 || componentid == 1020 || componentid == 1013 || componentid == 1024 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1001 || componentid == 1023 || componentid == 1003 || componentid == 1017 || componentid == 1007);
        case 411: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 412: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 415: return (componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1001 || componentid == 1023 || componentid == 1003 || componentid == 1017 || componentid == 1007);
        case 418: return (componentid == 1020 || componentid == 1021 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1002 || componentid == 1016);
        case 419: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 420: return (componentid == 1005 || componentid == 1004 || componentid == 1021 || componentid == 1019 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1001 || componentid == 1003);
        case 421: return (componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1014 || componentid == 1023 || componentid == 1016 || componentid == 1000);
        case 422: return (componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1013 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1017 || componentid == 1007);
        case 426: return (componentid == 1005 || componentid == 1004 || componentid == 1021 || componentid == 1019 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1001 || componentid == 1003);
        case 429: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 436: return (componentid == 1020 || componentid == 1021 || componentid == 1022 || componentid == 1019 || componentid == 1013 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1001 || componentid == 1003 || componentid == 1017 || componentid == 1007);
        case 438: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 439: return (componentid == 1003 || componentid == 1023 || componentid == 1001 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1017 || componentid == 1007 || componentid == 1142 || componentid == 1143 || componentid == 1144 || componentid == 1145 || componentid == 1013);
        case 442: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 445: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 451: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 458: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 466: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 467: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 474: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 475: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 477: return (componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1017 || componentid == 1007);
        case 478: return (componentid == 1005 || componentid == 1004 || componentid == 1012 || componentid == 1020 || componentid == 1021 || componentid == 1022 || componentid == 1013 || componentid == 1024 || componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 479: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 480: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 489: return (componentid == 1005 || componentid == 1004 || componentid == 1020 || componentid == 1019 || componentid == 1018 || componentid == 1013 || componentid == 1024 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1002 || componentid == 1016 || componentid == 1000);
        case 491: return (componentid == 1142 || componentid == 1143 || componentid == 1144 || componentid == 1145 || componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1014 || componentid == 1023 || componentid == 1003 || componentid == 1017 || componentid == 1007);
        case 492: return (componentid == 1005 || componentid == 1004 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1016 || componentid == 1000);
        case 496: return (componentid == 1006 || componentid == 1017 || componentid == 1007 || componentid == 1011 || componentid == 1019 || componentid == 1023 || componentid == 1001 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1003 || componentid == 1002 || componentid == 1142 || componentid == 1143 || componentid == 1020);
        case 500: return (componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1013 || componentid == 1024 || componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 506: return (componentid == 1009);
        case 507: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 516: return (componentid == 1004 || componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1002 || componentid == 1015 || componentid == 1016 || componentid == 1000 || componentid == 1017 || componentid == 1007);
        case 517: return (componentid == 1142 || componentid == 1143 || componentid == 1144 || componentid == 1145 || componentid == 1020 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1002 || componentid == 1023 || componentid == 1016 || componentid == 1003 || componentid == 1017 || componentid == 1007);
        case 518: return (componentid == 1005 || componentid == 1142 || componentid == 1143 || componentid == 1144 || componentid == 1145 || componentid == 1020 || componentid == 1018 || componentid == 1013 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1001 || componentid == 1023 || componentid == 1003 || componentid == 1017 || componentid == 1007);
        case 526: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 527: return (componentid == 1021 || componentid == 1020 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1001 || componentid == 1014 || componentid == 1015 || componentid == 1017 || componentid == 1007);
        case 529: return (componentid == 1012 || componentid == 1011 || componentid == 1020 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1001 || componentid == 1023 || componentid == 1003 || componentid == 1017 || componentid == 1007);
        case 533: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 534: return (componentid == 1126 || componentid == 1127 || componentid == 1179 || componentid == 1185 || componentid == 1100 || componentid == 1123 || componentid == 1125 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1180 || componentid == 1178 || componentid == 1101 || componentid == 1122 || componentid == 1124 || componentid == 1106);
        case 535: return (componentid == 1109 || componentid == 1110 || componentid == 1113 || componentid == 1114 || componentid == 1115 || componentid == 1116 || componentid == 1117 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1120 || componentid == 1118 || componentid == 1121 || componentid == 1119);
        case 536: return (componentid == 1104 || componentid == 1105 || componentid == 1182 || componentid == 1181 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1184 || componentid == 1183 || componentid == 1128 || componentid == 1103 || componentid == 1107 || componentid == 1108);
        case 540: return (componentid == 1004 || componentid == 1142 || componentid == 1143 || componentid == 1144 || componentid == 1145 || componentid == 1020 || componentid == 1019 || componentid == 1018 || componentid == 1024 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1001 || componentid == 1023 || componentid == 1017 || componentid == 1007);
        case 541: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 542: return (componentid == 1144 || componentid == 1145 || componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1014 || componentid == 1015);
        case 545: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 546: return (componentid == 1004 || componentid == 1142 || componentid == 1143 || componentid == 1144 || componentid == 1145 || componentid == 1019 || componentid == 1018 || componentid == 1024 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1002 || componentid == 1001 || componentid == 1023 || componentid == 1017 || componentid == 1007);
        case 547: return (componentid == 1142 || componentid == 1143 || componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1016 || componentid == 1003 || componentid == 1000);
        case 549: return (componentid == 1012 || componentid == 1011 || componentid == 1142 || componentid == 1143 || componentid == 1144 || componentid == 1145 || componentid == 1020 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1001 || componentid == 1023 || componentid == 1003 || componentid == 1017 || componentid == 1007);
        case 550: return (componentid == 1005 || componentid == 1004 || componentid == 1142 || componentid == 1143 || componentid == 1144 || componentid == 1145 || componentid == 1020 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1001 || componentid == 1023 || componentid == 1003);
        case 551: return (componentid == 1005 || componentid == 1020 || componentid == 1021 || componentid == 1019 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1002 || componentid == 1023 || componentid == 1016 || componentid == 1003);
        case 555: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 558: return (componentid == 1092 || componentid == 1089 || componentid == 1166 || componentid == 1165 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1168 || componentid == 1167 || componentid == 1088 || componentid == 1091 || componentid == 1164 || componentid == 1163 || componentid == 1094 || componentid == 1090 || componentid == 1095 || componentid == 1093);
        case 559: return (componentid == 1065 || componentid == 1066 || componentid == 1160 || componentid == 1173 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1159 || componentid == 1161 || componentid == 1162 || componentid == 1158 || componentid == 1067 || componentid == 1068 || componentid == 1071 || componentid == 1069 || componentid == 1072 || componentid == 1070 || componentid == 1009);
        case 560: return (componentid == 1028 || componentid == 1029 || componentid == 1169 || componentid == 1170 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1141 || componentid == 1140 || componentid == 1032 || componentid == 1033 || componentid == 1138 || componentid == 1139 || componentid == 1027 || componentid == 1026 || componentid == 1030 || componentid == 1031);
        case 561: return (componentid == 1064 || componentid == 1059 || componentid == 1155 || componentid == 1157 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1154 || componentid == 1156 || componentid == 1055 || componentid == 1061 || componentid == 1058 || componentid == 1060 || componentid == 1062 || componentid == 1056 || componentid == 1063 || componentid == 1057);
        case 562: return (componentid == 1034 || componentid == 1037 || componentid == 1171 || componentid == 1172 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1149 || componentid == 1148 || componentid == 1038 || componentid == 1035 || componentid == 1147 || componentid == 1146 || componentid == 1040 || componentid == 1036 || componentid == 1041 || componentid == 1039);
        case 565: return (componentid == 1046 || componentid == 1045 || componentid == 1153 || componentid == 1152 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1150 || componentid == 1151 || componentid == 1054 || componentid == 1053 || componentid == 1049 || componentid == 1050 || componentid == 1051 || componentid == 1047 || componentid == 1052 || componentid == 1048);
        case 566: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 567: return (componentid == 1129 || componentid == 1132 || componentid == 1189 || componentid == 1188 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1187 || componentid == 1186 || componentid == 1130 || componentid == 1131 || componentid == 1102 || componentid == 1133);
        case 575: return (componentid == 1044 || componentid == 1043 || componentid == 1174 || componentid == 1175 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1176 || componentid == 1177 || componentid == 1099 || componentid == 1042);
        case 576: return (componentid == 1136 || componentid == 1135 || componentid == 1191 || componentid == 1190 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1192 || componentid == 1193 || componentid == 1137 || componentid == 1134);
        case 579: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 580: return (componentid == 1020 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1001 || componentid == 1023 || componentid == 1017 || componentid == 1007);
        case 585: return (componentid == 1142 || componentid == 1143 || componentid == 1144 || componentid == 1145 || componentid == 1020 || componentid == 1019 || componentid == 1018 || componentid == 1013 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1001 || componentid == 1023 || componentid == 1003 || componentid == 1017 || componentid == 1007);
        case 587: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 589: return (componentid == 1005 || componentid == 1004 || componentid == 1144 || componentid == 1145 || componentid == 1020 || componentid == 1018 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1024 || componentid == 1013 || componentid == 1006 || componentid == 1016 || componentid == 1000 || componentid == 1017 || componentid == 1007);
        case 600: return (componentid == 1005 || componentid == 1004 || componentid == 1020 || componentid == 1022 || componentid == 1018 || componentid == 1013 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1017 || componentid == 1007);
        case 602: return (componentid == 1008 || componentid == 1009 || componentid == 1010);
        case 603: return (componentid == 1144 || componentid == 1145 || componentid == 1142 || componentid == 1143 || componentid == 1020 || componentid == 1019 || componentid == 1018 || componentid == 1024 || componentid == 1008 || componentid == 1009 || componentid == 1010 || componentid == 1006 || componentid == 1001 || componentid == 1023 || componentid == 1017 || componentid == 1007);
    }
    return false;
}

stock GetComponentName(component)
{
	new modname[50];
	switch(component)
	{
	   case 1000: format(modname, sizeof(modname), "Pro Spoiler");
	   case 1001: format(modname, sizeof(modname), "Win Spoiler");
	   case 1002: format(modname, sizeof(modname), "Drag Spoiler");
	   case 1003: format(modname, sizeof(modname), "Alpha Spoiler");
	   case 1004: format(modname, sizeof(modname), "Champ Scoop");
	   case 1005: format(modname, sizeof(modname), "Fury Scoop");
	   case 1006: format(modname, sizeof(modname), "Roof Scoop");
	   case 1007: format(modname, sizeof(modname), "Right Sideskirt");
	   case 1008: format(modname, sizeof(modname), "Nitrous x5");
	   case 1009: format(modname, sizeof(modname), "Nitrous x2");
	   case 1010: format(modname, sizeof(modname), "Nitrous x10");
	   case 1011: format(modname, sizeof(modname), "Race Scoop");
	   case 1012: format(modname, sizeof(modname), "Worx Scoop");
	   case 1013: format(modname, sizeof(modname), "Round Fog Lights");
	   case 1014: format(modname, sizeof(modname), "Champ Spoiler");
	   case 1015: format(modname, sizeof(modname), "Race Spoiler");
	   case 1016: format(modname, sizeof(modname), "Worx Spoiler");
	   case 1017: format(modname, sizeof(modname), "Left Sideskirt");
	   case 1018: format(modname, sizeof(modname), "Upswept Exhaust");
	   case 1019: format(modname, sizeof(modname), "Twin Exhaust");
	   case 1020: format(modname, sizeof(modname), "Large Exhaust");
	   case 1021: format(modname, sizeof(modname), "Medium Exhaust");
	   case 1022: format(modname, sizeof(modname), "Small Exhaust");
	   case 1023: format(modname, sizeof(modname), "Fury Spoiler");
	   case 1024: format(modname, sizeof(modname), "Square Fog Lights");
	   case 1025: format(modname, sizeof(modname), "Offroad Wheels");
	   case 1026, 1036, 1047, 1056, 1069, 1090: format(modname, sizeof(modname), "Right Alien Sideskirt");
	   case 1027, 1040, 1051, 1062, 1071, 1094: format(modname, sizeof(modname), "Left Alien Sideskirt");
	   case 1028, 1034, 1046, 1064, 1065, 1092: format(modname, sizeof(modname), "Alien Exhaust");
	   case 1029, 1037, 1045, 1059, 1066, 1089: format(modname, sizeof(modname), "X-Flow Exhaust");
	   case 1030, 1039, 1048, 1057, 1070, 1095: format(modname, sizeof(modname), "Right X-Flow Sideskirt");
	   case 1031, 1041, 1052, 1063, 1072, 1093: format(modname, sizeof(modname), "Left X-Flow Sideskirt");
	   case 1032, 1038, 1054, 1055, 1067, 1088: format(modname, sizeof(modname), "Alien Roof Vent");
	   case 1033, 1035, 1053, 1061, 1068, 1091: format(modname, sizeof(modname), "X-Flow Roof Vent");
	   case 1042: format(modname, sizeof(modname), "Right Chrome Sideskirt");
	   case 1099: format(modname, sizeof(modname), "Left Chrome Sideskirt");
	   case 1043, 1105, 1114, 1127, 1132, 1135: format(modname, sizeof(modname), "Slamin Exhaust");
	   case 1044, 1104, 1113, 1126, 1129, 1136: format(modname, sizeof(modname), "Chrome Exhaust");
	   case 1050, 1058, 1139, 1146, 1158, 1163: format(modname, sizeof(modname), "X-Flow Spoiler");
	   case 1049, 1060, 1138, 1147, 1162, 1164: format(modname, sizeof(modname), "Alien Spoiler");
	   case 1073: format(modname, sizeof(modname), "Shadow Wheels");
	   case 1074: format(modname, sizeof(modname), "Mega Wheels");
	   case 1075: format(modname, sizeof(modname), "Rimshine Wheels");
	   case 1076: format(modname, sizeof(modname), "Wires Wheels");
	   case 1077: format(modname, sizeof(modname), "Classic Wheels");
	   case 1078: format(modname, sizeof(modname), "Twist Wheels");
	   case 1079: format(modname, sizeof(modname), "Cutter Wheels");
	   case 1080: format(modname, sizeof(modname), "Stitch Wheels");
	   case 1081: format(modname, sizeof(modname), "Grove Wheels");
	   case 1082: format(modname, sizeof(modname), "Import Wheels");
	   case 1083: format(modname, sizeof(modname), "Dollar Wheels");
	   case 1084: format(modname, sizeof(modname), "Trance Wheels");
	   case 1085: format(modname, sizeof(modname), "Atomic Wheels");
	   case 1086: format(modname, sizeof(modname), "Stereo");
	   case 1087: format(modname, sizeof(modname), "Hydraulics");
	   case 1096: format(modname, sizeof(modname), "Ahab Wheels");
	   case 1097: format(modname, sizeof(modname), "Virtual Wheels");
	   case 1098: format(modname, sizeof(modname), "Access Wheels");
	   case 1100: format(modname, sizeof(modname), "Chrome Grill");
	   case 1101: format(modname, sizeof(modname), "Left Chrome Flames Sideskirt");
	   case 1102, 1107: format(modname, sizeof(modname), "Left Chrome Strip Sideskirt");
	   case 1103: format(modname, sizeof(modname), "Convertible Roof");
	   case 1106, 1124: format(modname, sizeof(modname), "Left Chrome Arches Sideskirt");
	   case 1108, 1133, 1134: format(modname, sizeof(modname), "Right Chrome Strip Sideskirt");
	   case 1109: format(modname, sizeof(modname), "Chrome Rear Bullbars");
	   case 1110: format(modname, sizeof(modname), "Slamin Rear Bullbars");
	   case 1111, 1112: format(modname, sizeof(modname), "Front Sign");
	   case 1115: format(modname, sizeof(modname), "Chrome Front Bullbars");
	   case 1116: format(modname, sizeof(modname), "Slamin Front Bullbars");
	   case 1117, 1174, 1179, 1182, 1189, 1191: format(modname, sizeof(modname), "Chrome Front Bumper");
	   case 1175, 1181, 1185, 1188, 1190: format(modname, sizeof(modname), "Slamin Front Bumper");
	   case 1176, 1180, 1184, 1187, 1192: format(modname, sizeof(modname), "Chrome Rear Bumper");
	   case 1177, 1178, 1183, 1186, 1193: format(modname, sizeof(modname), "Slamin Rear Bumper");
	   case 1118: format(modname, sizeof(modname), "Right Chrome Trim Sideskirt");
	   case 1119: format(modname, sizeof(modname), "Right Wheelcovers Sideskirt");
	   case 1120: format(modname, sizeof(modname), "Left Chrome Trim Sideskirt");
	   case 1121: format(modname, sizeof(modname), "Left Wheelcovers Sideskirt");
	   case 1122: format(modname, sizeof(modname), "Right Chrome Flames Sideskirt");
	   case 1123: format(modname, sizeof(modname), "Bullbar Chrome Bars");
	   case 1125: format(modname, sizeof(modname), "Bullbar Chrome Lights");
	   case 1128: format(modname, sizeof(modname), "Vinyl Hardtop Roof");
	   case 1130: format(modname, sizeof(modname), "Hardtop Roof");
	   case 1131: format(modname, sizeof(modname), "Softtop Roof");
	   case 1140, 1148, 1151, 1156, 1161, 1167: format(modname, sizeof(modname), "X-Flow Rear Bumper");
	   case 1141, 1149, 1150, 1154, 1159, 1168: format(modname, sizeof(modname), "Alien Rear Bumper");
	   case 1142: format(modname, sizeof(modname), "Left Oval Vents");
	   case 1143: format(modname, sizeof(modname), "Right Oval Vents");
	   case 1144: format(modname, sizeof(modname), "Left Square Vents");
	   case 1145: format(modname, sizeof(modname), "Right Square Vents");
	   case 1152, 1157, 1165, 1170, 1172, 1173: format(modname, sizeof(modname), "X-Flow Front Bumper");
	   case 1153, 1155, 1160, 1166, 1169, 1171: format(modname, sizeof(modname), "Alien Front Bumper");
	   
	}
	return modname;
}