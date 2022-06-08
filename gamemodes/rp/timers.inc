task MinuteTimer[60000]()
{	
  	new hour, minute, second;
	gettime(hour, minute, second);

	// Ustawiamy czas 
	WorldTime = hour;

	// Restart serwera
	if (hour == 4 && minute == 00)
	{
		SendClientMessageToAll(COLOR_LIGHTER_RED, "Society RP: Automatic server restart in five seconds.");
		
		foreach(new playerid: Player)
		{
			Kick(playerid);
		}

		//Mo¿e æwiczyæ
		mysql_query(mySQLconnection, "UPDATE ipb_characters SET char_training = 10");

		//Reset punktów aktywnoœci
		mysql_query(mySQLconnection, "UPDATE ipb_game_groups SET group_activity = 0");

		//Reset blokad 
		mysql_query(mySQLconnection, "DELETE FROM ipb_objects WHERE object_type = 2");

		defer RestartServer[5000]();
	}

	// Pozary here 
	foreach(new fsid: FireSources)
	{
		if(FireSource[fsid][fs_door] > 0)
		{
			if( DoorHasAccess(FireSource[fsid][fs_door], DOOR_ACCESS_FIRE_SPRINKLER) )
			{
				if(FireSource[fsid][fs_health] <= 25.0)
				{
					new str[10];
					FireSource[fsid][fs_health] += 5.0;
					format(str, sizeof(str), "%.2f%%", FireSource[fsid][fs_health]);
					UpdateDynamic3DTextLabelText(FireSource[fsid][fs_label], 0xF07800FF, str);

					if(FireSource[fsid][fs_health] == 30.0)
					{
						new d_id = FireSource[fsid][fs_door];
						Door[d_id][door_burned] = 30;
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_doors SET door_burned = 30 WHERE door_uid = %d", Door[d_id][door_uid]));
						StopFireSource(fsid);
						StopFire();
					}
				}
			}
			else
			{
				if(FireSource[fsid][fs_health] <= 90.0)
				{
					new str[10];
					FireSource[fsid][fs_health] += 10.0;
					format(str, sizeof(str), "%.2f%%", FireSource[fsid][fs_health]);
					UpdateDynamic3DTextLabelText(FireSource[fsid][fs_label], 0xF07800FF, str);

					if(FireSource[fsid][fs_health] > 9)
					{
						new d_id = FireSource[fsid][fs_door];
						Door[d_id][door_burned] = floatround(FireSource[fsid][fs_health]);
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_doors SET door_burned = %d WHERE door_uid = %d", Door[d_id][door_burned], Door[d_id][door_uid]));
					}
					else if(FireSource[fsid][fs_health] > 99)
					{
						new d_id = FireSource[fsid][fs_door];
						Door[d_id][door_burned] = 100;
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_doors SET door_burned = 100 WHERE door_uid = %d", Door[d_id][door_uid]));
						StopFireSource(fsid);
						StopFire();
					}
				}
			}
		}
	}

	foreach(new playerid : Player)
	{
		// Ustawiamy godzine dla gracza 
		if(GetPlayerVirtualWorld(playerid) == 0)
		{
			SetPlayerTime(playerid, WorldTime+2, 0);
		}

		// Niszczenie interioru
		if(pInfo[playerid][player_demolition] > 0)
		{
			if(GetPlayerVirtualWorld(playerid) == pInfo[playerid][player_demolition])
			{
				new d_id = GetDoorByUid(pInfo[playerid][player_demolition]);

				if(Door[d_id][door_demolition] <= 90)
				{
					Door[d_id][door_demolition] += 10;

					TextDrawSetString(Tutorial[playerid], sprintf("Demolition in ~p~progress~w~.~n~Destruction state: ~r~~h~~h~%d%%~n~~w~Every minute is 10 percent of destruction.", Door[d_id][door_demolition]));
					TextDrawShowForPlayer(playerid, Tutorial[playerid]);

					if(Door[d_id][door_demolition] >= 100)
					{
						SendClientMessage(playerid, COLOR_GOLD, "Success. Building has been 100 percent destroyed.");
						Door[d_id][door_destroyed] = 100;
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_doors SET door_destroyed = 100 WHERE door_uid = %d", Door[d_id][door_uid]));
						TextDrawHideForPlayer(playerid, Tutorial[playerid]);
						pInfo[playerid][player_demolition] = 0;
					}
				}
			}
			else if(GetPlayerVirtualWorld(playerid) == 0)
			{
				new d_id = GetDoorByUid(pInfo[playerid][player_demolition]);
				SendClientMessage(playerid, COLOR_GOLD, sprintf("Success. Building has been %d percent destroyed.", Door[d_id][door_demolition]));
				Door[d_id][door_destroyed] = Door[d_id][door_demolition];
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_doors SET door_destroyed = %d WHERE door_uid = %d", Door[d_id][door_demolition], Door[d_id][door_uid]));
				TextDrawHideForPlayer(playerid, Tutorial[playerid]);
				pInfo[playerid][player_demolition] = 0;
			}
		}

		// Silownia
		if(pInfo[playerid][player_training] == true)
		{
			if(pInfo[playerid][player_gym_carnet] > 0)
			{
				new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_GYM_KARNET);
				if(itemid != -1)
				{
					PlayerItem[playerid][itemid][player_item_value1] --;
					mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_value1 = %d WHERE item_uid = %d", PlayerItem[playerid][itemid][player_item_value1], PlayerItem[playerid][itemid][player_item_uid]));
				}

				pInfo[playerid][player_gym_carnet]--;
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_training = %d WHERE char_uid = %d", pInfo[playerid][player_gym_carnet], pInfo[playerid][player_id]));
			}
		}

		// Sprawdzamy czy mo¿e wyjœæ z paki 
		if(pInfo[playerid][player_arrest] > 0)
        {
            if(pInfo[playerid][player_arrest] < gettime())
            {
                mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_spawn = 0, char_spawn_type = 1, char_arrest = 0 WHERE char_uid = %d", pInfo[playerid][player_id]));
                SendGuiInformation(playerid, "Information", "You are free now.");
 				RemovePlayerStatus(playerid, PLAYER_STATUS_JAIL); 
 				pInfo[playerid][player_arrest] = 0;       
 			}
        }

		// Poprawiamy ostatni skin 
		pInfo[playerid][player_last_skin] = GetPlayerSkin(playerid);

		// Sprawdzamy czy mo¿e wyjœæ z AJ
		if(pInfo[playerid][player_aj] > 0)
		{
			if(pInfo[playerid][player_aj] == 1)
			{
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_aj = 0 WHERE char_uid = %d", pInfo[playerid][player_id]));
				SetPlayerVirtualWorld(playerid, 0);
				scrp_SpawnPlayer(playerid, true);
				pInfo[playerid][player_aj] = 0;
			}
			else
			{
				pInfo[playerid][player_aj]--;
				GameTextForPlayer(playerid,  sprintf("~w~end of ~y~aj~w~: ~y~%d ~w~min.", pInfo[playerid][player_aj]), 5000, 1);
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_aj = %d WHERE char_uid = %d", pInfo[playerid][player_aj], pInfo[playerid][player_id]));
			}
		}

		// Naliczamy czas gry
		if( pInfo[playerid][player_afk] == false )
		{
			pInfo[playerid][player_minutes] ++;
		    if(pInfo[playerid][player_minutes] > 59)
		    {
		    	pInfo[playerid][player_hours] ++;
		    	pInfo[playerid][player_minutes] =0;
		    	
				if(IsPlayerVip(playerid))
				{
					pGlobal[playerid][glo_score] += 20;
				}
				else							
				{
					pGlobal[playerid][glo_score] += 10;
				}
				
		    	SetPlayerScore(playerid, pGlobal[playerid][glo_score]);
		    	HourPayday(playerid);
			}
		}
	}
	return 1;
}

timer UnfreezePlayer[1000](playerid)
{
	TogglePlayerControllable(playerid, 1);
}

timer HidePlayerDoorText[5000](playerid)
{
	TextDrawHideForPlayer(playerid, DoorInfo[playerid]);
	TextDrawHideForPlayer(playerid, DoorInfo2[playerid]);
}

timer FixTires[5000](vehid)
{
	Vehicle[vehid][vehicle_damage][3] = 0;
	UpdateVehicleDamageStatus(vehid, Vehicle[vehid][vehicle_damage][0], Vehicle[vehid][vehicle_damage][1], Vehicle[vehid][vehicle_damage][2], Vehicle[vehid][vehicle_damage][3]);
}

timer CarAutoFix[60000](playerid, vehid)
{
	if(pInfo[playerid][player_area] != pInfo[playerid][player_dialog_tmp1]) return 1;
	new panels, doors, lights, tires;

	GetVehicleDamageStatus(vehid, panels, doors, lights, tires);
	
   	Vehicle[vehid][vehicle_damage][0] = 0;
    Vehicle[vehid][vehicle_damage][1] = 0;
    Vehicle[vehid][vehicle_damage][2] = 0;
    Vehicle[vehid][vehicle_damage][3] = tires;

	Vehicle[vehid][vehicle_health] = 1000.00;
	SetVehicleHealth(vehid, 1000);
    
    Vehicle[vehid][vehicle_destroyed] = false;

    UpdateVehicleDamageStatus(vehid, Vehicle[vehid][vehicle_damage][0], Vehicle[vehid][vehicle_damage][1], Vehicle[vehid][vehicle_damage][2], Vehicle[vehid][vehicle_damage][3]);

    new visual_damage[32];
	format(visual_damage, sizeof(visual_damage), "%d %d %d %d", Vehicle[vehid][vehicle_damage][0], Vehicle[vehid][vehicle_damage][1], Vehicle[vehid][vehicle_damage][2], Vehicle[vehid][vehicle_damage][3]);
    mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_health = '%f', vehicle_visual= '%s' WHERE vehicle_uid = %d", Vehicle[vehid][vehicle_health], visual_damage, Vehicle[vehid][vehicle_uid]));
    TextDrawHideForPlayer(playerid, Tutorial[playerid]);
    SendGuiInformation(playerid,"Informacja", "Success! Vehicle fixed.");
    return 1;
}

timer StartTraining[1000](playerid)
{
	SetPlayerAttachedObject(playerid, ATTACH_SLOT_GYM1, 3072, 5); // lewa reka
	SetPlayerAttachedObject(playerid, ATTACH_SLOT_GYM2, 3071, 6); // prawa reka

	new LocalLabel[130];
	format(LocalLabel,sizeof(LocalLabel),"~w~Press ~y~arrows ~u~ ~d~ ~w~ to train.~n~~n~Progress: ~y~%d~n~~w~Strenght ~y~%.2f%%~n~~w~Ticket time: ~y~%dmin", pInfo[playerid][player_gym_count], pInfo[playerid][player_strength], pInfo[playerid][player_gym_carnet]);
	TextDrawSetString(Tutorial[playerid], LocalLabel);
	TextDrawShowForPlayer(playerid, Tutorial[playerid]);
	
	pInfo[playerid][player_training] = true;
}


timer ResetDumb[2000](playerid, type)
{
	if(type == 1)
	{
		pInfo[playerid][player_can_train] = 2;
	}
	else if(type == 2)
	{
		pInfo[playerid][player_can_train] = 1;
	}
}


timer AnimHitPlayer[15000](playerid)
{
	if(pInfo[playerid][player_bw] > 0) return 1;
    SetPVarInt(playerid, "AnimHitPlayerGun", 0);
    ClearAnimations(playerid);
    SetPlayerDrunkLevel(playerid, 2500);
    return 1;
}

timer HideActor[540000](actorid, vid)
{
	ActorProx(actorid, "Marcus Bradford", "lefts meeting place.", PROX_ME);
    SetActorPos(actorid, 2154.9539,-2697.7468,17.8828);
    DeleteVehicle(vid);
    
	foreach(new a: Areas)
	{
		if(Area[a][area_type] == AREA_TYPE_ARMDEALER)
		{
			DestroyDynamicArea(a);
			for(new z=0; e_areas:z != e_areas; z++)
		    {
				Area[a][e_areas:z] = 0;
		    }
		    new next;
		    Iter_SafeRemove(Areas, a, next);
		    a = next;
		}
	}
}

timer EndCall[3000](playerid)
{
	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
}

timer KickPlayer[3000](playerid)
{
	Kick(playerid);
}

forward WelcomeTimer(playerid);
public WelcomeTimer(playerid)
{
    return KickAc(playerid, -1, "Login session too long");
}

timer GiveRobbedCash[1000](playerid)
{
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	ClearActorAnimations(Cluckin);
	ApplyActorAnimation(Cluckin,"SHOP","SHP_Rob_GiveCash",4.1,0,1,1,1,0);
	new randoms;
	randoms = random(700);
	pInfo[playerid][player_money]+=randoms;
	GivePlayerMoney(playerid, randoms);
	SendPlayerInformation(playerid, "~w~Napad przebiegl pomyslnie, otrzymujesz gotowke z ~y~kasy~w~.", 4000);
	PlayerPlaySound(playerid, 3401, x, y, z);
}

timer HideInformationTextdraw[100](playerid, information_id)
{
	if( pInfo[playerid][player_info_td_id] == information_id )
	{
		TextDrawHideForPlayer(playerid, Tutorial[playerid]);
		pInfo[playerid][player_info_td_id] = 0;
	}
}

timer ApplyAnim[1000](playerid, type)
{
	switch( type )
	{
		case ANIM_TYPE_BW: ApplyAnimation(playerid, "KNIFE", "KILL_Knife_Ped_Die", 4.0, 0, 0, 0, 1, 0, 1);
	}
}

timer HideGroupDutyPane[100](playerid)
{
	TextDrawHideForPlayer(playerid, DashBoardPane[playerid]);
	TextDrawHideForPlayer(playerid, DashBoardPane2[playerid]);
}

timer RestartServer[100]()
{
	SendRconCommand("gmx");
}

timer VehicleEngineStart[1000](playerid, vehicleid)
{
	TextDrawHideForPlayer(playerid, vehicleInfo);
	
	Vehicle[vehicleid][vehicle_engine_starting] = false;
	Vehicle[vehicleid][vehicle_engine] = true;
	
	SendPlayerInformation(playerid, "Engine ~y~started~w~!", 3500);
	UpdateVehicleVisuals(vehicleid);
}

timer DeliverProduct[540000](gid, prod_id, count)
{
	new prod_name[40];
    format(prod_name, sizeof(prod_name), "%s", Product[prod_id][product_name]);

	Item_Create(ITEM_OWNER_TYPE_GROUP, Group[gid][group_uid], Product[prod_id][product_type], Product[prod_id][product_model], Product[prod_id][product_value1], Product[prod_id][product_value2], prod_name, Product[prod_id][product_extra], count);

	foreach(new pid : Player)
	{
		new slot = GetPlayerGroupSlot(pid, gid);
		if( slot != -1 )
		{
			if( WorkerHasFlag(pid, slot, WORKER_FLAG_ORDER) )
			{
				SendClientMessage(pid, COLOR_YELLOW, "> (SMS) [333] Wholesale: Products delivered to storage.");
			}
		}
	}

	return 1;
}

timer DestroyQuitText[15000](Text3D:textid)
{
	DestroyDynamic3DTextLabel(Text3D:textid);
}

timer PreloadAllAnimLibs[200](playerid)
{
	PreloadAnimLib(playerid,"BOMBER");
   	PreloadAnimLib(playerid,"RAPPING");
    PreloadAnimLib(playerid,"SHOP");
   	PreloadAnimLib(playerid,"BEACH");
   	PreloadAnimLib(playerid,"SMOKING");
	PreloadAnimLib(playerid,"RYDER");
	PreloadAnimLib(playerid,"PLAYIDLES");
	PreloadAnimLib(playerid,"POOL");
	PreloadAnimLib(playerid,"DANCING");
	PreloadAnimLib(playerid,"LOWRIDER");
	PreloadAnimLib(playerid,"INT_SHOP");
	PreloadAnimLib(playerid,"wuzi");
	PreloadAnimLib(playerid,"CRIB");
	PreloadAnimLib(playerid,"POLICE");
	PreloadAnimLib(playerid,"GRAVEYARD");
	PreloadAnimLib(playerid,"FIGHT_D");
    PreloadAnimLib(playerid,"ON_LOOKERS");
	PreloadAnimLib(playerid,"RIOT");
	PreloadAnimLib(playerid,"GANGS");
    PreloadAnimLib(playerid,"DEALER");
	PreloadAnimLib(playerid,"VENDING");
	PreloadAnimLib(playerid,"HEIST9");
	PreloadAnimLib(playerid,"CRACK");
	PreloadAnimLib(playerid,"SPRAYCAN");
	PreloadAnimLib(playerid,"JST_BUISNESS");
	PreloadAnimLib(playerid,"PAULNMAC");
	PreloadAnimLib(playerid,"GRENADE");
	PreloadAnimLib(playerid,"WAYFARER");
	PreloadAnimLib(playerid,"INT_OFFICE");
	PreloadAnimLib(playerid,"ROB_BANK");
	PreloadAnimLib(playerid,"STRIP");
	PreloadAnimLib(playerid,"GHANDS");
	PreloadAnimLib(playerid,"PARK");
	PreloadAnimLib(playerid,"CAR");
	PreloadAnimLib(playerid,"CARRY");
	PreloadAnimLib(playerid,"KISSING");
	PreloadAnimLib(playerid,"MISC");
	PreloadAnimLib(playerid,"OTB");
	PreloadAnimLib(playerid,"COP_AMBIENT");
	PreloadAnimLib(playerid,"MEDIC");
	PreloadAnimLib(playerid,"INT_HOUSE");
	PreloadAnimLib(playerid,"FOOD");
	PreloadAnimLib(playerid,"PED");
	PreloadAnimLib(playerid,"SWEET");
	PreloadAnimLib(playerid,"KNIFE");
	PreloadAnimLib(playerid,"CASINO");
	PreloadAnimLib(playerid,"AIRPORT");
	PreloadAnimLib(playerid,"Attractors");
	PreloadAnimLib(playerid,"BAR");
	PreloadAnimLib(playerid,"BASEBALL");
	PreloadAnimLib(playerid,"BD_FIRE");
	PreloadAnimLib(playerid,"benchpress");
	PreloadAnimLib(playerid,"BF_injection");
	PreloadAnimLib(playerid,"BLOWJOBZ");
	PreloadAnimLib(playerid,"BOX");
	PreloadAnimLib(playerid,"BSKTBALL");
	PreloadAnimLib(playerid,"BUDDY");
	PreloadAnimLib(playerid,"CAMERA");
	PreloadAnimLib(playerid,"DAM_JUMP");
	PreloadAnimLib(playerid,"FAT");
}