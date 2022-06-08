new Float: LogAttachOffsets[10][4] = 
{
    {-0.223, -1.089, -0.230, -90.399},
	{-0.056, -1.091, -0.230, 90.399},
	{0.116, -1.092, -0.230, -90.399},
	{0.293, -1.088, -0.230, 90.399},
	{-0.123, -1.089, -0.099, -90.399},
	{0.043, -1.090, -0.099, 90.399},
	{0.216, -1.092, -0.099, -90.399},
	{-0.033, -1.090, 0.029, -90.399},
	{0.153, -1.089, 0.029, 90.399},
	{0.066, -1.091, 0.150, -90.399}
};

timer Tree_Cut[5000](playerid, object_id)
{
	if(!pInfo[playerid][player_cutting_tree]) return 1;
	new Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz;
	GetDynamicObjectPos(object_id, x, y, z);
	GetDynamicObjectRot(object_id, rx, ry, rz);
	MoveDynamicObject(object_id, x, y, z + 0.03, 0.025, rx, ry -80.0, rz);
	pInfo[playerid][player_cutting_tree] = false;
	new skin = GetPlayerSkin(playerid);
	SetPlayerSkin(playerid, skin);
	TogglePlayerControllable(playerid, 0);
	TogglePlayerControllable(playerid, 1);
	return 1;
}

timer Fish_Get[5000](playerid, veh)
{
	if(!pInfo[playerid][player_fishing]) return 1;

	new randx = random(6);

	switch(randx)
	{
		case 0:
		{
			SendClientMessage(playerid, COLOR_GOLD, "Failed to catch fish. Try again.");
			return 1;
		}
		case FISH_TYPE_SALMON:
		{
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Fish has been loaded to boat. It's salmon. Space left: %d/10", 10-Vehicle[veh][vehicle_fish_object]));
			Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_FOOD, 19630, 25, FISH_TYPE_SALMON, "Salmon");
		}
		case FISH_TYPE_COD:
		{
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Fish has been loaded to boat. It's cod. Space left: %d/10", 10-Vehicle[veh][vehicle_fish_object]));
			Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_FOOD, 19630, 25, FISH_TYPE_COD, "Cod");
		}
		case FISH_TYPE_SPRAT:
		{
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Fish has been loaded to boat. It's sprat. Space left: %d/10", 10-Vehicle[veh][vehicle_fish_object]));
			Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_FOOD, 19630, 25, FISH_TYPE_SPRAT, "Sprat");
		}
		case FISH_TYPE_GARFISH:
		{
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Fish has been loaded to boat. It's garfish. Space left: %d/10", 10-Vehicle[veh][vehicle_fish_object]));
			Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_FOOD, 19630, 25, FISH_TYPE_GARFISH, "Garfish");
		}
		case FISH_TYPE_RARE:
		{
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Fish has been loaded to boat. It's rare sort, illegal to fishing. Space left: %d/10.", 10-Vehicle[veh][vehicle_fish_object]));
			SendClientMessage(playerid, COLOR_GOLD, "To release this fish, type /release.");
			Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_FOOD, 19630, 25, FISH_TYPE_RARE, "Rare fish");
		}
	}

	Vehicle[veh][vehicle_fish_object] += 1;

	pInfo[playerid][player_fishing] = false;

	return 1;
}

stock GetVehicleLogCount(vehicleid)
{
	if(GetVehicleModel(vehicleid) == 0) return 0;
	new count;
	for(new i; i < 10; i++) if(IsValidDynamicObject(Vehicle[vehicleid][vehicle_log_object][i])) count++;
	return count;
}

stock GetVehicleFishCount(vehicleid)
{
	return Vehicle[vehicleid][vehicle_fish_object];
}