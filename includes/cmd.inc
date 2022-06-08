COMMAND:release(playerid, params[])
{
	if(pInfo[playerid][player_working] != WORK_TYPE_FISHER) return SendGuiInformation(playerid, "Information", "You are not at fishing place or you are not fisher.");

	new item = HasPlayerFish(FISH_TYPE_RARE, playerid);
	if (item == -1 ) return SendGuiInformation(playerid, "Information", "You don't have any rare fish to release.");

	SendGuiInformation(playerid, "Information", "Rare fish has been released.");
	ProxMessage(playerid, "releases fish to ocean.", PROX_SERWERME);

	DeleteItem(item, true, playerid);
	return 1;
}

COMMAND:golf(playerid, params[])
{
	new sub_cmd[20];
	if( sscanf(params, "s[32]", sub_cmd) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /golf [start, end]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "start") )
		{
			if(GetPlayerWeapon(playerid)!=2)
			{
				SendGuiInformation(playerid, "Information","You have to get golf club.");
			}
			else if(GetPlayerWeapon(playerid)==2)
			{
				// Sprawdzamy czy jest na polu golfowym

				if(pInfo[playerid][player_area] <= 0) return SendGuiInformation(playerid, "Information", "You are not at golf area.");
				if(!AreaHasFlag(pInfo[playerid][player_area], AREA_FLAG_GOLF))  return SendGuiInformation(playerid, "Information", "You are not at golf area.");

				if(golf==1) return SendGuiInformation(playerid, "Information", "Someone is playing right now, wait for you turn.");

				// Pobieramy pozycje dla pileczki

				new Float:o_pos[3];
				GetPointInAngleOfPlayer(playerid, o_pos[0], o_pos[1], o_pos[2], 3.0, 0.0);

				// Informacja
				SendGuiInformation(playerid, "Information", "Started golf game, read instructions from textdraw."); 

				// Textdraw z iloœci¹ punktów oraz uderzeniami

				SendPlayerInformation(playerid, "Key ~y~Y~w~ - light hit~n~Key ~y~N~w~ -strong hit", 15000);

				// Tworzenie obiektu pi³ki

				ballid = CreateObject(3003, o_pos[0]+1, o_pos[1], o_pos[2]-0.9, 0.0000000,0.0000000,0.0000000); 
				PHY_InitObject(ballid, 3003, 1.0);
				PHY_RollObject(ballid);
				PHY_SetObjectFriction(ballid, 1.00);

				pInfo[playerid][player_golf] = true;
				golf = 1;
			}
		}
		else if( !strcmp(sub_cmd, "end") )
		{
			// Informacja
			SendGuiInformation(playerid, "Information", "Game finished.");

			// Usuniêcie pi³ki
			DestroyObject(ballid);
			PHY_DeleteObject(ballid);
			pInfo[playerid][player_golf] = false;
			golf = 0;
		}
	}
	return 1;
}

COMMAND:zbadaj(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];
	if(gid == - 1) return SendGuiInformation(playerid, "Information", "Nie jesteœ na s³u¿bie grupy porz¹dkowej.");
	if(!GroupHasFlag(gid, GROUP_FLAG_911)) return SendGuiInformation(playerid, "Information", "Nie jesteœ na s³u¿bie grupy porz¹dkowej.");

	new sub_cmd[20], rest[64];
	
	if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /zbadaj [odciski]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "odciski") )
		{
			new item, rows, fields;
			if(sscanf(rest, "d", item)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /zbadaj odciski [uid przedmiotu]");
			new itemid = GetItemByUid(item, playerid);
			if(itemid == -1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /zbadaj odciski [uid przedmiotu]");
			if(!Iter_Contains(PlayerItems[playerid], itemid)) return SendGuiInformation(playerid, "Information", "Niepoprawne UID przedmiotu.");

			mysql_query(mySQLconnection, sprintf("SELECT finger_person FROM ipb_fingerprints WHERE finger_owner = %d LIMIT 15", item));
			cache_get_data(rows, fields);

			new list_prints[128], last_print;

			for(new row = 0; row != rows; row++)
			{
				new printowner = cache_get_row_int(row, 0);
				if(printowner == last_print) continue;
				last_print = printowner;
				format(list_prints, sizeof(list_prints), "%s\n%d", list_prints, printowner);
			}

			if(strlen(list_prints))
			{
				format(list_prints, sizeof(list_prints), "DNA w³aœciciela odcisku:\n%s", list_prints);
				ShowPlayerDialog(playerid, DIALOG_FINGERPRINTS, DIALOG_STYLE_TABLIST_HEADERS, "Ostatnie odciski palców", list_prints, "Badaj", "WyjdŸ");
			}
			else
			{
				SendGuiInformation(playerid, "Information", "Brak odcisków palców na tym przedmiocie.");
			}
		}
	}
	return 1;
}

COMMAND:compressor(playerid, params[])
{
	if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return SendGuiInformation(playerid, "Information", "You are not a driver.");

	new object_id = GetClosestObjectType(playerid, OBJECT_COMPRESS);

	if(object_id == INVALID_OBJECT_ID)
	{
		SendGuiInformation(playerid, "Information", "You are not near compressor.");
		return 1;
	}

	if(pInfo[playerid][player_money] < 50) return SendGuiInformation(playerid, "Information", "Pumping the wheels is costing $50. You don't have enough cash.");

	GivePlayerMoney(playerid, -50);

	new vehid = GetPlayerVehicleID(playerid);

	defer FixTires[5000](vehid);

	SendPlayerInformation(playerid, "Pumping the wheels, please wait~y~...", 5000);
	FreezePlayer(playerid, 5000);
	return 1;
}

COMMAND:gov(playerid, params[])
{
	if(!PlayerHasFlag(playerid, PLAYER_FLAG_PRESIDENT)) return SendGuiInformation(playerid, "Information", "You are not mayor of Los Santos.");

	new rows, fields;

	mysql_query(mySQLconnection, "SELECT group_uid, group_name, group_activity FROM ipb_game_groups WHERE group_activity > 0 ORDER BY group_activity DESC");
	cache_get_data(rows, fields);

	if(rows)
	{
		new list_groups[1500];

		for(new row = 0; row != rows; row++)
		{
			new tmp[64];
			cache_get_row(row, 1, tmp);
			format(list_groups, sizeof(list_groups), "%s\n%d\t%s\t%d", list_groups, cache_get_row_int(row, 0), tmp, cache_get_row_int(row, 2));
		}

		if(strlen(list_groups))
		{
			format(list_groups, sizeof(list_groups), "UID\tName\tActivity points\n%s", list_groups);
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST_HEADERS, "Activity ranking", list_groups, "OK", "");
		}
	}
	else
	{
		SendGuiInformation(playerid, "Information", "There are no actives group this day.");
	}
	return 1;
}

COMMAND:dotation(playerid, params[])
{
	if(!PlayerHasFlag(playerid, PLAYER_FLAG_PRESIDENT)) return SendGuiInformation(playerid, "Information", "You are not mayor of Los Santos.");

	new targetguid, amount;
	if( sscanf(params, "dd", targetguid, amount) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /dotation [group uid] [amount]");

	if(amount < 0 || amount > 1500) SendGuiInformation(playerid, "Information", "Invalid amount.");

	new targetgid = GetGroupByUid(targetguid);
	if(targetgid == -1) return SendGuiInformation(playerid, "Information", "Invalid group UID.");

	mysql_query(mySQLconnection, sprintf("UPDATE ipb_game_groups SET group_dotation = %d WHERE group_uid = %d", amount, targetguid));
	SendGuiInformation(playerid, "Information", sprintf("Dotation $%d has been set up for group %s.", amount, Group[targetgid][group_name]));

	return 1;
}

COMMAND:ignore(playerid, params[])
{
	new targetid;
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ignore [player id/part of name]");

	if( !IsPlayerConnected(targetid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ignore [player id/part of name]");
	if( !pInfo[targetid][player_logged]) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ignore [player id/part of name]");

	if(pInfo[playerid][player_ignore] == pGlobal[targetid][glo_id])
	{
		SendClientMessage(playerid, COLOR_GOLD, sprintf("Ignore for %s has been stoped.", pInfo[targetid][player_name]));
		pInfo[playerid][player_ignore] = INVALID_PLAYER_ID;
	}
	else
	{
		SendClientMessage(playerid, COLOR_GOLD, sprintf("Now you ignore %s.", pInfo[targetid][player_name]));
		pInfo[playerid][player_ignore] = pGlobal[targetid][glo_id];
	}
	return 1;
}

COMMAND:prices(playerid, params[])
{
	if(GetPlayerVirtualWorld(playerid) == 0) return SendGuiInformation(playerid, "Information", "You are not in any business.");
	new did = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if(did == -1) return SendGuiInformation(playerid, "Information", "You are not in any business.");
	if(Door[did][door_owner_type] != DOOR_OWNER_TYPE_GROUP) return SendGuiInformation(playerid, "Information", "You are not in any business.");

	new gid = GetGroupByUid(Door[did][door_owner]);
	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not in any business.");
	if( !GroupHasFlag(gid, GROUP_FLAG_BUSINESS) ) return SendGuiInformation(playerid, "Information", "You are not in any business.");

	new list_items[512];

	foreach(new itemid: Items)
	{
		if(Item[itemid][item_owner_type] != ITEM_OWNER_TYPE_GROUP) continue;
		if(Item[itemid][item_owner] != Door[did][door_owner]) continue;

		format(list_items, sizeof(list_items), "%s\n%s\t$%d", list_items, Item[itemid][item_name], Item[itemid][item_price]);
	}

	if(strlen(list_items))
	{
		format(list_items, sizeof(list_items), "Product\tPrice\n%s", list_items);
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST_HEADERS, sprintf("Offer of %s", Group[gid][group_name]), list_items, "OK", "");
	}
	else
	{
		SendGuiInformation(playerid, "Information", "This business has no products in storage.");
	}
	return 1;
}

COMMAND:eject(playerid, params[])
{
	new targetid;
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /eject [player id/part of name]");

	if( !IsPlayerConnected(targetid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /eject [player id/part of name]");
	if( !pInfo[targetid][player_logged]) return SendClientMessage(playerid, COLOR_GREY, "Tip: /eject [player id/part of name]");

	if(GetPlayerVehicleID(playerid) == 0) return SendGuiInformation(playerid, "Information", "You are not in vehicle.");
	if(GetPlayerVehicleID(playerid) != GetPlayerVehicleID(targetid)) return SendGuiInformation(playerid, "Information", "This player is not in your vehicle.");
	if(GetPlayerVehicleSeat(playerid) != 0) return SendGuiInformation(playerid, "Information", "You are not driver.");

	RemovePlayerFromVehicle(targetid);
	SendGuiInformation(targetid, "Information", "You've been ejected from vehicle by driver.");
	return 1;
}


COMMAND:pass(playerid, params[])
{
	new targetid, action[40];
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pass [player id/part of name]");

	if( !IsPlayerConnected(targetid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pass [player id/part of name]");
	if( !pInfo[targetid][player_logged]) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pass [player id/part of name]");

	if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_NONE) return SendGuiInformation(playerid, "Information", "You don't have cigarette or drink.");

	new Float:dist;
	dist = GetDistanceBetweenPlayers(playerid, targetid);
	if(dist > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

	if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_SMOKE_CIGGY)
	{
		format(action, sizeof(action), "Cigarette");
	}
	else if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_DRINK_BEER || GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_DRINK_WINE || GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_DRINK_SPRUNK)
	{
		format(action, sizeof(action), "Drink");
	}

	if(strlen(action))
	{
		new resp = SetOffer(playerid, targetid, OFFER_TYPE_PASS, 0, GetPlayerSpecialAction(playerid));
    	if( resp ) ShowPlayerOffer(targetid, playerid, "Pass", action, 0);
    }
    else
    {
    	SendGuiInformation(playerid, "Information", "You don't have cigarette or drink.");
    }
	return 1;
}

COMMAND:tribute(playerid, params[])
{
	new sub_cmd[20], rest[64];
	if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /tribute [pay/managment]");

	if( !strcmp(sub_cmd, "pay") )
	{	
		new gid = pInfo[playerid][player_duty_gid];
		if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");
		new slot = GetPlayerDutySlot(playerid);
		if(slot == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");

		if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return SendGuiInformation(playerid, "Information", "You are not leader of this business.");
		new targetid, price;
		if(sscanf(rest, "d", targetid, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /tribute pay [player id] [amount]");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
		
		new tgid = pInfo[targetid][player_duty_gid];
		new Float:dist;
		dist = GetDistanceBetweenPlayers(playerid, targetid);
		if(dist>3.0) return SendGuiInformation(playerid, "Information", "This player is too far.");

		if(!GroupHasFlag(tgid, GROUP_FLAG_BUSINESS)) return SendGuiInformation(playerid, "Information", "Only businesses are able pay tributes.");
        if(price <= 0) return SendGuiInformation(playerid, "Information", "Invalid price.");
        if(price > Group[gid][group_tribute]) return SendGuiInformation(playerid, "Information", "This business has no money for tribute.");
        if(Group[gid][group_tributers] != Group[tgid][group_uid]) return SendGuiInformation(playerid, "Information", "This organization is not controlling your business.");

        if(tgid == -1) return SendGuiInformation(playerid, "Information", "This player is not on crime organization duty.");
        if(Group[tgid][group_type] != GROUP_TYPE_GANG) return SendGuiInformation(playerid, "Information", "This player is not on crime organization duty.");

        new tslot = GetPlayerDutySlot(targetid);
        if(slot == -1) return SendGuiInformation(playerid, "Information", "This player is not on crime organization duty.");

        if( !WorkerHasFlag(targetid, tslot, WORKER_FLAG_OFFER) ) return SendGuiInformation(playerid, "Information", "This player has no privileges to accept tributes.");

        if(gettime() < Group[gid][group_last_tribute] + 168*3600 )
		{
			new nextpay = Group[gid][group_last_tribute] + 168*3600;
			new payHour, payMinute, payMonth, payDay, temp;

			TimestampToDate(nextpay, temp, payMonth, payDay, payHour, payMinute, temp, 1);
			if(payHour == 25)
			{
				SendGuiInformation(playerid, "Information", sprintf("Tribute from this group has been already taken in this week.\nYou can take next at: %02d/%02d : 01:%02d.", payDay, payMonth, payHour, payMinute));
			}
			else
			{
				SendGuiInformation(playerid, "Information", sprintf("Tribute from this group has been already taken in this week.\nYou can take next at: %02d/%02d : %02d:%02d.", payDay, payMonth, payHour+1, payMinute));
			}
			return 1;
		}

        Group[gid][group_tribute] -= price;
        GivePlayerMoney(targetid, price);
        Group[gid][group_last_tribute] = gettime();

        mysql_query(mySQLconnection, sprintf("UPDATE ipb_game_groups SET group_last_tribute = %d WHERE group_uid = %d", Group[gid][group_last_tribute], Group[gid][group_uid]));
	}
	else if( !strcmp(sub_cmd, "managment") )
	{	
		new gid = pInfo[playerid][player_duty_gid];
		if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");
		new slot = GetPlayerDutySlot(playerid);
		if(slot == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");
		if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return SendGuiInformation(playerid, "Information", "You are not group leader or you are on wrong duty.");
		if(Group[gid][group_type] != GROUP_TYPE_GANG) return SendGuiInformation(playerid, "Information", "Your type of group is not able to taking tributes.");

		new list_tributes[256], rows, fields;
		mysql_query(mySQLconnection, sprintf("SELECT group_uid, group_name, group_tribute FROM ipb_game_groups WHERE group_tributers = %d", Group[gid][group_uid]));
		cache_get_data(rows, fields);

		for(new row = 0; row != rows; row++)
		{
			new tmp[64];
			cache_get_row(row, 1, tmp);
			format(list_tributes, sizeof(list_tributes), "%s\n%d\t%s\t$%d", list_tributes, cache_get_row_int(row, 0), tmp, cache_get_row_int(row, 2));
		}

		if(strlen(list_tributes))
		{
			format(list_tributes, sizeof(list_tributes), "UID:\tName:\tBudget\n%s", list_tributes);
			ShowPlayerDialog(playerid, DIALOG_TRIBUTE_MENU, DIALOG_STYLE_TABLIST_HEADERS, "Tributes list", list_tributes, "Choose", "Cancel");
			new str[256];
			format(str, sizeof(str), "This is list of businesses under control of your crime organization. Budget is money that business is able to pay for tribute.~n~At this moment you are controlling: ~g~~h~%d~w~ businesses.", rows);
			TextDrawSetString(Tutorial[playerid], str);
			TextDrawShowForPlayer(playerid, Tutorial[playerid]);
		}
		else
		{
			SendGuiInformation(playerid, "Information", "Your group is not controlling any business.");
		}
	}
	return 1;
}

COMMAND:gps(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];
    if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");
    if(!IsPlayerInAnyVehicle(playerid)) return SendGuiInformation(playerid, "Information", "You are not in any vehicle.");

    if( Group[gid][group_type] == GROUP_TYPE_LSPD )
    {
    	if(pInfo[playerid][player_gps] == 0)
    	{
			pInfo[playerid][player_gps] = 1;
			SendGuiInformation(playerid, "Information", "GPS on.");

			new Float:x, Float:y, Float:z;
			GetPlayerPos(playerid, x, y, z);

			foreach(new police: Player)
			{
				if(pInfo[police][player_gps] == 1)
				{
					SetPlayerMapIcon(police, playerid, x, y, z, 30, 0);
				}
			}
		}
		else
		{
			pInfo[playerid][player_gps] = 0;
			SendGuiInformation(playerid, "Information", "GPS off.");

			for(new c;c<90;c++)
			{
				RemovePlayerMapIcon(playerid, c);
			}
		}
	}
	else
	{
		SendGuiInformation(playerid, "Information", "You are not on group duty with access to this command.");
	}
	return 1;
}

COMMAND:taxi(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];
    if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty with access to this command.");

    if( Group[gid][group_type] == GROUP_TYPE_TAXI )
    {
		pInfo[playerid][player_taxi_drive] = false;
		SendGuiInformation(playerid, "Information", "Taxi duty reseted.");
	}
	else
	{
		SendGuiInformation(playerid, "Information", "You are not on group duty with access to this command.");
	}
	return 1;
}

COMMAND:interview(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];
    if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty with access to this command.");

    if( Group[gid][group_type] == GROUP_TYPE_SN )
    {
		new targetid;
		if( sscanf(params, "u", targetid) )
		{
			if(pInfo[playerid][player_interview] > -1)
			{
				foreach(new target: Player)
				{
					if(pInfo[playerid][player_interview] == target)
					{
						pInfo[target][player_interview] = -1;
						SendClientMessage(target, COLOR_GOLD, "Interview has been finished by reporter.");
					}
				}
				pInfo[playerid][player_interview] = -1;
				SendClientMessage(playerid, COLOR_GOLD, "Interview has been finished.");
			}
			else
			{
				SendClientMessage(playerid, COLOR_GREY, "Tip: /interview [player id/part of name]");
			}
		}
		else
		{
			if(!IsPlayerConnected(targetid) || targetid==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
			if(!pInfo[targetid][player_logged]) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
			new Float:dist;
			dist = GetDistanceBetweenPlayers(playerid, targetid);
			if(dist > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

			new resp = SetOffer(playerid, targetid, OFFER_TYPE_INTERVIEW, 0, 0);
	        if( resp ) ShowPlayerOffer(targetid, playerid, "Interview", "Live interview", 0);
		}
	}
	else
	{
		SendGuiInformation(playerid, "Information", "You are not on group duty with access to this command.");
	}
	return 1;
}

COMMAND:train(playerid, params[])
{
	new sub_cmd[20];
	
	if( sscanf(params, "s[32]", sub_cmd) )
	{
		if(pInfo[playerid][player_training] == false)
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /train [dumbbells]");
		}
		else
		{
			ApplyAnimation(playerid, "freeweights", "gym_free_putdown", 1, 0, 0, 0, 1, 0, 1 );
			SetCameraBehindPlayer(playerid);
			
			RemovePlayerAttachedObject(playerid, ATTACH_SLOT_GYM1);
			RemovePlayerAttachedObject(playerid, ATTACH_SLOT_GYM2);

			Object[pInfo[playerid][player_gym_object]][object_can_be_used] = false;

			pInfo[playerid][player_training] = false;
			TextDrawHideForPlayer(playerid, Tutorial[playerid]);
			FreezePlayer(playerid, 1000);
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_strength = '%f' WHERE char_uid = %d", pInfo[playerid][player_strength], pInfo[playerid][player_id]));
		}
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "dumbbells") )
		{
			if(pInfo[playerid][player_training] == true)
			{
				SendGuiInformation(playerid, "Information", "You are already training, use /train to finish.");
				return 1;
			}

			if( pInfo[playerid][player_gym_carnet] == 0)
			{
				SendGuiInformation(playerid, "Information", "You have already trained today.\nWait for the next day.");
				return 1;
			}

			new object_id = GetClosestObjectType(playerid, OBJECT_DUMB);

			if(object_id == INVALID_OBJECT_ID)
			{
				SendGuiInformation(playerid, "Information", "You are not near dumbbells.");
				return 1;
			}	

			if(GetPlayerUsedItem(playerid, ITEM_TYPE_GYM_KARNET ) == -1) return SendGuiInformation(playerid, "Information", "You are not using gym ticket.");
			
			new did = GetDoorByUid(GetPlayerVirtualWorld(playerid));

			if(did == -1)
			{
				SendGuiInformation(playerid, "Information", "You are not at gym.");
				return 1;
			}
			else
			{
				if(Door[did][door_owner_type] != DOOR_OWNER_TYPE_GROUP) return SendGuiInformation(playerid, "Information", "You are not at correct gym.");
				if(pInfo[playerid][player_gym] != Door[did][door_owner]) return SendGuiInformation(playerid, "Information", "You are not at correct gym.");
				if(Object[object_id][object_can_be_used] == true) return SendGuiInformation(playerid, "Information", "Someone is using this dumbbells now.");

				SetPlayerPos(playerid, Object[object_id][object_pos][0]-1, Object[object_id][object_pos][1], Object[object_id][object_pos][2]+1);
				SetPlayerFacingAngle( playerid, Object[object_id][object_pos][3]);
				ApplyAnimation(playerid, "Freeweights", "gym_free_pickup", 1, 0, 0, 0, 1, 0, 1);

				SetPlayerCameraPos(playerid, Object[object_id][object_pos][0]+2.3, Object[object_id][object_pos][1], Object[object_id][object_pos][2]+0.3 );
				SetPlayerCameraLookAt( playerid, Object[object_id][object_pos][0], Object[object_id][object_pos][1], Object[object_id][object_pos][2]+0.5);

				Object[object_id][object_can_be_used] = false;
				pInfo[playerid][player_can_train] = 1;

				defer StartTraining[2500](playerid);
			}
		}
	}
	return 1;
}

COMMAND:desc(playerid, params[])
{
	DynamicGui_Init(playerid);
	new string[1000];
	
	if( !isnull(pInfo[playerid][player_description]) )
	{
		new str[256];
		strcopy(str, pInfo[playerid][player_description], 256);
		strdel(str, 48, 256);
		format(string, sizeof(string), "%s» Current description: {9A9CCD}%s...\n", string, str);
		DynamicGui_AddRow(playerid, DG_NO_ACTION);
		
		format(string, sizeof(string), "%s» Delete description\n", string);
		DynamicGui_AddRow(playerid, DG_CHAR_DESC_DELETE);
	}
	else
	{
		format(string, sizeof(string), "%s» Set description\n", string);
		DynamicGui_AddRow(playerid, DG_CHAR_DESC_ADD);
	}

	format(string, sizeof(string), "%s\t\t\n", string);
	DynamicGui_AddBlankRow(playerid);
	format(string, sizeof(string), "%s{A9C4E4}10 last descriptions:\n", string);
	DynamicGui_AddBlankRow(playerid);

	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT uid, text FROM `ipb_descriptions` WHERE `owner` = %d ORDER BY `last_used` DESC LIMIT 10", pInfo[playerid][player_id]));
	cache_get_data(rows, fields);
	
	if( rows )
	{
		for(new row = 0; row != rows; row++)
		{
			new tmpText[256];
			cache_get_row(row, 1, tmpText);
			strdel(tmpText, 64, 256);
			format(string, sizeof(string), "%s  %d.   %s...\n", string, row+1, tmpText);
			DynamicGui_AddRow(playerid, DG_CHAR_DESC_OLD, cache_get_row_int(row, 0));
		}
	}
	else 
	{
		format(string, sizeof(string), "%s  No description history\n", string);
		DynamicGui_AddBlankRow(playerid);
	}

	ShowPlayerDialog(playerid, DIALOG_CHAR_DESCRIPTION, DIALOG_STYLE_LIST, "Char description", string, "Choose", "Close");
	return 1;
}

COMMAND:dealership(playerid, params[])
{
	if(!IsPlayerInCarDealer(playerid))
	{
		SendGuiInformation(playerid, "Information", "You are not in dealership.");
		return 1;
	}

	ShowPlayerDialog(playerid, DIALOG_SALON, DIALOG_STYLE_TABLIST_HEADERS, "Dealership", "Category\tPrices\nThree doors\tfrom $1800\nFive doors\tfrom $1500\nTrucks\tfrom $4000\nBikes\tfrom $300\nSport cars\tfrom $15000\nBoats\tfrom $4000\nFlying vehicles\tfrom $60000\nPremium vehicles\tfrom $7500", "Choose", "Exit");
	return 1;
}

COMMAND:serial(playerid, params[])
{
	SendGuiInformation(playerid, "Serial", sprintf("%s", pInfo[playerid][player_serial]));
	return 1;
}

/*COMMAND:streamed(playerid, params[])
{
	SendClientMessage(playerid, -1, sprintf("Streamowane pojazdy: %d", Iter_Count(PlayerVehicles[playerid])));
	
	new Float:pobieramy, Float:pobieramy2, Float:pobieramy3;
	Streamer_SetFloatData(STREAMER_TYPE_PICKUP, 5, E_STREAMER_EXTRAX, Door[5][door_spawn_pos][0]);
	Streamer_SetFloatData(STREAMER_TYPE_PICKUP, 5, E_STREAMER_EXTRAY, Door[5][door_spawn_pos][1]);
	Streamer_SetFloatData(STREAMER_TYPE_PICKUP, 5, E_STREAMER_EXTRAZ, Door[5][door_spawn_pos][2]);

	Streamer_GetFloatData(STREAMER_TYPE_PICKUP, 5, E_STREAMER_EXTRAX, pobieramy);
	Streamer_GetFloatData(STREAMER_TYPE_PICKUP, 5, E_STREAMER_EXTRAY, pobieramy2);
	Streamer_GetFloatData(STREAMER_TYPE_PICKUP, 5, E_STREAMER_EXTRAZ, pobieramy3);
	
	SendClientMessage(playerid, -1, sprintf("Pozycja drzwi SAMPID 5: %0.2f, %0.2f, %0.2f", pobieramy, pobieramy2, pobieramy3));
	return 1;
}*/

COMMAND:mdc(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];
	if (gid == - 1) return SendGuiInformation(playerid, "Information", "You are not on group duty with access to MDC.");

	if(!IsPlayerInAnyVehicle(playerid))
	{
		if(GetPlayerVirtualWorld(playerid) == 0) return SendGuiInformation(playerid, "Information", "Access to MDC is only possible in PD vehicles.");
		return SendGuiInformation(playerid, "Information", "Access to MDC is only possible in PD vehicles.");
	} 

	if(!GroupHasFlag(gid, GROUP_FLAG_TAKE)) return SendGuiInformation(playerid, "Information", "You are not on group duty with access to MDC.");
	ShowPlayerDialog(playerid, DIALOG_MDC, DIALOG_STYLE_LIST, "Mobile Data Computer", "1. Find person\n2. DMV database\n3. Wanted list\n4. Add APB\n5. Remove APB", "Choose", "Exit");
	return 1;
}

COMMAND:damages(playerid, params[])
{
	new targetid;
	
	if( sscanf(params, "u", targetid) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /damages [player id]");
		return 1;
	}
	else
	{
		if(!IsPlayerConnected(targetid) || targetid==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawne ID gracza.");

		new Float:dist;
		dist = GetDistanceBetweenPlayers(playerid, targetid);
		if(dist > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

		new list_injuries[256], header[40], gunname[32], time;

		for(new i=0;i<10;i++)
		{
			if( pInjury[targetid][i][injury_id] > 0 )
			{
				time = (gettime() - pInjury[targetid][i][injury_time])/60;
				GetWeaponName(pInjury[targetid][i][injury_weapon], gunname, sizeof(gunname));

				format(list_injuries, sizeof(list_injuries), "%s%d dmg from %s in %s %dminutes ago\n", list_injuries, pInjury[targetid][i][injury_damage], gunname, GetBodyPartName(pInjury[targetid][i][injury_bodypart]), time);
			}
		}

		if(strlen(list_injuries) > 0)
		{
			format(header, sizeof(header), "Damages of %s", pInfo[targetid][player_name]);
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, header, list_injuries, "OK", "");
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This player has no damages.");
		}
	}
	return 1;
}

COMMAND:atm(playerid, params[])
{
	new object_id = GetClosestObjectType(playerid, OBJECT_ATM);

	if(object_id == INVALID_OBJECT_ID)
	{
		SendGuiInformation(playerid, "Information", "You are not near ATM.");
		return 1;
	}	

	ShowPlayerDialog(playerid, DIALOG_BANKOMAT ,DIALOG_STYLE_LIST, "ATM","1. Account state\n2. Deposit\n3. Withdraw", "Choose","Exit");
	ApplyAnimation(playerid, "PED", "ATM", 4.0, 0, 0, 0, 0, 0, true);
	return 1;
}

COMMAND:confiscate(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];
	if (gid == - 1) return SendGuiInformation(playerid, "Information", "You are not on group duty with flag to confiscating.");
	if(!GroupHasFlag(gid, GROUP_FLAG_TAKE)) return SendGuiInformation(playerid, "Information", "You are not on group duty with flag to confiscating.");

	new sub_cmd[20], rest[64], targetid;
	
	if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /confiscate [driverlic/item]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "driverlic") )
		{
			if(!GroupHasFlag(gid, GROUP_FLAG_911)) return SendGuiInformation(playerid, "Information", "You don't have privileges to confiscating driver licenses.");
			if(sscanf(rest, "d", targetid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /confiscate driverlic [player id]");
			if(!IsPlayerConnected(targetid)||targetid==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");

			if(GetDistanceBetweenPlayers(playerid, targetid) > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

			pInfo[targetid][player_documents] -= DOCUMENT_DRIVE;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_documents = %d WHERE char_uid = %d", pInfo[targetid][player_documents], pInfo[targetid][player_id]));

			SendGuiInformation(playerid, "Information", sprintf("You have confiscated %s driver license.", pInfo[targetid][player_name]));
			SendGuiInformation(targetid, "Information", sprintf("Player %s has confiscated your driver license.", pInfo[playerid][player_name]));
		}
		else if( !strcmp(sub_cmd, "item") )
		{
			new item, itemid;
			if(sscanf(rest, "dd", targetid, item)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /confiscate item [player id] [item uid]");
			if(!IsPlayerConnected(targetid)||targetid==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");

			if(GetDistanceBetweenPlayers(playerid, targetid) > 3.0) return SendGuiInformation(playerid, "Information", "Ten gracz znajduje siê zbyt daleko.");

			itemid = GetItemByTargetUid(item, targetid);
			if(itemid == -1) return SendGuiInformation(playerid, "Information", "Invalid item uid.");
			if(!Iter_Contains(PlayerItems[targetid], itemid)) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

			pInfo[playerid][player_capacity] += PlayerItem[targetid][itemid][player_item_weight];
			pInfo[targetid][player_capacity] -= PlayerItem[targetid][itemid][player_item_weight];

			if(PlayerItem[targetid][itemid][player_item_used])
			{
				SendGuiInformation(playerid, "Information", "This item is used.");
				return 1;
			}

			mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_owner = 195, item_ownertype = 6, item_count = 1 WHERE item_uid = %d", item));

			SendGuiInformation(playerid, "Information", sprintf("You have confiscated %s (UID %d) from %s.", PlayerItem[targetid][itemid][player_item_name], item, pInfo[targetid][player_name]));
			SendGuiInformation(targetid, "Information", sprintf("Player %s has confiscated your %s (UID %d).", pInfo[playerid][player_name], PlayerItem[targetid][itemid][player_item_name], item));

			DeleteItem(itemid, false, targetid);
			LoadItem(sprintf("WHERE `item_uid` = %d", item), true);
		}
	}
	return 1;
}

COMMAND:audio(playerid, params[])
{
	SendGuiInformation(playerid, "Information", "Audio stream off.");
    StopAudioStreamForPlayer(playerid);
    return 1;
}

COMMAND:engine(playerid, params[])
{
    new vid = GetPlayerVehicleID(playerid);
    if(vid == INVALID_VEHICLE_ID) return SendGuiInformation(playerid, "Information", "You are not in any vehicle.");
    if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return SendGuiInformation(playerid, "Information", "You are not a driver.");
    if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Information", "You have no access to this vehicle.");
    if( Vehicle[vid][vehicle_engine] )
    {
        new model = GetVehicleModel(vid);
        if(model == 509 || model == 510 || model == 481)
        {
            return 1;
        }

        // Gaszenie silnika
        if( CanPlayerUseVehicle(playerid, vid) ) TextDrawShowForPlayer(playerid, vehicleInfo);
        Vehicle[vid][vehicle_engine] = false;
        SaveVehicle(vid);
        UpdateVehicleVisuals(vid);
    }
    else
    {
        new model = GetVehicleModel(vid);
        if(model == 509 || model == 510 || model == 481)
        {
            return 1;
        }
        
        // Odpalanie silnika
        if( Vehicle[vid][vehicle_state] > 0 ) return SendGuiInformation(playerid, ""guiopis"Alert", "You can't use this vehicle when action on it is still in progress.");
        if( Vehicle[vid][vehicle_destroyed] == true)
		{
			RemovePlayerFromVehicle(playerid);
			SendGuiInformation(playerid, "Information", "Engine is totally destroyed.");
			return 1;
		}
        if( Vehicle[vid][vehicle_blocked] != 0) return SendGuiInformation(playerid, ""guiopis"Alert", sprintf("Vehicle has a blocked wheel. (Reason: %s, amount: $%d)", Vehicle[vid][vehicle_block_reason], Vehicle[vid][vehicle_blocked]));
		if( Vehicle[vid][vehicle_fuel_current] == 0.0 ) return SendGuiInformation(playerid, ""guiopis"Alert", "There is no fuel in this vehicle.");

        Vehicle[vid][vehicle_engine_starting] = true;

        defer VehicleEngineStart[2000](playerid, vid);

        GameTextForPlayer(playerid,"~n~~n~~n~~n~~n~~n~~n~~n~~n~~w~Starting ~y~engine~w~...",2000,3);
    }
    return 1;
}

COMMAND:race(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];
	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty with race access.");
	if(Group[gid][group_type] != GROUP_TYPE_RACERS) return SendGuiInformation(playerid, "Information", "You are not on group duty with race access.");

	new type[32], varchar[32], string[128];
	if(sscanf(params, "s[32]S()[32]", type, varchar))
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /race [create | invite | kick]");
	    return 1;
	}
	if(!strcmp(type, "create", true))
	{
	    if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
	    {
	        SendGuiInformation(playerid, "Information", "You are not in vehicle.");
	        return 1;
	    }
	    if(pInfo[playerid][player_race_phase])
	    {
	       	SendGuiInformation(playerid, "Information", "You are already in race creator.");
	        return 1;
	    }

	    foreach(new p: Player)
	    {
	        if(pInfo[p][player_logged])
	        {
	            if(pInfo[p][player_race_phase])
	            {
	               	SendGuiInformation(playerid, "Information", "Some is doing race right now. Wait for your turn.");
	                return 1;
	            }
	        }
	    }
	    format(string, sizeof(string), "~y~Race ~w~creator.~w~~n~~n~~y~~k~~VEHICLE_FIREWEAPON~ ~w~- setting checkpoint~n~~y~PPM ~w~- setting finish line~n~~n~Checkpoints: ~y~%d/%d", pInfo[playerid][player_race_point], MAX_RACE_CP);

		TextDrawSetString(Tutorial[playerid], string);
		TextDrawShowForPlayer(playerid, Tutorial[playerid]);

		pInfo[playerid][player_race_phase] = 1;
		pInfo[playerid][player_race_point] = 0;

		SendGuiInformation(playerid, "Information", "Started race creator.");
	    return 1;
	}
	if(!strcmp(type, "invite", true))
	{
	    if(pInfo[playerid][player_race_phase] != 2)
	    {
	        SendGuiInformation(playerid, "Information", "You can't invite players now. Finish creating first.");
	        return 1;
	    }
	    new giveplayer_id;
		if(sscanf(varchar, "d", giveplayer_id))
		{
		    SendClientMessage(playerid, COLOR_GREY, "Tip: /race invite [player ID]");
		    return 1;
		}
  		if(giveplayer_id == playerid)
    	{
     		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "You can't invite yourself.", "OK", "");
       		return 1;
     	}
		if(giveplayer_id == INVALID_PLAYER_ID)
		{
  			SendGuiInformation(playerid, "Information", "Invalid player id.");
   			return 1;
		}
		if(pInfo[giveplayer_id][player_race_phase])
		{
		    SendGuiInformation(playerid, "Information", "This player is already in race.");
		    return 1;
		}
		pInfo[giveplayer_id][player_race_phase] = 2;
		pInfo[giveplayer_id][player_race_point] = 0;

		pInfo[giveplayer_id][player_race_checkpoints] = pInfo[playerid][player_race_checkpoints];

		format(string, sizeof(string), "%s has been invited to the race.", pInfo[giveplayer_id][player_name]);
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", string, "OK", "");

		format(string, sizeof(string), "%s invited you to his race.", pInfo[playerid][player_name]);
		ShowPlayerDialog(giveplayer_id, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", string, "OK", "");
	    return 1;
	}
	if(!strcmp(type, "kick", true))
	{
 		if(pInfo[playerid][player_race_phase] != 2)
	    {
	        ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "You can't kick players now. Finish creating first.", "OK", "");
	        return 1;
	    }
	    new giveplayer_id;
		if(sscanf(varchar, "d", giveplayer_id))
		{
		    SendClientMessage(playerid, COLOR_GREY, "Tip: /race kick [player ID]");
		    return 1;
		}
  		if(giveplayer_id == playerid)
    	{
     		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "You can't kick yourself.", "OK", "");
       		return 1;
     	}
		if(giveplayer_id == INVALID_PLAYER_ID)
		{
  			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "Invalid player id.", "OK", "");
   			return 1;
		}
		if(!pInfo[giveplayer_id][player_logged])
		{
  			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "This player is not logged in.", "OK", "");
	    	return 1;
		}
		if(!pInfo[giveplayer_id][player_race_phase])
		{
		    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "This player is not in any race.", "OK", "");
		    return 1;
		}
		pInfo[giveplayer_id][player_race_phase] = 0;
		pInfo[giveplayer_id][player_race_point] = 0;

		pInfo[giveplayer_id][player_race_checkpoints] = 0;

		format(string, sizeof(string), "%s has been kicked from your race.", pInfo[giveplayer_id][player_name]);
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", string, "OK", "");

		format(string, sizeof(string), "%s has kicked you from his race.", pInfo[playerid][player_name]);
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", string, "OK", "");
	    return 1;
	}
	if(!strcmp(type, "start", true))
	{
	    if(pInfo[playerid][player_race_phase] != 2)
	    {
	        ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "You can't start race right now. Finish creation first.", "OK", "");
	        return 1;
	    }

	    foreach(new p: Player)
	    {
	        if(pInfo[p][player_logged])
	        {
	            if(pInfo[p][player_race_phase] == 2)
	            {
	               	pInfo[p][player_race_count] = 4;
	    			pInfo[p][player_race_phase] = 3;
				    pInfo[p][player_race_point] = 0;
				    TogglePlayerControllable(p, 0);

 					SetPlayerRaceCheckpoint(p, 0, RaceCheckpoint[0][0], RaceCheckpoint[0][1], RaceCheckpoint[0][2], RaceCheckpoint[1][0], RaceCheckpoint[1][1], RaceCheckpoint[1][2], 5.0);
				}
			}
		}
		return 1;
	}
	return 1;
}

COMMAND:tie(playerid, params[])
{
    new target;
    if(sscanf(params, "d", target))
    {
        SendClientMessage(playerid, COLOR_GREY, "Tip: /tie [player id]");
        return 1;
    }
 
    if( !pInfo[target][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
    if( playerid == target) return SendGuiInformation(playerid, "Information", "Invalid player id.");
 
    if(GetPlayerUsedItem(playerid, ITEM_TYPE_ROPE) == -1) return SendGuiInformation(playerid, "Information", "You don't have rope.");
 
    new Float:dist;
    dist = GetDistanceBetweenPlayers(playerid, target);
 
    if(dist>3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
 
    if(GetPlayerSpecialAction(target) == SPECIAL_ACTION_CUFFED)
    {
        SetPlayerSpecialAction(target, SPECIAL_ACTION_NONE);
        SendGuiInformation(playerid, "Information", "Player has been untied.");
    }
    else
    {
        SetPlayerSpecialAction(target, SPECIAL_ACTION_CUFFED);
        SendGuiInformation(playerid, "Information", "Player has been tied.");
    }
 
    return 1;
}

COMMAND:as(playerid, params[])
{
	return ClearAnimations(playerid);
}

COMMAND:r(playerid, params[])
{
	new slot, text[128];
	if(sscanf(params, "ds[128]", slot, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /r [slot] [text]");
	if(slot > 5 || slot < 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /r [slot] [text]");

	SendGroupIC(playerid, slot, text);
	return 1;
}

COMMAND:leader(playerid, params[])
{
	new sub_cmd[40], param[150];
	new slot = GetPlayerDutySlot(playerid);
	if(slot == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");
	if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return SendGuiInformation(playerid, "Information", "You are not a leader of group you are on duty.");
	new gid = pInfo[playerid][player_duty_gid];
	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");

	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /leader [price/tag]");

	if( !strcmp(sub_cmd, "price") )
	{	
		new price, id;
		if( sscanf(param, "dd", id, price) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /leader price [product uid] [price]");

		if(gid == -1) return SendGuiInformation(playerid, "Information", "This product doesnt belongs to your group duty.");
		new prod_id = GetGroupItemByUid(id);
		if(prod_id == -1) return SendGuiInformation(playerid, "Information", "Invalid product uid.");
		if(Item[prod_id][item_owner_type] != ITEM_OWNER_TYPE_GROUP) return SendGuiInformation(playerid, "Information", "Invalid product uid.");
		if(Item[prod_id][item_owner] != Group[gid][group_uid]) return SendGuiInformation(playerid, "Information", "This product doesnt belongs to your group duty.");
		if(!WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return SendGuiInformation(playerid, "Information", "You are not a leader of group you are on duty.");
		if(price < 0 ) return SendGuiInformation(playerid, "Information", "Invalid price.");
		Item[prod_id][item_price] = price;

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_price` = '%d' WHERE `item_uid` = %d", price, Item[prod_id][item_uid]));
		SendGuiInformation(playerid, "Information", "Price has been changed.");
	}
	else if( !strcmp(sub_cmd, "tag") )
	{
		new tag[6];
		if( sscanf(param, "s[6]", tag) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /leader tag [group tag (max 6 chars)]");
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_game_groups` SET `group_tag` = '%s' WHERE `group_uid` = %d", tag, Group[gid][group_uid]));
		SendGuiInformation(playerid, "Information", "Tag has been changed.");
	}
	else if( !strcmp(sub_cmd, "color") )
	{
		new color[16];
		if( sscanf(param, "s[16]",  color) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /leader color [HEX]");
		if(!ishex(color))  return SendClientMessage(playerid, COLOR_GREY, "Tip: /leader color [HEX]");
		
		format(color, sizeof(color), "0x%sDD", color);
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_game_groups` SET `group_color` = '%d' WHERE `group_uid` = %d", hexstring(color), Group[gid][group_uid]));
		SendGuiInformation(playerid, "Information", "Group color has been changed.");
	}
	return 1;
}

COMMAND:ro(playerid, params[])
{
	new slot, text[128];
	if(sscanf(params, "ds[128]", slot, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ro [slot] [text]");
	if(slot > 5 || slot < 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ro [slot] [text]");
	SendGroupOOC(playerid, slot, text);
	return 1;
}

COMMAND:show(playerid, params[])
{
	new id, text[64];
	if(sscanf(params, "s[64]d", text, id)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /show [id/driverlic/badge/license] [player id]");
	if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	if( !pInfo[id][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	new Float:dist;
	dist = GetDistanceBetweenPlayers(playerid, id);
	if(dist>3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

	if( !strcmp(text, "driverlic") )
	{	
		if(!(pInfo[playerid][player_documents] & DOCUMENT_DRIVE)) return SendGuiInformation(playerid, "Information", "You don't have driver license.");
		SendGuiInformation(id, "Driver license", sprintf("Name: %s\nBirth: %d", pInfo[playerid][player_name], pInfo[playerid][player_age]));
		ProxMessage(playerid, sprintf("shows his driver license to %s.", pInfo[id][player_name]), PROX_SERWERME);
	}
	else if( !strcmp(text, "id") )
	{	
		if(!(pInfo[playerid][player_documents] & DOCUMENT_ID)) return SendGuiInformation(playerid, "Information", "You don't have ID.");
		SendGuiInformation(id, "ID Card", sprintf("Name: %s\nBirth: %d", pInfo[playerid][player_name], pInfo[playerid][player_age]));
		ProxMessage(playerid, sprintf("shows his id to %s.", pInfo[id][player_name]), PROX_SERWERME);
	}
	else if( !strcmp(text, "badge") )
	{	
		new gid = GetGroupByUid(2);
		if(pInfo[playerid][player_duty_gid] != gid ) return SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
		SendGuiInformation(id, "Police badge", sprintf("Name: %s\nBirth: %d", pInfo[playerid][player_name], pInfo[playerid][player_age]));
		ProxMessage(playerid, sprintf("shows his badge to %s.", pInfo[id][player_name]), PROX_SERWERME);
	}
	else if( !strcmp(text, "license") )
	{	
		if(!(pInfo[playerid][player_documents] & DOCUMENT_WEAPON)) return SendGuiInformation(playerid, "Information", "You don't have weapon license.");
		SendGuiInformation(id, "Weapon license", sprintf("Name: %s\nBirth: %d", pInfo[playerid][player_name], pInfo[playerid][player_age]));
		ProxMessage(playerid, sprintf("shows his weapon license to %s.", pInfo[id][player_name]), PROX_SERWERME);
	}
	return 1;
}

/*COMMAND:truck(playerid, params[])
{
	if(!IsPlayerInRangeOfPoint(playerid, 10.0, -74.2711,-1131.7954,1.6705))
	{
		SendGuiInformation(playerid, "Information", "You are not in truckers base.");
		SetPlayerMapIcon(playerid, 13, -74.2711,-1131.7954,1.6705, 0, 0xFF0000AA, MAPICON_GLOBAL);
		return 1;
	}
	if(pInfo[playerid][player_job] == WORK_TYPE_TRUCKER) 
	{
		if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			new vehicleid = GetPlayerVehicleID(playerid);
			if(!IsTrailerAttachedToVehicle(vehicleid))
			{
			   SendGuiInformation(playerid, "Information", "This vehicle has no trailer.");
			   return 1;
			}
			pInfo[playerid][player_truck] = 1;
			SendPlayerInformation(playerid, "Delivery started, go to the red checkpoint.", 8000);
			new randomss;
	        randomss = random(sizeof(TruckPoints));
	        SetPlayerCheckpoint(playerid, TruckPoints[randomss][0], TruckPoints[randomss][1], TruckPoints[randomss][2], 5.0);
		}
		else
		{
			SendGuiInformation(playerid, "Information", "You are not in any vehicle.");
		}
	}
	return 1;
}*/

COMMAND:siren(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];
	if(gid == -1)
	{
		SendGuiInformation(playerid, "Information", "You are not on group duty.");
		return 1;
	}
    if(!(Group[gid][group_flags] & GROUP_FLAG_SEARCH))
    {
	  	SendGuiInformation(playerid, "Information", "You are not on group duty with search flag.");
    	return 1;
    }
    new vid = GetPlayerVehicleID(playerid);
    if(!IsPlayerInAnyVehicle(playerid)) return SendGuiInformation(playerid, "Information", "You are not in any vehicle.");
    if(Vehicle[vid][vehicle_owner] != Group[gid][group_uid]) return SendGuiInformation(playerid, "Information", "This vehicle is not your group duty car. Check your duty status.");

    if(Vehicle[vid][vehicle_siren] == false)
    {
		Vehicle[vid][vehicle_siren_object] = CreateDynamicObject(18646, 10.0, 10.0, 10.0, 0, 0, 0);
	 	AttachDynamicObjectToVehicle(Vehicle[vid][vehicle_siren_object], vid, -0.0, -0.39, attInfo[GetVehicleModel(vid) - 400][_attPos], 0.0, 0.0, 0.0);
	 	Vehicle[vid][vehicle_siren] = true;
	}
	else
	{
		DestroyDynamicObject(Vehicle[vid][vehicle_siren_object]);
		Vehicle[vid][vehicle_siren] = false;
	}
 	return 1;
}

COMMAND:search(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];

	if(gid == -1)
	{
		SendGuiInformation(playerid, "Information", "You are not on group duty.");
		return 1;
	}
	
    if( !GroupHasFlag(gid, GROUP_FLAG_SEARCH) )
    {
	  	SendGuiInformation(playerid, "Information", "You are not on group duty with search flag.");
    	return 1;
    }
    else
    {
    	new id;
    	if(sscanf(params, "d", id)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /search [player id]");
    	if( (!IsPlayerConnected(id) || !pInfo[id][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");

    	new Float:dist;
		dist = GetDistanceBetweenPlayers(playerid, id);

		if(dist>3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

		new count, string[1024];

		ProxMessage(playerid, sprintf("searches %s.", pInfo[id][player_name]), PROX_ME);
		
		DynamicGui_Init(playerid);
		
		format(string, sizeof(string), "%s{C0C0C0}Items in player inventory: (%d$)\n", string, pInfo[id][player_money]);
		DynamicGui_AddBlankRow(playerid);
		
		foreach (new item : PlayerItems[id])
		{
			new prefix[10]; 
			if( PlayerItem[id][item][player_item_used] ) prefix = "{CFFD66}";
			
			format(string, sizeof(string), "%s %s%s\t\t%d %d  (%d:%d)\n", string, prefix, PlayerItem[id][item][player_item_name], PlayerItem[id][item][player_item_uid], PlayerItem[id][item][player_item_type], PlayerItem[id][item][player_item_value1], PlayerItem[id][item][player_item_value2]);
			DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, item);	
			count++;
		}
		
		if( count == 0 ) SendGuiInformation(playerid, "Information", "This player has no items.");
		else ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, "Player's inventory", string, "OK", "");

    }
	return 1;
}

COMMAND:news(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];
    new text[170], str[250];
    if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");

    if( Group[gid][group_type] == GROUP_TYPE_SN )
    {
		if(sscanf(params, "s[150]", text))
		{
			TextDrawSetString(TextDrawSanNews, "~r~~h~~h~Weazel ~>~ ~w~There is no broadcast at this moment.");
		 	SendClientMessage(playerid, COLOR_GREY, "Tip: /news [text]");
		 	return 1;
		}
		
		if(!CheckTextDrawString(text)) return SendGuiInformation(playerid, "Information", "In this message you are not able to place 2 of ~ 1 after 1.");

		EscapePL(text);
		format(str, sizeof(str), "~r~~h~~h~Weazel~w~ (~p~LIVE~w~ - %s): %s", pInfo[playerid][player_name], text);
		TextDrawSetString(TextDrawSanNews, str);
		
    } 
    else 
    {
    	SendGuiInformation(playerid, "Information", "You are not on group duty with access to this command.");
    }
	return 1;
}

COMMAND:advertise(playerid, params[])
{
	new gid = pInfo[playerid][player_duty_gid];
    new text[150], str[200];
    if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");

    if( Group[gid][group_type] == GROUP_TYPE_SN )
    {
		if(sscanf(params, "s[150]", text))
		{
			TextDrawSetString(TextDrawSanNews, "~r~~h~~h~Weazel ~>~ ~w~There is no broadcast at this moment.");
		 	SendClientMessage(playerid, COLOR_GREY, "Tip: /advertise [text]");
		 	return 1;			
		}
		if(!CheckTextDrawString(text)) return SendGuiInformation(playerid, "Information", "In this message you are not able to place 2 of ~ 1 after 1.");
		
		EscapePL(text);
		format(str, sizeof(str), "~r~~h~~h~Weazel~w~ (~p~Advertise~w~): %s", text);
		TextDrawSetString(TextDrawSanNews, str);
    } 
    else
    {
    	SendGuiInformation(playerid, "Information", "You are not on group duty with access to this command.");
    }
	return 1;
}

COMMAND:id(playerid, params[])
{
	new nick[24], list_players[256];
    if(sscanf(params, "s[24]", nick))
	{
        SendClientMessage(playerid, COLOR_GREY, "Tip: /id [nick]");
		return 1;
    }
	if(strlen(nick) < 3)
	{
		SendGuiInformation(playerid, "Information", "You need to type at least 3 letters.");
		return 1;
	}

	foreach(new i: Player)
   	{
   		if(pInfo[i][player_admin_duty]) continue;

   	    new name[24];
		GetPlayerName(i, name, sizeof(name));
        if(strfind(name, nick, true) >= 0)
		{
			if( GetPlayerUsedItem(i, ITEM_TYPE_MASKA) > -1 ) format(list_players, sizeof(list_players), "%s\n%s", list_players, pInfo[i][player_name]);
			else format(list_players, sizeof(list_players), "%s\n%d\t%s", list_players, i, pInfo[i][player_name]);
		}
    }
    
	if(strlen(list_players))
	{
	    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, "Search results:", list_players, "OK", "");
	}
	else
	{
	    SendGuiInformation(playerid, "Information", "No results for this name.");
	}
    return 1;
}

COMMAND:ame(playerid, params[])
{
	new text[128], str[174];
	if( sscanf(params, "s[128]", text) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /ame [text]");
		return 1;
	}
	else
	{
		format(str, sizeof(str), "* %s %s", pInfo[playerid][player_name], text);
		SetPlayerChatBubble(playerid, str, 0xBAA9CDFF, 7.0, 6000);
		SendClientMessage(playerid, 0xB58ADAFF, str);
	}
	return 1;
}

COMMAND:exit(playerid, params[])
{
	new ds_id = -1;

	foreach(new d : Doors)
	{
		if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_spawn_pos][0],  Door[d][door_spawn_pos][1], Door[d][door_spawn_pos][2]) && GetPlayerVirtualWorld(playerid) == Door[d][door_spawn_vw])
		{
			ds_id = d;
		}
	}
	
	if( ds_id != -1 )
	{
		if( Door[ds_id][door_closed] ) return SendClientMessage(playerid, COLOR_GREY, "These doors are locked.");
		
		FreezePlayer(playerid, 2500);
		
		RP_PLUS_SetPlayerPos(playerid, Door[ds_id][door_pos][0], Door[ds_id][door_pos][1], Door[ds_id][door_pos][2]);
		SetPlayerFacingAngle(playerid, Door[ds_id][door_pos][3]);
		
		SetCameraBehindPlayer(playerid);
		
		SetPlayerVirtualWorld(playerid, Door[ds_id][door_vw]);
		SetPlayerInterior(playerid, Door[ds_id][door_int]);
	}
	return 1;
}

COMMAND:enter(playerid, params[])
{
	new d_id = -1;

	foreach(new d : Doors)
	{
		if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]) && GetPlayerVirtualWorld(playerid) == Door[d][door_vw])
		{
			d_id = d;
		}
	}
	
	if( d_id != -1 )
	{

		if( Door[d_id][door_closed] ) return SendClientMessage(playerid, COLOR_GREY, "Te drzwi s¹ zamkniête.");
		
		if( Door[d_id][door_payment] > 0 )
		{
			if( Door[d_id][door_payment] > pInfo[playerid][player_money] ) return SendClientMessage(playerid, COLOR_GREY, "You don't have enough money to pay entrance fee.");
			
			GivePlayerMoney(playerid, -Door[d_id][door_payment]);
			new gid = Door[d_id][door_owner];
			if(gid == -1) return 1;
			Group[gid][group_bank_money] += Door[d_id][door_payment];
		}
		
		FreezePlayer(playerid, 2500);
		
		RP_PLUS_SetPlayerPos(playerid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
		SetPlayerFacingAngle(playerid, Door[d_id][door_spawn_pos][3]);
		
		SetCameraBehindPlayer(playerid);
		
		SetPlayerVirtualWorld(playerid, Door[d_id][door_spawn_vw]);
		SetPlayerInterior(playerid, Door[d_id][door_spawn_int]);
	}
	return 1;
}

COMMAND:d(playerid, params[]) 
{
	new text[128];
	new gid = pInfo[playerid][player_duty_gid];

	if(gid == -1)
	{
		SendGuiInformation(playerid, "Information", "To be able to use this chat, you have to be on faction duty.");
		return 1;
	}

	if(!(Group[gid][group_flags] & GROUP_FLAG_DEP))
	{
		SendGuiInformation(playerid, "Information", "To be able to use this chat, you have to be on faction duty.");
		return 1;
	}

	if( sscanf(params, "s[128]", text) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /d [text]");
		return 1;
	}
	else
	{
		new tag[15];
		tag = GetGroupTag(pInfo[playerid][player_duty_gid]);

		foreach(new pid : Player)
		{
			if(pInfo[pid][player_duty_gid] > 0)
			{
				if(Group[pInfo[pid][player_duty_gid]][group_flags] & GROUP_FLAG_DEP)
				{
					SendClientMessage(pid, COLOR_LIGHTER_RED, sprintf("[%s] %s (dep. radio): %s", tag, pInfo[playerid][player_name], text));
				}
			}
		}
		ProxMessage(playerid, text, PROX_RADIO);
	}
	return 1;
}

COMMAND:house(playerid, params[])
{
	new sub_cmd[20], rest[64], id, price;
	if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /house [invite, kick, sell]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "invite") )
		{	
			if(sscanf(rest, "d", id)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /house invite [player id]");
			if( !pInfo[id][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");

			new d_id = -1;

			foreach(new d : Doors)
			{
				if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
				{
					d_id = d;
				}
				else if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_spawn_pos][0],  Door[d][door_spawn_pos][1], Door[d][door_spawn_pos][2]) && GetPlayerVirtualWorld(playerid) == Door[d][door_spawn_vw])
				{
					d_id = d;
				}
			}
			
			if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not near any doors.");
			

			if( !CanPlayerEditDoor(playerid, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "This building is not belong to you.");

			pInfo[id][player_spawn] = Door[d_id][door_uid];
			pInfo[id][player_spawn_type] = SPAWN_TYPE_HOUSE;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_spawn = %d, char_spawn_type = %d WHERE char_uid = %d", pInfo[id][player_spawn], pInfo[id][player_spawn_type], pInfo[id][player_id]));
			SendGuiInformation(id, "Information",sprintf("%s has invited you to his house.", pInfo[playerid][player_name]));
			SendGuiInformation(playerid, "Information", "Spawn has been set up.");
		}
		else if( !strcmp(sub_cmd, "spawn") )
		{
			return cmd_house(playerid, sprintf("invite %d", playerid));
		}
		else if( !strcmp(sub_cmd, "kick") )
		{		
			if(sscanf(rest, "d", id)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /house kick [player id]");
			if( !pInfo[id][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");

			new d_id = -1;

			foreach(new d : Doors)
			{
				if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
				{
					d_id = d;
				}
				else if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_spawn_pos][0],  Door[d][door_spawn_pos][1], Door[d][door_spawn_pos][2]) && GetPlayerVirtualWorld(playerid) == Door[d][door_spawn_vw])
				{
					d_id = d;
				}
			}
			
			if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not near any doors.");

			if( !CanPlayerEditDoor(playerid, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "This building is not belong to you.");

			pInfo[id][player_spawn] = 0;
			pInfo[id][player_spawn_type] = SPAWN_TYPE_GLOBAL;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_spawn = %d, char_spawn_type = %d WHERE char_uid = %d", pInfo[id][player_spawn], pInfo[id][player_spawn_type], pInfo[id][player_id]));

			SendGuiInformation(playerid, "Information", "Player has been kicked from your house.");
			SendPlayerInformation(id, sprintf("~w~Player ~p~%s~w~ kicked you from his house.", pInfo[playerid][player_name]), 6000);
		}
		else if( !strcmp(sub_cmd, "sell") )
		{		
			if(sscanf(rest, "dd", id, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /house sell [player id, price]");
			if( !pInfo[id][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");

			new d_id = -1;

			foreach(new d : Doors)
			{
				if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
				{
					d_id = d;
				}
				else if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_spawn_pos][0],  Door[d][door_spawn_pos][1], Door[d][door_spawn_pos][2]))
				{
					d_id = d;
				}
			}
			
			if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not near any doors.");

			if(Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_PLAYER) return SendClientMessage(playerid, COLOR_GREY, "These doors are not private.");

			if( !CanPlayerEditDoor(playerid, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "This building is not belong to you.");

			if(price < 0) return SendGuiInformation(playerid, "Information", "Invalid price.");

			new resp = SetOffer(playerid, id, OFFER_TYPE_DOOR, price, d_id);
		    if( resp ) ShowPlayerOffer(id, playerid, "Door", sprintf("Door: %s", Door[pOffer[id][offer_extraid]][door_name]), price);
		}
	}
	return 1;
}

COMMAND:bank(playerid, params[])
{
	if(IsPlayerInBank(playerid))
	{
		if(pInfo[playerid][player_bank_number] == 0)
		{
			new numer[32];
			format(numer, sizeof(numer), "%c%c%c%c%d", randomNumber(), randomNumber(), randomNumber(), randomNumber(), pInfo[playerid][player_id]);
			new nr = strval(numer);
	        
	        pInfo[playerid][player_bank_number] = nr;

			SendGuiInformation(playerid, "Maze Bank", sprintf("It's your first visit in bank.\nYour account number is now %d.", nr));
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_banknumb = %d WHERE char_uid = %d", nr, pInfo[playerid][player_id]));
			return 1;
		}

		ShowPlayerDialog(playerid, DIALOG_BANKOMAT ,DIALOG_STYLE_LIST, "Maze Bank","1. Account details\n2. Deposit\n3. Withdraw\n4. Bank transfer\n5. Get payday (groups)\n6. Get payday (extra job)","Choose","Exit");
	}
	else
	{
		SendGuiInformation(playerid, "Information", "You are not in bank.");
	}
	return 1;
}

COMMAND:hotel(playerid, params[])
{
	new sub_cmd[20], rest[64], nr;
	if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /hotel [rent, unrent, enter, exit, invite, kick]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "rent") )
		{
			if(IsPlayerInMotel(playerid))
			{
				if(pInfo[playerid][player_money]<50)
				{
					SendGuiInformation(playerid, "Information", "You don't have enough money to rent a room. It costs $50.");
					return 1;
				}
				pInfo[playerid][player_spawn] = GetPlayerVirtualWorld(playerid);
				pInfo[playerid][player_spawn_type] = SPAWN_TYPE_HOTEL;
				
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_spawn = %d, char_spawn_type = %d WHERE char_uid = %d", pInfo[playerid][player_spawn], pInfo[playerid][player_spawn_type], pInfo[playerid][player_id]));
				
				GivePlayerMoney(playerid, -50);
				SendGuiInformation(playerid, "Information", sprintf("You've rented a room. Your room number is %d.\n$50 has been paid from your cash.", pInfo[playerid][player_id]));
			}
			else
			{
				SendGuiInformation(playerid, "Information", "You are not in hotel/motel.");
			}
		}
		else if( !strcmp(sub_cmd, "unrent") )
		{
			if(IsPlayerInMotel(playerid))
			{
				pInfo[playerid][player_spawn] = 0;
				pInfo[playerid][player_spawn_type] = SPAWN_TYPE_GLOBAL;
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_spawn = 0, char_spawn_type = 1 WHERE char_uid = %d", pInfo[playerid][player_id]));
				SendGuiInformation(playerid, "Information", "You left your hotel room. Now you will be spawned at global spawn.");
			}
			else
			{
				SendGuiInformation(playerid, "Information", "You are not in hotel/motel.");
			}
		}
		else if( !strcmp(sub_cmd, "enter") )
		{
			if(IsPlayerInMotel(playerid))
			{
				if(sscanf(rest, "d", nr)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /hotel enter [room number]");
				if(nr != pInfo[playerid][player_id]) return SendGuiInformation(playerid, "Information", "Wrong room number.");

				RP_PLUS_SetPlayerPos(playerid, 266.6632, 305.1765, 999.1484);
				SetPlayerInterior(playerid, 2);
				SetPlayerVirtualWorld(playerid, pInfo[playerid][player_id]);
			}
			else
			{
				SendGuiInformation(playerid, "Information", "You are not in hotel/motel.");
			}
		}
		else if( !strcmp(sub_cmd, "exit") )
		{
			if(GetPlayerVirtualWorld(playerid) != pInfo[playerid][player_id]) return SendGuiInformation(playerid, "Information", "You are not in hotel room.");

			new vw = pInfo[playerid][player_spawn];
			new d_id = GetDoorByUid(vw);
			if(d_id == -1) return SendGuiInformation(playerid, "Information", "[Society] Spawn bug (cmd hotel). Someone deleted hotel door. Ask administration for help.");

			if(IsPlayerInRangeOfPoint(playerid, 15.0, 266.6632, 305.1765, 999.1484) && GetPlayerInterior(playerid) == 2)
			{
				SetPlayerPos(playerid, Door[d_id][door_spawn_pos][0],Door[d_id][door_spawn_pos][1],Door[d_id][door_spawn_pos][2]);
				SetPlayerInterior(playerid, Door[d_id][door_spawn_int]);
				SetPlayerVirtualWorld(playerid, vw);
			}
		}
		else if( !strcmp(sub_cmd, "invite") )
		{
			if(GetPlayerVirtualWorld(playerid) != pInfo[playerid][player_id]) return SendGuiInformation(playerid, "Information", "You are not in hotel room.");
			new targetid;
			if(sscanf(rest, "d", targetid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /hotel invite [player id]");
			if(!IsPlayerConnected(targetid)) return SendGuiInformation(playerid, "Information", "This player is not at this hotel.");
			if(GetPlayerVirtualWorld(targetid) != pInfo[playerid][player_spawn]) return SendGuiInformation(playerid, "Information", "This player is not at this hotel.");

			SetPlayerPos(targetid, 266.6632, 305.1765, 999.1484);
			SetPlayerVirtualWorld(targetid, pInfo[playerid][player_id]);
			SetPlayerInterior(targetid, 2);
			SendGuiInformation(playerid, "Information", "Player has been invited to your hotel room.");
			SendGuiInformation(targetid, "Information", sprintf("Player %s has invited you to his hotel room.", pInfo[playerid][player_name]));
		}
		else if( !strcmp(sub_cmd, "kick") )
		{
			if(GetPlayerVirtualWorld(playerid) != pInfo[playerid][player_id]) return SendGuiInformation(playerid, "Information", "You are not in hotel room.");
			new targetid;
			if(sscanf(rest, "d", targetid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /hotel kick [player id]");
			if(!IsPlayerConnected(targetid)) return SendGuiInformation(playerid, "Information", "This player is not in your hotel room.");
			if(GetPlayerVirtualWorld(targetid) != pInfo[playerid][player_id]) return SendGuiInformation(playerid, "Information", "This player is not in your hotel room.");

			new vw = pInfo[playerid][player_spawn];
			new d_id = GetDoorByUid(vw);
			if(d_id == -1) return SendGuiInformation(playerid, "Information", "[Society] Spawn bug (cmd hotel). Someone deleted hotel door. Ask administration for help.");

			if(IsPlayerInRangeOfPoint(targetid, 15.0, 266.6632, 305.1765, 999.1484) && GetPlayerInterior(targetid) == 2)
			{
				SetPlayerPos(targetid, Door[d_id][door_spawn_pos][0],Door[d_id][door_spawn_pos][1],Door[d_id][door_spawn_pos][2]);
				SetPlayerInterior(targetid, Door[d_id][door_spawn_int]);
				SetPlayerVirtualWorld(targetid, vw);
			}

			SendGuiInformation(playerid, "Information", "Player has been kicked from your hotel room.");
			SendGuiInformation(targetid, "Information", sprintf("Player %s has kicked you from his hotel room.", pInfo[playerid][player_name]));
		}
	}
	return 1;
}

COMMAND:mgate(playerid, params[])
{
    if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Information", "You aren't editing any object.");

    new o_id = pInfo[playerid][player_edited_object];
 
    SendGuiInformation(playerid, "Information", "Object transfered to gate.");
 
    new uid = Object[o_id][object_uid];
 
    mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_gates` (gate_uid, gate_object, gate_posx, gate_posy, gate_posz, gate_rotx, gate_roty, gate_rotz) VALUES (null, %d, '%f', '%f', '%f', '%f', '%f', '%f')", uid, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]));
 
    new g_uid = cache_insert_id();
 
    Object[o_id][object_gate]=g_uid;
 
    mysql_query(mySQLconnection, sprintf("UPDATE `ipb_objects` SET `object_gate`= %d  WHERE `object_uid` = %d", g_uid, uid));
 
    new gate_id = LoadGate(sprintf("WHERE `gate_object` = %d", uid), true);
    Iter_Add(Gates, gate_id);
 
    CancelEdit(playerid);
    Object[o_id][object_is_edited] = false;
    pInfo[playerid][player_edited_object] = -1;
    TextDrawHideForPlayer(playerid, Dashboard[playerid]);
 
    RefreshPlayer(playerid);
 
    DeleteObject(o_id, false);
    LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
 
    return 1;
}
 
COMMAND:gate(playerid, params[])
{
    new g_id = -1;
 	
	foreach(new g : Gates)
    {
        if(IsPlayerInRangeOfPoint(playerid, 4.0, Gate[g][gate_pos][0], Gate[g][gate_pos][1], Gate[g][gate_pos][2]) && GetPlayerVirtualWorld(playerid) == Object[Gate[g][gate_object_id]][object_vw])
        {
            g_id = g;
        }
    }
	
    if(g_id == -1) return SendGuiInformation(playerid, "Information", "You are not near any gate.");

    new o_id = Gate[g_id][gate_object_id];

    if(o_id <= 0) return SendGuiInformation(playerid, "Information", "You are not near any gate.");
 
    if(CanPlayerUseObject(playerid, o_id))
    {
        if(Gate[g_id][gate_opened] == false)
        {
            MoveDynamicObject(o_id, Gate[g_id][gate_pos][0], Gate[g_id][gate_pos][1], Gate[g_id][gate_pos][2], 2.0, Gate[g_id][gate_pos][3], Gate[g_id][gate_pos][4], Gate[g_id][gate_pos][5]);
            GameTextForPlayer(playerid, "~w~Gate ~y~opened", 2500, 3);
            Gate[g_id][gate_opened] = true;
        }
        else
        {
            MoveDynamicObject(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], 2.0, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
            GameTextForPlayer(playerid, "~w~Gate ~y~closed", 2500, 3);
            Gate[g_id][gate_opened] = false;
        }
    }
    else
    {
        SendGuiInformation(playerid, "Information", "You can't use this gate.");
    }
    return 1;
}

COMMAND:garage(playerid, params[])
{
	if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return SendGuiInformation(playerid, "Information", "You are not in any vehicle.");
	new d_id = -1;

	foreach(new d : Doors)
	{
		if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
		{
			d_id = d;
		}
		else if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_spawn_pos][0],  Door[d][door_spawn_pos][1], Door[d][door_spawn_pos][2]))
		{
			d_id = d;
		}
	}
	
	if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not near any doors.");

	if(Door[d_id][door_car_crosing] == false) return SendGuiInformation(playerid, "Information", "These doors has not enabled car passing.");

	if( Door[d_id][door_closed] )
	{
		SendGuiInformation(playerid, "Information", "You can't enter locked building.");
		return 1;
	}
	else
	{
		if(Door[d_id][door_payment]>pInfo[playerid][player_money]) return SendGuiInformation(playerid, "Information", "You don't have enough money to pay entrance fee.");
		FreezePlayer(playerid, 2500);

		if(GetPlayerVirtualWorld(playerid)== Door[d_id][door_vw])
		{
			new vid = GetPlayerVehicleID(playerid);
			FreezePlayer(playerid, 3000);

			SetVehicleZAngle(vid, Door[d_id][door_spawn_pos][3]);
			SetVehicleVirtualWorld(vid, Door[d_id][door_spawn_vw]);
			SetVehiclePos(vid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
			LinkVehicleToInterior(vid, Door[d_id][door_spawn_int]);

			foreach(new p: Player)
			{
				if(GetPlayerVehicleID(p) == vid)
				{
					SetPlayerVirtualWorld(p, Door[d_id][door_spawn_vw]);
					SetPlayerInterior(p, Door[d_id][door_spawn_int]);
					//PutPlayerInVehicle(playerid, vid, 0);

					//RP_PLUS_SetPlayerPos(playerid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
				}
			}

			if(Door[d_id][door_payment]>0)
			{
				GivePlayerMoney(playerid,-Door[d_id][door_payment]);
			}
		}
		else if(GetPlayerVirtualWorld(playerid) == Door[d_id][door_spawn_vw])
		{
			new vid = GetPlayerVehicleID(playerid);

			SetVehicleZAngle(vid, Door[d_id][door_pos][3]);
			SetVehicleVirtualWorld(vid, Door[d_id][door_vw]);
			SetVehiclePos(vid, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
			LinkVehicleToInterior(vid, Door[d_id][door_int]);

			foreach(new p: Player)
			{
				if(GetPlayerVehicleID(p) == vid)
				{
					//RP_PLUS_SetPlayerPos(p, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
					SetPlayerVirtualWorld(p, Door[d_id][door_vw]);
					SetPlayerInterior(p, Door[d_id][door_int]);
					//PutPlayerInVehicle(playerid, vid, 0);
				}
			}

			new slot = GetPlayerDutySlot(playerid);

			if(slot != -1)
			{
				new grid = pInfo[playerid][player_duty_gid];
				if( GroupHasFlag(grid, GROUP_FLAG_DUTY) )
				{
					cmd_g(playerid, sprintf("%d duty", slot+1));
				}
			}
		}
	}
	return 1;
}

COMMAND:time(playerid, params[])
{
	return cmd_watch(playerid,params);
}

COMMAND:watch(playerid, params[])
{
	new item = HasPlayerItem(ITEM_TYPE_WATCH, playerid);
	if (item == -1 ) return SendGuiInformation(playerid, "Information", "You don't have watch.");
	new str[64];
	new h,m,s;	gettime(h,m,s);
	ProxMessage(playerid, sprintf("looks at his %s.", PlayerItem[playerid][item][player_item_name]), PROX_SERWERME);
	ApplyAnimation(playerid,"COP_AMBIENT","Coplook_watch",4.1,0,0,0,0,0,1);
	format(str, sizeof(str), "~w~Time: ~y~%02d~w~:~y~%02d~w~:~y~%02d", h, m, s);
	GameTextForPlayer(playerid, str, 5000, 1);
	return 1;
}

COMMAND:dice(playerid, params[])
{
	new item = HasPlayerItem(ITEM_TYPE_CUBE, playerid);
	if (item == -1 ) return SendGuiInformation(playerid, "Information", "You don't have dice.");
	Item_Use(item, playerid);
	return 1;
}

COMMAND:report(playerid, params[])
{
	new id, text[70], str[128];

	if( sscanf(params, "ds[70]", id, text) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /report [id] [reason]");
		return 1;
	}

	if(!IsPlayerConnected(id)) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	if( !pInfo[id][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");

	// Info dla gracza

	SendGuiInformation(playerid, "Information", "Report mailed. Wait for administration response.");

	// Formatujemy raport

	format(str, sizeof(str), "Report from %s (%d) for %s (%d), reason: %s.", pInfo[playerid][player_name], playerid, pInfo[id][player_name], id, text);

	// Wysylamy raport do tych ktorzy sa na duty

	foreach(new p : Player)
	{
		if(pInfo[p][player_admin_duty]==true)
		{
			SendClientMessage(p, COLOR_LIGHTER_RED, str);
		}
	}
	return 1;
}

COMMAND:o(playerid, params[])
{
	return cmd_offer(playerid, params);
}

COMMAND:offer(playerid, params[])
{
    new sub_cmd[20], rest[64], playa, price, compid, clr1, clr2;
    if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) )
    {
        SendClientMessage(playerid, COLOR_GREY, "Tip: /o(ffer) [vcard, product, fix, refuel, tow, fare, montage, vehicle, component, document, tribute]");
        return 1;
    }
    else
    {
        if( !strcmp(sub_cmd, "fix") )
        {
            new gid = pInfo[playerid][player_duty_gid];
           	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on workshop duty.");
           	if(Group[gid][group_type] != GROUP_TYPE_WORKSHOP) return SendGuiInformation(playerid, "Information", "You are not on workshop duty.");
            if(sscanf(rest, "dd", playa, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o fix [player id] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Niepoprawne ID gracza.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");
            if(pInfo[playerid][player_repairing] > 0) return SendGuiInformation(playerid, "Information", "You are already fixing vehicle.");

            if(GetPlayerVirtualWorld(playerid) == 0)
            {
	            new areaid = pInfo[playerid][player_area];
	            if(areaid == -1)
	            {
	            	SendGuiInformation(playerid, "Information", "You are not in area with fixing flag.");
	            	return 1;
	            }
	            if(!AreaHasFlag(areaid, AREA_FLAG_SERWIS))
	            {
	            	SendGuiInformation(playerid, "Information", "You are not in area with fixing flag.");
	            	return 1;
	            }
	        }
	        else
	        {
	        	new doorid = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	        	if (doorid == -1)
	        	{
        			SendGuiInformation(playerid, "Information", "You are not in workshop.");
	            	return 1;
	        	}
	        }

            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
            if(!IsPlayerInAnyVehicle(playa)) return SendGuiInformation(playerid, "Information", "This player must be in vehicle.");
            if(Vehicle[GetPlayerVehicleID(playa)][vehicle_health] >= 1000.0) return SendGuiInformation(playerid, ""guiopis"Information", "This vehicle dont need fix.");
 
            new model = Vehicle[GetPlayerVehicleID(playa)][vehicle_model];
 
            new secprice = 1000 - floatround(Vehicle[GetPlayerVehicleID(playa)][vehicle_health], floatround_ceil);
 
            new allprice = price+secprice;
 
            new resp = SetOffer(playerid, playa, OFFER_TYPE_REPAIR, allprice, GetPlayerVehicleID(playa));
            if( resp ) ShowPlayerOffer(playa, playerid, "Fix", sprintf("Fix for %s (Worker fee %d)", VehicleNames[model-400], price), allprice);
            pInfo[playerid][player_fixprice] = price;
        }
        else if( !strcmp(sub_cmd, "tow") )
        {
            if(sscanf(rest, "dd", playa, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o tow [player id] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

 			if(GetPlayerVehicleID(playerid) == INVALID_VEHICLE_ID) return SendGuiInformation(playerid, "Information", "You are not in any vehicle.");
 			if(GetPlayerVehicleID(playa) == INVALID_VEHICLE_ID) return SendGuiInformation(playerid, "Information", "This player must be in vehicle.");
 			if(GetPlayerVehicleID(playa) == GetPlayerVehicleID(playerid)) return SendGuiInformation(playerid, "Information", "You can tow your own car.");
 			if(!CanPlayerUseVehicle(playa, GetPlayerVehicleID(playa))) return SendGuiInformation(playerid, "Information", "This player has no access to use this car.");
 			if( GetPlayerUsedItem(playerid, ITEM_TYPE_LINE) == -1 ) return SendGuiInformation(playerid, "Information", "You don't have used rope.");
            new resp = SetOffer(playerid, playa, OFFER_TYPE_TOW, 0, GetPlayerVehicleID(playa));
            if( resp ) ShowPlayerOffer(playa, playerid, "Tow", sprintf("Tow from %s", pInfo[playerid][player_name]), 0);
        }
        else if( !strcmp(sub_cmd, "tribute") )
        {
        	new gid = pInfo[playerid][player_duty_gid];
        	if(gid == -1) return SendGuiInformation(playerid, "Information", "Command only for people on crime organization duty.");
        	if(Group[gid][group_type] != GROUP_TYPE_GANG) return SendGuiInformation(playerid, "Information", "Command only for people on crime organization duty.");
        	new slot = GetPlayerDutySlot(playerid);
        	if(slot == -1) return SendGuiInformation(playerid, "Information", "Command only for people on crime organization duty.");
        	if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_OFFER) ) return SendGuiInformation(playerid, "Information", "You don't have offer privileges. Ask your leader.");

            if(sscanf(rest, "d", playa)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o tribute [player id]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

            new tslot = GetPlayerDutySlot(playa);
            if(tslot == -1) return SendGuiInformation(playerid, "Information", "This player is not on group duty.");

            if( !WorkerHasFlag(playa, tslot, WORKER_FLAG_LEADER) ) return SendGuiInformation(playerid, "Information", "This player is no leader.");
            new targetgid = pInfo[playa][player_duty_gid];
            if(targetgid == -1) return 1;
            if(!GroupHasFlag(targetgid, GROUP_FLAG_BUSINESS)) return SendGuiInformation(playerid, "Information", "Only businesses can pay tributes.");

            if(Group[targetgid][group_tributers] == Group[gid][group_uid]) return SendGuiInformation(playerid, "Information", "This group is already under controll of your organization.");
 
            new resp = SetOffer(playerid, playa, OFFER_TYPE_TRIBUTE, 0, Group[gid][group_uid]);
            if( resp ) ShowPlayerOffer(playa, playerid, "Tribute", sprintf("Tribute from %s", Group[gid][group_name]), 0);
        }
        else if( !strcmp(sub_cmd, "advertise") )
        {
        	new gid = pInfo[playerid][player_duty_gid];
        	if(gid == -1) return SendGuiInformation(playerid, "Information", "Command only for Weazel News workers (on duty).");
        	if(Group[gid][group_type] != GROUP_TYPE_SN) return SendGuiInformation(playerid, "Information", "Command only for Weazel News workers (on duty).");

            if(sscanf(rest, "dd", playa, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o advertise [player id] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Niepoprawne ID gracza.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

            if(price < 0) return SendGuiInformation(playerid, "Information", "Invalid price.");
 
            new resp = SetOffer(playerid, playa, OFFER_TYPE_ADVERTISE, price);
            if( resp ) ShowPlayerOffer(playa, playerid, "Advertise", "Advert in Weazel News", price);
        }
        else if( !strcmp(sub_cmd, "rp") )
        {
        	new text[64];
        	new gid = pInfo[playerid][player_duty_gid];
        	if(gid == -1) return SendGuiInformation(playerid, "Information", "Command only for group members (on duty).");

            if(sscanf(rest, "dds[64]", playa, price, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o rp [player id] [price] [service name]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(strlen(text) > 60) return SendGuiInformation(playerid, "Information", "Service name too long, max 60 chars.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

            if(price < 0) return SendGuiInformation(playerid, "Information", "Invalid price.");
 
            new resp = SetOffer(playerid, playa, OFFER_TYPE_RP, price);
            if( resp ) ShowPlayerOffer(playa, playerid, "RP service", text, price);
        }
        else if( !strcmp(sub_cmd, "montage") )
        {
           	new gid = pInfo[playerid][player_duty_gid];
           	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on worshop duty.");
           	if(Group[gid][group_type] != GROUP_TYPE_WORKSHOP) return SendGuiInformation(playerid, "Information", "You are not on worshop duty.");
            if(sscanf(rest, "dd", playa, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o montage [player id] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
            if(!IsPlayerInAnyVehicle(playa)) return SendGuiInformation(playerid, "Information", "This player is not in vehicle.");

            new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_TUNING);
            if(GetPlayerUsedItem(playerid, ITEM_TYPE_TUNING) == -1) return SendGuiInformation(playerid, "Information", "You are not using component item.");

            new vid = GetPlayerVehicleID(playa);
            new model = GetVehicleModel(vid);

 			if(!IsComponentidCompatible(model, PlayerItem[playerid][itemid][player_item_value1])) return SendGuiInformation(playerid, "Information", "Component is not compatible with this vehicle.");

            new resp = SetOffer(playerid, playa, OFFER_TYPE_MONTAGE, price, PlayerItem[playerid][itemid][player_item_value1]);
            if( resp ) ShowPlayerOffer(playa, playerid, "Montage", sprintf("Tuning: %s", PlayerItem[playerid][itemid][player_item_name]), price);
        }
        else if( !strcmp(sub_cmd, "painting") )
        {
            new gid = pInfo[playerid][player_duty_gid];
           	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on worshop duty.");
           	if(Group[gid][group_type] != GROUP_TYPE_WORKSHOP) return SendGuiInformation(playerid, "Information", "You are not on worshop duty.");
            if(sscanf(rest, "dddd", playa, price, clr1, clr2)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o painting [player id] [price] [color1] [color2]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");
        
            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
            if(!IsPlayerInAnyVehicle(playa)) return SendGuiInformation(playerid, "Information", "This player is not in vehicle.");
            new vid = GetPlayerVehicleID(playa);
 
            new resp = SetOffer(playerid, playa, OFFER_TYPE_RESPRAY, price, clr2);
            if( resp ) ShowPlayerOffer(playa, playerid, "Painting", sprintf("Painting %s (colors %d, %d)", VehicleNames[Vehicle[vid][vehicle_model]-400], clr1, clr2), price);

            pInfo[playerid][player_spraying_color] = clr1;
        }
        else if( !strcmp(sub_cmd, "paintjob") )
        {
            new gid = pInfo[playerid][player_duty_gid];
           	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on worshop duty.");
           	if(Group[gid][group_type] != GROUP_TYPE_WORKSHOP) return SendGuiInformation(playerid, "Information", "You are not on worshop duty.");

           	if( !GroupHasFlag(gid, GROUP_FLAG_PAINTJOB) ) return SendGuiInformation(playerid, "Information", "Your workshop dont have paintjob flag.");

            if(sscanf(rest, "ddd", playa, price, clr1)) 
            {
            	SendClientMessage(playerid, COLOR_GREY, "Tip: /o paintjob [player id] [price] [type (0-3)]");
            	SendClientMessage(playerid, COLOR_GREY, "Tip: Type 3 is removing paintjob.");
            	return 1;
            }

            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
            if(!IsPlayerInAnyVehicle(playa)) return SendGuiInformation(playerid, "Information", "This player is not in vehicle.");
            if(clr1 <0 || clr1 >3) return SendGuiInformation(playerid, "Information", "Wrong paintjob type.");

            new vid = GetPlayerVehicleID(playa);
 
            new resp = SetOffer(playerid, playa, OFFER_TYPE_PAINTJOB, price, clr1);
            if( resp ) ShowPlayerOffer(playa, playerid, "Paintjob", sprintf("Paintjob for %s (type %d)", VehicleNames[Vehicle[vid][vehicle_model]-400], clr1), price);
        }
        else if( !strcmp(sub_cmd, "component") )
        {
            new gid = pInfo[playerid][player_duty_gid];
           	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on worshop duty.");
           	if(Group[gid][group_type] != GROUP_TYPE_WORKSHOP) return SendGuiInformation(playerid, "Information", "You are not on worshop duty.");

            if(sscanf(rest, "ddd", compid, playa, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o component [item uid] [player id] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
            if(!IsPlayerInAnyVehicle(playa)) return SendGuiInformation(playerid, "Information", "This player is not in any vehicle.");
            if(GetPlayerUsedItem(playerid, ITEM_TYPE_ACCESS) == -1) return SendGuiInformation(playerid, "Information", "You are not using component item.");
 
            new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_ACCESS);
            if(PlayerItem[playerid][itemid][player_item_uid] != compid) return SendGuiInformation(playerid, "Information", "Invalid item UID.");
 
            new resp = SetOffer(playerid, playa, OFFER_TYPE_ACCESS, price, PlayerItem[playerid][itemid][player_item_value1]);
            if( resp ) ShowPlayerOffer(playa, playerid, "Montage", sprintf("Montage: %s", PlayerItem[playerid][itemid][player_item_name]), price);
        }
        else if( !strcmp(sub_cmd, "mount") )
        {
           	new gid = pInfo[playerid][player_duty_gid];
           	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on security company duty.");
           	if(Group[gid][group_type] != GROUP_TYPE_SECURITY) return SendGuiInformation(playerid, "Information", "You are not on security company duty.");
            if(sscanf(rest, "dd", playa, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o mount [player id] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

            if(GetPlayerVirtualWorld(playerid) == 0) return SendGuiInformation(playerid, "Information", "You are not near any doors.");

            new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
            if(d_id == -1) return SendGuiInformation(playerid, "Information", "You are not near any doors.");

            new targetgid = pInfo[playa][player_duty_gid];
            if(targetgid == -1) return SendGuiInformation(playerid, "Information", "This player is not on group duty.");

            if(Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP) return SendGuiInformation(playerid, "Information", "Accessories can be mounted only in group doors.");
            if(Door[d_id][door_owner] != Group[targetgid][group_uid]) return SendGuiInformation(playerid, "Information", "These doors are not belong to that players group duty.");

            new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_DOOR_ACCESS);
            if(GetPlayerUsedItem(playerid, ITEM_TYPE_DOOR_ACCESS) == -1) return SendGuiInformation(playerid, "Information", "You are not using door mount item.");
 
            new resp = SetOffer(playerid, playa, OFFER_TYPE_DOOR_ACCESS, price, itemid);
            if( resp ) ShowPlayerOffer(playa, playerid, "Door mount", sprintf("Mount: %s", PlayerItem[playerid][itemid][player_item_name]), price);
        }
        else if( !strcmp(sub_cmd, "document") )
        {
        	new gid = pInfo[playerid][player_duty_gid];
        	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty with access to offering documents.");

        	new slot = GetPlayerGroupSlot(playerid, gid);
        	if(slot == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty with access to offering documents.");

        	if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_OFFER) ) return SendGuiInformation(playerid, "Information", "You have no privileges to offer. Ask leader.");

            if(sscanf(rest, "dd", compid, playa))
            {
            	SendClientMessage(playerid, COLOR_GREY, "Tip: /o document [type] [player id]");
            	SendPlayerInformation(playerid, "~w~Types:~n~~p~1~w~ - weapon license~n~~p~2~w~ - driver license~n~~p~3~w~ - health certificate", 6000);
            	return 1;
            }

            if(compid <1 || compid > 3)
            {
            	SendClientMessage(playerid, COLOR_GREY, "Tip: /o document [type] [player id]");
            	SendPlayerInformation(playerid, "~w~Types:~n~~p~1~w~ - weapon license~n~~p~2~w~ - driver license~n~~p~3~w~ - health certificate", 6000);
            	return 1;
            }

            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            
            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
 	
 			if(compid == 1)
 			{
 				if(Group[gid][group_type] != GROUP_TYPE_LSPD)
 				{
 					SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
 					return 1;
 				}

 				if(!WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER)) return SendGuiInformation(playerid, "Information", "Only leader can offer weapon licenses.");

 				if(GetPlayerVirtualWorld(playerid) == 0)
	            {
		            SendGuiInformation(playerid, "Information", "You are not in City Hall.");
		            return 1;
		        }
		        else
		        {
		        	new doorid = GetDoorByUid(GetPlayerVirtualWorld(playerid));

		        	if (doorid == -1) return SendGuiInformation(playerid, "Information", "You are not in LSPD base.");

		        	new govid = GetGroupByUid(Door[doorid][door_owner]);
		        	if(govid == -1) return SendGuiInformation(playerid, "Information", "You are not in LSPD base.");

		        	if(Group[govid][group_type] != GROUP_TYPE_LSPD)
		        	{
		        		SendGuiInformation(playerid, "Information", "You are not in LSPD base.");
		            	return 1;
		        	}
		        }

	           	new resp = SetOffer(playerid, playa, OFFER_TYPE_DOCUMENT, 500, DOCUMENT_WEAPON);
				if( resp ) ShowPlayerOffer(playa, playerid, "Document", "Weapon license", 500);
			}
			else if(compid ==2)
			{
				if(Group[gid][group_type] != GROUP_TYPE_GOV)
 				{
 					SendGuiInformation(playerid, "Information", "You are not on GOV duty.");
 					return 1;
 				}

 				if(GetPlayerVirtualWorld(playerid) == 0)
	            {
		            SendGuiInformation(playerid, "Information", "You are not in City Hall.");
		            return 1;
		        }
		        else
		        {
		        	new doorid = GetDoorByUid(GetPlayerVirtualWorld(playerid));

		        	if (doorid == -1) return SendGuiInformation(playerid, "Information", "You are not in City Hall.");

		        	new govid = GetGroupByUid(Door[doorid][door_owner]);
		        	if(govid == -1) return SendGuiInformation(playerid, "Information", "You are not in City Hall.");
		        	
		        	if(Group[govid][group_type] != GROUP_TYPE_GOV)
		        	{
		        		SendGuiInformation(playerid, "Information", "You are not in City Hall.");
		            	return 1;
		        	}
		        }

				new resp = SetOffer(playerid, playa, OFFER_TYPE_DOCUMENT, 150, DOCUMENT_DRIVE);
				if( resp ) ShowPlayerOffer(playa, playerid, "Document", "Driver license", 150);
			}
			else if(compid == 3)
			{
				if(Group[gid][group_type] != GROUP_TYPE_MEDIC)
 				{
 					SendGuiInformation(playerid, "Information", "You are not on medic duty.");
 					return 1;
 				}

 				if(GetPlayerVirtualWorld(playerid) == 0)
	            {
		            SendGuiInformation(playerid, "Information", "You are not in hospital.");
		            return 1;
		        }
		        else
		        {
		        	new doorid = GetDoorByUid(GetPlayerVirtualWorld(playerid));

		        	if (doorid == -1) return SendGuiInformation(playerid, "Information", "You are not in hospital.");

		        	new govid = GetGroupByUid(Door[doorid][door_owner]);
		        	if(govid == -1) return SendGuiInformation(playerid, "Information", "You are not in hospital.");
		        	
		        	if(Group[govid][group_type] != GROUP_TYPE_MEDIC)
		        	{
		        		SendGuiInformation(playerid, "Information", "You are not in hospital.");
		            	return 1;
		        	}
		        }

				new resp = SetOffer(playerid, playa, OFFER_TYPE_DOCUMENT, 150, DOCUMENT_METRIC);
				if( resp ) ShowPlayerOffer(playa, playerid, "Document", "Health certificate", 150);
			}
        }
        else if( !strcmp(sub_cmd, "vehicle") )
        {
            if(sscanf(rest, "dd", playa, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o vehicle [player id] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 4.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
            if(!IsPlayerInAnyVehicle(playerid)) return SendGuiInformation(playerid, "Information", "You are not in your vehicle.");
 
            new vehicleid = GetPlayerVehicleID(playerid);
 
            if( Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_PLAYER )
            {
                if( Vehicle[vehicleid][vehicle_owner] == pInfo[playerid][player_id] )
                {
                    new model = Vehicle[vehicleid][vehicle_model];
                    new resp = SetOffer(playerid, playa, OFFER_TYPE_VEHICLE, price, vehicleid);
                    if( resp ) ShowPlayerOffer(playa, playerid, "Vehicle", sprintf("Vehicle %s", VehicleNames[model-400]), price);
                }
                else
                {
                    SendGuiInformation(playerid, "Information", "This vehicle is not yours.");
                }
            }
            else
            {
                SendGuiInformation(playerid, "Information", "This vehicle is not yours.");
            }
        }
        else if( !strcmp(sub_cmd, "refuel") )
        {
            new count, secprice;
            new gid = pInfo[playerid][player_duty_gid];
           	if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on worshop duty.");
           	if(Group[gid][group_type] != GROUP_TYPE_WORKSHOP) return SendGuiInformation(playerid, "Information", "You are not on worshop duty.");

            new object_id = GetClosestObjectType(playerid, OBJECT_PETROL);

			if(object_id == INVALID_OBJECT_ID)
			{
				SendGuiInformation(playerid, "Information", "You are not near gas pump.");
				return 1;
			}	

            if(sscanf(rest, "ddd", playa, count, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o refuel [player id] [amount of fuel] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");
            if(count<0) return SendGuiInformation(playerid, "Information", "Invalid fuel amount.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

            if(!IsPlayerInAnyVehicle(playa)) return SendGuiInformation(playerid, "Information", "This player is not in any vehicle.");
 
            new vid = GetPlayerVehicleID(playa);
 
            if(count + floatround(Vehicle[vid][vehicle_fuel_current], floatround_ceil) > VehicleFuelMax[GetVehicleModel(vid)-400])
            {
                SendGuiInformation(playerid, "Information", "This vehicle cant fit that amount of fuel.");
                return 1;
            }
 			
 			switch( Vehicle[vid][vehicle_fuel_type] )
			{
				case VEHICLE_FUEL_TYPE_BENZYNA:
				{
					secprice = count*3;
				}
				
				case VEHICLE_FUEL_TYPE_DIESEL:
				{
					secprice = count*2;
				}

				case VEHICLE_FUEL_TYPE_LOTNICZE:
				{
					secprice = count*8;
				}
			}
 
            new allprice = price+secprice;
            
            pInfo[playerid][player_dialog_tmp1] = price;
 
            new resp = SetOffer(playerid, playa, OFFER_TYPE_REFUEL, allprice, count);
            if( resp ) ShowPlayerOffer(playa, playerid, "Refuel", sprintf("Refuel %d liters (Worker fee %d)", count, price), allprice);
        }
        else if( !strcmp(sub_cmd, "fare") )
        {
            if(sscanf(rest, "dd", playa, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o fare [player id] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(!IsPlayerInAnyVehicle(playerid)) return SendGuiInformation(playerid, "Information", "You are not in taxi cab.");
            if(pInfo[playerid][player_taxi_drive] == true) return SendGuiInformation(playerid, "Information", "You are already on fare.");
            if(pInfo[playa][player_money] < price*2) return SendGuiInformation(playerid, "Information", "This player has no money.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
            if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");
 
            new gid = pInfo[playerid][player_duty_gid];
 			if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on taxi duty.");

            if(Group[gid][group_type] == GROUP_TYPE_TAXI)
            {
                new resp = SetOffer(playerid, playa, OFFER_TYPE_TAXI, 0, price);
                if( resp ) ShowPlayerOffer(playa, playerid, "Taxi fare", sprintf("Taxi fare (%d/km)", price), 0);
                return 1;
            }
            SendGuiInformation(playerid, "Information", "You are not on taxi duty.");
        }
        else if( !strcmp(sub_cmd, "heal") )
        {
            if(sscanf(rest, "dd", playa, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o heal [player id] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you");
            if(price < 0) return SendGuiInformation(playerid, "Information", "Invalid price.");
 
            new gid = pInfo[playerid][player_duty_gid];
 			if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on medic duty.");

            if(Group[gid][group_type] == GROUP_TYPE_MEDIC)
            {
                new resp = SetOffer(playerid, playa, OFFER_TYPE_MEDIC, price);
                if( resp ) ShowPlayerOffer(playa, playerid, "Healing", "HP recovery", price);
                return 1;
            }
            SendGuiInformation(playerid, "Information", "You are not on medic duty.");
        }
        else if( !strcmp(sub_cmd, "vcard") )
        {
            if(sscanf(rest, "d", playa)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o vcard [player id]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( pInfo[playa][player_spec] != INVALID_PLAYER_ID) return SendGuiInformation(playerid, "Information", "This player is too far from you");

            if(GetDistanceBetweenPlayers(playerid, playa) > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you");

            new resp = SetOffer(playerid, playa, OFFER_TYPE_VCARD, 0, GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE));

            if( resp ) ShowPlayerOffer(playa, playerid, "vCard", sprintf("vCard %s [%d]", pInfo[playerid][player_name], Item[pOffer[playa][offer_extraid]][item_value1]), 0);
        }
        else if( !strcmp(sub_cmd, "product") )
        {
            new slot = GetPlayerDutySlot(playerid);
            if(slot == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");

            cmd_g(playerid, sprintf("%d storage", slot+1));
        }
        else if( !strcmp(sub_cmd, "carnet") )
        {
            if(sscanf(rest, "dd", playa, price)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /o carnet [player id] [price]");
            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(price < 50) return SendGuiInformation(playerid, "Information", "Invalid price.");

            if(GetDistanceBetweenPlayers(playerid, playa) > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
            if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");
 
            new gid = pInfo[playerid][player_duty_gid];
            if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on gym duty.");
 
            if(Group[gid][group_type] == GROUP_TYPE_GYM)
            {
                new resp = SetOffer(playerid, playa, OFFER_TYPE_GYM, price, 0);
                if( resp ) ShowPlayerOffer(playa, playerid, "Carnet", sprintf("Carnet (10 minutes) from %s", pInfo[playerid][player_name]), price);
            }
            else
            {
                SendGuiInformation(playerid, "Information", "You are not on gym duty.");
            }
        }
        else if( !strcmp(sub_cmd, "fightlesson") )
        {
        	new styl;
            if(sscanf(rest, "ddd", playa, styl, price))
            {
            	SendClientMessage(playerid, COLOR_GREY, "Tip: /o fightlesson [player id] [style] [price]");
            	SendClientMessage(playerid, COLOR_GREY, "Tip: Styles: 5 - box, 6 - kung fu, 7 - kneehead, 16 - elbow");
            	return 1;
            }

            if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
            if(price < 500) return SendGuiInformation(playerid, "Information", "Minimal price is $500.");

            new Float:dist;
            dist = GetDistanceBetweenPlayers(playerid, playa);
            if(dist>3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
 
            new gid = pInfo[playerid][player_duty_gid];
            if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on gym duty.");
 	
            if(Group[gid][group_type] == GROUP_TYPE_GYM)
            {
            	if(styl != 5 && styl != 6 && styl != 7) return SendGuiInformation(playerid, "Information", "Invalid fighting style.");

            	new object_id = GetClosestObjectType(playerid, OBJECT_PUNCHBAG);

				if(object_id == INVALID_OBJECT_ID)
				{
					SendGuiInformation(playerid, "Information", "There is no punch bag in your area.");
					return 1;
				}

				pInfo[playa][player_gym_object] = object_id;

                new resp = SetOffer(playerid, playa, OFFER_TYPE_GYM_FIGHT, price, styl);
                if( resp ) ShowPlayerOffer(playa, playerid, "Fight training", sprintf("Training from %s", pInfo[playerid][player_name]), price);
            }
            else
            {
                SendGuiInformation(playerid, "Information", "You are not on gym duty.");
            }
        }
    }
    return 1;
}

COMMAND:try(playerid, params[])
{
	new text[128], string[160];
	if(sscanf(params, "s[128]", text))
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /try [action]");
		return 1;
	}
	new loss = random(2);
	new
			Float:x,
			Float:y,
			Float:z,
			vw = GetPlayerVirtualWorld(playerid),
			int = GetPlayerInterior(playerid);
			
	GetPlayerPos(playerid, x, y, z);
	
	switch(loss)
	{
		case 0:
		{
			if(pInfo[playerid][player_sex] == 0)
			{
				format(string, sizeof(string), "*** %s failed tries to %s.", pInfo[playerid][player_name], text);
			}
			else
			{
				format(string, sizeof(string), "*** %s failed tries to %s.", pInfo[playerid][player_name], text);
			}

			foreach(new p : Player)
			{
				if( !pInfo[p][player_logged] ) continue;
				if( GetPlayerVirtualWorld(p) != vw || GetPlayerInterior(p) != int ) continue;
				new Float:distance = GetPlayerDistanceFromPoint(p, x, y, z);
				if( distance > 12.0 ) continue;
				SendClientMessage(p, 0xB58ADAFF, string);
			}
		}
		case 1:
		{
			if(pInfo[playerid][player_sex] == 0)
			{
				format(string, sizeof(string), "*** %s successfully tries to %s.", pInfo[playerid][player_name], text);
			}
			else
			{
				format(string, sizeof(string), "*** %s successfully tries to %s.", pInfo[playerid][player_name], text);
			}

			foreach(new p : Player)
			{
				if( !pInfo[p][player_logged] ) continue;
				if( GetPlayerVirtualWorld(p) != vw || GetPlayerInterior(p) != int ) continue;
				new Float:distance = GetPlayerDistanceFromPoint(p, x, y, z);
				if( distance > 12.0 ) continue;
				SendClientMessage(p, 0xB58ADAFF, string);
			}
		}
	}
	return 1;
}

COMMAND:trunk(playerid, params[])
{
	new vid = GetNearestPlayerVehicle(playerid);
	if (vid == -1) return SendGuiInformation(playerid, "Information", "You are not near vehicle or you dont have access to it.");
	Vehicle[vid][vehicle_boot] = !Vehicle[vid][vehicle_boot];
	UpdateVehicleVisuals(vid);
	return 1;
}

COMMAND:hood(playerid, params[])
{
	new vid = GetNearestPlayerVehicle(playerid);
	if (vid == -1) return SendGuiInformation(playerid, "Information", "You are not near vehicle or you dont have access to it.");
	Vehicle[vid][vehicle_bonnet] = !Vehicle[vid][vehicle_bonnet];
	UpdateVehicleVisuals(vid);
	return 1;
}

COMMAND:bomb(playerid, params[])
{
	new sub[80],rest[80];
	if(sscanf(params, "s[80]S()[80]", sub, rest)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /bomb [plant, detonate]");
	if(!strcmp(sub, "podloz", true))
	{
		if( GetPlayerUsedItem(playerid, ITEM_TYPE_BOMB) == -1 ) return SendGuiInformation(playerid, "Information", "Nie posiadasz w uzyciu ladunkow wybuchowych.");
	   
		new vid = GetNearestVehicle(playerid);
		if( vid == -1 ) return SendGuiInformation(playerid, "Information", "Brak pojazdów w okolicy.");

		ApplyAnimation(playerid, "CAR", "Fixn_Car_Loop", 4.1, 0, 1, 1, 1, 0, true);
		pInfo[playerid][player_bomb_car]=vid;
		pInfo[playerid][player_bomb]=20;
		pInfo[playerid][player_bomb_planting] = 1;
		SendPlayerInformation(playerid, "~w~You are planting a ~y~bomb~w~.~n~~n~Wait 20 seconds.");
		return 1;
	}
	else if(!strcmp(sub, "detonate", true))
	{
		if(pInfo[playerid][player_bomb_car] == INVALID_VEHICLE_ID)
		{
			SendGuiInformation(playerid, "Information", "You didnt planted any bomb.");
			return 1;
		}

		new Float:Pos[3];
		GetVehiclePos(pInfo[playerid][player_bomb_car], Pos[0],Pos[1],Pos[2]);
		CreateExplosion(Pos[0],Pos[1],Pos[2], 3, 30.0); // JEB
		Vehicle[pInfo[playerid][player_bomb_car]][vehicle_health] = 25.0;
		SetVehicleHealth(pInfo[playerid][player_bomb_car], 25.0);
		SendPlayerInformation(playerid, "~w~Bomb ~y~exploded~w~.");
		
		foreach(new p : Player)
		{
			if(GetPlayerVehicleID(p) == pInfo[playerid][player_bomb_car])
			{
				SendClientMessage(p, COLOR_LIGHTER_RED, "You are dead. Someone planted a bomb in your car and detonated it.");
				CharacterKill(p, -1, "Bomb explosion");
				continue;
			}
		}

		pInfo[playerid][player_bomb_car] = INVALID_VEHICLE_ID;
		return 1;
	}
	else SendClientMessage(playerid, COLOR_GREY, "Tip: /bomb [plant/detonate]");
	return 1;
}

COMMAND:heal(playerid, params[])
{
	new target;
	if(sscanf(params, "d", target))
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /heal [player id]");
		return 1;
	}

	if( !IsPlayerConnected(target) ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	if( !pInfo[target][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");

	if( GetDistanceBetweenPlayers(playerid, target) > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

	if(IsPlayerInAnyGroup(playerid))
	{
		new gid = pInfo[playerid][player_duty_gid];
		if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on medic duty.");

		if(Group[gid][group_type] == GROUP_TYPE_MEDIC)
		{
			if(pInfo[target][player_bw]>0)
			{
				SetPlayerHealth(target, 20);
				pInfo[target][player_bw]=0;
				ClearAnimations(target);
				GameTextForPlayer(target,"-",1,1);
				TogglePlayerControllable(target, 1);
				SetCameraBehindPlayer(target);
				SendGuiInformation(playerid, "Information", "Player has been healed.");
				RemovePlayerStatus(target, PLAYER_STATUS_BW);
			}
			else
			{
				SendGuiInformation(playerid, "Information", "This player is not brutally wounded.");
			}
		}
		else
		{
			SendGuiInformation(playerid, "Information", "You are not on medic duty.");
		}
	}
	else
	{
		SendGuiInformation(playerid, "Information", "You are not on medic duty.");
	}
	return 1;
}

COMMAND:unblock(playerid, params[])
{
	new target, money, text[32];
	if(sscanf(params, "d", target, money, text))
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /unblock [vehicle id]");
		return 1;
	}

	if( target == INVALID_VEHICLE_ID ) return SendGuiInformation(playerid, "Information", "Invalid vehicle id.");

	if(IsPlayerInAnyGroup(playerid))
	{
		new gid = pInfo[playerid][player_duty_gid];

		if(gid == -1)
		{
			SendGuiInformation(playerid, "Information", "You are not on group duty.");
			return 1;
		}

		if(Group[gid][group_type] != GROUP_TYPE_LSPD)
		{
			SendGuiInformation(playerid, "Information", "You are not on group duty with blokade flag.");
			return 1;
		}

		new Float:x, Float:y, Float:z;
		GetVehiclePos(target, x, y, z);
		if(!IsPlayerInRangeOfPoint(playerid, 5.0, x, y, z)) return SendGuiInformation(playerid, "Information", "You are not near this vehicle.");

		if(Vehicle[target][vehicle_blocked] > 0)
		{
			Vehicle[target][vehicle_blocked] = 0;
			SendGuiInformation(playerid, "Information", "Wheel has been unblocked.");
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_blockwheel` = 0 WHERE `vehicle_uid` = %d", Vehicle[target][vehicle_uid]));
		}
		else
		{
			SendGuiInformation(playerid, "Information",  "This vehicle has no wheel block.");
		}
	}
	else
	{
		SendGuiInformation(playerid, "Information", "You are not belong to any group.");
	}
	return 1;
}

COMMAND:wheelblock(playerid, params[])
{
	new target, money, text[32];
	if(sscanf(params, "dds[32]", target, money, text))
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /wheelblock [vehicle id] [price] [reason (max 32 chars)]");
		return 1;
	}

	if( target == INVALID_VEHICLE_ID ) return SendGuiInformation(playerid, "Information", "Invalid vehicle id.");

	if(IsPlayerInAnyGroup(playerid))
	{
		new gid = pInfo[playerid][player_duty_gid];

		if(gid == -1)
		{
			SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
			return 1;
		}

		if(Group[gid][group_type] != GROUP_TYPE_LSPD)
		{
			SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
			return 1;
		}

		if(money < 0 || money > 5000)
		{
			SendGuiInformation(playerid, "Information", "Minimal price is $1 and max is $5000.");
			return 1;
		}

		new Float:x, Float:y, Float:z;
		GetVehiclePos(target, x, y, z);
		if(!IsPlayerInRangeOfPoint(playerid, 5.0, x, y, z)) return SendGuiInformation(playerid, "Information", "You are not near this vehicle.");

		if(Vehicle[target][vehicle_blocked] == 0)
		{
			Vehicle[target][vehicle_blocked] = money;
			Vehicle[target][vehicle_engine] = false;
			UpdateVehicleVisuals(target);

			if(strlen(text) > 32) return SendGuiInformation(playerid, "Information", "Max 32 chars in reason.");
			mysql_escape_string(text, text, mySQLconnection, 32);
			sscanf(text, "s[32]", Vehicle[target][vehicle_block_reason]);
			logprintf(LOG_PLAYER, "[BLOCKADE] %s blocked wheel of vehicle UID %d", pInfo[playerid][player_name], Vehicle[target][vehicle_uid]);
			SendGuiInformation(playerid, "Information", sprintf("Wheel has been blocked. (ID: %d, price: $%d, reason: %s)", target, money, text));
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_blockwheel` = %d, `vehicle_block_reason` = '%s' WHERE `vehicle_uid` = %d", Vehicle[target][vehicle_blocked], text, Vehicle[target][vehicle_uid]));
		}
		else
		{
			SendGuiInformation(playerid, "Information", sprintf("This vehicle already has wheel blockade. (Reason: %s, price: $%d)", Vehicle[target][vehicle_block_reason], Vehicle[target][vehicle_blocked]));
		}
	}
	else
	{
		SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
	}
	return 1;
}

COMMAND:m(playerid, params[])
{
	new tekst[128];
	if(sscanf(params, "s[128]", tekst))
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /m [text]");
		return 1;
	}

	new gid = pInfo[playerid][player_duty_gid];

	if(gid == -1)
	{
		SendGuiInformation(playerid, "Information", "You are not on group duty.");
		return 1;
	}

	if(Group[gid][group_type] == GROUP_TYPE_LSPD)
	{
		ProxMessage(playerid, tekst, PROX_MEGAPHONE);
		return 1;
	}

	if(Group[gid][group_type] == GROUP_TYPE_SN)
	{
		ProxMessage(playerid, tekst, PROX_MEGAPHONE);
		return 1;
	}
	return SendGuiInformation(playerid, "Information", "You are not on LSPD or radio duty.");
}


COMMAND:tow(playerid, params[])
{
	new vid;
	new v = GetPlayerVehicleID(playerid);
	if(sscanf(params, "d", vid))
	{
		if(GetPVarInt(playerid, "holowanie") == 0)
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /tow [vehicle id]");
			return 1;
		}
		else
		{
			SendGuiInformation(playerid, "Information", "Towing finished.");
			DetachTrailerFromVehicle(v);
			SetPVarInt(playerid, "holowanie", 0);
			return 1;
		}
	}

	if(v == INVALID_VEHICLE_ID) return SendGuiInformation(playerid, "Information", "You are not in towtruck.");

	new model = GetVehicleModel(v);
	if(model != 525 && model != 531 ) return SendGuiInformation(playerid, "Information", "You are not in towtruck.");
	
	new Float:vehiclePos[3];
	GetVehiclePos(vid, vehiclePos[0], vehiclePos[1], vehiclePos[2]);

	if(GetPlayerDistanceFromPoint(playerid, vehiclePos[0], vehiclePos[1], vehiclePos[2]) > 8.0)
	{
		SendGuiInformation(playerid, "Information", "Vehicle is too far from your towtruck.");
		return 1;
	}

	SetPVarInt(playerid, "holowanie", 1);
	AttachTrailerToVehicle(vid, v);
	return 1;
}

COMMAND:arrest(playerid, params[])
{
    new target, time;
    if(sscanf(params, "dd", target, time))
    {
        SendClientMessage(playerid, COLOR_GREY, "Tip: /arerest [player id] [time in hours]");
        return 1;
    }
 
    if( !pInfo[target][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
    if( playerid == target) return SendGuiInformation(playerid, "Information", "Invalid player id.");
 
    new gid = pInfo[playerid][player_duty_gid];
 
    if(gid == -1)
    {
        SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
        return 1;
    }
 
    if(Group[gid][group_type] != GROUP_TYPE_LSPD)
    {
        SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
        return 1;
    }
 
    if(GetDistanceBetweenPlayers(playerid, target) > 5.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");
 
    if(time < 0) return SendGuiInformation(playerid, "Information", "Invalid arrest time.");
 
    if(time == 0)
    {
        SendGuiInformation(playerid, "Information", "Player has been released from jail.");
        SendGuiInformation(target, "Information", "You are free now.");
       	RemovePlayerStatus(target, PLAYER_STATUS_JAIL);
        mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_spawn = 0, char_spawn_type = 1, char_arrest = 0 WHERE char_uid = %d", pInfo[target][player_id]));
        pInfo[target][player_arrest]= 0;
        return 1;
    }
 
    new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
 
 	if(d_id == -1) return SendGuiInformation(playerid, "Information", "Jail door not detected.");

    pInfo[target][player_spawn] = Door[d_id][door_uid];
    pInfo[target][player_spawn_type] = SPAWN_TYPE_HOUSE;
    pInfo[target][player_arrest] = 3600*time+gettime();
    AddPlayerStatus(target, PLAYER_STATUS_JAIL);
    mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_spawn = %d, char_spawn_type = %d, char_arrest = %d WHERE char_uid = %d", pInfo[target][player_spawn], pInfo[target][player_spawn_type], 3600*time+gettime(), pInfo[target][player_id]));
 
    SendGuiInformation(playerid, "Information", "Player has been arrested.");
    SendGuiInformation(target, "Information",sprintf("Player %s arrest you.", pInfo[playerid][player_name]));
 
    return 1;
}

COMMAND:cuff(playerid, params[])
{
	new target;
	if(sscanf(params, "d", target))
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /cuff [player id]");
		return 1;
	}

	if(!IsPlayerConnected(target)) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	if( !pInfo[target][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	if( playerid == target) return SendGuiInformation(playerid, "Information", "Invalid player id.");

	new gid = pInfo[playerid][player_duty_gid];

	if(gid == -1)
	{
		SendGuiInformation(playerid, "Information", "You are not on group duty with cuff flag.");
		return 1;
	}

	if(!(Group[gid][group_flags] & GROUP_FLAG_CUFFS))
	{
		SendGuiInformation(playerid, "Information", "You are not on group duty with cuff flag.");
		return 1;
	}

	if(GetDistanceBetweenPlayers(playerid, target) > 3.0) return SendGuiInformation(playerid, "Information", "This player is too far from you.");

	if(GetPlayerSpecialAction(target) == SPECIAL_ACTION_CUFFED)
	{
		SetPlayerSpecialAction(target, SPECIAL_ACTION_NONE);
		SendGuiInformation(playerid, "Information", "Player uncuffed.");
	}
	else
	{
		SetPlayerSpecialAction(target, SPECIAL_ACTION_CUFFED);
		SendGuiInformation(playerid, "Information", "Player cuffed.");
	}

	return 1;
}

COMMAND:refuel(playerid, params[])
{
	new sub_cmd[20], rest[64];
	if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /refuel [vehicle/vanister] [amount of litres]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "vehicle") )
		{
			new amount;
			if(sscanf(rest, "d", amount))
			{
				SendClientMessage(playerid, COLOR_GREY, "Tip: /refuel vehicle [amount]");
				return 1;
			}
		  
		    new object_id = GetClosestObjectType(playerid, OBJECT_PETROL);

			if(object_id == INVALID_OBJECT_ID)
			{
				SendGuiInformation(playerid, "Information", "You are not near gas pump.");
				return 1;
			}	

			if(GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return SendGuiInformation(playerid, "Information", "You have to exit car to refuel it.");

			new vid = GetNearestVehicle(playerid);
			if (vid == -1) return SendGuiInformation(playerid, "Information", "You are too far from any vehicle.");

			if(amount<0) return SendGuiInformation(playerid, "Information", "Invalid amount.");

			new actual = floatround(Vehicle[vid][vehicle_fuel_current], floatround_ceil) + amount;
			new maximum = VehicleFuelMax[GetVehicleModel(vid)-400];

			if(actual > maximum)
			{
				SendGuiInformation(playerid, "Information", "This tank has no place for this amount of fuel.");
				return 1;
			}

			new price;

			switch( Vehicle[vid][vehicle_fuel_type] )
			{
				case VEHICLE_FUEL_TYPE_BENZYNA:
				{
					price = amount*3;
				}
				
				case VEHICLE_FUEL_TYPE_DIESEL:
				{
					price = amount*2;
				}

				case VEHICLE_FUEL_TYPE_LOTNICZE:
				{
					price = amount*10;
				}
			}
			
			if(price > pInfo[playerid][player_money]) return SendGuiInformation(playerid, "Information", "You don't have enough money.");
			GivePlayerMoney(playerid,-price);
			
			Vehicle[vid][vehicle_fuel_current] += float(amount);
			ApplyAnimation(playerid, "INT_HOUSE", "wash_up",4.1, 0, 0, 0, 0, 0, 1);
			cmd_ame(playerid, "refuels vehicle.");
		}
		else if( !strcmp(sub_cmd, "canister") )
		{
			new item = HasPlayerItem(ITEM_TYPE_CANISTER, playerid);
			if (item == -1 ) return SendGuiInformation(playerid, "Information", "You don't have canister/gas tank.");

			new amount;
			if(sscanf(rest, "d", amount))
			{
				SendClientMessage(playerid, COLOR_GREY, "Tip: /refuel canister [amount of litres]");
				return 1;
			}
		  
		    new object_id = GetClosestObjectType(playerid, OBJECT_PETROL);

			if(object_id == INVALID_OBJECT_ID)
			{
				SendGuiInformation(playerid, "Information", "You are not near gas pump.");
				return 1;
			}

			if(amount < 0) return SendGuiInformation(playerid, "Information", "Invalid amount.");

			new maximum = 20;

			if(amount + PlayerItem[playerid][item][player_item_value1] > maximum)
			{
				SendGuiInformation(playerid, "Information", "Amount too high.");
				return 1;
			}	

			new price;

			switch(PlayerItem[playerid][item][player_item_value2])
			{
				case VEHICLE_FUEL_TYPE_BENZYNA:
				{
					price = amount*3;
				}
				
				case VEHICLE_FUEL_TYPE_DIESEL:
				{
					price = amount*2;
				}

				case VEHICLE_FUEL_TYPE_LOTNICZE:
				{
					price = amount*10;
				}
			}

			if(pInfo[playerid][player_money] < price) return SendGuiInformation(playerid, "Information", "You don't have enough money.");

			ApplyAnimation(playerid, "INT_HOUSE", "wash_up",4.1, 0, 0, 0, 0, 0, 1);
			cmd_ame(playerid, "refuels his gas tank.");
			PlayerItem[playerid][item][player_item_value1] += amount;
			GivePlayerMoney(playerid, -price);
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_value1 = %d WHERE item_uid = %d", PlayerItem[playerid][item][player_item_value1], PlayerItem[playerid][item][player_item_uid]));
		}
	}
	return 1;
}

COMMAND:yo(playerid, params[])
{
	new targetid, type;
	if(sscanf(params, "ud", targetid, type))
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /yo [player id] [1-6]");
		return 1;
	}

	if(!IsPlayerConnected(targetid)) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	if( pInfo[targetid][player_spec] != INVALID_PLAYER_ID) return SendClientMessage(playerid, COLOR_GREY, "This player is too far from you.");

	if(GetDistanceBetweenPlayers(playerid, targetid) > 5.0) return SendClientMessage(playerid, COLOR_GREY, "This player is too far from you.");

	if(type < 1 || type > 6)
	{
		SendGuiInformation(playerid, "Information", "Wrong type of handshake (1-6).");
		return 1;
	}

	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");

	new resp = SetOffer(playerid, targetid, OFFER_TYPE_YO, 0, type);

	if( resp ) ShowPlayerOffer(targetid, playerid, "Handshake", sprintf("Handshake from %s", pInfo[playerid][player_name]), 0);

	return 1;
}

COMMAND:kiss(playerid, params[])
{
	new targetid, type;
	if(sscanf(params, "ud", targetid, type))
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /kiss [player id] [1-3]");
		return 1;
	}

	if(GetDistanceBetweenPlayers(playerid, targetid) > 5.0)
	{
		SendGuiInformation(playerid, "Information", "This player is too far.");
		return 1;
	}

	if(type < 1 || type > 3)
	{
		SendGuiInformation(playerid, "Information", "Wrong type of kiss (1-3).");
		return 1;
	}

	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");

	new resp = SetOffer(playerid, targetid, OFFER_TYPE_YO, 0, type+6);

	if( resp ) ShowPlayerOffer(targetid, playerid, "Kiss", sprintf("Kiss from %s", pInfo[playerid][player_name]), 0);

	return 1;
}

COMMAND:stats(playerid, params[])
{
	new giveplayer_id;

	if(!HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_GM))
	{
		ShowPlayerStats(playerid, playerid);
		return 1;
	}

	if(sscanf(params, "d", giveplayer_id))
	{
		ShowPlayerStats(playerid, playerid);
		return 1;
	} 

	if(giveplayer_id == INVALID_PLAYER_ID)
	{
		SendGuiInformation(playerid, "Information", "Invalid player id.");
		return 1;
	}
	if(!IsPlayerConnected(giveplayer_id))
	{
		SendGuiInformation(playerid, "Information", "Invalid player id.");
		return 1;
	}
	
	ShowPlayerStats(playerid, giveplayer_id);
	return 1;
}

COMMAND:clothes(playerid, params[])
{
	if (IsPlayerInVictim(playerid))
	{
		ShowPlayerDialog(playerid, DIALOG_CLOTH, DIALOG_STYLE_LIST, "Clothing store", "1. Clothes\n2. Accessories", "Choose", "Close");
	}
	else
	{
		SendGuiInformation(playerid, "Information", "You are not in clothing store.");
	}
	return 1;
}

COMMAND:seatbelts(playerid, params[])
{
	if(IsPlayerInAnyVehicle(playerid))
	{
		new vehicleid = GetPlayerVehicleID(playerid);
		new model = GetVehicleModel(vehicleid);
		if(model == 509 || model == 510 || model == 481 || model == 461 || model == 462 || model == 463 || model == 468 || model == 521 || model == 522 || model == 523 || model == 581 || model == 586)
        {
            SendGuiInformation(playerid, ""guiopis"Information", "This vehicle has no seatbelts");
            return 1;
        }

		if(!pInfo[playerid][player_belt])
		{
			AddPlayerStatus(playerid, PLAYER_STATUS_BELT);
			SendPlayerInformation(playerid, "~w~Seatbelts~y~ on~w~.", 2500);
			pInfo[playerid][player_belt]=true;
		}
		else
		{
			RemovePlayerStatus(playerid, PLAYER_STATUS_BELT);
			SendPlayerInformation(playerid, "~w~Seatbelts~y~ off~w~.", 2500);
			pInfo[playerid][player_belt]=false;
		}
	}
	else
	{
		SendGuiInformation(playerid, "Information", "You are not in any vehicle.");
	}
	return 1;
}

COMMAND:knock(playerid, params[])
{
	new d_id = -1;

	foreach(new d : Doors)
	{
		if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
		{
			d_id = d;
		}
		else if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_spawn_pos][0],  Door[d][door_spawn_pos][1], Door[d][door_spawn_pos][2]))
		{
			d_id = d;
		}
	}
	
	if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not near any door.");
	
	ProxMessage(playerid, "knocks the door.", PROX_ME);

	foreach(new p : Player)
	{
		if(GetPlayerVirtualWorld(p) == Door[d_id][door_spawn_vw] && Door[d_id][door_spawn_vw] != 0)
		{
			SendClientMessage(p, 0x9B91ECFF, "** Someone is knocking door.");
		}
	}
	return 1;
}

COMMAND:ddo(playerid, params[])
{
	new tekst[128];
	if(sscanf(params, "s[128]", tekst) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ddo [text]");

	new d_id = -1;

	foreach(new d : Doors)
	{
		if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
		{
			d_id = d;
		}
	}
	
	if(d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not outside door.");
	
	foreach(new p : Player)
	{
		if(GetPlayerVirtualWorld(p) == Door[d_id][door_spawn_vw] && Door[d_id][door_spawn_vw] != 0)
		{
			SendClientMessage(p, 0x9B91ECFF, sprintf("** %s (( %s ))", tekst, pInfo[playerid][player_name]));
		}
	}

	ProxMessage(playerid, sprintf("%s", tekst), PROX_DO);
	return 1;
}

COMMAND:ds(playerid, params[])
{
	new tekst[128];
	if(sscanf(params, "s[128]", tekst) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /dk [text]");

	new d_id = -1;

	foreach(new d : Doors)
	{
		if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
		{
			d_id = d;
		}
	}
	
	if(d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not outside door.");
	
	foreach(new p : Player)
	{
		if(GetPlayerVirtualWorld(p) == Door[d_id][door_spawn_vw] && Door[d_id][door_spawn_vw] != 0)
		{
			SendClientMessage(p, 0x888888FF, sprintf("%s shouts: %s", pInfo[playerid][player_name], tekst));
		}
	}

	ProxMessage(playerid, sprintf("%s", tekst), PROX_SHOUT);
	return 1;
}

COMMAND:pry(playerid, params[])
{
	new playercanpry[MAX_PLAYERS];
	new gid = pInfo[playerid][player_duty_gid];
	if(gid == -1) return SendClientMessage(playerid, COLOR_GREY, "You have to be on LSPD duty.");

	if(IsPlayerInAnyGroup(playerid))
	{
		if(Group[gid][group_flags] & GROUP_FLAG_PRYING)
		{
			playercanpry[playerid]=1;
		}
	}

	if(playercanpry[playerid]==1)
	{
		new d_id = -1;

		foreach(new d : Doors)
		{
			if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
			{
				d_id = d;
			}
			else if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_spawn_pos][0],  Door[d][door_spawn_pos][1], Door[d][door_spawn_pos][2]))
			{
				d_id = d;
			}
		}
		
		if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not near any door.");

		if(!Door[d_id][door_closed])
		{
			SendGuiInformation(playerid, "Information", "You can't open doors which are already open.");
			return 1;
		}
		
		if( Door[d_id][door_closed] )
		{
			Door[d_id][door_closed] = false;
		}

		ProxMessage(playerid, "pries building doors.", PROX_SERWERME);
		playercanpry[playerid]=0;
	}
	else
	{
		SendClientMessage(playerid, COLOR_GREY, "You have to be on LSPD duty.");
	}
	return 1;
}

COMMAND:bus(playerid, params[])
{
	new list_busstops[512];
	foreach(new b : Buses)
	{
		if(BusStopData[b][bUID] != 0)
		{
			if(IsPlayerInRangeOfPoint(playerid, 5.0, BusStopData[b][bPosX], BusStopData[b][bPosY], BusStopData[b][bPosZ]) && GetPlayerVirtualWorld(playerid) == 0)
			{
				foreach(new bs : Buses)
				{
					if(BusStopData[bs][bUID] != 0 && bs != b)
					{
						format(list_busstops, sizeof(list_busstops), "%s\n%d\t%s", list_busstops, bs, BusStopData[bs][bName]);
					}
				}
				if(strlen(list_busstops) <= 0)
				{
					SendGuiInformation(playerid, "Information", "There are not bus stops. Report it to administration.");
					return 1;
				}
				pInfo[playerid][player_bus_start] = b;
				ShowPlayerDialog(playerid, DIALOG_SELECT_BUSSTOP, DIALOG_STYLE_LIST, "Choose your destination:", list_busstops, "Choose", "Close");
				return 1;
			}
		}
	}

	SendGuiInformation(playerid, "Information", "You are not at the bus stop");
	return 1;
}

COMMAND:kill(playerid, params[])
{
	if(pInfo[playerid][player_bw]>0)
	{
		ShowPlayerDialog(playerid, DIALOG_AS, DIALOG_STYLE_MSGBOX, "Usmiercanie postaci", "Do you really want to permanently kill your character?\nThere is no way back.", "Yes", "No");
	}
	else
	{
		SendGuiInformation(playerid, "Information", "Your character must be brutally wounded.");
	}
	return 1;
}

COMMAND:buy(playerid, params[])
{
    if(IsPlayerInStore(playerid))
    {
        new string[1024], count;
        DynamicGui_Init(playerid);
 
        format(string, sizeof(string), "%sProduct\tPrice\n", string);
 
        foreach (new prod: Products)
        {
            if( Product[prod][product_owner] != PRODUCT_OWNER_SHOP ) continue;
 
 
            format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
            DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);  
            count++;
        }
 
        if( count == 0 ) SendGuiInformation(playerid, "Information", "There are no products.");
        else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADD, DIALOG_STYLE_TABLIST_HEADERS, "24/7", string, "Buy", "Close");
    }
    else if(IsPlayerInAmmunation(playerid))
    {
    	if(!(pInfo[playerid][player_documents] & DOCUMENT_WEAPON)) return SendGuiInformation(playerid, "Information", "You don't have weapon license.");
    	new string[1024], count;
        DynamicGui_Init(playerid);
 
        format(string, sizeof(string), "%sWeapon\tPrice\n", string);
 
        foreach (new prod: Products)
        {
            if( Product[prod][product_owner] != PRODUCT_OWNER_AMMUNATION ) continue;
 
 
            format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
            DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);  
            count++;
        }
 
        if( count == 0 ) SendGuiInformation(playerid, "Information", "There are no products.");
        else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADD, DIALOG_STYLE_TABLIST_HEADERS, "Ammunation", string, "Buy", "Close");

    }
    else if(IsPlayerInToolShop(playerid))
    {
    	new string[1024], count;
        DynamicGui_Init(playerid);
 
        format(string, sizeof(string), "%sProduct\tPrice\n", string);
 
        foreach (new prod: Products)
        {
            if( Product[prod][product_owner] != PRODUCT_OWNER_TOOLSHOP ) continue;
 
 
            format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
            DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);  
            count++;
        }
 
        if( count == 0 ) SendGuiInformation(playerid, "Information", "There are no products.");
        else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADD, DIALOG_STYLE_TABLIST_HEADERS, "Toolshop", string, "Buy", "Close");
    }
    else if(IsPlayerInPharmacy(playerid))
    {
    	//if(IsAnyFirefighterOnline()) return SendGuiInformation(playerid, "Information", "Na s³u¿bie s¹ pracownicy ERU, w tym przypadku apteki automatyczne nie s¹ dostepne.");
        new string[1024], count;
        DynamicGui_Init(playerid);
 
        format(string, sizeof(string), "%sProduct\tPrice\n", string);
 
        foreach (new prod: Products)
        {
            if( Product[prod][product_owner] != PRODUCT_OWNER_PHARMACY ) continue;
 
 
            format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
            DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);  
            count++;
        }
 
        if( count == 0 ) SendGuiInformation(playerid, "Information", "There are no products.");
        else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADD, DIALOG_STYLE_TABLIST_HEADERS, "Pharmacy", string, "Buy", "Close");
    }
    else
    {
        SendGuiInformation(playerid, "Information", "You are not in shop.");
    }
    return 1;
}

COMMAND:helpers(playerid, params[])
{
	new count, str[300];
	foreach(new p : Player)
	{
		if( PlayerHasFlag(p, PLAYER_FLAG_HELPER) )
		{
			format(str, sizeof(str), "%s%d\tHELPER\t%s%s\n", str, p, HEX_COLOR_CREW_ADMIN, pInfo[p][player_name]);				
			count++;
		}
	}
	
	if( !count ) return SendGuiInformation(playerid, "Information", "There are no helpers online.");
	
	ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, "Helpers online:", str, "OK", "");
	return 1;
}

COMMAND:admins(playerid, params[])
{
	return cmd_a(playerid, params);
}

COMMAND:a(playerid, params[])
{
	new sub_cmd[128];
	if( sscanf(params, "s[128]", sub_cmd) )
	{
		new count, str[300];
	    DynamicGui_Init(playerid);
		foreach(new p : Player)
		{
			if( pInfo[p][player_admin_duty] )
			{
				if( HasCrewFlag(p, CREW_FLAG_GM) ) format(str, sizeof(str), "%s%d\tAdmin\t%s%s\n", str, p, HEX_COLOR_CREW_GM, pInfo[p][player_name]);
				if( HasCrewFlag(p, CREW_FLAG_ADMIN) ) format(str, sizeof(str), "%s%d\tLead Admin\t%s%s\n", str, p, HEX_COLOR_CREW_ADMIN, pInfo[p][player_name]);
				if( HasCrewFlag(p, CREW_FLAG_ADMIN_ROOT) ) format(str, sizeof(str), "%s%d\tManagement\t%s%s\n", str, p, HEX_COLOR_CREW_ADMIN_ROOT, pInfo[p][player_name]);				
				count++;
	            DynamicGui_AddRow(playerid, p);
			}
		}
		
		if( !count ) return SendGuiInformation(playerid, "Information", "There are no admins on duty.");
		
		ShowPlayerDialog(playerid, DIALOG_ADMIN_FLAGS, DIALOG_STYLE_LIST, "Admins on duty:", str, "Privileges", "Close");
		return 1;
	}
	else
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Alert", "Nie masz uprawnieñ do korzystania z tej komendy.");
		return cmd_ac(playerid, sub_cmd);
	}
}

COMMAND:pay(playerid, params[])
{
	new targetid, amount;
	if( sscanf(params, "dd", targetid, amount) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pay [player id] [amount]");
	if( amount <= 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pay [player id] [amount]");
	if( targetid == playerid ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pay [player id] [amount]");
	if( !IsPlayerConnected(targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");
	if( !pInfo[targetid][player_logged] ) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");
	if( pInfo[targetid][player_spec] != INVALID_PLAYER_ID) return SendClientMessage(playerid, COLOR_GREY, "Invalid player id.");
	if( pInfo[playerid][player_money] < amount ) return SendClientMessage(playerid, COLOR_GREY, "You don't have enough money.");

	new Float:dist;
	dist = GetDistanceBetweenPlayers(playerid, targetid);

	if(dist>5.0)
	{
		SendGuiInformation(playerid, "Information", "This player is too far from you.");
		return 1;
	}

	GivePlayerMoney(playerid, -amount);
	GivePlayerMoney(targetid, amount);

	ProxMessage(playerid, sprintf("gives some cash to %s.", pInfo[targetid][player_name]), PROX_SERWERME);
	
	SendFormattedClientMessage(playerid, COLOR_GOLD, "You've paid %s (%d) $%d.", pInfo[targetid][player_name], targetid, amount);
	SendFormattedClientMessage(targetid, COLOR_GOLD, "Player %s (%d) gave you $%d.", pInfo[playerid][player_name], playerid, amount);


	logprintf(LOG_PLAYER, "[PAY] %s (UID: %d) podal %d graczowi %s (UID %d)", pInfo[playerid][player_name], pInfo[playerid][player_id], amount, pInfo[targetid][player_name], pInfo[targetid][player_id]);
	
	return 1;
}

COMMAND:anim(playerid, params[])
{
	return cmd_anims(playerid, params);
}

COMMAND:anims(playerid, params[])
{
	new list_anims[3000];
	DynamicGui_Init(playerid);
	foreach(new anim_id: Anims)
	{
	    format(list_anims, sizeof(list_anims), "%s\n%s", list_anims, AnimInfo[anim_id][aCommand]);
		DynamicGui_AddRow(playerid, DG_ANIMS, anim_id);
	}

	if(strlen(list_anims))
	{
	    ShowPlayerDialog(playerid, DIALOG_ANIMATIONS, DIALOG_STYLE_LIST, "Animations list:", list_anims, "Start", "Close");
	}
	else
	{
	    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "There are no animations in database. Report it to admin.", "OK", "");
	}
	return 1;
}

COMMAND:g(playerid, params[])
{
	return cmd_groups(playerid, params);
}

COMMAND:groups(playerid, params[])
{
	if( !IsPlayerInAnyGroup(playerid) ) return SendGuiInformation(playerid, "Information", "You are not member of any group.");
	
	new slot, sub_cmd[20], param[60];
	if( sscanf(params, "dS()[20] S()[60]", slot, sub_cmd, param) )
	{
		ShowPlayerGroups(playerid);
		SendClientMessage(playerid, COLOR_GREY, "Tip: /g {slot} [info, online, duty, clothes, invite, kick, deposit, withdraw, vehicles, respawn, storage]");
		return 1;
	}
	else
	{
		slot -= 1;
		if( slot < 0 || slot > 4 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /g {slot} [info, online, duty, clothes, invite, kick, deposit, withdraw, vehicles, respawn, storage]");
		if( pGroup[playerid][slot][pg_id] == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /g {slot (1-5)} [info, online, duty, clothes, invite, kick, deposit, withdraw, vehicles, respawn, storage]");
		if( isnull(sub_cmd) ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /g %d [info, online, duty, clothes, invite, kick, deposit, withdraw, vehicles, respawn, storage]", slot+1));
		
		new gid = pGroup[playerid][slot][pg_id];
		
		if( !strcmp(sub_cmd, "info") )
		{
			new str[300];
			
			format(str, sizeof(str), "%sType:\t\t\t%s\n", str, GroupTypes[Group[gid][group_type]]);
			format(str, sizeof(str), "%sBudget:\t\t\t$%d\n", str, Group[gid][group_bank_money]);
			format(str, sizeof(str), "%sCapital:\t\t\t$%d\n", str, Group[gid][group_capital]);
			format(str, sizeof(str), "%sActivity points:\t%s%d\n", str, ((Group[gid][group_activity_points] >= 1000) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)), Group[gid][group_activity_points]);
			format(str, sizeof(str), "%sDotation:\t\t$%d\n", str, GetGroupDotation(gid));
			format(str, sizeof(str), "%sFor tributes:\t\t$%d\n", str, Group[gid][group_tribute]);
			if( Group[gid][group_parent_uid] > 0 ) format(str, sizeof(str), "%sParrent:\t\t%s (UID: %d)\n", str, Group[GetGroupByUid(Group[gid][group_parent_uid])][group_name], Group[gid][group_parent_uid]);
			new dslot = GetPlayerDutySlot(playerid);
			if( dslot > -1 && pGroup[playerid][dslot][pg_id] == gid ) 
			{
				new duty_time = gettime() - pGroup[playerid][dslot][pg_duty_start] - pInfo[playerid][player_onduty_afk];
				format(str, sizeof(str), "%sDuty time:\t\t%dm %ds\n", str, floatround(duty_time/60, floatround_floor), duty_time%60);
			}
			format(str, sizeof(str), "%sTag:\t\t\t%s\n", str, GetGroupTag(gid));
				
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) » Details", Group[gid][group_name], Group[gid][group_uid]), str, "OK", "");
		}
		else if( !strcmp(sub_cmd, "vehicles"))
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_VEHICLES) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);

			new str[600], rows, fields;
			
			mysql_query(mySQLconnection, sprintf("SELECT vehicle_uid, vehicle_model FROM `ipb_vehicles` WHERE `vehicle_ownertype` = %d AND `vehicle_owner` = %d", VEHICLE_OWNER_TYPE_GROUP, Group[gid][group_uid]));
			cache_get_data(rows, fields);
			
			if( !rows ) SendGuiInformation(playerid, "Information", "This group has no vehicles.");
			else
			{
				DynamicGui_Init(playerid);
				for(new row = 0; row != rows; row++)
				{
					new uid = cache_get_row_int(row, 0);
					
					format(str, sizeof(str), "%s%s%d\t\t%s\n", str, ((GetVehicleByUid(uid) != INVALID_VEHICLE_ID) ? (HEX_COLOR_GREEN) : (HEX_COLOR_WHITE)), uid, VehicleNames[cache_get_row_int(row, 1)-400]);
					DynamicGui_AddRow(playerid, uid);
				}
			
				ShowPlayerDialog(playerid, DIALOG_GROUP_VEHICLES, DIALOG_STYLE_LIST, sprintf("%s vehicles:", Group[gid][group_name]), str, "(Un)Spawn", "Close");
			}

		}
		else if( !strcmp(sub_cmd, "chat") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);	

			if(Group[gid][group_chat] == false)
			{
				SendClientMessage(playerid, COLOR_GOLD, sprintf("Chat of %s has been disabled.", Group[gid][group_name]));
				Group[gid][group_chat] = true;
			}
			else
			{
				SendClientMessage(playerid, COLOR_GOLD, sprintf("Chat of %s has been enabled.", Group[gid][group_name]));
				Group[gid][group_chat] = false;
			}
		}
		else if( !strcmp(sub_cmd, "respawn") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			new count, str[95];
			foreach(new vid : Vehicles)
			{
				if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP && Vehicle[vid][vehicle_owner] == Group[gid][group_uid] )
				{
					if( IsVehicleOccupied(vid) ) continue;
					
					SetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]-1.1);
					SetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
					SetVehicleVirtualWorld(vid, Vehicle[vid][vehicle_park_world]);
					LinkVehicleToInterior(vid, Vehicle[vid][vehicle_park_interior]);
					Vehicle[vid][vehicle_locked] = true;
					Vehicle[vid][vehicle_engine] = false;
					UpdateVehicleVisuals(vid);
					count++;
				}
			}

			format(str, sizeof(str), "[Group %s]: %s has respawned all group vehicles.", GetGroupTag(gid), pInfo[playerid][player_name]);

			foreach(new p: Player)
			{
				if( GetPlayerGroupSlot(p, gid) != -1 )
				{
					SendClientMessage(p, COLOR_GOLD, str);
				}
			}
			
			SendGuiInformation(playerid, "Information", sprintf("Respawned %d vehicles in group %s.", count, Group[gid][group_name]));
		}
		else if( !strcmp(sub_cmd, "duty") )
		{
			new dslot = GetPlayerDutySlot(playerid);
			if( dslot != -1 && pGroup[playerid][dslot][pg_id] != gid ) return SendGuiInformation(playerid, "Information", sprintf("You are on other slot duty. Use /g %d duty to leave it.", dslot+1));
			if( pGroup[playerid][slot][pg_duty] )
			{
				pGroup[playerid][slot][pg_duty] = false;
				
				new duty_time = gettime() - pGroup[playerid][slot][pg_duty_start] - pInfo[playerid][player_onduty_afk];
				new color = GetGroupColor(gid);
				new session_start = pGroup[playerid][slot][pg_duty_start];

				new activity = floatround(duty_time/60, floatround_floor)*10;
				Group[gid][group_activity_points] += activity;

				activity = activity/10;

				if(Group[gid][group_tributers] > 0)
				{
					if(Group[gid][group_tribute] < 3500 && activity > 0)
					{
						Group[gid][group_tribute] += 5*activity;
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_game_groups SET group_tribute = %d WHERE group_uid = %d", Group[gid][group_tribute], Group[gid][group_uid]));
					}
				}

				mysql_query(mySQLconnection, sprintf("UPDATE ipb_game_groups SET group_activity = %d WHERE group_uid = %d", Group[gid][group_activity_points], Group[gid][group_uid]));

				SendFormattedClientMessage(playerid, color, "You've left %s (UID: %d) duty. Time spent on duty: %d minutes %d seconds.", Group[gid][group_name], Group[gid][group_uid], floatround(duty_time/60, floatround_floor), duty_time%60);
				pInfo[playerid][player_onduty_afk] = 0;
				pInfo[playerid][player_duty_gid] = -1;
				mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_game_sessions` (session_uid, session_type, session_extraid, session_owner, session_start, session_end) VALUES (null, %d, %d, %d, %d, %d)", SESSION_TYPE_DUTY, Group[gid][group_uid], pInfo[playerid][player_id], session_start, (gettime() - pInfo[playerid][player_onduty_afk])));
				
				UpdatePlayerLabel(playerid);
				
				PlayerTextDrawHide(playerid, GroupDutyTag);
			}
			else
			{
                if( GroupHasFlag(gid, GROUP_FLAG_DUTY) )
                {
                    if(GetPlayerVirtualWorld(playerid) == 0)
                    {
                        new area = pInfo[playerid][player_area];
                        if(area < 1) return SendGuiInformation(playerid, "Information", "This group has limited duty flag. You can use this command only in group buildings or areas.");
                        if(Area[area][area_owner_type] != AREA_OWNER_TYPE_GROUP ) return SendGuiInformation(playerid, "Information", "This group has limited duty flag. You can use this command only in group buildings or areas.");
                        if(Area[area][area_owner] != Group[gid][group_uid] ) return SendGuiInformation(playerid, "Information", "This group has limited duty flag. You can use this command only in group buildings or areas.");
                    }
                    else
                    {
                        new door = GetDoorByUid(GetPlayerVirtualWorld(playerid));
                        if(door == -1) return SendGuiInformation(playerid, "Information", "This group has limited duty flag. You can use this command only in group buildings or areas.");
                        if(Door[door][door_owner_type] != DOOR_OWNER_TYPE_GROUP) return SendGuiInformation(playerid, "Information", "This group has limited duty flag. You can use this command only in group buildings or areas.");
                        if(Door[door][door_owner] != Group[gid][group_uid]) return SendGuiInformation(playerid, "Information", "This group has limited duty flag. You can use this command only in group buildings or areas.");
                    }
                }
				new color = GetGroupColor(gid);
				SendFormattedClientMessage(playerid, color, "You are now on %s (UID: %d) duty. Use /g %d duty again to leave duty.", Group[gid][group_name], Group[gid][group_uid], slot+1);
				pGroup[playerid][slot][pg_duty] = true;
				pGroup[playerid][slot][pg_duty_start] = gettime();

				PlayerTextDrawBackgroundColor(playerid, GroupDutyTag, color);
				PlayerTextDrawSetString(playerid, GroupDutyTag, GetGroupTag(gid));
				PlayerTextDrawShow(playerid, GroupDutyTag);

				pInfo[playerid][player_duty_gid] = gid;

				UpdatePlayerLabel(playerid);
			}
		}
		else if( !strcmp(sub_cmd, "storage") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_OFFER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			new list_items[600], header[64];

			DynamicGui_Init(playerid);
			format(header, sizeof(header), "UID\tName\tPrice\tAmount\n");

			foreach(new itemid : Items)
			{
				if( Item[itemid][item_count] == 0 ) continue;
				
				if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_GROUP && Item[itemid][item_owner] == Group[gid][group_uid] )
				{
					format(list_items, sizeof(list_items), "%s %d\t\t%s\t\t%d\t\t%d\n", list_items, Item[itemid][item_uid], Item[itemid][item_name], Item[itemid][item_price], Item[itemid][item_count]);
					DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, itemid);
				}
			}
			
			if( strlen(list_items) )
			{
				if( WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) )
				{
					TextDrawSetString(Tutorial[playerid], "~w~Listed products of your grup. You are leader, so you can change ~b~~h~~h~prices~w~ in GameCP or /leader price.");
					TextDrawShowForPlayer(playerid, Tutorial[playerid]);
				}
				format(list_items, sizeof(list_items), "%s%s", header, list_items);
				ShowPlayerDialog(playerid, DIALOG_GROUP_MAGAZYN, DIALOG_STYLE_TABLIST_HEADERS, sprintf("Products of %s [UID: %d]:", Group[gid][group_name], Group[gid][group_uid]), list_items, "Offer", "Close");
			}
			else
			{
				SendGuiInformation(playerid, "Information", "This group has no products in storage.");
			}
		}
		else if( !strcmp(sub_cmd, "clothes") )
		{
			new rows, fields;
			mysql_query(mySQLconnection, sprintf("SELECT group_skin FROM ipb_char_groups WHERE `char_uid` = %d AND group_belongs = %d", pInfo[playerid][player_id], Group[gid][group_uid]));
			cache_get_data(rows, fields);

			new skin = cache_get_row_int(0, 0);

			if(skin==0) return SendGuiInformation(playerid, "Information", "You don't have group skin. Ask leader.");
			
			SetPlayerSkin(playerid, skin);
			pInfo[playerid][player_skin] = skin;
		}
		else if( !strcmp(sub_cmd, "online") )
		{
			new str[600], pslot;
			foreach(new p : Player)
			{
				pslot = GetPlayerGroupSlot(p, gid);
				if( pslot > -1 )
				{
					if( pGroup[p][pslot][pg_duty] ) format(str, sizeof(str), "%s"HEX_COLOR_GREEN"%d\t\t%s\n", str, p, pInfo[p][player_name]);
					else format(str, sizeof(str), "%s%d\t\t%s\n", str, p, pInfo[p][player_name]);
				}
			}
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) » Members online", Group[gid][group_name], Group[gid][group_uid]), str, "OK", "");
		}
		else if( !strcmp(sub_cmd, "invite") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			new targetid;
			if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)roup %d invite [player id]", slot+1));
			if( targetid == playerid ) return SendGuiInformation(playerid, "Information", "You can't invite yourself.");

			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
			if( GetPlayerGroupSlot(targetid, gid) > -1 ) return SendGuiInformation(playerid, "Information", "This player is already belongs to this group.");			
			new free_slot = GetPlayerGroupFreeSlot(targetid);
			if( free_slot == -1 ) return SendGuiInformation(playerid, "Information", "This player has no free slot.");
			
			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_char_groups` (char_uid, group_belongs, group_perm) VALUES (%d, %d, %d)", pInfo[targetid][player_id], Group[gid][group_uid], 0));
				
			pGroup[targetid][free_slot][pg_id] = gid;
			pGroup[targetid][free_slot][pg_rank_perm] = 0;
			SendGuiInformation(playerid, "Information", sprintf("%s has been invited to %s (UID: %d).", pInfo[targetid][player_name], Group[gid][group_name], Group[gid][group_uid]));
			SendGuiInformation(targetid, "Information", sprintf("%s invited you to %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
		}
		else if( !strcmp(sub_cmd, "kick") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			new targetid;
			if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)roup %d kick [player id]", slot+1));
			if( targetid == playerid ) return SendGuiInformation(playerid, "Information", "You can't kick yourself.");
			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
			
			new pslot = GetPlayerGroupSlot(targetid, gid);
			if( pslot == -1 ) return SendGuiInformation(playerid, "Information", "This player is not belongs to this group");
			
			mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_char_groups` WHERE `char_uid` = %d AND `group_belongs` = %d", pInfo[targetid][player_id], Group[gid][group_uid]));
			
			if( pGroup[targetid][pslot][pg_duty] )
			{
				PlayerTextDrawHide(targetid, GroupDutyTag);
				if(Group[pslot][group_type] == GROUP_TYPE_LSPD || Group[pslot][group_type] == GROUP_TYPE_MEDIC || Group[pslot][group_type] == GROUP_TYPE_SN)
				UpdatePlayerLabel(targetid);

			}	
			pInfo[targetid][player_onduty_afk] = 0;
			pInfo[targetid][player_duty_gid] = 0;

			CleanPlayerGroup(targetid, pslot);
			
			SendGuiInformation(playerid, "Information", sprintf("Player %s has been kicked from %s (UID: %d).", pInfo[targetid][player_name], Group[gid][group_name], Group[gid][group_uid]));
			SendGuiInformation(targetid, "Information", sprintf("%s kicked you from %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
		}
		else if( !strcmp(sub_cmd, "leave") )
		{
			new pslot = GetPlayerGroupSlot(playerid, gid);
			if( pslot == -1 ) return SendGuiInformation(playerid, "Information", "No group at this slot.");
			
			mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_char_groups` WHERE `char_uid` = %d AND `group_belongs` = %d", pInfo[playerid][player_id], Group[gid][group_uid]));
			
			if( pGroup[playerid][pslot][pg_duty] ) PlayerTextDrawHide(playerid, GroupDutyTag);
			
			CleanPlayerGroup(playerid, pslot);
			
			SendGuiInformation(playerid, "Information", sprintf("You've left %s (UID: %d).", Group[gid][group_name], Group[gid][group_uid]));
		}
		else if( !strcmp(sub_cmd, "deposit") )
		{
			if(!IsPlayerInBank(playerid)) return SendGuiInformation(playerid, "Information", "You are not in bank.");
			new amount;
			if( sscanf(param, "d", amount) ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)roup %d deposit [amount]", slot+1));
			if( amount < 0 ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)roup %d deposit [amount]", slot+1));
			if( pInfo[playerid][player_money] < amount ) return SendGuiInformation(playerid, "Information", "You don't have enough cash.");
			
			GivePlayerMoney(playerid, -amount);
			GiveGroupCapital(gid, amount);
			
			SendGuiInformation(playerid, "Information", sprintf("Deposited $%d to capital of group %s (UID: %d).", amount, Group[gid][group_name], Group[gid][group_uid]));
		}
		else if( !strcmp(sub_cmd, "withdraw") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			new amount;
			if( sscanf(param, "d", amount) ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)roup %d withdraw [amount]", slot+1));
			if( amount < 0 ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)roup %d withdraw [amount]", slot+1));
			if( Group[gid][group_bank_money] < amount ) return SendGuiInformation(playerid, "Information", "This group has no enough money in budget.");
			
			GivePlayerMoney(playerid, amount);
			GiveGroupMoney(gid, -amount);
			
			SendGuiInformation(playerid, "Information", sprintf("Paid $%d from budget of group %s (UID: %d).", amount, Group[gid][group_name], Group[gid][group_uid]));
		}
		else return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /g %d [info, online, duty, clothes, invite, kick, deposit, withdraw, vehicles, respawn, storage]", slot+1));
	}
	return 1;
}

COMMAND:fine(playerid, params[])
{	
	new gid = pInfo[playerid][player_duty_gid];

	if(gid == -1)
	{
		SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
		return 1;
	}

	if(Group[gid][group_type] != GROUP_TYPE_LSPD)
	{
		SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
		return 1;
	}

	new playa, price, powod[64];
	if( sscanf(params, "dds[64]", playa, price, powod) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /fine [id] [cost] [reason]");
	}
	else
	{
		if(!IsPlayerConnected(playa)||playa==playerid) return SendGuiInformation(playerid, "Information", "Invalid player id.");
		if( !pInfo[playa][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
		if(price<0) return SendGuiInformation(playerid, "Information", "Invalid price.");
		new resp = SetOffer(playerid, playa, OFFER_TYPE_MANDAT, price, 0);

		if( resp ) ShowPlayerOffer(playa, playerid, "Fine", sprintf("Fine ($%d)~n~Reason: %s", price, powod), price);
	}
	return 1;
}
// ---- Obiekty ---- //

COMMAND:blockade(playerid, params[])
{	
	new gid = pInfo[playerid][player_duty_gid];
	new owner = 0;
	new owner_type = OBJECT_OWNER_TYPE_GLOBAL;

	if(gid == -1)
	{
		SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
		return 1;
	}

	if(!(Group[gid][group_flags] & GROUP_FLAG_BLOCKADE))
	{
		SendGuiInformation(playerid, "Information", "You are not on LSPD duty.");
		return 1;
	}

	new model;
	if( sscanf(params, "d", model) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /blockade [model]");
		TextDrawSetString(Tutorial[playerid], "Blockade ~p~models~w~:~n~19834, 1949, 1427, 1424, 1423, 1459, 1237, 3091, 3578. 1425, 18728(flara), 1997(nosze)~n~Key ~p~N~w~ to close this window.");
		TextDrawShowForPlayer(playerid, Tutorial[playerid]);
	}
	else
	{
		if( model < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /blockade [model]");
		if(!IsValidBlockadeModel(model)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /blockade [model]");
		
		new Float:o_pos[3];
		GetPointInAngleOfPlayer(playerid, o_pos[0], o_pos[1], o_pos[2], 3.0, 0.0);
		
		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_objects` (object_uid, object_type, object_ownertype, object_owner, object_map, object_model, object_world, object_posx, object_posy, object_posz) VALUES (null, %d, %d, %d, 0, %d, %d, %f, %f, %f)", OBJECT_TYPE_BLOCKADE, owner_type, owner, model, GetPlayerVirtualWorld(playerid), o_pos[0], o_pos[1], o_pos[2]));

		new uid = cache_insert_id(), oid = LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
		
		RefreshPlayer(playerid);

		EditDynamicObject(playerid, oid);
	}
	return 1;
}

COMMAND:oc(playerid, params[])
{
	return cmd_mc(playerid, params);
}

COMMAND:mc(playerid, params[])
{	
	new owner = -1, owner_type = -1;
	GetObjectDataForPlayer(playerid, owner, owner_type);
	
	if( owner_type == -1 ) return SendGuiInformation(playerid, "Information", "You don't have access to place objects there.");
	
	if( owner_type == OBJECT_OWNER_TYPE_DOOR )
	{
		new d_id = GetDoorByUid(owner);
		if( CountDoorObjects(d_id) >= Door[d_id][door_objects_limit] ) return SendGuiInformation(playerid, "Information", "This building has reached object limit.");
	}
	
	if( owner_type == OBJECT_OWNER_TYPE_AREA )
	{
		new a_id = GetAreaByUid(owner);
		if( CountAreaObjects(a_id) >= Area[a_id][area_objects_limit] ) return SendGuiInformation(playerid, "Information", "This area has reached object limit.");
	}
	
	new model;
	if( sscanf(params, "d", model) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mc [model]");
	if( model < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mc [model]");

	if(model == OBJECT_ATM)
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_EDITOR) )
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /mc [model]");
			return 1;
		}
	}
	
	new Float:o_pos[3];
	GetPointInAngleOfPlayer(playerid, o_pos[0], o_pos[1], o_pos[2], 3.0, 0.0);
	
	mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_objects` (object_uid, object_type, object_ownertype, object_owner, object_map, object_model, object_world, object_posx, object_posy, object_posz) VALUES (null, %d, %d, %d, 0, %d, %d, %f, %f, %f)", OBJECT_TYPE_NORMAL, owner_type, owner, model, GetPlayerVirtualWorld(playerid), o_pos[0], o_pos[1], o_pos[2]));

	new uid = cache_insert_id(), oid = LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
	
	RefreshPlayer(playerid);
	
	switch( owner_type )
	{
		case OBJECT_OWNER_TYPE_GLOBAL:
		{
			SendPlayerInformation(playerid, sprintf("Global object ~y~created~w~ (UID: %d, ID: %d, MODEL: %d).", uid, oid, model), 4000);
		}
		
		case OBJECT_OWNER_TYPE_DOOR:
		{
			SendPlayerInformation(playerid, sprintf("Door object ~y~created~w~ (UID: %d, ID: %d, MODEL: %d).", uid, oid, model), 4000);
		}
		
		case OBJECT_OWNER_TYPE_AREA:
		{
			SendPlayerInformation(playerid, sprintf("Area object ~y~created~w~ (UID: %d, ID: %d, MODEL: %d).", uid, oid, model), 4000);
		}
	}
	return 1;
}

COMMAND:getvw(playerid, params[])
{	
	if(pInfo[playerid][player_admin_duty] == true)
	{
		new id;
    	if(sscanf(params, "d", id))
    	{
    		SendGuiInformation(playerid, "Information", sprintf("You virtual world is %d.", GetPlayerVirtualWorld(playerid)));
    	}
		else
		{
			if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, ""guiopis"Alert", "Invalid player id.");
			SendGuiInformation(playerid, "Information", sprintf("World for player id %d is %d.", id, GetPlayerVirtualWorld(id)));
		}
	}
	else
	{
		SendGuiInformation(playerid, "Information", sprintf("Your virtual world is %d.", GetPlayerVirtualWorld(playerid)));
	}
	return 1;
}

COMMAND:osel(playerid, params[])
{
	return cmd_msel(playerid, params);
}

COMMAND:msel(playerid, params[])
{
	new model;
	if( sscanf(params, "d", model) )
	{
		SelectObject(playerid);
	}
	else
	{
		new pretend_id = -1, Float:pretend_distance = 15.0;
		foreach(new oid : Objects)
		{
			if( Object[oid][object_model] != model || !CanPlayerEditObject(playerid, oid) || IsObjectEdited(oid) ) continue;

			new Float:distance = GetPlayerDistanceToObject(playerid, oid);
			if( distance < pretend_distance )
			{
				pretend_id = oid;
				pretend_distance = distance;
			}
		}
		
		if( pretend_id == -1 ) return SendPlayerInformation(playerid, "There is no object with this model id in your area.", 4000);
		
		new o_id = pretend_id;
		EditDynamicObject(playerid, o_id);
		
		Object[o_id][object_is_edited] = true;
		pInfo[playerid][player_edited_object] = o_id;

		// Pobieramy sobie poczatkowa pozycje obiektu w razie jakiegos bledu
		GetDynamicObjectPos(o_id, pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]);
		GetDynamicObjectRot(o_id, pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5]);
		
		Object[o_id][object_pos][0] = pInfo[playerid][player_edited_object_pos][0];
		Object[o_id][object_pos][1] = pInfo[playerid][player_edited_object_pos][1];
		Object[o_id][object_pos][2] = pInfo[playerid][player_edited_object_pos][2];
		Object[o_id][object_pos][3] = pInfo[playerid][player_edited_object_pos][3];
		Object[o_id][object_pos][4] = pInfo[playerid][player_edited_object_pos][4];
		Object[o_id][object_pos][5] = pInfo[playerid][player_edited_object_pos][5];
		
		UpdateObjectInfoTextdraw(playerid, o_id);
		TextDrawShowForPlayer(playerid, Dashboard[playerid]);
	}
	
	return 1;
}

COMMAND:odel(playerid, params[])
{
	return cmd_mdel(playerid, params);
}

COMMAND:mdel(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Information", "You are not in object edit mode.");
	
	SendPlayerInformation(playerid, sprintf("Object ~y~deleted~w~ (UID: %d i ID: %d).", Object[pInfo[playerid][player_edited_object]][object_uid], pInfo[playerid][player_edited_object]), 4000);
		
	DeleteObject(pInfo[playerid][player_edited_object]);
	CancelEdit(playerid);
	
	if(Object[pInfo[playerid][player_edited_object]][object_area] !=0)
	{
		DestroyDynamicArea(Object[pInfo[playerid][player_edited_object]][object_area]);
	}
	
	pInfo[playerid][player_edited_object] = -1;
	TextDrawHideForPlayer(playerid, Dashboard[playerid]);
	return 1;
}

COMMAND:ocopy(playerid, params[])
{
	return cmd_mkopia(playerid,params);
}

COMMAND:mk(playerid, params[])
{
	return cmd_mkopia(playerid,params);
}

COMMAND:mcopy(playerid, params[])
{
	return cmd_mkopia(playerid,params);
}

COMMAND:mkopia(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Information", "You are not in object edit mode.");
	
	new owner = -1, owner_type = -1;
	GetObjectDataForPlayer(playerid, owner, owner_type);

	if( owner_type == OBJECT_OWNER_TYPE_AREA )
	{
		new a_id = GetAreaByUid(owner);
		if( CountAreaObjects(a_id) >= Area[a_id][area_objects_limit] ) return SendGuiInformation(playerid, "Information", "This area has reached object limit.");
	}

	SendPlayerInformation(playerid, sprintf("Object ~y~cloned~w~ (UID: %d i ID: %d).", Object[pInfo[playerid][player_edited_object]][object_uid], pInfo[playerid][player_edited_object]), 4000);

	new o_id = pInfo[playerid][player_edited_object];
	new material[100], index;

	new Float:o_pos[6];

	GetDynamicObjectPos(o_id, o_pos[0], o_pos[1], o_pos[2]);
	GetDynamicObjectRot(o_id, o_pos[3], o_pos[4], o_pos[5]);

	/*new mid[16], txdname[16][64], tname[16][64], clr[16];
	for(new index; index < 16; index++)
	{
		if(IsDynamicObjectMaterialUsed(o_id, index))
		{
			GetDynamicObjectMaterial(o_id, index, mid[index], txdname[index], tname[index], clr[index], 64, 64);
		}
	}*/
	
	if(Object[o_id][object_textured] > 0)
	{
		new rows, fields;
		mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_materials WHERE material_owner = %d", Object[o_id][object_uid]));
		cache_get_data(rows, fields);

		cache_get_row(0, 1, material);
		index = cache_get_row_int(0, 3);
	}

	mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_objects` (object_uid, object_type, object_ownertype, object_owner, object_map, object_model, object_world, object_posx, object_posy, object_posz) VALUES (null, %d, %d, %d, 0, %d, %d, '%f', '%f', '%f')", OBJECT_TYPE_NORMAL, Object[o_id][object_owner_type], Object[o_id][object_owner], Object[o_id][object_model], GetPlayerVirtualWorld(playerid), o_pos[0], o_pos[1], o_pos[2]));
	new uid = cache_insert_id();
	mysql_query(mySQLconnection, sprintf("UPDATE ipb_objects SET object_rotx = '%f', object_roty = '%f', object_rotz = '%f' WHERE object_uid = %d", o_pos[3], o_pos[4], o_pos[5], uid));

	if(Object[o_id][object_textured] > 0)
	{
		mysql_query(mySQLconnection, sprintf("INSERT INTO ipb_materials (material_id, material_texture, material_owner, material_index) VALUES (null, '%s', %d, %d)", material, uid, index));
	}

	LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);

	CancelEdit(playerid);

	RefreshPlayer(playerid);
	TextDrawHideForPlayer(playerid, Dashboard[playerid]);
	return 1;
}

COMMAND:omat(playerid, params[])
{
	return cmd_mmat(playerid,params);
}

COMMAND:mmat(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Information", "You are not in object edit mode.");
	if( strfind(params, "|", true) != -1) return SendGuiInformation(playerid, "Information", "Detected invalid characters.");
	new sub_cmd, param[100];
	if( sscanf(params, "d S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mmat [0-1] (parameters)");
	
	switch(sub_cmd)
	{
		// tekstury
		case 0:
		{
			new index, color, model, txd_name[64], texture_name[64];
			if( sscanf(param, "dxds[64]s[64]", index, color, model, txd_name, texture_name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mmat 0 [Index (0-15)] [Kolor (ARGB)] [Model] [Txdname] [Texturename]");
			if( index < 0 || index > 15 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mmat 0 [Index (0-15)] [Kolor (ARGB)] [Model] [Txdname] [Texturename]");
			if(strlen(texture_name) > 30) return SendGuiInformation(playerid, "Information", "Texture name too long.");
			if(strlen(txd_name) > 30) return SendGuiInformation(playerid, "Information", "Txd name too long.");

			mysql_real_escape_string(texture_name, texture_name, mySQLconnection, 64);
			mysql_real_escape_string(txd_name, txd_name, mySQLconnection, 64);
			
			new o_id = pInfo[playerid][player_edited_object], rows, fields;

			mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_materials WHERE material_owner = %d AND material_index = %d LIMIT 1", Object[o_id][object_uid], index));
			cache_get_data(rows, fields);

			if( rows )
			{
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_materials SET material_texture = '0^%d:%06x:%d:%s:%s' WHERE material_owner = %d AND material_index = %d", index, color, model, txd_name, texture_name, Object[o_id][object_uid], index));
			}
			else
			{
				mysql_query(mySQLconnection, sprintf("INSERT INTO ipb_materials SET material_texture = '0^%d:%06x:%d:%s:%s', material_owner = %d, material_index = %d", index, color, model, txd_name, texture_name, Object[o_id][object_uid], index));
			}
			
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_objects` SET `object_textured` = 1, `object_posx` = %f, `object_posy` = %f, `object_posz` = %f, `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5], Object[pInfo[playerid][player_edited_object]][object_uid]));
			
			pInfo[playerid][player_edited_object_no_action] = true;
			CancelEdit(playerid);
			
			new uid = Object[o_id][object_uid];
			
			pInfo[playerid][player_edited_object] = -1;
			Object[o_id][object_is_edited] = false;
			
			TextDrawHideForPlayer(playerid, Dashboard[playerid]);
			
			DeleteObject(o_id, false);
			LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
			
			RefreshPlayer(playerid);
			
		}
		
		// napisy
		case 1:
		{
			new index, msize, fsize, bold, fcolor, bcolor, align, font[32], text[128];
			if( sscanf(param, "ddddxxds[32]s[128]", index, msize, fsize, bold, fcolor, bcolor, align, font, text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mmat 1 [index (0-15)] [Matsize (10-140) [Fontsize (24-255)] [Bold] [Fontcol] [Backcol] [Align (0-2)] [Font] [Txt]");
			if( index < 0 || index > 15 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mmat 1 [index (0-15)] [Matsize (10-140) [Fontsize (24-255)] [Bold] [Fontcol] [Backcol] [Align (0-2)] [Font] [Txt]");
			if(strlen(text) > 50) return SendGuiInformation(playerid, "Information", "Input text too long.");
			if(strlen(font) > 10) return SendGuiInformation(playerid, "Information", "Font text too long.");
			if( strfind(text, ":", true) != -1 || strfind(text, "^", true) != -1 ) return SendGuiInformation(playerid, "Information", "Invalid characters: ^ and :.");
			
			mysql_real_escape_string(font, font, mySQLconnection, 32);
			mysql_real_escape_string(text, text, mySQLconnection, 128);

			new o_id = pInfo[playerid][player_edited_object], rows, fields;

			mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_materials WHERE material_owner = %d AND material_index = %d LIMIT 1", Object[o_id][object_uid], index));
			cache_get_data(rows, fields);

			if( rows )
			{
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_materials SET material_texture = '1^%d:%d:%d:%d:%06x:%06x:%d:%s:%s' WHERE material_owner = %d AND material_index = %d", index, msize, fsize, bold, fcolor, bcolor, align, font, text, Object[o_id][object_uid], index));
			}
			else
			{
				mysql_query(mySQLconnection, sprintf("INSERT INTO ipb_materials SET material_texture = '1^%d:%d:%d:%d:%06x:%06x:%d:%s:%s', material_owner = %d, material_index = %d",index, msize, fsize, bold, fcolor, bcolor, align, font, text, Object[o_id][object_uid], index));
			}
			
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_objects` SET `object_textured` = 1, `object_posx` = %f, `object_posy` = %f, `object_posz` = %f, `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5], Object[pInfo[playerid][player_edited_object]][object_uid]));
			
			pInfo[playerid][player_edited_object_no_action] = true;
			CancelEdit(playerid);
			
			new uid = Object[o_id][object_uid];
			
			pInfo[playerid][player_edited_object] = -1;
			Object[o_id][object_is_edited] = false;
			
			TextDrawHideForPlayer(playerid, Dashboard[playerid]);
			
			DeleteObject(o_id, false);
			LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
			
			RefreshPlayer(playerid);
			
		}
	}
	
	return 1;
}


COMMAND:rx(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Information", "You are not in object edit mode.");
	
	new Float:rxs;
	if( sscanf(params, "f", rxs) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /rx [amount]");
	
	new o_id = pInfo[playerid][player_edited_object];
	
	SetDynamicObjectRot(pInfo[playerid][player_edited_object], 360.0-rxs, Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
	
	OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_UPDATE, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], 360.0-rxs, Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
	pInfo[playerid][player_edited_object_no_action] = true;
	
	CancelEdit(playerid);
	EditDynamicObject(playerid, pInfo[playerid][player_edited_object]);

	return 1;
}

COMMAND:ry(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Information", "You are not in object edit mode.");
	
	new Float:rys;
	if( sscanf(params, "f", rys) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ry [amount]");
	
	new o_id = pInfo[playerid][player_edited_object];
	
	SetDynamicObjectRot(pInfo[playerid][player_edited_object],  Object[o_id][object_pos][3], 360.0-rys, Object[o_id][object_pos][5]);
	
	OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_UPDATE, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], 360.0-rys, Object[o_id][object_pos][5]);
	pInfo[playerid][player_edited_object_no_action] = true;
	
	CancelEdit(playerid);
	EditDynamicObject(playerid, pInfo[playerid][player_edited_object]);
	
	return 1;
}

COMMAND:rz(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Information", "You are not in object edit mode.");
	
	new Float:rzs;
	if( sscanf(params, "f", rzs) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /rz [amount]");
	
	new o_id = pInfo[playerid][player_edited_object];
	
	SetDynamicObjectRot(pInfo[playerid][player_edited_object],  Object[o_id][object_pos][3], Object[o_id][object_pos][4], 360.0-rzs);
	
	OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_UPDATE, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], 360.0-rzs);
	pInfo[playerid][player_edited_object_no_action] = true;
	
	CancelEdit(playerid);
	EditDynamicObject(playerid, pInfo[playerid][player_edited_object]);
	
	return 1;
}

COMMAND:oz(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Information", "You are not in object edit mode.");
	
	new Float:pzs;
	if( sscanf(params, "f", pzs) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /oz [amount]");
	
	new o_id = pInfo[playerid][player_edited_object];
	
	SetDynamicObjectPos(pInfo[playerid][player_edited_object],  Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]+pzs);
	
	OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_UPDATE, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]+pzs, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
	return 1;
}

COMMAND:ox(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Information", "You are not in object edit mode.");
	
	new Float:pzs;
	if( sscanf(params, "f", pzs) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ox [amount]");
	
	new o_id = pInfo[playerid][player_edited_object];
	
	SetDynamicObjectPos(pInfo[playerid][player_edited_object],  Object[o_id][object_pos][0]+pzs, Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
	
	OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_UPDATE, Object[o_id][object_pos][0]+pzs, Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
	return 1;
}

COMMAND:oy(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Information", "You are not in object edit mode.");
	
	new Float:pzs;
	if( sscanf(params, "f", pzs) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /oy [amount]");
	
	new o_id = pInfo[playerid][player_edited_object];
	
	SetDynamicObjectPos(pInfo[playerid][player_edited_object],  Object[o_id][object_pos][0], Object[o_id][object_pos][1]+pzs, Object[o_id][object_pos][2]);
	
	OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_UPDATE, Object[o_id][object_pos][0], Object[o_id][object_pos][1]+pzs, Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
	return 1;
}

// ---- 3d teksty ---- //
COMMAND:ec(playerid, params[])
{
	new owner = -1, owner_type = -1;
	GetLabelDataForPlayer(playerid, owner, owner_type);
	
	if( owner_type == -1 ) return SendGuiInformation(playerid, "Information", "You don't have access to place label there.");
	
	if( owner_type == LABEL_OWNER_TYPE_DOOR )
	{
		new d_id = GetDoorByUid(owner), count = CountDoorLabels(d_id);
		
		if( !HasCrewFlag(playerid, CREW_FLAG_EDITOR) )
		{
			if( IsPlayerVip(playerid) && count >= 5 ) return SendGuiInformation(playerid, "Information", "You can place maximum 5 text labels with premium account.");
			else if( !IsPlayerVip(playerid) && count >= 1 ) return SendGuiInformation(playerid, "Information", "You can place maximum 1 text labels without premium account.");
		}
	}
	
	if( owner_type == LABEL_OWNER_TYPE_AREA )
	{
		new a_id = GetAreaByUid(owner), count = CountAreaLabels(a_id);
		
		if( !HasCrewFlag(playerid, CREW_FLAG_EDITOR) )
		{
			if( IsPlayerVip(playerid) && count >= 5 ) return SendGuiInformation(playerid, "Information", "Posiadaj¹c konto premium mo¿esz stworzyæ maksymalnie 5 3d textow w ka¿dej strefie.");
			else if( !IsPlayerVip(playerid) && count >= 1 ) return SendGuiInformation(playerid, "Information", "Nie posiadaj¹c konta premium mo¿esz stworzyæ maksymalnie 1 3d text w ka¿dej strefie.");
		}
	}
	
	new Float:distance, text[256];
	if( sscanf(params, "fs[256]", distance, text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ec [draw distance] [text]");
	if(strlen(text) > 120) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ec [draw distance] [text]");
	if( distance <= 0.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ec [draw distance] [text]");
	
	new Float:p_pos[3];
	GetPointInAngleOfPlayer(playerid, p_pos[0], p_pos[1], p_pos[2], 2.0, 0.0);
	mysql_real_escape_string(text, text, mySQLconnection, 256);
	mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_3dlabels` (label_uid, label_owner, label_ownertype, label_desc, label_color, label_posx, label_posy, label_posz, label_drawdist, label_world, label_interior) VALUES (null, %d, %d, '%s', '%s', %f, %f, %f, %f, %d, %d)", owner, owner_type, text, "FFFFFF", p_pos[0], p_pos[1], p_pos[2], distance, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid)));
	
	new uid = cache_insert_id(), lid = LoadLabel(sprintf("WHERE `label_uid` = %d", uid), true);
	
	SendGuiInformation(playerid, "Information", sprintf("3d text label created [UID: %d, ID: %d].", uid, lid));
	
	return 1;
}

COMMAND:esel(playerid, params[])
{
	new l_id = GetClosestLabel(playerid);
	if( l_id == INVALID_3DTEXT_ID ) return SendPlayerInformation(playerid, "There are no labels in your area.", 4000);
	
	new string[200], text[15];
	GetDynamic3DTextLabelText(Text3D:l_id, string);			
	str_replace("\\n", "\n", string, text);
	string[0] = EOS;

	format(string, sizeof(string), "%d. 3dtext '%s...'~n~", Label[Text3D:l_id][label_uid], text);
	format(string, sizeof(string), "%s~y~Owner: ~w~%d:%d    ~y~Color: ~w~%06x    ~y~Draw: ~w~%.1f~n~", string, Label[Text3D:l_id][label_owner_type], Label[Text3D:l_id][label_owner], Label[Text3D:l_id][label_color], Label[Text3D:l_id][label_draw]);
	format(string, sizeof(string), "%s~n~~r~X: ~w~%.2f   ~r~Y: ~w~%.2f    ~r~Z: ~w~%.2f", string, Label[Text3D:l_id][label_pos][0], Label[Text3D:l_id][label_pos][1], Label[Text3D:l_id][label_pos][2]);
	
	TextDrawSetString(Dashboard[playerid], string);
	TextDrawShowForPlayer(playerid, Dashboard[playerid]);
	
	pInfo[playerid][player_esel_edited_label] = Label[Text3D:l_id][label_uid];

	pInfo[playerid][player_esel_edited_object] = CreateDynamicObject(1455, Label[Text3D:l_id][label_pos][0], Label[Text3D:l_id][label_pos][1], Label[Text3D:l_id][label_pos][2], 0.0, 0.0, 0.0, -1, -1, playerid, 50.0);
	EditDynamicObject(playerid, pInfo[playerid][player_esel_edited_object]);
	
	SendPlayerInformation(playerid, "~w~Edit object to set ~y~3d text ~w~position.", 10000);
	
	DeleteLabel(l_id, false);
	
	return 1;
}

COMMAND:edel(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_esel_edited_object]) ) return SendGuiInformation(playerid, "Information", "You are not editing any 3dtext label.");
	
	SendGuiInformation(playerid, "Information", sprintf("You've deleted 3dtext label [UID: %d].", pInfo[playerid][player_esel_edited_label]));

	mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_3dlabels` WHERE `label_uid` = %d", pInfo[playerid][player_esel_edited_label]));
	pInfo[playerid][player_esel_edited_label] = 0;
	
	DestroyDynamicObject(pInfo[playerid][player_esel_edited_object]);
	pInfo[playerid][player_esel_edited_object] = -1;
	
	SendPlayerInformation(playerid, "", 0);
	TextDrawHideForPlayer(playerid, Dashboard[playerid]);
	
	CancelEdit(playerid);
	return 1;
}

COMMAND:shout(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(s)hout [text]");
	
	ProxMessage(playerid, params, PROX_SHOUT);
	return 1;
}

COMMAND:s(playerid, params[])
{
	return cmd_shout(playerid, params);
}

COMMAND:silent(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /silent [text]");
	
	ProxMessage(playerid, params, PROX_QUIET);
	return 1;
}

COMMAND:me(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /me [text]");
	
	ProxMessage(playerid, params, PROX_ME);
	return 1;
}

COMMAND:do(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /do [text]");
	
	ProxMessage(playerid, params, PROX_DO);
	return 1;
}

COMMAND:w(playerid, params[])
{
	return cmd_whisper(playerid, params);
}

COMMAND:pm(playerid, params[])
{
	return cmd_whisper(playerid, params);
}

COMMAND:whisper(playerid, params[])
{
	new targetid, message[180];
	if( sscanf(params, "ds[180]", targetid, message) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(w)hisper [player id] [text]");
	if( isnull(message) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(w)hisper [player id] [text]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Information", "Invalid player id.");
	if( targetid == playerid ) return SendGuiInformation(playerid, "Information", "You can't pm yourself.");
	if(pGlobal[playerid][glo_ooc]) return SendGuiInformation(playerid, "Information", "Posiadasz aktywn¹ blokadê czatu OOC.");

	if(pInfo[playerid][player_admin_duty] == false)
	{
		if( PlayerHasTog(targetid, TOG_W) ) return SendGuiInformation(playerid, "Information", "This player has disabled private messages.");
	}
	
	if( pInfo[playerid][player_bw] > 0 && (!HasCrewFlag(targetid, CREW_FLAG_GM) && !HasCrewFlag(targetid, CREW_FLAG_ADMIN) && !HasCrewFlag(targetid, CREW_FLAG_ADMIN_ROOT)) ) return SendGuiInformation(playerid, "Information", "Podczas bw mozesz wysy³aæ wiadomoœci tylko do administratorów.");

	if(pInfo[playerid][player_admin_duty] == false)
	{
		if(pInfo[targetid][player_ignore] == pGlobal[playerid][glo_id]) return SendGuiInformation(playerid, "Information", "This player is ignoring your messages.");
	}

	if(strlen(message)>75)
	{
		new lineend[76], String[200];
	    format(lineend, sizeof(lineend), message);
	    strdel(lineend, 76, strlen(lineend));

    	format(String, sizeof(String),"(( > %s (%d): %s ... ))", pInfo[targetid][player_name], targetid, lineend);
		SendClientMessage(playerid, 0xFFCB73FF, String);

		format(String, sizeof(String),"(( %s (%d): %s ... ))", pInfo[playerid][player_name], playerid, lineend);
		SendClientMessage(targetid, 0xFFAE31FF, String);

		strdel(message, 0, 75);
		format(String, sizeof(String),"(( ... %s ))", message);
		SendClientMessage(playerid, 0xFFCB73FF, String);
		format(String, sizeof(String),"(( ... %s ))", message);
		SendClientMessage(targetid, 0xFFAE31FF, String);

		pInfo[targetid][player_last_pm_playerid] = playerid;
	}
	else
	{
		if( GetPlayerUsedItem(targetid, ITEM_TYPE_MASKA) > -1 ) SendClientMessage(playerid, 0xFFCB73FF, sprintf("(( > %s: %s ))", pInfo[targetid][player_name], message));
		else SendFormattedClientMessage(playerid, 0xFFCB73FF, "(( > %s (%d): %s ))", pInfo[targetid][player_name], targetid, message);
		
		pInfo[targetid][player_last_pm_playerid] = playerid;
		if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASKA) > -1 ) SendClientMessage(targetid, 0xFFAE31FF, sprintf("(( %s: %s ))", pInfo[playerid][player_name], message));
		else SendFormattedClientMessage(targetid, 0xFFAE31FF, "(( %s (%d): %s ))", pInfo[playerid][player_name], playerid, message);
	}
	
	if( pInfo[targetid][player_afk] )
	{
		new afkHour, afkMinute, temp;
		TimestampToDate(pInfo[targetid][player_last_activity], temp, temp, temp, afkHour, afkMinute, temp, 1);
		
		if( afkHour == 24 ) afkHour = 0;
		SendPlayerInformation(playerid, sprintf("Message receiver is ~y~AFK~w~ since %02d:%02d", afkHour+1, afkMinute), 4000);
	}
	
	return 1;
}

COMMAND:tog(playerid, params[])
{
	if( !strcmp(params, "w") )
	{
		if( PlayerHasTog(playerid, TOG_W) )
		{
			pInfo[playerid][player_tog] -= TOG_W;
			SendGuiInformation(playerid, "Information", "Private messages unblocked.");
		}
		else
		{
			pInfo[playerid][player_tog] += TOG_W;
			SendGuiInformation(playerid, "Information", "Private messages blocked.");
		}
	}
	else if( !strcmp(params, "hud") )
	{
		if( PlayerHasTog(playerid, TOG_HUD) )
		{
			pInfo[playerid][player_tog] -= TOG_HUD;
			SendGuiInformation(playerid, "Information", "Textdraws are now visible.");

			TextDrawShowForPlayer(playerid, TextDrawSanNews);
		}
		else
		{
			pInfo[playerid][player_tog] += TOG_HUD;
			SendGuiInformation(playerid, "Information", "Textdraws are now invisible.");

			TextDrawHideForPlayer(playerid, TextDrawSanNews);
			TextDrawHideForPlayer(playerid, AreaText[playerid]);
			TextDrawHideForPlayer(playerid, AreaFlags[playerid]);

			PlayerTextDrawHide(playerid, GroupDutyTag);
		}
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /tog [w/hud]");
	return 1;
}

COMMAND:re(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /re [text]");
	if( !IsPlayerConnected(pInfo[playerid][player_last_pm_playerid]) ) return SendClientMessage(playerid, COLOR_GREY, "Player left the server.");
	if( !pInfo[pInfo[playerid][player_last_pm_playerid]][player_logged] ) return SendClientMessage(playerid, COLOR_GREY, "Player left the server.");
	
	cmd_w(playerid, sprintf("%d %s", pInfo[playerid][player_last_pm_playerid], params));
	return 1;
}

COMMAND:resms(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /resms [text]");
	if( !IsPlayerConnected(pInfo[playerid][player_last_sms])) return SendClientMessage(playerid, COLOR_GREY, "Player left the server.");
	if( !pInfo[pInfo[playerid][player_last_sms]][player_logged] ) return SendClientMessage(playerid, COLOR_GREY, "Player left the server.");
	
	cmd_sms(playerid, sprintf("%d %s", pInfo[playerid][player_last_sms], params));
	return 1;
}

COMMAND:qs(playerid, params[])
{
	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d, `char_quittime`=%d WHERE `char_uid`=%d", x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), gettime(), pInfo[playerid][player_id]));
	Kick(playerid);
	return 1;
}

COMMAND:login(playerid, params[])
{
	TextDrawHideForPlayer(playerid, Tutorial[playerid]);
	
	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
		
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d, `char_quittime`=%d  WHERE `char_uid`=%d", x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), gettime(), pInfo[playerid][player_id]));
	OnPlayerDisconnect(playerid, 0);
	OnPlayerConnect(playerid);
	mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_logged_players` (char_uid, user_id, seen) VALUES (%d, %d, %d)", pInfo[playerid][player_id], pGlobal[playerid][glo_id], pInfo[playerid][player_last_join]));
	return 1;
}

COMMAND:l(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /l [text]");
	ProxMessage(playerid, params, PROX_LOCAL_L);
	return 1;
}

COMMAND:b(playerid, params[])
{
	if(pGlobal[playerid][glo_ooc]) return SendGuiInformation(playerid, "Information", "You have active OOC blockade.");
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /b [text]");

	ProxMessage(playerid, params, PROX_OOC);
	return 1;
}

COMMAND:door(playerid, params[])
{
	new sub_cmd[40], param[150];	
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /drzwi [lock/managment/rent/unrent]");
		return 1;
	}
	
	if( !strcmp(sub_cmd, "lock") )
	{
		new d_id = -1;

		foreach(new d : Doors)
		{
			if(IsPlayerInRangeOfPoint(playerid, 2.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]) && GetPlayerVirtualWorld(playerid) == Door[d][door_vw])
			{
				d_id = d;
			}
			else if(IsPlayerInRangeOfPoint(playerid, 2.0,  Door[d][door_spawn_pos][0],  Door[d][door_spawn_pos][1], Door[d][door_spawn_pos][2]) && GetPlayerVirtualWorld(playerid) == Door[d][door_spawn_vw])
			{
				d_id = d;
			}
		}
		
		if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not near any door.");

		if(Door[d_id][door_rentable] == 0)
		{
			if( !CanPlayerUseDoor(playerid, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "You don't have access to this door.");
			
			if( Door[d_id][door_closed] )
			{
                GameTextForPlayer(playerid, "~w~door ~g~opened", 3000, 6);
				Door[d_id][door_closed] = false;
			}
			else
			{
				GameTextForPlayer(playerid, "~w~door ~r~locked", 3000, 6);
				Door[d_id][door_closed] = true;
			}
			
			ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.1, 0, 0, 0, 0, 0, 1);
		}
		else
		{
			if(pInfo[playerid][player_renting] == Door[d_id][door_uid] || HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT))
			{
				if( Door[d_id][door_closed] )
				{
                    GameTextForPlayer(playerid, "~w~door ~g~opened", 3000, 6);
					Door[d_id][door_closed] = false;
				}
				else
				{
                    GameTextForPlayer(playerid, "~w~door ~r~locked", 3000, 6);
					Door[d_id][door_closed] = true;
				}

				ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.1, 0, 0, 0, 0, 0, 1);
			}
			else
			{
				SendGuiInformation(playerid, "Information", "You are not renting this flat.");
			}
		}
	}

	else if( !strcmp(sub_cmd, "buy") )
	{
		new d_id = -1;

		foreach(new d : Doors)
		{
			if(IsPlayerInRangeOfPoint(playerid, 2.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
			{
				d_id = d;
			}
		}
		
		if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not near any door.");


		if(Door[d_id][door_buyable] == 1)
		{
			if(Door[d_id][door_price] < pInfo[playerid][player_money])
			{
				GivePlayerMoney(playerid, -Door[d_id][door_price]);
				Door[d_id][door_owner]=pInfo[playerid][player_id];
				Door[d_id][door_owner_type]=DOOR_OWNER_TYPE_PLAYER;
				Door[d_id][door_buyable]=0;
				new gid = GetGroupByUid(1);
				GiveGroupMoney(gid, Door[d_id][door_price]);
				SendGuiInformation(playerid, "Information", "Property bough, use /door managment to manage.");
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_owner`= %d, `door_ownertype`= %d, `door_buyable`= 0  WHERE `door_uid` = %d", pInfo[playerid][player_id], DOOR_OWNER_TYPE_PLAYER, Door[d_id][door_uid]));
			}
			else 
			{
				SendGuiInformation(playerid, "Information", "You don't have enough money.");
			}
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This building is not for sale.");
		}
	}

	else if( !strcmp(sub_cmd, "rent") )
	{
		new d_id = -1;

		foreach(new d : Doors)
		{
			if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
			{
				d_id = d;
			}
		}
		
		if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not near any door.");

		if(IsPlayerInAnyGroup(playerid))
		{
			if(Door[d_id][door_rentable] == 1)
			{
				if(Door[d_id][door_rent] < pInfo[playerid][player_money])
				{
					Door[d_id][door_rentable] = 2;
					GivePlayerMoney(playerid, -Door[d_id][door_rent]);
					pInfo[playerid][player_renting] = Door[d_id][door_uid];
					SendGuiInformation(playerid, "Information", "You've rented flat.\nPayment will be taken with payday.");
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_renting`= %d  WHERE `char_uid` = %d", Door[d_id][door_uid], pInfo[playerid][player_id]));
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_rentable`= 2  WHERE `door_uid` = %d", Door[d_id][door_uid]));
				}
				else 
				{
					SendGuiInformation(playerid, "Information", "You don't have enough money.");
				}
			}
			else
			{
				SendGuiInformation(playerid, "Information", "This flat is not for rent.");
			}
		}
		else
		{
			SendGuiInformation(playerid, "Information", "You can't rent this flat if you are not member of any group.");
		}
	}

	else if( !strcmp(sub_cmd, "unrent") )
	{
		new d_id = -1;

		foreach(new d : Doors)
		{
			if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
			{
				d_id = d;
			}
			else if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_spawn_pos][0],  Door[d][door_spawn_pos][1], Door[d][door_spawn_pos][2]))
			{
				d_id = d;
			}
		}
		
		if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "You are not near any door.");

		if(Door[d_id][door_rentable]==2)
		{
			if(Door[d_id][door_uid] == pInfo[playerid][player_renting])
			{
				Door[d_id][door_rentable]=1;
				pInfo[playerid][player_renting]=0;
				SendGuiInformation(playerid, "Information", "Flat is free now.");
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_renting`= 0  WHERE `char_uid` = %d", pInfo[playerid][player_id]));
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_rentable`= 1  WHERE `door_uid` = %d", Door[d_id][door_uid]));
			}
			else
			{
				SendGuiInformation(playerid, "Information", "You are not renting this flat.");
			}
		}
		else
		{
			SendGuiInformation(playerid, "Information", "This flat is not for rent.");
		}
	}

	else if( !strcmp(sub_cmd, "managment") )
	{
		new header[80], str[450];

		new d_id = -1;
		new ds_id = -1;

		foreach(new d : Doors)
		{
			if(IsPlayerInRangeOfPoint(playerid, 4.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
			{
				d_id = d;
			}
		}
		
		if( d_id == -1 ) 
		{
			ds_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
			
			if(ds_id == - 1)
			{
				SendClientMessage(playerid, COLOR_GREY, "You are not near any door.");
				return 1;
			}

			d_id = ds_id;
		}

		
		if(Door[d_id][door_rentable]==0)
		{
			if( !CanPlayerEditDoor(playerid, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "You don't have access to this door.");

			format(header, sizeof(header), "Drzwi %d (%s; SampID: %d)", Door[d_id][door_uid], Door[d_id][door_name], d_id);
			
			DynamicGui_Init(playerid);
			DynamicGui_SetDialogValue(playerid, d_id);
					
			format(str, sizeof(str), "%s01\tChange name\n", str);
			DynamicGui_AddRow(playerid, DG_DRZWI_NAME);	
			
			if( GetPlayerVirtualWorld(playerid) == Door[d_id][door_vw] )
			{			
				format(str, sizeof(str), "%s02\tChange positio inside (by coorinates)\n", str);
				DynamicGui_AddRow(playerid, DG_DRZWI_SPAWN_COORDS);
				
				format(str, sizeof(str), "%s03\tLoad .map\n", str);
				DynamicGui_AddRow(playerid, DG_DRZWI_MAP_LOAD);
			}
			
			format(str, sizeof(str), "%s04\tSet audio\n", str);
			DynamicGui_AddRow(playerid, DG_DRZWI_AUDIO);

			format(str, sizeof(str), "%s05\tHome storage\n", str);
			DynamicGui_AddRow(playerid, DG_DRZWI_SCHOWEK);	

			if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
			{
				format(str, sizeof(str), "%s06\tGroup storage\n", str);
				DynamicGui_AddRow(playerid, DG_DRZWI_MAGAZYN);	
			}		
			
			if( GetPlayerVirtualWorld(playerid) == Door[d_id][door_spawn_vw] )
			{
				format(str, sizeof(str), "%s07\tChange position inside\n", str);
				DynamicGui_AddRow(playerid, DG_DRZWI_SPAWN);

				format(str, sizeof(str), "%s08\tDelete interior (objects cleaning)\n", str);
            	DynamicGui_AddRow(playerid, DG_DRZWI_CLEAR);
			}

            format(str, sizeof(str), "%s09\tSet time\n", str);
            DynamicGui_AddRow(playerid, DG_DRZWI_TIME);  
			
			format(str, sizeof(str), "%s10\tEntrance fee\n", str);
			DynamicGui_AddRow(playerid, DG_DRZWI_PAYMENT);
			
			format(str, sizeof(str), "%s11\tGarage mode\n", str);
			DynamicGui_AddRow(playerid, DG_DRZWI_CARS);
			
			format(str, sizeof(str), "%s12\tClosing after restart\n", str);
			DynamicGui_AddRow(playerid, DG_DRZWI_CLOSING);

			format(str, sizeof(str), "%s13\tBuy more objects\n", str);
			DynamicGui_AddRow(playerid, DG_DRZWI_BUY);

			if(Door[d_id][door_burned] > 0)
			{
				new price = Door[d_id][door_burned] *50;
				format(str, sizeof(str), "%s14\tFix destructions - $%d\n", str, price);
				DynamicGui_AddRow(playerid, DG_DRZWI_FIX_BURN);
			}
			else if(Door[d_id][door_destroyed] > 0)
			{
				new price = Door[d_id][door_destroyed] * 25;
				format(str, sizeof(str), "%s14\tFix destructions - $%d\n", str, price);
				DynamicGui_AddRow(playerid, DG_DRZWI_FIX);
			}

			if( DoorHasAccess(d_id, DOOR_ACCESS_CAMERA) )
			{
				format(str, sizeof(str), "%s15\tWatch recordings\n", str);
				DynamicGui_AddRow(playerid, DG_DRZWI_CAMERA);
			}
		}
		else
		{
			format(header, sizeof(header), "Door %d (%s; SampID: %d)", Door[d_id][door_uid], Door[d_id][door_name], d_id);
		
			DynamicGui_Init(playerid);
			DynamicGui_SetDialogValue(playerid, d_id);
					
			format(str, sizeof(str), "%s01\tChange name\n", str);
			DynamicGui_AddRow(playerid, DG_DRZWI_NAME);	

			format(str, sizeof(str), "%s02\tSet audio\n", str);
			DynamicGui_AddRow(playerid, DG_DRZWI_AUDIO);
		}

		new owner_str[80];
		switch( Door[d_id][door_owner_type] )
		{
			case DOOR_OWNER_TYPE_GLOBAL:
			{
				format(owner_str, sizeof(owner_str), "global");
			}
			
			case DOOR_OWNER_TYPE_PLAYER:
			{			
				new name[MAX_PLAYER_NAME+1];
				GetPlayerNameByUid(Door[d_id][door_owner], name);
				
				format(owner_str, sizeof(owner_str), "private; %s", name);
			}
			
			case DOOR_OWNER_TYPE_GROUP:
			{			
				new gid = GetGroupByUid(Door[d_id][door_owner]);
				format(owner_str, sizeof(owner_str), "group; %s", Group[gid][group_name]);
			}
		}

		if(Door[d_id][door_rentable]!=0)
		{
			format(owner_str, sizeof(owner_str), "rent");
		}
		
		new string[340];
		format(string, sizeof(string), "Door UID %d - %s~n~~n~~y~Owner: ~w~%d:%d(%s)~n~~y~Objects:~w~ %d/%d~n~", Door[d_id][door_uid], Door[d_id][door_name], Door[d_id][door_owner_type], Door[d_id][door_owner], owner_str, CountDoorObjects(d_id), Door[d_id][door_objects_limit]);
		format(string, sizeof(string), "%s~n~~y~Entrance fee: ~w~$%d~n~~y~Garage: ~w~%s~n~~y~Closing: ~w~%s", string, Door[d_id][door_payment], ((Door[d_id][door_car_crosing]) ? ("Yes") : ("No")), ((Door[d_id][door_auto_closing]) ? ("Yes") : ("No")));
		format(string, sizeof(string), "%s~n~~n~~b~~h~~h~VW:~w~ %d~n~~b~~h~~h~INT: ~w~%d", string, Door[d_id][door_spawn_vw], Door[d_id][door_spawn_int]);

		
		TextDrawSetString(Tutorial[playerid], string);
		TextDrawShowForPlayer(playerid, Tutorial[playerid]);
		
		ShowPlayerDialog(playerid, DIALOG_DRZWI, DIALOG_STYLE_LIST, header, str, "Choose", "Close");
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /door [close/managment]");
	return 1;
}

COMMAND:phone(playerid, params[])
{
	new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
	if( itemid == -1 ) return SendGuiInformation(playerid, "Information", "You don't have enabled phone.");
	
	new sub_cmd[40], param[150];
	sscanf(params, "s[40] S()[150]", sub_cmd, param);
	
	if( isnull(params) )
	{
		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, itemid);
		
		new str[150];
		
		format(str, sizeof(str), "%s01\tCall number\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_CALL);
		
		format(str, sizeof(str), "%s02\tSend text\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_SMS);
		
		format(str, sizeof(str), "%s03\tContacts\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS);

		format(str, sizeof(str), "%s04\tAdd contact\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_ADD_CONTACT);
		
		format(str, sizeof(str), "%s05\tSend vCard\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_VCARD);
		
		format(str, sizeof(str), "%s06\tTurn off\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_TURNOFF);
		
		new th, tmin, tsek;
		gettime(th, tmin, tsek);
		
		ShowPlayerDialog(playerid, DIALOG_PHONE, DIALOG_STYLE_LIST, sprintf("%s [%d] [%02d:%02d:%02d]", PlayerItem[playerid][itemid][player_item_name], PlayerItem[playerid][itemid][player_item_value1], th, tmin, tsek), str, "Wybierz", "Anuluj");
	}
	else
	{
		if( !strcmp(sub_cmd, "answer") )
		{
			if( pInfo[playerid][player_phone_caller] != INVALID_PLAYER_ID && !pInfo[playerid][player_phone_call_started] )
			{
				// Odbieramy polaczenie
				SendClientMessage(playerid, COLOR_YELLOW, "Call received. To end use /phone reject.");
				SendClientMessage(pInfo[playerid][player_phone_caller], COLOR_YELLOW, "Call received. To end unse /phone reject.");
				pInfo[playerid][player_phone_call_started] = true;
				pInfo[pInfo[playerid][player_phone_caller]][player_phone_call_started] = true;
				
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
	
				pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
				if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);
			}
		}
		else if( !strcmp(sub_cmd, "reject") )
		{
			if( pInfo[playerid][player_phone_call_started] )
			{
				SendClientMessage(playerid, COLOR_YELLOW, "Call ended.");
				pInfo[playerid][player_phone_call_started] = false;
				
				new targetid = -1;
				if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
				else targetid = pInfo[playerid][player_phone_caller];
				
				pInfo[playerid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SendClientMessage(targetid, COLOR_YELLOW, "Call ended.");
				pInfo[targetid][player_phone_call_started] = false;
				pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[targetid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				SetPlayerSpecialAction(targetid, SPECIAL_ACTION_STOPUSECELLPHONE);
				if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				if( pInfo[targetid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(targetid, pInfo[targetid][player_phone_object_index]);
			}
			else if( !pInfo[playerid][player_phone_call_started] && pInfo[playerid][player_phone_caller] != INVALID_PLAYER_ID )
			{
				// odrzuca polaczenie odbiorca
				SendClientMessage(playerid, COLOR_YELLOW, "Call ended.");
				pInfo[playerid][player_phone_call_started] = false;
				
				new targetid = -1;
				if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
				else targetid = pInfo[playerid][player_phone_caller];
				
				pInfo[playerid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SendClientMessage(targetid, COLOR_YELLOW, "Number not available...");
				pInfo[targetid][player_phone_call_started] = false;
				pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[targetid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				SetPlayerSpecialAction(targetid, SPECIAL_ACTION_STOPUSECELLPHONE);
				if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				if( pInfo[targetid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(targetid, pInfo[targetid][player_phone_object_index]);
			}
			else if( !pInfo[playerid][player_phone_call_started] && pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID )
			{			
				new targetid = -1;
				if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
				else targetid = pInfo[playerid][player_phone_caller];

				if(targetid == -1 || targetid > 999) return 1;

				// przerywa polaczenie dzwoniacy
				SendClientMessage(playerid, COLOR_YELLOW, "Call ended.");
				pInfo[playerid][player_phone_call_started] = false;

				pInfo[playerid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SendClientMessage(targetid, COLOR_YELLOW, "Call ended");
				pInfo[targetid][player_phone_call_started] = false;
				pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[targetid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				SetPlayerSpecialAction(targetid, SPECIAL_ACTION_STOPUSECELLPHONE);
				if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				if( pInfo[targetid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(targetid, pInfo[targetid][player_phone_object_index]);
			}
		}
	}
	return 1;
}

COMMAND:sms(playerid, params[])
{
	new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
	if( itemid == -1 ) return SendGuiInformation(playerid, "Information", "You don't have enabled phone.");
	
	new number, text[128];
	if( sscanf(params, "ds[128]", number, text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /sms [number] [text]");
	if( number == Item[itemid][item_value1] ) return SendGuiInformation(playerid, "Information", "You can't text yourself.");
	
	new targetid = GetPlayerByPhone(number);
	if( targetid == INVALID_PLAYER_ID ) return SendGuiInformation(playerid, "Information", "Number not available.");
	new item = GetPlayerUsedItem(targetid, ITEM_TYPE_PHONE);
	if( item == -1 ) return SendGuiInformation(playerid, "Information", "Number not available.");
	
	new tmp_name[MAX_PLAYER_NAME+1], rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT contact_name FROM `ipb_contacts` WHERE `contact_owner` = %d AND `contact_number` = %d AND `contact_deleted` = 0", PlayerItem[targetid][itemid][player_item_uid], PlayerItem[playerid][itemid][player_item_value1]));	
	cache_get_data(rows, fields);
	
	if( rows )
	{
		cache_get_row(0, 0, tmp_name);
		SendFormattedClientMessage(targetid, COLOR_YELLOW, "(SMS) [%d] %s: %s", PlayerItem[playerid][itemid][player_item_value1], tmp_name, text);
	}
	else
	{
		SendFormattedClientMessage(targetid, COLOR_YELLOW, "(SMS) %d: %s", PlayerItem[playerid][itemid][player_item_value1], text);
	}

	ProxMessage(playerid, "sends SMS.", PROX_ME);
	ProxMessage(targetid, "received SMS.", PROX_ME);
	pInfo[targetid][player_last_sms] = playerid;
	
	new Float:p_pos[3];
	GetPlayerPos(targetid, p_pos[0], p_pos[1], p_pos[2]);

	mysql_query(mySQLconnection, sprintf("SELECT contact_name FROM `ipb_contacts` WHERE `contact_owner` = %d AND `contact_number` = %d AND `contact_deleted` = 0", PlayerItem[playerid][itemid][player_item_uid], PlayerItem[targetid][item][player_item_value1]));	
	cache_get_data(rows, fields);
	
	if( rows )
	{
		cache_get_row(0, 0, tmp_name);
		SendFormattedClientMessage(playerid, COLOR_YELLOW, "> (SMS) [%d] %s: %s", PlayerItem[targetid][item][player_item_value1], tmp_name, text);
	}
	else
	{
		SendFormattedClientMessage(playerid, COLOR_YELLOW, "> (SMS) %d: %s", PlayerItem[targetid][item][player_item_value1], text);
	}
	
	return 1;
}

COMMAND:call(playerid, params[])
{
	if( pInfo[playerid][player_phone_receiver] != INVALID_PLAYER_ID || pInfo[playerid][player_phone_caller] != INVALID_PLAYER_ID ) return SendGuiInformation(playerid, "Information", "You are already calling someone.");
	new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
	if( itemid == -1 ) return SendGuiInformation(playerid, "Information", "You don't have enabled phone.");
	
	if(strval(params) < 3) return SendGuiInformation(playerid, "Information", "Phone with this number is not available or doesnt exist.");

	new number;
	if( sscanf(params, "d", number) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /call [number]");
	if( number == PlayerItem[playerid][itemid][player_item_value1] ) return SendGuiInformation(playerid, "Information", "You can't call yourself.");
	
	if( number == 911) // Centrala PD
	{
		ShowPlayerDialog(playerid, DIALOG_911, DIALOG_STYLE_INPUT, "911 call:", "Please input content of notification:", "OK", "");
		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
		pInfo[playerid][player_dialog_tmp1] = PlayerItem[playerid][itemid][player_item_value1];
		pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
		if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);
		return 1;
	}

	if( number == 444) // Weazel
	{
		ShowPlayerDialog(playerid, DIALOG_WEAZEL, DIALOG_STYLE_INPUT, "Weazel News:", "Please input content of notification:", "OK", "");
		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
		pInfo[playerid][player_dialog_tmp1] = PlayerItem[playerid][itemid][player_item_value1];
		pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
		if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);
		return 1;
	}

	if( number == 333 ) // Hurtownia
	{
		if(IsPlayerInAnyGroup(playerid))
		{
			new slot = GetPlayerDutySlot(playerid);
			if(slot==-1) return SendGuiInformation(playerid, "Information", "You are not on group duty with access to the wholesale.");
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return SendGuiInformation(playerid, "Information", "You are not leader on your duty slot.");

			new list[128];
			format(list, sizeof(list), "1. Gastronomy\n2. Workshop\n3. Police\n4. Radio\n5. Security\n6. Gym\n7. Criminals\n8. Medics\n.9. None");
			ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_LEGAL, DIALOG_STYLE_LIST, "Hurtownia", list, "Wybierz", "WyjdŸ");
			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
			pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
			if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);
			return 1;
		}
		else
		{
			SendGuiInformation(playerid, "Information", "You are not member of any group.");
			return 1;
		}
	}

	if( number == 555 ) // Hurtownia przestêpczych
	{
		if(IsPlayerInAnyGroup(playerid))
		{
			new slot = GetPlayerDutySlot(playerid), gid = pInfo[playerid][player_duty_gid];
			if(slot == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty with access to this phone number.");
			if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty with access to this phone number.");
			if( !GroupHasFlag(gid, GROUP_FLAG_BOT)) return SendGuiInformation(playerid, "Information", "You are not on group duty with access to this phone number.");
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return SendGuiInformation(playerid, "Information", "You are not leader on your duty slot.");
			if(bot_taken > gettime()) return SendGuiInformation(playerid, "Information", "Dealer is doing business right now, try to find him or wait 15 minutes and call again.");

			SendClientMessage(playerid, COLOR_YELLOW, "Call received.");
			SendClientMessage(playerid, COLOR_YELLOW, "[Phone]: I've got some time now. Where you want to meet?");
			ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ILLEGAL, DIALOG_STYLE_LIST, "Choose meeting place", "Market\nRodeo\nMullholand\nEast LS\nOcean Docks", "Choose", "Cancel");

			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
			pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
			if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);
			return 1;
		}
		else
		{
			SendGuiInformation(playerid, "Information", "You are not member of any group.");
			return 1;
		}
	}

	new targetid = GetPlayerByPhone(number);
	new item = GetPlayerUsedItem(targetid, ITEM_TYPE_PHONE);
	if( targetid == INVALID_PLAYER_ID ) return SendGuiInformation(playerid, "Information", "Phone with that number is not enabled or doesnt exist.");
	if( item == -1 ) return SendGuiInformation(playerid, "Information", "Phone with that number is not enabled or doesnt exist.");
	
	if( pInfo[targetid][player_phone_receiver] != INVALID_PLAYER_ID || pInfo[targetid][player_phone_caller] != INVALID_PLAYER_ID ) return SendGuiInformation(playerid, "Information", "Number is not available.");
	
	pInfo[targetid][player_phone_caller] = playerid;
	pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
	
	pInfo[playerid][player_phone_receiver] = targetid;
	pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;

	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
	
	pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
	if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);

	ProxMessage(targetid, sprintf("%s's phone is calling.", pInfo[targetid][player_name]), PROX_DO);

	new tmp_name[MAX_PLAYER_NAME+1], rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT contact_name FROM `ipb_contacts` WHERE `contact_owner` = %d AND `contact_number` = %d AND `contact_deleted` = 0", PlayerItem[targetid][item][player_item_uid], PlayerItem[playerid][itemid][player_item_value1]));	
	cache_get_data(rows, fields);
	
	if( rows )
	{
		cache_get_row(0, 0, tmp_name);
		SendFormattedClientMessage(targetid, COLOR_YELLOW, "Incoming call from %s (%d). Type /phone answer or /phone reject.", tmp_name, PlayerItem[playerid][itemid][player_item_value1]);
	}
	else
	{
		SendFormattedClientMessage(targetid, COLOR_YELLOW, "Incoming call from %d. Type /phone answer or /phone reject.", PlayerItem[playerid][itemid][player_item_value1]);
	}
	
	SendClientMessage(playerid, COLOR_YELLOW, "Call signal...");
	
	return 1;
}

COMMAND:job(playerid, params[])
{
	if( pInfo[playerid][player_job] == 0 ) return SendGuiInformation(playerid, "Information", "You don't have any extra job.");
	
	new sub_cmd[40], param[150];
	sscanf(params, "s[40] S()[150]", sub_cmd, param);
	
	if( isnull(params) )
	{
		if(pInfo[playerid][player_job] == WORK_TYPE_LUMBERJACK)
		{
			new Float:x, Float:y, Float:z;
			GetActorPos(Lumberjack, x, y, z);
			if(IsPlayerInRangeOfPoint(playerid, 5.0, x, y, z))
			{
				ShowPlayerDialog(playerid, DIALOG_LUMBERJACK, DIALOG_STYLE_TABLIST_HEADERS, "Lumberjack", "Product\tPrice\nChainsaw\t$150", "Buy", "Exit");
			}
		}

		else if(pInfo[playerid][player_job] == WORK_TYPE_FISHER)
		{
			new Float:x, Float:y, Float:z;
			GetActorPos(Fisher, x, y, z);
			if(IsPlayerInRangeOfPoint(playerid, 5.0, x, y, z))
			{
				//ShowPlayerDialog(playerid, DIALOG_MINER, DIALOG_STYLE_TABLIST_HEADERS, "Rybak", "Produkt\tCena\nM³otek\t$125", "Kup", "WyjdŸ");
			}
		}

		SendClientMessage(playerid, COLOR_GREY, "Tip: /job leave");
	}
	else
	{
		if( !strcmp(sub_cmd, "leave") )
		{
			pInfo[playerid][player_job] = 0;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_job` = 0 WHERE `char_uid` = %d", pInfo[playerid][player_id]));
			
			SendGuiInformation(playerid, "Information", "You have no job now.");
		}
	}
	
	return 1;
}

COMMAND:v(playerid, params[])
{
	return cmd_vehicle(playerid, params);
}

COMMAND:vehicle(playerid, params[])
{	
	if( IsPlayerInAnyVehicle(playerid) )
	{
		new vid = GetPlayerVehicleID(playerid);
		
		new sub_cmd[40], param[150];
		sscanf(params, "s[40] S()[150]", sub_cmd, param);
		
		if( isnull(sub_cmd) )
		{
			// Sprawdzamy czy jest kierowca
			if( GetPlayerVehicleSeat(playerid) != 0 ) return 1;
			
			// Wyswietlamy panel pojazdu
			if( !CanPlayerUseVehicle(playerid, vid) ) return SendClientMessage(playerid, COLOR_GREY, "No access to this vehicle.");
			
			new header[80];
			format(header, sizeof(header), "%s (UID: %d, ID: %d)", VehicleNames[Vehicle[vid][vehicle_model]-400], Vehicle[vid][vehicle_uid], vid);
			
			DynamicGui_Init(playerid);
			
			new str[600];			
			format(str, sizeof(str), "%sHood:\t%s\n", str, (Vehicle[vid][vehicle_bonnet]==false) ? (""HEX_COLOR_LIGHTER_RED"[Closed]") : (""HEX_COLOR_LIGHTER_GREEN"[Opened]"));
			DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_BONNET);
			format(str, sizeof(str), "%sTrunk:\t%s\n", str, (Vehicle[vid][vehicle_boot]==false) ? (""HEX_COLOR_LIGHTER_RED"[Closed]") : (""HEX_COLOR_LIGHTER_GREEN"[Opened]"));
			DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_BOOT);
			format(str, sizeof(str), "%sLights:\t%s\n", str, (Vehicle[vid][vehicle_lights]==false) ? (""HEX_COLOR_LIGHTER_RED"[Off]") : (""HEX_COLOR_LIGHTER_GREEN"[On]"));
			DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_LIGHTS);
			format(str, sizeof(str), "%sDriver window:\t%s\n", str, (Vehicle[vid][vehicle_win_driver]==true) ? (""HEX_COLOR_LIGHTER_RED"[Closed]") : (""HEX_COLOR_LIGHTER_GREEN"[Opened]"));
			DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_WIN_DRIVER);
			format(str, sizeof(str), "%sPassenger window:\t%s\n", str, (Vehicle[vid][vehicle_win_pp]==true) ? (""HEX_COLOR_LIGHTER_RED"[Closed]") : (""HEX_COLOR_LIGHTER_GREEN"[Opened]"));
			DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_WIN_PP);
			format(str, sizeof(str), "%sLeft back window:\t%s\n", str, (Vehicle[vid][vehicle_win_lt]==true) ? (""HEX_COLOR_LIGHTER_RED"[Closed]") : (""HEX_COLOR_LIGHTER_GREEN"[Opened]"));
			DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_WIN_LT);
			format(str, sizeof(str), "%sRight back window:\t%s\n", str, (Vehicle[vid][vehicle_win_pt]==true) ? (""HEX_COLOR_LIGHTER_RED"[Closed]") : (""HEX_COLOR_LIGHTER_GREEN"[Opened]"));
			DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_WIN_PT);
			
			format(str, sizeof(str), "%s-----------------\n", str);
			DynamicGui_AddBlankRow(playerid);
			
			format(str, sizeof(str), "%sAccessories:\t%sImmobiliser, ", str, (VehicleHasAccessory(vid, VEHICLE_ACCESSORY_IMMOBILISER)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_GREY));
			format(str, sizeof(str), "%s%sAlarm ", str, (VehicleHasAccessory(vid, VEHICLE_ACCESSORY_ALARM)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_GREY));
			DynamicGui_AddBlankRow(playerid);
			
			ShowPlayerDialog(playerid, DIALOG_PLAYER_VEHICLE_PANEL, DIALOG_STYLE_TABLIST, header, str, "Choose", "Close");
		}
		else
		{
			if( !strcmp(sub_cmd, "park", false) )
			{
				// Sprawdzamy czy jest kierowca
				if( GetPlayerVehicleSeat(playerid) != 0 ) return 1;
			
				if( !CanPlayerEditVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Information", "No access to this vehicle.");

				new a_id = pInfo[playerid][player_area];
				if(a_id > 0 )
				{
					if(AreaHasFlag(a_id, AREA_FLAG_PARKING))
					{
						SendGuiInformation(playerid, "Information", "You can't park in this area.");
						return 1;
					}
				}
				
				new Float:vehiclePos[4];
				GetVehiclePos(vid, vehiclePos[0], vehiclePos[1], vehiclePos[2]);
				GetVehicleZAngle(vid, vehiclePos[3]);
				new rows, fields;

				mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_vehicles` WHERE `vehicle_posx` >= %f AND `vehicle_posx` <= %f AND `vehicle_posy` >= %f AND `vehicle_posy` <= %f AND `vehicle_world` = %d AND `vehicle_interior` = %d AND `vehicle_uid` <> %d", vehiclePos[0]-2.0, vehiclePos[0]+2.0, vehiclePos[1]-2.0, vehiclePos[1]+2.0, GetVehicleVirtualWorld(vid), Vehicle[vid][vehicle_interior], Vehicle[vid][vehicle_uid]));
				cache_get_data(rows, fields);
				
				if( rows > 0 )
				{
					SendGuiInformation(playerid, "Information", "There is other car parked over here.");
					
				}
				else
				{
					GetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]);
					GetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
					Vehicle[vid][vehicle_park_world] = GetVehicleVirtualWorld(vid);
					Vehicle[vid][vehicle_park_interior] = Vehicle[vid][vehicle_interior];
					
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_posx` = %f, `vehicle_posy` = %f, `vehicle_posz` = %f, `vehicle_posa` = %f, `vehicle_world` = %d, `vehicle_interior` = %d WHERE `vehicle_uid` = %d", Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2], Vehicle[vid][vehicle_park][3], Vehicle[vid][vehicle_park_world], Vehicle[vid][vehicle_park_interior], Vehicle[vid][vehicle_uid]));
					
					SendGuiInformation(playerid, "Information", sprintf("%s (UID: %d) has been parked.", VehicleNames[Vehicle[vid][vehicle_model]-400], Vehicle[vid][vehicle_uid], vid));
				}
			}
			if( !strcmp(sub_cmd, "lock")  )
			{
				if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Information", "No access to this vehicle.");
				
				if( Vehicle[vid][vehicle_locked] )
				{
					Vehicle[vid][vehicle_locked] = false;
					GameTextForPlayer(playerid, "~w~car ~g~opened", 3000, 6);
				}
				else
				{
					Vehicle[vid][vehicle_locked] = true;
					GameTextForPlayer(playerid, "~w~car ~r~locked", 3000, 6);
				}
				
				if( !VehicleHasAccessory(vid, VEHICLE_ACCESSORY_ALARM) )
				{
					ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.1, 0, 0, 0, 0, 0, 1);
				}
				else
				{
					foreach(new p : Player)
					{
						if( GetPlayerDistanceFromPoint(p, Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2]) <= 20.0 ) PlayAudioStreamForPlayer(p, "http://iphonegu.com/wp-content/uploads/2012/09/car_lock.mp3", Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2], 20.0, 1);
					}
					
					ApplyAnimation(playerid, "HEIST9", "Use_SwipeCard", 4.1, 0, 0, 0, 0, 0, 1);
				}
			}
			if( !strcmp(sub_cmd, "tuning") )
			{
				if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Information", "No access to this vehicle.");
				new tune_list[450];
				DynamicGui_Init(playerid);

				for (new j = 0; j < 14; ++j)
		   		{
					new componentid = GetVehicleComponentInSlot(vid, j);
		  			if (componentid != 0 && componentid == (Vehicle[vid][vehicle_component][j] + 999))
		        	{
		        		format(tune_list, sizeof(tune_list), "%s\n%s", tune_list, GetComponentName(componentid));
		        		DynamicGui_AddRow(playerid, componentid);
			        }
			    }

			    if(strlen(tune_list))
			    {
			    	ShowPlayerDialog(playerid, DIALOG_TUNE, DIALOG_STYLE_LIST, "Vehicle parts", tune_list, "Remove", "Close");
			    }
			    else
			    {
			    	SendGuiInformation(playerid, "Information", "This vehicle has no extra parts.");
			    }
			}
			else if( !strcmp(sub_cmd, "desc") )
			{
				if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Information", "No access to this vehicle.");
				if( isnull(param) && !Vehicle[vid][vehicle_has_description] ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /v desc [text]");
				else if( isnull(param) && Vehicle[vid][vehicle_has_description] )
				{
					UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_description], LABEL_DESCRIPTION, "");
					Vehicle[vid][vehicle_has_description] = false;
					SendGuiInformation(playerid, "Information", sprintf("You've removed your %s desription [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid]));
					return 1;
				}
				
				UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_description], LABEL_DESCRIPTION, BreakLines(param, "\n", 32));
				Vehicle[vid][vehicle_has_description] = true;
				SendGuiInformation(playerid, "Information", sprintf("You've set up new description for %s [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid]));
			}
			else if( !strcmp(sub_cmd, "sign") )
			{
				if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Information", "No access to this vehicle.");
				new gid = pInfo[playerid][player_duty_gid];
				if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");
				if(!GroupHasFlag(gid, GROUP_FLAG_911)) return SendGuiInformation(playerid, "Information", "You are not on group duty with 911 flag.");
				new slot = GetPlayerDutySlot(playerid);
				if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_VEHICLES) ) return SendGuiInformation(playerid, "Information", "You don't have car privileges. Ask leader.");

				if( isnull(param) && !Vehicle[vid][vehicle_has_sign] ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /v sign [text]");
				else if( isnull(param) && Vehicle[vid][vehicle_has_sign] )
				{
					UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_sign], COLOR_WHITE, "");
					Vehicle[vid][vehicle_has_sign] = false;
					SendGuiInformation(playerid, "Information", sprintf("You've removed car sign from %s [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid]));
					return 1;
				}
				
				UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_sign], COLOR_WHITE, BreakLines(param, "\n", 32));
				Vehicle[vid][vehicle_has_sign] = true;
				SendGuiInformation(playerid, "Information", sprintf("You've set up car sign on %s [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid]));
			}
			else if( !strcmp(sub_cmd, "assign") )
			{
				if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Information", "No access to this vehicle.");
				new gslot;
				if( sscanf(param, "d", gslot) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /v assign [group slot (1-5)]");
				if( gslot < 1 || gslot > 5 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /v assign [group slot (1-5)]");
				if( pGroup[playerid][gslot-1][pg_id] == -1 ) return SendGuiInformation(playerid, "Information", "No group on this slot.");
				
				new gid = pGroup[playerid][gslot-1][pg_id];
				Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_GROUP;
				Vehicle[vid][vehicle_owner] = Group[gid][group_uid];
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_GROUP, Group[gid][group_uid], Vehicle[vid][vehicle_uid]));
				
				SendGuiInformation(playerid, "Information", sprintf("Vehicle %s [UID: %d] assigned to the group %s [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], Group[gid][group_name], Group[gid][group_uid]));
			}
			else if( !strcmp(sub_cmd, "info") )
			{
				if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Information", "No access to this vehicle.");
				
				new str[600];
				
				format(str, sizeof(str), "%sModel:\t\t\t%d\n", str, GetVehicleModel(vid));
				format(str, sizeof(str), "%sColors:\t\t\t%d:%d\n", str, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);
				format(str, sizeof(str), "%sFuel:\t\t\t%d/%dL\n", str, floatround(Vehicle[vid][vehicle_fuel_current], floatround_ceil), VehicleFuelMax[GetVehicleModel(vid)-400]);
				format(str, sizeof(str), "%sFuel type:\t\t%s\n", str, VehicleFuelTypes[Vehicle[vid][vehicle_fuel_type]]);
				format(str, sizeof(str), "%sState:\t%.1f HP\n", str, Vehicle[vid][vehicle_health]);
				format(str, sizeof(str), "%sMileage:\t\t%d km\n", str, floatround(Vehicle[vid][vehicle_mileage], floatround_floor));
				if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_PLAYER )
				{
					format(str, sizeof(str), "%sOwner type:\t\tplayer\n", str);
					format(str, sizeof(str), "%sPlayer UID:\t\t%d\n", str, Vehicle[vid][vehicle_owner]);
				}
				else if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP )
				{
					format(str, sizeof(str), "%sOwner type:\t\tgroup\n", str);
					format(str, sizeof(str), "%sGroup UID:\t\t%d\n", str, Vehicle[vid][vehicle_owner]);
				}
				else
				{
					format(str, sizeof(str), "%sOwner type:\t\tNone\n", str);
				}
				format(str, sizeof(str), "%sEngine capacity:\t%.1f cm3\n\n", str, VehicleEngineCapacity[GetVehicleModel(vid)-400]*1000);
				format(str, sizeof(str), "%sParking:\n", str);
				format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"X:"HEX_COLOR_WHITE"\t\t%.4f\n", str, Vehicle[vid][vehicle_park][0]);
				format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"Y:"HEX_COLOR_WHITE"\t\t%.4f\n", str, Vehicle[vid][vehicle_park][1]);
				format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"Z:"HEX_COLOR_WHITE"\t\t%.4f\n", str, Vehicle[vid][vehicle_park][2]);
				format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"A:"HEX_COLOR_WHITE"\t\t%.4f\n", str, Vehicle[vid][vehicle_park][3]);
				format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"VW:"HEX_COLOR_WHITE"\t\t%d\n", str, Vehicle[vid][vehicle_park_world]);
				format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"INT:"HEX_COLOR_WHITE"\t\t%d\n", str, Vehicle[vid][vehicle_park_interior]);
				
				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("%s (SampID: %d, UID: %d) » Informations", VehicleNames[GetVehicleModel(vid)-400], vid, Vehicle[vid][vehicle_uid]), str, "OK", "");
			}
		}
	}
	else
	{
		new sub_cmd[40], param[150];
		sscanf(params, "s[40] S()[150]", sub_cmd, param);
		
		if( isnull(sub_cmd) )
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /v [track, park, tuning, assign, lock, info, desc, sign]");
			
			new str[400], rows, fields;
		
			mysql_query(mySQLconnection, sprintf("SELECT vehicle_uid, vehicle_model FROM `ipb_vehicles` WHERE `vehicle_ownertype` = %d AND `vehicle_owner` = %d", VEHICLE_OWNER_TYPE_PLAYER, pInfo[playerid][player_id]));
			cache_get_data(rows, fields);
			
			if( !rows ) SendGuiInformation(playerid, "Information", "You don't have any vehicles.");
			else
			{
				DynamicGui_Init(playerid);
				for(new row = 0; row != rows; row++)
				{
					new uid = cache_get_row_int(row, 0);
					
					// Wszystkie pojazdy gracza dodajemy do listy
					format(str, sizeof(str), "%s%s%d\t\t%s\n", str, ((GetVehicleByUid(uid) != INVALID_VEHICLE_ID) ? (HEX_COLOR_GREEN) : (HEX_COLOR_WHITE)), uid, VehicleNames[cache_get_row_int(row, 1)-400]);
					DynamicGui_AddRow(playerid, uid);
				}
			
				ShowPlayerDialog(playerid, DIALOG_PLAYER_VEHICLES, DIALOG_STYLE_LIST, "Your private vehicles:", str, "(Un)spawn", "Close");
			}
		}
		else
		{
			if( !strcmp(sub_cmd, "lock") )
			{
				new v_id = GetNearestPlayerVehicle(playerid);
				if( v_id == -1 ) return SendGuiInformation(playerid, "Information", "No access to this vehicle.");
				
				if( Vehicle[v_id][vehicle_locked] )
				{
					Vehicle[v_id][vehicle_locked] = false;
					GameTextForPlayer(playerid, "~w~car ~g~opened", 3000, 6);
				}
				else
				{
					Vehicle[v_id][vehicle_locked] = true;
					GameTextForPlayer(playerid, "~w~car ~r~locked", 3000, 6);
				}
				
				if( !VehicleHasAccessory(v_id, VEHICLE_ACCESSORY_ALARM) )
				{
					ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.1, 0, 0, 0, 0, 0, 1);
				}
				else
				{
					foreach(new p : Player)
					{
						if( GetPlayerDistanceFromPoint(p, Vehicle[v_id][vehicle_last_pos][0], Vehicle[v_id][vehicle_last_pos][1], Vehicle[v_id][vehicle_last_pos][2]) <= 20.0 ) PlayAudioStreamForPlayer(p, "http://iphonegu.com/wp-content/uploads/2012/09/car_lock.mp3", Vehicle[v_id][vehicle_last_pos][0], Vehicle[v_id][vehicle_last_pos][1], Vehicle[v_id][vehicle_last_pos][2], 20.0, 1);
					}
					
					ApplyAnimation(playerid, "HEIST9", "Use_SwipeCard", 4.1, 0, 0, 0, 0, 0, 1);
				}
			}
			else if( !strcmp(sub_cmd, "track", false) )
			{
				new v_uid;
				if( sscanf(param, "d", v_uid) )
				{
					if( pInfo[playerid][player_vehicle_target] > -1 )
					{
						Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
						Streamer_UpdateEx(playerid, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][0], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][1], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][2]);
						
						SendClientMessage(playerid, COLOR_GOLD, "Vehicle tracking off.");
						pInfo[playerid][player_vehicle_target] = -1;
					}
					else
					{
						SendClientMessage(playerid, COLOR_GREY, "Tip: /v track [uid]");
					}
					return 1;
				}
				
				new vid = GetVehicleByUid(v_uid);
				if( vid == INVALID_VEHICLE_ID ) return SendGuiInformation(playerid, "Information", "Invalid vehicle uid.");
				
				if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Information", "No access to this vehicle.");
				
				// Jesli juz jakies namierzanie jest wlaczone to je wylaczamy
				if( pInfo[playerid][player_vehicle_target] > -1 )
				{
					Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
					
					Streamer_UpdateEx(playerid, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][0], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][1], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][2]);
				}
				
				pInfo[playerid][player_vehicle_target] = vid;
				Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
				
				Streamer_UpdateEx(playerid, Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2]);
				
				new other_vw[60];
				if( GetVehicleVirtualWorld(vid) > 0 ) format(other_vw, sizeof(other_vw), "\n\n"HEX_COLOR_LIGHTER_RED"This vehicle is in other virtual world (%d).", GetVehicleVirtualWorld(vid));
				
				SendGuiInformation(playerid, "Information", sprintf("Vehicle has been marked on your map.\nType /v track off or get in vehicle to off. %s", other_vw));
				return 1;	
			}
		}
	}
	return 1;
}


COMMAND:inv(playerid, params[])
{
	return cmd_inventory(playerid, params);
}

COMMAND:i(playerid, params[])
{
	return cmd_inventory(playerid, params);
}

COMMAND:items(playerid, params[])
{
	return cmd_inventory(playerid, params);
}

COMMAND:inventory(playerid, params[])
{
	if( isnull(params) )
	{
		ListPlayerItems(playerid);
	}
	else
	{
		new sub_cmd[40], param[128];
		sscanf(params, "s[40] S()[128]", sub_cmd, param);
		
		if( !strcmp(sub_cmd, "pickup") )
		{
			ListPlayerGroundItems(playerid);
			return 1;
		}

		else if( !strcmp(sub_cmd, "take") )
		{
			if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Alert", "No access to this command.");
			new item, itemid, targetid;
			if(sscanf(param, "dd", targetid, item)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /inv take [player id] [item uid]");
			if(!IsPlayerConnected(targetid)||targetid==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");

			itemid = GetItemByTargetUid(item, targetid);
			if(itemid == -1) return SendGuiInformation(playerid, "Information", "Invalid item UID.");
			if(!Iter_Contains(PlayerItems[targetid], itemid)) return SendGuiInformation(playerid, "Information", "Invalid item UID.");

			pInfo[playerid][player_capacity] += PlayerItem[targetid][itemid][player_item_weight];
			pInfo[targetid][player_capacity] -= PlayerItem[targetid][itemid][player_item_weight];

			if(PlayerItem[targetid][itemid][player_item_used])
			{
				SendGuiInformation(playerid, "Information", "This item is used.");
				return 1;
			}

			mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_owner = %d WHERE item_uid = %d", pInfo[playerid][player_id], item));

			SendGuiInformation(playerid, "Information", sprintf("You took %s (UID %d) froom %s.", PlayerItem[targetid][itemid][player_item_name], item, pInfo[targetid][player_name]));
			SendGuiInformation(targetid, "Information", sprintf("Administrator %s has took your %s (UID %d).", pInfo[playerid][player_name], PlayerItem[targetid][itemid][player_item_name], item));

			DeleteItem(itemid, false, targetid);
			LoadPlayerItem(playerid, sprintf("WHERE `item_uid` = %d", item), true);
		}

		else if( !strcmp(sub_cmd, "list") )
		{
			if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Alert", "No access to this command.");
			new player;
			if(	sscanf(param, "d", player) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /inv list [player id]");
			if(!IsPlayerConnected(player)) return SendClientMessage(playerid, COLOR_GREY, "Tip: Invalid player id.");
			
			new count, string[1024], header[64];
			
			format(string, sizeof(string), "%sName\tWeight\tUID\n", string);
			format(header, sizeof(header), "Items of%s", pInfo[player][player_name]);

			foreach (new item : PlayerItems[player])
			{
				new prefix[10]; 
				if( PlayerItem[player][item][player_item_used] ) prefix = "{CFFD66}";

				format(string, sizeof(string), "%s %s%s\t%dg\t%d\n", string, prefix, PlayerItem[player][item][player_item_name], PlayerItem[player][item][player_item_weight], PlayerItem[player][item][player_item_uid]);
				count++;
			}
		
			if( count == 0 ) SendGuiInformation(playerid, "Information", "This player has no items.");
			else ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST_HEADERS, header, string, "OK", "");
		}

		foreach(new item: PlayerItems[playerid])
		{
		 	if(strfind(PlayerItem[playerid][item][player_item_name], sub_cmd, true) >= 0)
		 	{
		 		if(PlayerItem[playerid][item][player_item_type] == ITEM_TYPE_WEAPON)
		 		{
		 			if(PlayerItem[playerid][item][player_item_used])
		 			{
		 				Item_Use(item, playerid, true);
		 			}
		 			else
		 			{
			 		 	if(strlen(param) <= 0)
			 		 	{
			 		 		SendPlayerInformation(playerid, "~w~You need to input item ~p~place~w~.~n~Example: /inv Deagle from holster.", 5000);
			 		 		return 1;
			 		 	}
			 		 	Item_Use(item, playerid, true, param);
			 		 }
		 		}	
		 	}
		}
	}
	
	return 1;
}

COMMAND:debugicon(playerid, params[])
{
	RemovePlayerMapIcon(playerid, 12);
	RemovePlayerMapIcon(playerid, 13);
	RemovePlayerMapIcon(playerid, 14);
	RemovePlayerMapIcon(playerid, 15);
	RemovePlayerMapIcon(playerid, 16);
	RemovePlayerMapIcon(playerid, 16);
	return 1;
}

COMMAND:help(playerid, params[])
{
	new text[128], str[160];
	if( sscanf(params, "s[128]", text) )
	{
		if( HasCrewFlag(playerid, CREW_FLAG_GM) || HasCrewFlag(playerid, CREW_FLAG_ADMIN) || HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) )
		{
			ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_LIST, ""opis"Help", "Have a question?\nCommands\nJob\nVehicles\nAnimations\nAdmin commands", "OK", "");
		}
		else
		{
			ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_LIST, ""opis"Help", "Have a question?\nCommands\nJob\nVehicles\nAnimations", "OK", "");
		}
	}
	else
	{
		format(str, sizeof(str), "[Question from %s] (ID %d): %s.", pInfo[playerid][player_name], playerid, text);
		foreach(new p : Player)
		{
			if(PlayerHasFlag(playerid, PLAYER_FLAG_HELPER))
			{
				SendClientMessage(p, COLOR_CAR_STATE, str);
			}
		}
	}
	return 1;
}