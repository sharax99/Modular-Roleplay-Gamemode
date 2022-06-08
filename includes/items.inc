stock ListPlayerItems(playerid)
{
	new count, string[1024], header[64];
		
	DynamicGui_Init(playerid);
	
	format(string, sizeof(string), "%sName\tWeight\tUID\n", string);
	format(header, sizeof(header), "Your items list [Capacity: %dg]", pInfo[playerid][player_capacity]);
	
	foreach (new item : PlayerItems[playerid])
	{
		new prefix[10]; 
		if( PlayerItem[playerid][item][player_item_used] ) prefix = "{CFFD66}";

		format(string, sizeof(string), "%s %s%s\t%dg\t%d\n", string, prefix, PlayerItem[playerid][item][player_item_name], PlayerItem[playerid][item][player_item_weight], PlayerItem[playerid][item][player_item_uid]);
		DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, item);	
		count++;
	}
	
	if( count == 0 ) SendGuiInformation(playerid, "Information", "You don't have any items.");
	else ShowPlayerDialog(playerid, DIALOG_PLAYER_ITEMS, DIALOG_STYLE_TABLIST_HEADERS, header, string, "Use", "Options");
	return 1;
}

stock ListPlayerGroundItems(playerid)
{
    new list_items[512], main_query[2048], query[256],
        ground_item_uid, ground_item_name[40];

    format(main_query, sizeof(main_query), "SELECT `item_uid`, `item_name` FROM `ipb_items` WHERE ");

    DynamicGui_Init(playerid);

    if(!IsPlayerInAnyVehicle(playerid))
    {
        new object_id = INVALID_OBJECT_ID, Float:distance,
            Float:PosX, Float:PosY, Float:PosZ;

        GetPlayerPos(playerid, PosX, PosY, PosZ);
        for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
        {
            if(IsValidPlayerObject(playerid, player_object))
            {
                object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);
                Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, distance);

                if(distance <= 3.0 && Object[object_id][object_uid] == 0)
                {
                    ground_item_uid = (Streamer_GetIntData(STREAMER_TYPE_OBJECT, object_id, E_STREAMER_EXTRA_ID) * -1);
                    format(query, sizeof(query), "item_uid = %d", ground_item_uid);

   					if(strlen(main_query) > 64)
   					{
   				    	if(strlen(main_query) + strlen(query) < sizeof(main_query))
   				    	{
				   			strcat(main_query, " OR ", sizeof(main_query));
						}
						else
						{
					    	strcat(main_query, ";", sizeof(main_query));
						}
					}
			  		strcat(main_query, query, sizeof(main_query));
                }
            }
        }

        if(!ground_item_uid)
        {
        	SendGuiInformation(playerid, "Information", "There are no items in your area.");
        	return 1;
        }
        
        new rows, fields;
        
        mysql_query(mySQLconnection, main_query);
       	cache_get_data(rows, fields);
       	
		for(new row = 0; row != rows; row++)
		{
			ground_item_uid = cache_get_row_int(row, 0);
			cache_get_row(row, 1, ground_item_name, mySQLconnection, 40);

			format(list_items, sizeof(list_items), "%s\n%d\t%s", list_items, ground_item_uid, ground_item_name);
			DynamicGui_AddRow(playerid, DG_ITEMS_PICKUP_ROW, ground_item_uid);
		}
		
		if(strlen(list_items) > 0)
		{
			ShowPlayerDialog(playerid, DIALOG_ITEMS_PICKUP, DIALOG_STYLE_LIST, "Closest items:", list_items, "Pickup", "Cancel");
      	}
       	else
       	{
        	SendGuiInformation(playerid, "Information", "There are no items in your area.");
		}
    }
    else
    {
    	new vid = GetPlayerVehicleID(playerid), rows, fields;

    	new gid = pInfo[playerid][player_duty_gid];
    	if(gid != -1)
    	{
    		if(!GroupHasFlag(gid, GROUP_FLAG_TAKE))
    		{
		    	if(!CanPlayerUseVehicle(playerid, vid))
		    	{
		    		SendGuiInformation(playerid, "Information", "You don't have access to this vehicle.");
		    		return 1;
		    	}
		    }
	    }
	    else
	    {
	    	if(!CanPlayerUseVehicle(playerid, vid))
	    	{
	    		SendGuiInformation(playerid, "Information", "You don't have access to this vehicle.");
	    		return 1;
	    	}
	    }

    	mysql_query(mySQLconnection, sprintf("SELECT item_uid, item_name FROM ipb_items WHERE item_owner = %d AND item_ownertype = %d", Vehicle[vid][vehicle_uid], ITEM_OWNER_TYPE_VEHICLE));
    	cache_get_data(rows, fields);

    	for(new row = 0; row != rows; row++)
		{
		    ground_item_uid = cache_get_row_int(row, 0);
			cache_get_row(row, 1, ground_item_name, mySQLconnection, 40);

			format(list_items, sizeof(list_items), "%s\n%d\t%s", list_items, ground_item_uid, ground_item_name);
			DynamicGui_AddRow(playerid, DG_ITEMS_PICKUP_ROW, ground_item_uid);
		}

		if(strlen(list_items) > 0)
		{
			ShowPlayerDialog(playerid, DIALOG_ITEMS_PICKUP, DIALOG_STYLE_LIST, "Items in vehicle:", list_items, "Pickup", "Cancel");
      	}
       	else
       	{
        	SendGuiInformation(playerid, "Information", "There are no items in this vehicle.");
		}
    }
    return 1;
}

stock LoadPlayerItem(playerid, limit[] = "", bool:return_id = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_items` %s", limit));  
	cache_get_data(rows, fields);
	
	new i_id = -1, ownertype;
	
	for(new row = 0; row != rows; row++)
	{
		ownertype = cache_get_row_int(row, 14);

		if(ownertype == ITEM_OWNER_TYPE_PLAYER)
		{
			i_id = Iter_Free(PlayerItems[playerid]);
			Iter_Add(PlayerItems[playerid], i_id);
			
			PlayerItem[playerid][i_id][player_item_uid] = cache_get_row_int(row, 0);
			PlayerItem[playerid][i_id][player_item_type] = cache_get_row_int(row, 4);
			PlayerItem[playerid][i_id][player_item_value1] = cache_get_row_int(row, 2);
			PlayerItem[playerid][i_id][player_item_value2] = cache_get_row_int(row, 3);
			PlayerItem[playerid][i_id][player_item_used] = cache_get_row_int(row, 19);
			
			PlayerItem[playerid][i_id][player_item_model] = cache_get_row_int(row, 18);
			
			PlayerItem[playerid][i_id][player_item_extraid] = cache_get_row_int(row, 23);
			PlayerItem[playerid][i_id][player_item_weight] = cache_get_row_int(row, 13);
			PlayerItem[playerid][i_id][player_item_created] = cache_get_row_int(row, 20);
			
			cache_get_row(row, 1, PlayerItem[playerid][i_id][player_item_name], mySQLconnection, 40);

			SetupPlayerItemWeight(playerid, i_id);
		}
		else
		{
			return 1;
		}
	}
	
	if( return_id ) return i_id;
	return 1;
}

stock LoadItem(limit[] = "", bool:return_id = false, bool:force_date = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_items` %s", limit));  
	cache_get_data(rows, fields);
	
	new i_id = -1, ownertype;
	
	for(new row = 0; row != rows; row++)
	{
		ownertype = cache_get_row_int(row, 14);

		if(ownertype == ITEM_OWNER_TYPE_GROUP)
		{
			i_id = Iter_Free(Items);
			Iter_Add(Items, i_id);
			
			Item[i_id][item_uid] = cache_get_row_int(row, 0);
			Item[i_id][item_type] = cache_get_row_int(row, 4);
			Item[i_id][item_value1] = cache_get_row_int(row, 2);
			Item[i_id][item_value2] = cache_get_row_int(row, 3);
			
			Item[i_id][item_owner_type] = cache_get_row_int(row, 14);
			Item[i_id][item_owner] = cache_get_row_int(row, 15);
			Item[i_id][item_group] = cache_get_row_int(row, 17);
			
			Item[i_id][item_model] = cache_get_row_int(row, 18);

			Item[i_id][item_price] = cache_get_row_int(row, 21);
			Item[i_id][item_count] = cache_get_row_int(row, 22);

			Item[i_id][item_extraid] = cache_get_row_int(row, 23);
			Item[i_id][item_weight] = cache_get_row_int(row, 13);
			
			cache_get_row(row, 1, Item[i_id][item_name], mySQLconnection, 40);
		}

		else if(ownertype == ITEM_OWNER_TYPE_GROUND)
		{
			new Float:ground_x, Float:ground_y, Float:ground_z, Float:ground_rx, Float:ground_ry, Float:ground_rz, ground_world, ground_interior, ground_object, ground_model, ground_uid, ground_date;

			ground_date = cache_get_row_int(row, 20);
			ground_world = cache_get_row_int(row, 11);

			if(ground_world == 0)
			{
				if(ground_date + 48*3600 > gettime())
				{
					ground_uid = cache_get_row_int(row, 0);
					ground_model = cache_get_row_int(row, 18);

					ground_x = cache_get_row_float(row, 5);
					ground_y = cache_get_row_float(row, 6);
					ground_z = cache_get_row_float(row, 7);
					ground_rx = cache_get_row_float(row, 8);
					ground_ry = cache_get_row_float(row, 9);
					ground_rz = cache_get_row_float(row, 10);
					
					ground_interior = cache_get_row_int(row, 12);

					ground_object = CreateDynamicObject(ground_model, ground_x, ground_y, ground_z, ground_rx, ground_ry, ground_rz, ground_world, ground_interior, -1, 100.0);
					Streamer_SetIntData(STREAMER_TYPE_OBJECT, ground_object, E_STREAMER_EXTRA_ID, (ground_uid * -1));
				}
				else
				{
					if(force_date == true)
					{
						ground_uid = cache_get_row_int(row, 0);
						ground_model = cache_get_row_int(row, 18);

						ground_x = cache_get_row_float(row, 5);
						ground_y = cache_get_row_float(row, 6);
						ground_z = cache_get_row_float(row, 7);
						ground_rx = cache_get_row_float(row, 8);
						ground_ry = cache_get_row_float(row, 9);
						ground_rz = cache_get_row_float(row, 10);

						ground_interior = cache_get_row_int(row, 12);

						ground_object = CreateDynamicObject(ground_model, ground_x, ground_y, ground_z, ground_rx, ground_ry, ground_rz, ground_world, ground_interior, -1, 100.0);
						Streamer_SetIntData(STREAMER_TYPE_OBJECT, ground_object, E_STREAMER_EXTRA_ID, (ground_uid * -1));
					}
				}
			}
			else
			{
				ground_uid = cache_get_row_int(row, 0);
				ground_model = cache_get_row_int(row, 18);

				ground_x = cache_get_row_float(row, 5);
				ground_y = cache_get_row_float(row, 6);
				ground_z = cache_get_row_float(row, 7);
				ground_rx = cache_get_row_float(row, 8);
				ground_ry = cache_get_row_float(row, 9);
				ground_rz = cache_get_row_float(row, 10);

				ground_interior = cache_get_row_int(row, 12);

				ground_object = CreateDynamicObject(ground_model, ground_x, ground_y, ground_z, ground_rx, ground_ry, ground_rz, ground_world, ground_interior, -1, 100.0);
				Streamer_SetIntData(STREAMER_TYPE_OBJECT, ground_object, E_STREAMER_EXTRA_ID, (ground_uid * -1));
			}
		}
	}
	
	if( return_id ) return i_id;
	return rows;
}

stock DeleteItem(itemid, bool:from_db = false, playerid = INVALID_PLAYER_ID)
{
	if(playerid != INVALID_PLAYER_ID)
	{
		if( from_db ) mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_items` WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_uid]));

		Iter_Remove(PlayerItems[playerid], itemid);

		for(new z=0; e_player_items:z != e_player_items; z++)
	    {
			PlayerItem[playerid][itemid][e_player_items:z] = 0;
	    }
	}
	else
	{
		if( from_db ) mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_items` WHERE `item_uid` = %d", Item[itemid][item_uid]));
		Iter_Remove(Items, itemid);
	
		for(new z=0; e_items:z != e_items; z++)
	    {
			Item[itemid][e_items:z] = 0;
	    }
	}
}

stock Item_Use(itemid, playerid = INVALID_PLAYER_ID, bool:via_cmd = false, addparam[]="")
{	
	switch( PlayerItem[playerid][itemid][player_item_type] )
	{
		case ITEM_TYPE_WEAPON:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				new wslot = GetWeaponSlot(PlayerItem[playerid][itemid][player_item_value1]);
				
				if(PlayerItem[playerid][itemid][player_item_value1] == 41)
				{
					new tmp, ammo;
					GetPlayerWeaponData(playerid, 9, tmp, ammo);
					PlayerItem[playerid][itemid][player_item_value2] = ammo;
				}
				else
				{
					PlayerItem[playerid][itemid][player_item_value2] = pWeapon[playerid][wslot][pw_ammo];
				}
				
				PlayerItem[playerid][itemid][player_item_used] = false;
				
				if( pWeapon[playerid][wslot][pw_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pWeapon[playerid][wslot][pw_object_index]);
				
				pWeapon[playerid][wslot][pw_itemid] = -1;
				pWeapon[playerid][wslot][pw_id] = -1;
				pWeapon[playerid][wslot][pw_ammo] = -1;
				pWeapon[playerid][wslot][pw_object_index] = -1;
				
				ResetPlayerWeapons(playerid);
				
				for(new i;i<13;i++)
				{
					if( pWeapon[playerid][i][pw_itemid] > -1 ) GivePlayerWeapon(playerid, pWeapon[playerid][i][pw_id], pWeapon[playerid][i][pw_ammo]);
				}
				
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value2` = %d WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_value2], PlayerItem[playerid][itemid][player_item_uid]));

				SetPVarInt(playerid, "weaping", 0);
				SetPVarInt(playerid, "taser", 0);

				if(via_cmd == true)
				{
					if(strlen(addparam))
					{
						cmd_ame(playerid, sprintf("hides %s %s.", PlayerItem[playerid][itemid][player_item_name], addparam));
					}
					else
					{
						cmd_ame(playerid, sprintf("hides %s.", PlayerItem[playerid][itemid][player_item_name]));
					}
				}
				else
				{
					cmd_ame(playerid, sprintf("hides %s.", PlayerItem[playerid][itemid][player_item_name]));
				}
			}
			else
			{
				if(IsPlayerInAnyVehicle(playerid)) return SendGuiInformation(playerid, "Information", "Action not allowed in vehicle.");
				if(GetPVarInt(playerid, "weaping") > 0) return SendGuiInformation(playerid, ""guiopis"Information", "You are already using weapon.\nHide current to use next!");
				new wslot = GetWeaponSlot(PlayerItem[playerid][itemid][player_item_value1]);
				
				// Sprawdzamy czy jest amunicja
				if( PlayerItem[playerid][itemid][player_item_value2] == 0 ) return SendGuiInformation(playerid, ""guiopis"Information", "There is no ammunation in this weapon!");
				
				// Sprawdzamy czy czasem nie ma juz broni tego samego typu
				new wtype = GetWeaponType(PlayerItem[playerid][itemid][player_item_value1]);
				for(new i;i<13;i++)
				{
					if( pWeapon[playerid][i][pw_itemid] > -1 && GetWeaponType(pWeapon[playerid][i][pw_id]) == wtype ) return SendGuiInformation(playerid, ""guiopis"Information", "You are already using weapon.\nHide current to use next!");
				}

				if( GetPlayerUsedItem(playerid, ITEM_TYPE_GLOVES) == -1 )
				{
					mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_fingerprints` (finger_owner, finger_person) VALUES ('%d', '%d')", PlayerItem[playerid][itemid][player_item_uid], pInfo[playerid][player_id]));
				}
				
				if( PlayerItem[playerid][itemid][player_item_extraid] == 1)
				{
					new gid = pInfo[playerid][player_duty_gid];
					if(gid == -1) return SendGuiInformation(playerid, ""guiopis"Information", "You are not on group duty.");
					if(!(Group[gid][group_flags] & GROUP_FLAG_WEAPON_FLAG)) return SendGuiInformation(playerid, ""guiopis"Information", "You group has no weapon flag.");
				}

				if( PlayerItem[playerid][itemid][player_item_extraid] == 2)
				{
					new gid = pInfo[playerid][player_duty_gid];
					if(gid == -1) return SendGuiInformation(playerid, ""guiopis"Information", "You are not on LSPD duty.");
					if(!(Group[gid][group_flags] & GROUP_FLAG_911)) return SendGuiInformation(playerid, ""guiopis"Information", "You are not on LSPD duty.");
					SetPVarInt(playerid, "taser", 1);
				}

				if( PlayerItem[playerid][itemid][player_item_extraid] == 3)
				{
					if(!(pInfo[playerid][player_documents] & DOCUMENT_WEAPON)) return SendGuiInformation(playerid, "Information", "You don't have weapon license.");
				}

				//Edycja obiektu doczepialnego broni
				new ow = PlayerItem[playerid][itemid][player_item_value1], rows, fields;

				mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_attached_objects WHERE attach_owner = %d AND attach_model = %d LIMIT 1", pInfo[playerid][player_id], WeaponVisualModel[ow]));
				cache_get_data(rows, fields);

				if( rows )
				{
					new index = ATTACH_SLOT_WEAPON;

					ao[playerid][index][ao_x] = cache_get_row_float(0, 5);
			        ao[playerid][index][ao_y] = cache_get_row_float(0, 6);
			        ao[playerid][index][ao_z] = cache_get_row_float(0, 7);
			        ao[playerid][index][ao_rx] = cache_get_row_float(0, 8);
			        ao[playerid][index][ao_ry] = cache_get_row_float(0, 9);
			        ao[playerid][index][ao_rz] = cache_get_row_float(0, 10);

					ao[playerid][index][ao_inserted]=true;
				}
				else
				{
					ao[playerid][ATTACH_SLOT_WEAPON][ao_inserted] = false;
				}

				if(wslot == -1) return SendGuiInformation(playerid, "Information", "Try again.");
				
				pWeapon[playerid][wslot][pw_itemid] = itemid;
				pWeapon[playerid][wslot][pw_id] = PlayerItem[playerid][itemid][player_item_value1];
				pWeapon[playerid][wslot][pw_ammo] = PlayerItem[playerid][itemid][player_item_value2];
				GivePlayerWeapon(playerid, PlayerItem[playerid][itemid][player_item_value1], PlayerItem[playerid][itemid][player_item_value2]);
				
				PlayerItem[playerid][itemid][player_item_used] = true;

				SetPVarInt(playerid, "weaping", PlayerItem[playerid][itemid][player_item_value1]);

				if(via_cmd == false)
				{
					cmd_ame(playerid, sprintf("pulls out %s.", PlayerItem[playerid][itemid][player_item_name]));
				}
				else
				{
					cmd_ame(playerid, sprintf("pulls out %s %s.", PlayerItem[playerid][itemid][player_item_name], addparam));
				}
			}
		}

       	case ITEM_TYPE_ACCESS:
        {
            if( !PlayerItem[playerid][itemid][player_item_used] )
            {
            	if( GetPlayerUsedItem(playerid, ITEM_TYPE_ACCESS) > -1)
				{
					SendGuiInformation(playerid, "Information", "You are already using component.");
					return 1;
				}

                SendGuiInformation(playerid, ""guiopis"Information", "Component used, you can install it by /offer command.");
                PlayerItem[playerid][itemid][player_item_used]=true;
            }
            else
            {
                PlayerItem[playerid][itemid][player_item_used]=false;
            }
        }

        case ITEM_TYPE_DOOR_ACCESS:
        {
            if( !PlayerItem[playerid][itemid][player_item_used] )
            {
            	if( GetPlayerUsedItem(playerid, ITEM_TYPE_DOOR_ACCESS) > -1)
				{
					SendGuiInformation(playerid, "Information", "You are already using component.");
					return 1;
				}

                SendGuiInformation(playerid, ""guiopis"Information", "Component used, you can install it by /offer command.");
                PlayerItem[playerid][itemid][player_item_used]=true;
            }
            else
            {
                PlayerItem[playerid][itemid][player_item_used]=false;
            }
        }

		case ITEM_TYPE_BOMB:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				PlayerItem[playerid][itemid][player_item_used]=false;
				DeleteItem(itemid, true, playerid);
			}
			else
			{
				SendGuiInformation(playerid, ""guiopis"Information", "Use /bomb command.");
				PlayerItem[playerid][itemid][player_item_used]=true;
			}
		}

		case ITEM_TYPE_MOLOTOV:
		{
			if(Fire[fire_is_burning]) return SendGuiInformation(playerid, "Information", "There is another fire burning at this moment.");
			if(!IsAnyFirefighterOnline()) return SendGuiInformation(playerid, "Information", "To use this item, players from medic group have to be on duty.");

			new gid = pInfo[playerid][player_duty_gid];
			if(gid == -1) return SendGuiInformation(playerid, "Information", "You can use this item only in doors assigned to it.");
			if(Group[gid][group_uid] != PlayerItem[playerid][itemid][player_item_value2]) return SendGuiInformation(playerid, "Information", "You are not on group duty assigned to this item.");

			new d_uid = GetPlayerVirtualWorld(playerid);
			if(d_uid == 0 ) return SendGuiInformation(playerid, "Information", "You can use this item only in doors assigned to it.");
			if(d_uid != PlayerItem[playerid][itemid][player_item_value1]) return SendGuiInformation(playerid, "Information", "You can use this item only in doors assigned to it.");

			new d_id = GetDoorByUid(d_uid);
			if(d_id == -1) return SendGuiInformation(playerid, "Information", "Cannot find door with uid assigned to this item.");

			new source = CreateScriptedSource(d_id, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
			if(source == -1) return printf("[Fire] Wrong source id.");
			SetupScriptedFire(source);

			SendGuiInformation(playerid, "Information", "Fire initiated.");

			if( DoorHasAccess(d_id, DOOR_ACCESS_ALARM) )
			{
				foreach(new p : Player)
				{
					if( IsPlayerInAnyGroup(p) )
					{
						new gidd = pInfo[p][player_duty_gid];
						if(gidd != -1)
						{
							if( Group[gidd][group_type] == GROUP_TYPE_LSPD)
							{
								SendClientMessage(p, COLOR_GOLD, "911 report: fire in business (position marked on map).");
								Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, p);
							}
						}
					}
				}
			}

			if( DoorHasAccess(d_id, DOOR_ACCESS_CAMERA) )
			{
				new suspects_list[256];
				foreach(new p: Player)
				{
					if(GetPlayerVirtualWorld(playerid) == Door[d_id][door_uid])
					{
						format(suspects_list, sizeof(suspects_list), "%s %s", suspects_list, pInfo[p][player_name]);
					}
				}

				if(strlen(suspects_list))
				{
					mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_game_cameras WHERE camera_door = %d", Door[d_id][door_uid]));
					mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_game_cameras` (camera_door, camera_suspects) VALUES ('%d', '%s')", Door[d_id][door_uid], suspects_list));
				}
			}
			
			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_DEMOLITION:
		{
			if(!SherifsOnline()) return SendGuiInformation(playerid, "Information", "To use this item, players from LSPD must be on duty.");

			new gid = pInfo[playerid][player_duty_gid];
			if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty assigned to this item.");
			if(Group[gid][group_uid] != PlayerItem[playerid][itemid][player_item_value2]) return SendGuiInformation(playerid, "Information", "You are not on group duty assigned to this item.");

			new d_uid = GetPlayerVirtualWorld(playerid);
			if(d_uid == 0 ) return SendGuiInformation(playerid, "Information", "You can use this item only in doors assigned to it.");
			if(d_uid != PlayerItem[playerid][itemid][player_item_value1]) return SendGuiInformation(playerid, "Information", "This item is not assigned to that door.");

			new d_id = GetDoorByUid(d_uid);
			if(d_id == -1) return SendGuiInformation(playerid, "Information", "Cannot find door with uid assigned to item.");

			pInfo[playerid][player_demolition] = d_uid;
			TextDrawSetString(Tutorial[playerid], sprintf("Demolition ~p~in progress~w~.~n~Desctruction state: ~r~~h~~h~%d%%~n~~w~Every minute is 10 percent of destruction.", Door[d_id][door_demolition]));
			TextDrawShowForPlayer(playerid, Tutorial[playerid]);

			if( DoorHasAccess(d_id, DOOR_ACCESS_ALARM) )
			{
				foreach(new p : Player)
				{
					if( IsPlayerInAnyGroup(p) )
					{
						new gidd = pInfo[p][player_duty_gid];
						if(gidd != -1)
						{
							if( Group[gidd][group_type] == GROUP_TYPE_LSPD)
							{
								SendClientMessage(p, COLOR_GOLD, "911 report: business demoliton (position on map).");
								Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, p);
							}
						}
					}
				}
			}

			if( DoorHasAccess(d_id, DOOR_ACCESS_CAMERA) )
			{
				new suspects_list[256];
				foreach(new p: Player)
				{
					if(GetPlayerVirtualWorld(playerid) == Door[d_id][door_uid])
					{
						format(suspects_list, sizeof(suspects_list), "%s %s", suspects_list, pInfo[p][player_name]);
					}
				}

				if(strlen(suspects_list))
				{
					mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_game_cameras WHERE camera_door = %d", Door[d_id][door_uid]));
					mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_game_cameras` (camera_door, camera_suspects) VALUES ('%d', '%s')", Door[d_id][door_uid], suspects_list));
				}
			}

			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_MASKA:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				if(pInfo[playerid][player_admin_duty] == true) return SendGuiInformation(playerid, "Information", "You can't use mask when you are on admin duty.");
				strcopy(pInfo[playerid][player_name], pInfo[playerid][player_duty_tmp], MAX_PLAYER_NAME+1);
				pInfo[playerid][player_duty_tmp][0] = EOS;
				PlayerItem[playerid][itemid][player_item_used]=false;

				UpdatePlayerLabel(playerid);
				
				SendGuiInformation(playerid, ""guiopis"Information", "Mask off.\nYour name is standard now.");
			}
			else
			{
				if(pInfo[playerid][player_admin_duty] == true) return SendGuiInformation(playerid, "Information", "You can't use mask when you are on admin duty.");
				if(PlayerItem[playerid][itemid][player_item_value1] > 0)
				{
					new gid = pInfo[playerid][player_duty_gid];
					if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty with masks flag.");
					if( !GroupHasFlag(gid, GROUP_FLAG_MASK)) return SendGuiInformation(playerid, "Information", "You are not on group duty with masks flag.");

					new name[MAX_PLAYER_NAME+1];
			   		GetPlayerName(playerid, name, sizeof(name));

					strcopy(pInfo[playerid][player_duty_tmp], name, MAX_PLAYER_NAME+1);

					new nazwa[MAX_PLAYER_NAME+1];
					format(nazwa, sizeof(nazwa), "Stranger_%c%c%c%c%c%c", randomString(), randomString(), randomString(), randomString(), randomString(), randomString());
					PlayerItem[playerid][itemid][player_item_used]=true;

					strcopy(pInfo[playerid][player_name], nazwa, MAX_PLAYER_NAME+1);

					UpdatePlayerLabel(playerid);
					
					PlayerItem[playerid][itemid][player_item_value1] --;
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value1` = %d WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_value1], PlayerItem[playerid][itemid][player_item_uid]));
					SendGuiInformation(playerid, ""guiopis"Information", "Mask on.");
				}
				else
				{
					SendGuiInformation(playerid, ""guiopis"Information", "Item has no more uses and has been deleted.");
					DeleteItem(itemid, true, playerid);
				}
			}
		}

		case ITEM_TYPE_EMPTY_PLATE:
		{
			if( !PlayerItem[playerid][itemid][player_item_used] )
			{
				ShowPlayerDialog(playerid, DIALOG_CD_LINK, DIALOG_STYLE_INPUT, "CD recording", "Input audio stream link, max 128 chars.", "Record", "Cancel");
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
			}
		}

		case ITEM_TYPE_PLATE:
        {
            if( !PlayerItem[playerid][itemid][player_item_used] )
            {
                if(GetPlayerState(playerid) == PLAYER_STATE_PASSENGER || GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
                {
                    new vehid = GetPlayerVehicleID(playerid), rows, fields;
 					
 					if( !CanPlayerUseVehicle(playerid, vehid) ) return SendClientMessage(playerid, COLOR_GREY, "No access to this vehicle.");
                    mysql_query(mySQLconnection, sprintf("SELECT cd_link FROM `ipb_cds` WHERE `cd_item` = %d", PlayerItem[playerid][itemid][player_item_uid]));
                   	cache_get_data(rows, fields);
 
                    new cdlink[128];
                    cache_get_row(0, 0, cdlink, mySQLconnection, 128);
 
 
                    Vehicle[vehid][vehicle_streaming] = 1;
                    strmid(Vehicle[vehid][vehicle_stream], cdlink, 0, strlen(cdlink), 128);
 
                    foreach(new p: Player)
                    {
                        if(GetPlayerVehicleID(p) == vehid)
                        {
                            PlayAudioStreamForPlayer(p, cdlink);
                        }
                    }

                    ProxMessage(playerid, "turns on car audio.", PROX_ME);
                    PlayerItem[playerid][itemid][player_item_used]=true;
                }
                else
                {
                    SendGuiInformation(playerid, "Information", "You are not in any vehicle.");
                }
            }
            else
            {
                if(GetPlayerState(playerid) == PLAYER_STATE_PASSENGER || GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
                {
                    new vehid = GetPlayerVehicleID(playerid);
                    Vehicle[vehid][vehicle_streaming] = 0;
                    StopAudioStreamForPlayer(playerid);
 
                    foreach(new p: Player)
                    {
                        if(GetPlayerVehicleID(p) == vehid)
                        {
                            StopAudioStreamForPlayer(p);
                        }
                    }
 
                    ProxMessage(playerid, "turns off car audio.", PROX_ME);
                    PlayerItem[playerid][itemid][player_item_used]=false;
                }
                else
                {
                    SendGuiInformation(playerid, "Information", "You are not in any vehicle.");
                }
            }
        }

		case ITEM_TYPE_ROPE:
        {
            if( PlayerItem[playerid][itemid][player_item_used] )
            {
                GameTextForPlayer(playerid, "~w~rope ~y~used", 3000, 3);
                PlayerItem[playerid][itemid][player_item_used] = false;
            }
            else
            {
                GameTextForPlayer(playerid, "~w~rope ~y~off", 3000, 3);
                PlayerItem[playerid][itemid][player_item_used] = true;
            }
        }
		
		case ITEM_TYPE_GLOVES:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				ProxMessage(playerid, "takes off gloves.", PROX_ME);
				RemovePlayerStatus(playerid, PLAYER_STATUS_REKAWICZKI);
				PlayerItem[playerid][itemid][player_item_used]=false;
			}
			else
			{
				ProxMessage(playerid, "takes on gloves.", PROX_ME);
				AddPlayerStatus(playerid, PLAYER_STATUS_REKAWICZKI);
				PlayerItem[playerid][itemid][player_item_used]=true;
			}
		}

		case ITEM_TYPE_TUNING:
		{
			if( !PlayerItem[playerid][itemid][player_item_used] )
			{
				if( GetPlayerUsedItem(playerid, ITEM_TYPE_TUNING) > -1)
				{
					SendGuiInformation(playerid, "Information", "You are already using tuning component.");
					return 1;
				}

				SendGuiInformation(playerid, ""guiopis"Information", "Component used, you can now install it by /o montage.");
				PlayerItem[playerid][itemid][player_item_used]=true;
			}
			else
			{
				PlayerItem[playerid][itemid][player_item_used]=false;
			}
		}

		case ITEM_TYPE_ATTACH:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				PlayerItem[playerid][itemid][player_item_used]=false;
				RemovePlayerAttachedObject(playerid, PlayerItem[playerid][itemid][player_item_value2]);
			}
			else
			{
				new slot = GetPlayerFreeAttachSlot(playerid);
				if(slot == -1) return SendGuiInformation(playerid, "Information", "No free slots for attached objects.");

				new Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:sx, Float:sy, Float:sz, rows, fields;
				mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_attached_objects WHERE attach_owner = %d AND attach_model = %d LIMIT 1", pInfo[playerid][player_id], PlayerItem[playerid][itemid][player_item_model]));
				cache_get_data(rows, fields);

				if( rows )
				{
					x  = cache_get_row_float(0, 5);
					y  = cache_get_row_float(0, 6);
					z  = cache_get_row_float(0, 7);
					rx = cache_get_row_float(0, 8);
					ry = cache_get_row_float(0, 9);
					rz = cache_get_row_float(0, 10);
					sx = cache_get_row_float(0, 11);
					sy = cache_get_row_float(0, 12);
					sz = cache_get_row_float(0, 13);

					RemovePlayerAttachedObject(playerid, slot);
					SetPlayerAttachedObject(playerid, slot, PlayerItem[playerid][itemid][player_item_model], PlayerItem[playerid][itemid][player_item_value1], x, y, z, rx, ry, rz, sx, sy, sz);
					SendPlayerInformation(playerid, "~w~Loaded position from database.~n~Press ~y~ESC~w~ if you dont want to change it.", 6000);
					PlayerItem[playerid][itemid][player_item_used]=true;
					PlayerItem[playerid][itemid][player_item_value2]=slot;
					ao[playerid][slot][ao_inserted]=true;
					EditAttachedObject(playerid, slot);
				}
				else
				{
					RemovePlayerAttachedObject(playerid, slot);
					SetPlayerAttachedObject(playerid, slot, PlayerItem[playerid][itemid][player_item_model], PlayerItem[playerid][itemid][player_item_value1]);
					SendPlayerInformation(playerid, "~w~Set up position for object and press ~y~disc icon~w~ to save it.", 6000);
					PlayerItem[playerid][itemid][player_item_used]=true;
					PlayerItem[playerid][itemid][player_item_value2]=slot;
					ao[playerid][slot][ao_inserted]=false;
					EditAttachedObject(playerid, slot);
				}
			}
		}

		case ITEM_TYPE_BAG:
		{
	    	mysql_query(mySQLconnection, sprintf("SELECT item_uid, item_name FROM ipb_items WHERE item_owner = %d AND item_ownertype = %d", PlayerItem[playerid][itemid][player_item_uid], ITEM_OWNER_TYPE_ITEM));

	    	new rows, fields, header[32], bag_item_uid, bag_item_name[40], list_items[256];

	    	cache_get_data(rows, fields);

	    	for(new row = 0; row != rows; row++)
			{
			    bag_item_uid = cache_get_row_int(row, 0);
   				cache_get_row(row, 1, bag_item_name, mySQLconnection, 40);

				format(list_items, sizeof(list_items), "%s\n%d\t%s", list_items, bag_item_uid, bag_item_name);
			}

			format(header, sizeof(header), "UID\tName\n");

			if(strlen(list_items) > 0)
			{
				format(list_items, sizeof(list_items), "%s%s", header, list_items);
				ShowPlayerDialog(playerid, DIALOG_TAKE_BAG, DIALOG_STYLE_TABLIST_HEADERS, "Items in bag:", list_items, "Take", "Cancel");
				DynamicGui_SetDialogValue(playerid, itemid);
	      	}
	       	else
	       	{
	        	SendGuiInformation(playerid, ""guiopis"Information", "There are no items in this bag.");
			}
		}
		
		case ITEM_TYPE_AMMO:
		{
			if( !PlayerItem[playerid][itemid][player_item_used] )
			{
				if( PlayerItem[playerid][itemid][player_item_extraid] == 1)
				{
					new gid = pInfo[playerid][player_duty_gid];
					if(gid == -1) return SendGuiInformation(playerid, ""guiopis"Information", "You are not on group duty..");
					if(!(Group[gid][group_flags] & GROUP_FLAG_WEAPON_FLAG)) return SendGuiInformation(playerid, ""guiopis"Information", "No access to this ammo. Its flagged, so you have to be on duty with special flag.");
				}

				if(PlayerItem[playerid][itemid][player_item_extraid] == 3)
				{
					if(!(pInfo[playerid][player_documents] & DOCUMENT_WEAPON)) return SendGuiInformation(playerid, "Information", "You don't have weapon license..");
				}
				
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
				new count, string[200];
				foreach(new item : PlayerItems[playerid])
				{
					if( PlayerItem[playerid][item][player_item_type] == ITEM_TYPE_WEAPON && GetWeaponType(PlayerItem[playerid][item][player_item_value1]) == PlayerItem[playerid][itemid][player_item_value1] && !PlayerItem[playerid][item][player_item_used] )
					{
						format(string, sizeof(string), "%s%d\t\t%s\n", string, PlayerItem[playerid][item][player_item_uid], PlayerItem[playerid][item][player_item_name]);
						DynamicGui_AddRow(playerid, item);
						count++;
					}
				}
				if( count == 0 ) return SendGuiInformation(playerid, ""guiopis"Information", "You don't have unused weapons which fits to this type of amo.");
				else ShowPlayerDialog(playerid, DIALOG_USE_AMMO, DIALOG_STYLE_LIST, "Choose weapon for reload:", string, "Choose", "Close");
			}
		}
		
		case ITEM_TYPE_PHONE:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				cmd_phone(playerid, "");
			}
			else
			{
				if( GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE) > -1 ) return SendGuiInformation(playerid, ""guiopis"Information", "You can have only one enabled phone.");
			
				if( PlayerItem[playerid][itemid][player_item_value1] == 0 )
				{
					new str[7];
					format(str, sizeof(str), "%04d", pInfo[playerid][player_id]);
					
					new length = 6 - strlen(str);
					
					if( length == 2 ) format(str, sizeof(str), "%02d%s", rand(10, 99), str);
					else if( length == 1 ) format(str, sizeof(str), "%d%s", rand(1, 9), str);
					
					PlayerItem[playerid][itemid][player_item_value1] = strval(str);
					
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value1` = %d WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_value1], PlayerItem[playerid][itemid][player_item_uid]));
				}
				
				PlayerItem[playerid][itemid][player_item_used] = true;
				
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_used` = 1 WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_uid]));
				
				GameTextForPlayer(playerid, "~w~Phone ~g~enabled", 3000, 3);

				pInfo[playerid][player_phone_model] = PlayerItem[playerid][itemid][player_item_model];
			}
		}
		case ITEM_TYPE_WATCH:
		{
			new hour, minute, second,string[64];
			gettime(hour, minute, second);

			format(string, sizeof(string), "~w~Time:~w~ ~p~%02d:%02d:%02d~p~~w~.", hour, minute, second);
			GameTextForPlayer(playerid, string, 5000, 1);

			ProxMessage(playerid, sprintf("looks at his %s.", PlayerItem[playerid][itemid][player_item_name]), PROX_SERWERME);

			ApplyAnimation(playerid,"COP_AMBIENT","Coplook_watch",4.1,0,0,0,0,0,1);
		}

		case ITEM_TYPE_HOWITZER:
		{
			if(GetPlayerWeapon(playerid)==0)
			{
				SendGuiInformation(playerid, ""guiopis"Information", "You have to get M4A1.");
			}
			SendGuiInformation(playerid, ""guiopis"Information", "Loaded howitzer to your weapon.");
			PlayerItem[playerid][itemid][player_item_used] = true;
			pInfo[playerid][player_howitzer]=PlayerItem[playerid][itemid][player_item_value1];
			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_DRINK:
		{
			//value1 = special_action (20 - piwo, 22 - wino, 23 - sprunk)
			SetPlayerSpecialAction(playerid, PlayerItem[playerid][itemid][player_item_value1]);
			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_SUPLEMENT:
		{
			pInfo[playerid][player_supple] = PlayerItem[playerid][itemid][player_item_value1];
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Suplement used, now your strenght will come %d times faster while training.", pInfo[playerid][player_supple]));
			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_CLOTH:
		{
			if(!PlayerItem[playerid][itemid][player_item_used])
			{
				ProxMessage(playerid, "wears clothes.", PROX_ME);
				SetPlayerSkin(playerid, PlayerItem[playerid][itemid][player_item_value1]);
				pInfo[playerid][player_skin] = PlayerItem[playerid][itemid][player_item_value1];
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_skin = '%d' WHERE char_uid = '%d'", pInfo[playerid][player_skin], pInfo[playerid][player_id]));
				PlayerItem[playerid][itemid][player_item_used] = true;
			}
			else
			{
				PlayerItem[playerid][itemid][player_item_used] = false;
			}
		}

		case ITEM_TYPE_DILDO_CHAINSAW:
		{
			new a_id = pInfo[playerid][player_area];
			if(a_id < 1 || !AreaHasFlag(a_id, AREA_FLAG_WORK)) return SendGuiInformation(playerid, "Information", "You are not in working area.");

			if(pInfo[playerid][player_job] == 0) return SendGuiInformation(playerid, "Information", "You are not lumberjack.");

			if(pInfo[playerid][player_job] == WORK_TYPE_LUMBERJACK)
			{
				if(!PlayerItem[playerid][itemid][player_item_used])
				{
					PlayerItem[playerid][itemid][player_item_used] = true;
					SetPlayerAttachedObject(playerid, 8, 341, 6,0.068999, 0.068000, 0.119000, -2.799999, -23.600008, -4.200000, 1.000000, 1.000000, 1.000000);
				}
				else
				{
					PlayerItem[playerid][itemid][player_item_used] = false;
					RemovePlayerAttachedObject(playerid, 8);
				}
			}
		}

		case ITEM_TYPE_LINE:
		{
			if(!PlayerItem[playerid][itemid][player_item_used])
			{
				PlayerItem[playerid][itemid][player_item_used] = true;
			}
			else
			{
				PlayerItem[playerid][itemid][player_item_used] = false;
			}
		}

		case ITEM_TYPE_CUBE:
		{
			new los = random(PlayerItem[playerid][itemid][player_item_value1]);

			ProxMessage(playerid, sprintf("throws %d/%d.", los+1, PlayerItem[playerid][itemid][player_item_value1]), PROX_SERWERME);
		}

		case ITEM_TYPE_CANISTER:
		{
			if(PlayerItem[playerid][itemid][player_item_value1] == 0) return SendGuiInformation(playerid, "Information", "This gas can is empty.");
			new vid = GetNearestVehicle(playerid);
			if (vid == -1) return SendGuiInformation(playerid, "Information", "You are too far from any vehicle.");

			if(Vehicle[vid][vehicle_fuel_type] != PlayerItem[playerid][itemid][player_item_value2])
			{
				SendGuiInformation(playerid, "Information", "Type of fuel in gas can is different than in vehicle.");
				return 1;
			}

			new amount = PlayerItem[playerid][itemid][player_item_value1];
			new actual = floatround(Vehicle[vid][vehicle_fuel_current], floatround_ceil) + amount;
			new maximum = VehicleFuelMax[GetVehicleModel(vid)-400];

			if(actual > maximum)
			{
				SendGuiInformation(playerid, "Information", "Not enough space in vehicle tank.");
				return 1;
			}

			Vehicle[vid][vehicle_fuel_current] += float(amount);
			PlayerItem[playerid][itemid][player_item_value1] = 0;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_value1 = '%d' WHERE item_uid = %d", PlayerItem[playerid][itemid][player_item_value1], PlayerItem[playerid][itemid][player_item_uid]));
			ApplyAnimation(playerid, "INT_HOUSE", "wash_up",4.1, 0, 0, 0, 0, 0, 1);
			SendGuiInformation(playerid, "Information", "Vehicle refueled.");
			cmd_ame(playerid, "refuels his vehicle by gas can.");
			SaveVehicle(vid);
		}

		case ITEM_TYPE_FOOD:
		{
			if(pInfo[playerid][player_health]<21)
			{
				SendGuiInformation(playerid, ""guiopis"Information", "You are under 21 percent of HP. You have to go to hospital or take pill from pharmacy");
				return 1;
			}
			if(pInfo[playerid][player_health]>=100)
			{
				SendGuiInformation(playerid, ""guiopis"Information", "Your HP is full.");
				return 1;
			}

			new str[32];
			pInfo[playerid][player_health]+=PlayerItem[playerid][itemid][player_item_value1];

			format(str, sizeof(str), "eats %s", PlayerItem[playerid][itemid][player_item_name]);
			DeleteItem(itemid, true, playerid);
			ProxMessage(playerid, str, PROX_ME);
			if(pInfo[playerid][player_health]>100)
			{
				pInfo[playerid][player_health]=100;
				SetPlayerHealth(playerid, 100);
			}
		}

		case ITEM_TYPE_CIGGY:
		{
			if(PlayerItem[playerid][itemid][player_item_value1]>0)
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_SMOKE_CIGGY);
				PlayerItem[playerid][itemid][player_item_value1]--;
			}
			else
			{
				DeleteItem(itemid, true, playerid);
			}
		}

		case ITEM_TYPE_LEK:
		{
			new str[32];
			if(PlayerItem[playerid][itemid][player_item_value1] == 1)
			{
				if(pInfo[playerid][player_health]<=29)
				{
					pInfo[playerid][player_health]+=30;
					SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
					format(str, sizeof(str), "eats %s", PlayerItem[playerid][itemid][player_item_name]);
					ProxMessage(playerid, str, PROX_ME);
					DeleteItem(itemid, true, playerid);
				}
				else
				{
					SendPlayerInformation(playerid, "~w~To fill rest ~g~~h~HP~w~ eat something", 5000);
				}
			}
			else if(PlayerItem[playerid][itemid][player_item_value1] == 2)
			{
				if(pInfo[playerid][player_health]<=29)
				{
					pInfo[playerid][player_health]+=30;
					SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
					format(str, sizeof(str), "eats %s", PlayerItem[playerid][itemid][player_item_name]);
					ProxMessage(playerid, str, PROX_ME);
					DeleteItem(itemid, true, playerid);
				}
				else
				{
					SendPlayerInformation(playerid, "~w~To fill rest ~g~~h~HP~w~ eat something", 5000);
				}
			}
			else if(PlayerItem[playerid][itemid][player_item_value1] == 3)
			{
				SendGuiInformation(playerid, ""guiopis"Information", "Item has no usage.");
			}
		}
		
		case ITEM_TYPE_DRUG:
		{
			if(pInfo[playerid][player_dealing]) return SendGuiInformation(playerid, "Information", "Wait for end of dealing.");

			if( GetPlayerUsedItem(playerid, ITEM_TYPE_GLOVES) == -1 )
			{
				mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_fingerprints` (finger_owner, finger_person) VALUES ('%d', '%d')", PlayerItem[playerid][itemid][player_item_uid], pInfo[playerid][player_id]));
			}

			if(PlayerItem[playerid][itemid][player_item_value1] == DRUG_TYPE_COCAINE)
			{
				ShowPlayerDialog(playerid, DIALOG_USE_DRUG, DIALOG_STYLE_LIST, "Cocaine", "1. Use\n2. Deal\n3. Split\n4. Cook", "Choose", "Close");
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
			}
			else if(PlayerItem[playerid][itemid][player_item_value1] == DRUG_TYPE_CRACK)
			{
				ShowPlayerDialog(playerid, DIALOG_USE_DRUG, DIALOG_STYLE_LIST, "Crack", "1. Use\n2. Deal\n3. Split", "Choose", "Close");
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
			}
			else if(PlayerItem[playerid][itemid][player_item_value1] == DRUG_TYPE_WEED)
			{
				ShowPlayerDialog(playerid, DIALOG_USE_DRUG, DIALOG_STYLE_LIST, "Weed", "1. Use\n2. Deal\n3. Split", "Choose", "Close");
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
			}
			else if(PlayerItem[playerid][itemid][player_item_value1] == DRUG_TYPE_AMFA)
			{
				ShowPlayerDialog(playerid, DIALOG_USE_DRUG, DIALOG_STYLE_LIST, "Amph", "1. Use\n2. Deal\n3. Split", "Choose", "Close");
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
			}
			else if(PlayerItem[playerid][itemid][player_item_value1] == DRUG_TYPE_METH)
			{
				ShowPlayerDialog(playerid, DIALOG_USE_DRUG, DIALOG_STYLE_LIST, "Meth", "1. Use\n2. Deal\n3. Split", "Choose", "Close");
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
			}
			else if(PlayerItem[playerid][itemid][player_item_value1] == DRUG_TYPE_HEROINE)
			{
				ShowPlayerDialog(playerid, DIALOG_USE_DRUG, DIALOG_STYLE_LIST, "Heroine", "1. Use\n2. Deal\n3. Split", "Choose", "Close");
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
			}
			else if(PlayerItem[playerid][itemid][player_item_value1] == DRUG_TYPE_EXTASY)
			{
				ShowPlayerDialog(playerid, DIALOG_USE_DRUG, DIALOG_STYLE_LIST, "Molly", "1. Use\n2. Deal\n3. Split", "Choose", "Close");
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
			}
		}

		case ITEM_TYPE_CORPSE:
		{
			new death_name[64];
		    if(PlayerItem[playerid][itemid][player_item_value2] == 0)
		    {
		    	format(death_name, sizeof(death_name), "beaten");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 0 && PlayerItem[playerid][itemid][player_item_value2] <=3)
		    {
		    	format(death_name, sizeof(death_name), "beaten");
		    }
		 	if(PlayerItem[playerid][itemid][player_item_value2] == 4)
		    {
		    	format(death_name, sizeof(death_name), "stabbed (short knife)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 4 && PlayerItem[playerid][itemid][player_item_value2] <8 )
		    {
		    	format(death_name, sizeof(death_name), "beaten");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 8)
		    {
		    	format(death_name, sizeof(death_name), "stabbed");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 9)
		    {
		    	format(death_name, sizeof(death_name), "massacred)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 9 && PlayerItem[playerid][itemid][player_item_value2] < 16)
		    {
		    	format(death_name, sizeof(death_name), "beaten");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 16)
		    {
		    	format(death_name, sizeof(death_name), "explosion");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 18)
		    {
		    	format(death_name, sizeof(death_name), "burned");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 21 && PlayerItem[playerid][itemid][player_item_value2] < 24)
		    {
		    	format(death_name, sizeof(death_name), "shot (pistol)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 24)
		    {
		    	format(death_name, sizeof(death_name), "shot (pistol)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 24 && PlayerItem[playerid][itemid][player_item_value2] < 28)
		    {
		    	format(death_name, sizeof(death_name), "shot (rifle)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 27 && PlayerItem[playerid][itemid][player_item_value2] < 30)
		    {
		    	format(death_name, sizeof(death_name), "shot (smg)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 29 && PlayerItem[playerid][itemid][player_item_value2] < 32)
		    {
		    	format(death_name, sizeof(death_name), "shot (karabin)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 32)
		    {
		    	format(death_name, sizeof(death_name), "shot (smg)");
		    }
			if(PlayerItem[playerid][itemid][player_item_value2] == 33)
		    {
		    	format(death_name, sizeof(death_name), "shot (rifle)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 34)
		    {
		    	format(death_name, sizeof(death_name), "shot (sniper rifle)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 35)
		    {
		    	format(death_name, sizeof(death_name), "explosion");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 36)
		    {
		    	format(death_name, sizeof(death_name), "explosion");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 37)
		    {
		    	format(death_name, sizeof(death_name), "burned");
		    }
		 	if(PlayerItem[playerid][itemid][player_item_value2] == 38)
		    {
		    	format(death_name, sizeof(death_name), "shot (minigun)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 39)
		    {
		    	format(death_name, sizeof(death_name), "explosion");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 41)
		    {
		    	format(death_name, sizeof(death_name), "intoxication");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 42)
		    {
		    	format(death_name, sizeof(death_name), "intoxication");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 49)
		    {
		    	format(death_name, sizeof(death_name), "road accident");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 51)
		    {
		    	format(death_name, sizeof(death_name), "explosion");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 53)
		    {
		    	format(death_name, sizeof(death_name), "drowned");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 54)
		    {
		    	format(death_name, sizeof(death_name), "fall");
		    }

		    new deathtime = PlayerItem[playerid][itemid][player_item_created];
			new deathHour, deathMinute, deathDay, temp;

			TimestampToDate(deathtime, temp, temp, deathDay, deathHour, deathMinute, temp, 1);

			new corpses[128];
			format(corpses, sizeof(corpses), "Identification:\t%s\nDeath reason:\t%s\nDate: %02d: %02d:%02d\nDNA of murderer: %d", PlayerItem[playerid][itemid][player_item_name], death_name, deathDay, deathHour, deathMinute, PlayerItem[playerid][itemid][player_item_value1]);
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST, "Body details", corpses, "Close", "");
		}

		case ITEM_TYPE_ARMOUR:
		{
			if(PlayerItem[playerid][itemid][player_item_value1] < 1)
			{
				SendGuiInformation(playerid, "Information", "Kevlar used.");
				DeleteItem(itemid, true, playerid);
				return 1;
			}

			if(!PlayerItem[playerid][itemid][player_item_used])
			{
				if( PlayerItem[playerid][itemid][player_item_extraid] > 0)
				{
					new gid = pInfo[playerid][player_duty_gid];
					if(gid == -1) return SendGuiInformation(playerid, ""guiopis"Information", "You are not on LSPD duty.");
					if(!(Group[gid][group_flags] & GROUP_FLAG_911)) return SendGuiInformation(playerid, ""guiopis"Information", "You are not on LSPD duty.");
				}

				pInfo[playerid][player_armour] = PlayerItem[playerid][itemid][player_item_value1];

				AddPlayerStatus(playerid, PLAYER_STATUS_ARMOUR);
				SetPlayerArmour(playerid, floatround(PlayerItem[playerid][itemid][player_item_value1]));
				PlayerItem[playerid][itemid][player_item_used] = true;
			}
			else
			{
				SetPlayerArmour(playerid, 0);
				RemovePlayerStatus(playerid, PLAYER_STATUS_ARMOUR);
				PlayerItem[playerid][itemid][player_item_used] = false;
			}
		}

		case ITEM_TYPE_GYM_KARNET:
		{
			if(!PlayerItem[playerid][itemid][player_item_used])
			{
				if(GetPlayerUsedItem(playerid, ITEM_TYPE_GYM_KARNET) > -1) return SendGuiInformation(playerid, "Information", "You are already using gym ticket.");

				if(PlayerItem[playerid][itemid][player_item_value1] < 1)
				{
					SendGuiInformation(playerid, "Information", "Item has no more usages, deleting.");
					DeleteItem(itemid, true, playerid);
					return 1;
				}
				
				if(pInfo[playerid][player_gym_carnet] == 0) return SendGuiInformation(playerid, "Information", "You've rached train limit for today.");
				SendClientMessage(playerid, COLOR_GOLD, "Ticket has been used, go to train machine and use /train.");
				//pInfo[playerid][player_gym_carnet] = PlayerItem[playerid][itemid][player_item_value1];
				pInfo[playerid][player_gym] = PlayerItem[playerid][itemid][player_item_value2];
				PlayerItem[playerid][itemid][player_item_used] = true;
			}
			else
			{
				if(pInfo[playerid][player_training] == true) return SendGuiInformation(playerid, "Information", "Action not alowed while training.");
				PlayerItem[playerid][itemid][player_item_used] = false;
			}
		}
	}
	
	return 1;
}

stock GetPlayerByPhone(number)
{
	foreach(new playerid: Player)
	{
		foreach(new itemid : PlayerItems[playerid])
		{
			if( PlayerItem[playerid][itemid][player_item_value1] == number && PlayerItem[playerid][itemid][player_item_used]) return playerid;
		}
	}
	return INVALID_PLAYER_ID;
}


stock GetItemByData(i_type, i_value1 = -1, i_value2 = -1, &player, &item)
{
	new bool:item_good = false;

	foreach(new playerid: Player)
	{
		foreach(new itemid : PlayerItems[playerid])
		{
			item_good = true;
			if( PlayerItem[playerid][itemid][player_item_type] == i_type )
			{
				if( i_value1 > -1 )
				{
					if( PlayerItem[playerid][itemid][player_item_value1] != i_value1 ) item_good = false;
				}
				
				if( i_value2 > -1 )
				{
					if( PlayerItem[playerid][itemid][player_item_value2] != i_value2 ) item_good = false;
				}
			}
			else 
			{
				item_good = false;
				item = -1;
				player = INVALID_PLAYER_ID;
			}
			
			if( item_good )
			{
				item = itemid;
				player = playerid;
				break;
			}
		}
	}
	
	return 1;
}

stock GetPlayerUsedItem(playerid, i_type)
{
	if(playerid == INVALID_PLAYER_ID) return -1;
	foreach(new itemid : PlayerItems[playerid])
	{
		if( PlayerItem[playerid][itemid][player_item_type] == i_type && PlayerItem[playerid][itemid][player_item_used] ) return itemid;
	}
	
	return -1;
}

stock Item_Drop(itemid, playerid = INVALID_PLAYER_ID)
{
    if( playerid == INVALID_PLAYER_ID) return 1;
    if( PlayerItem[playerid][itemid][player_item_used] ) return SendGuiInformation(playerid, ""guiopis"Information", "You can't drop used item.");
   	
    if( !IsPlayerInAnyVehicle(playerid) )
    {
        if( playerid != INVALID_PLAYER_ID )
        {
        	mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_created = %d WHERE item_uid = %d", gettime(), PlayerItem[playerid][itemid][player_item_uid]));

        	new Float:x, Float:y, Float:z, iuid;
            GetPointInAngleOfPlayer(playerid, x, y, z, 1.0, 0.0);
           
            z = floatadd(z, -0.973339);
          
            RefreshPlayer(playerid);
            ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
           
      		iuid = PlayerItem[playerid][itemid][player_item_uid];

            new str[400];
            strcat(str, sprintf("UPDATE `ipb_items` SET `item_ownertype` = %d, `item_owner` = 0, `item_posx` = %f, `item_posy` = %f, `item_posz` = %f,", ITEM_OWNER_TYPE_GROUND, x, y, z));
            strcat(str, sprintf(" `item_rotx` = '90.8', `item_roty` = '0', `item_rotz` = '0', `item_world` = %d, `item_interior` = %d  WHERE `item_uid` = %d", GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), iuid));
            mysql_query(mySQLconnection, str);

            pInfo[playerid][player_capacity] += PlayerItem[playerid][itemid][player_item_weight];
            printf("[ITEM] %s (UID %d) od³o¿y³ przedmiot na ziemie - UID %d. (%s)", pInfo[playerid][player_name], pInfo[playerid][player_id], PlayerItem[playerid][itemid][player_item_uid], PlayerItem[playerid][itemid][player_item_name]);

            DeleteItem(itemid, false, playerid);
            LoadItem(sprintf("WHERE item_uid = %d", iuid));
           
            cmd_ame(playerid, "drops something on the ground.");
        }
    }
    else
    {
        new vid = GetPlayerVehicleID(playerid);
        new iuid = PlayerItem[playerid][itemid][player_item_uid];
        if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, ""guiopis"Information", "You don't have access to this vehicle.");
        new model = GetVehicleModel(vid);
        if(model == 509 || model == 510 || model == 481 || model == 461 || model == 462 || model == 463 || model == 468 || model == 521 || model == 522 || model == 523 || model == 581 || model == 586)
        {
            SendGuiInformation(playerid, ""guiopis"Information", "You can't drop item in bike.");
            return 1;
        }

        new rows, fields;
        mysql_query(mySQLconnection, sprintf("SELECT item_uid FROM ipb_items WHERE item_ownertype = %d AND item_owner = %d", ITEM_OWNER_TYPE_VEHICLE, Vehicle[vid][vehicle_uid]));
        cache_get_data(rows, fields);

        mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_ownertype` = %d, `item_owner` = %d WHERE `item_uid` = %d", ITEM_OWNER_TYPE_VEHICLE, Vehicle[vid][vehicle_uid], iuid));
        ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);

       	pInfo[playerid][player_capacity] += PlayerItem[playerid][itemid][player_item_weight];
       	DeleteItem(itemid, false, playerid);

        cmd_ame(playerid, "puts something in vehicle.");
    }
   
    return 1;
}

stock Item_Pickup(itemuid, playerid = INVALID_PLAYER_ID)
{
	if( playerid == INVALID_PLAYER_ID) return 1;
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT item_weight, item_ownertype, item_name, item_type FROM ipb_items WHERE item_uid = %d AND item_ownertype != %d", itemuid, ITEM_OWNER_TYPE_PLAYER));
	cache_get_data(rows, fields);

	new weight = cache_get_row_int(0, 0);

	if(pInfo[playerid][player_capacity] < weight) return SendGuiInformation(playerid, "Information", "This item is too heavy for you.");
	
	new ownertype = cache_get_row_int(0, 1);
	new itype = cache_get_row_int(0, 3);
	new itname[40];

	cache_get_row(0, 2, itname, mySQLconnection, 40);

	if( ownertype == ITEM_OWNER_TYPE_GROUND )
	{
		new object_id = GetItemObject(playerid, itemuid);

		if(object_id != INVALID_OBJECT_ID)
		{
			DestroyDynamicObject(object_id);
		}	
		cmd_ame(playerid, sprintf("pickups %s from the ground.", itname));
	}
	else if( ownertype == ITEM_OWNER_TYPE_VEHICLE )
	{
		cmd_ame(playerid, sprintf("pickups %s from vehicle.", itname));
	}
	else if( ownertype == ITEM_OWNER_TYPE_DOOR )
	{
		cmd_ame(playerid, sprintf("pickups %s from door storage.", itname));
	}
	
	pInfo[playerid][player_capacity] -= weight;
	
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
	
	if(itype == ITEM_TYPE_WEAPON || itype == ITEM_TYPE_DRUG)
	{
		if( GetPlayerUsedItem(playerid, ITEM_TYPE_GLOVES) == -1 )
		{
			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_fingerprints` (finger_owner, finger_person) VALUES ('%d', '%d')", itemuid, pInfo[playerid][player_id]));
		}
	}

	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_ownertype` = %d, `item_owner` = %d, `item_posx` = 0.0, `item_posy` = 0.0, `item_posz` = 0.0, `item_rotx` = 0.0, `item_roty` = 0.0, `item_rotz` = 0.0, `item_world` = 0, `item_interior` = 0  WHERE `item_uid` = %d AND item_ownertype != %d", ITEM_OWNER_TYPE_PLAYER, pInfo[playerid][player_id], itemuid, ITEM_OWNER_TYPE_PLAYER ));
	
	if(cache_affected_rows(mySQLconnection))
	{
		LoadPlayerItem(playerid, sprintf("WHERE item_uid = %d", itemuid));
		return 1;
	}
	return 1;
}

stock Item_Create(owner_type, owner_id, i_type, i_model, i_val1, i_val2, i_name[40], i_extra=0, i_count=0)
{
	new itemid = -1;
	if( owner_type == ITEM_OWNER_TYPE_PLAYER )
	{
		new playerid = owner_id;
		if( owner_id > -1 )
		{
			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_items` (`item_uid`,`item_model`,`item_ownertype`,`item_owner`,`item_type`,`item_value1`,`item_value2`,`item_name`,`item_created`, `item_extraid` ) VALUES (null, %d, %d, %d, %d, %d, %d,'%s', %d, %d)", i_model, owner_type, pInfo[playerid][player_id], i_type, i_val1, i_val2, i_name, gettime(), i_extra));
			new uid = cache_insert_id();
			
			itemid = LoadPlayerItem(playerid, sprintf("WHERE `item_uid` = %d", uid), true);
		}
	}
	else if( owner_type == ITEM_OWNER_TYPE_GROUP )
	{
		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_items` (`item_uid`,`item_model`,`item_ownertype`,`item_owner`,`item_type`,`item_value1`,`item_value2`,`item_name`,`item_created`,`item_extraid`, `item_group`, `item_count`) VALUES (null, %d, %d, %d, %d, %d, %d,'%s', %d, %d, %d, %d)", i_model, owner_type, owner_id, i_type, i_val1, i_val2, i_name, gettime(), i_extra, owner_id, i_count));
		new uid = cache_insert_id();
		
		itemid = LoadItem(sprintf("WHERE `item_uid` = %d", uid), true);
		SetupItemWeight(itemid);
	}
	return itemid;
}

stock GetItemByUid(itemuid, playerid)
{
	foreach(new item : PlayerItems[playerid])
	{
		if( PlayerItem[playerid][item][player_item_uid] == itemuid ) return item;
	}
	
	return -1;
}

stock GetItemByGlobalUid(itemuid)
{
	foreach(new playerid: Player)
	{
		foreach(new item : PlayerItems[playerid])
		{
			if( PlayerItem[playerid][item][player_item_uid] == itemuid ) return item;
		}
	}
	
	return -1;
}

stock GetPlayerFish(playerid)
{
	foreach(new item : PlayerItems[playerid])
	{
		if( PlayerItem[playerid][item][player_item_type] == ITEM_TYPE_FOOD &&  PlayerItem[playerid][item][player_item_value2] != 0) return item;
	}
	return -1;
}

stock HasPlayerFish(fvalue, playerid)
{
	foreach(new item : PlayerItems[playerid])
	{
		if( PlayerItem[playerid][item][player_item_type] == ITEM_TYPE_FOOD &&  PlayerItem[playerid][item][player_item_value2] == fvalue) return item;
	}
	return -1;
}

stock HasPlayerItem(itemtype, playerid)
{
	foreach(new item : PlayerItems[playerid])
	{
		if( PlayerItem[playerid][item][player_item_type] == itemtype ) return item;
	}
	return -1;
}

stock GetItemByTargetUid(itemuid, target)
{
	foreach(new item : PlayerItems[target])
	{
		if( PlayerItem[target][item][player_item_uid] == itemuid ) return item;
	}
	return -1;
}


stock GetGroupItemByUid(itemuid)
{
	foreach(new item : Items)
	{
		if( Item[item][item_uid] == itemuid ) return item;
	}
	
	return -1;
}

stock IsValidDrugType(playerid, itemid)
{
	switch(PlayerItem[playerid][itemid][player_item_value1])
	{
		case DRUG_TYPE_COCAINE: return 1;
		case DRUG_TYPE_HEROINE: return 1;
		case DRUG_TYPE_CRACK: return 1;
		case DRUG_TYPE_WEED: return 1;
		case DRUG_TYPE_AMFA: return 1;
		case DRUG_TYPE_METH: return 1;
		case DRUG_TYPE_EXTASY: return 1;
	}
	return 0;
}

stock SetupItemWeight(itemid)
{
	if(itemid == -1) return 1;
	new type = Item[itemid][item_type];

	switch(type)
	{
		case ITEM_TYPE_AMMO:
		{
			Item[itemid][item_weight] = 250;
		}
		case ITEM_TYPE_MASKA:
		{
			Item[itemid][item_weight] = 500;
		}  
		case ITEM_TYPE_PHONE:
		{
			Item[itemid][item_weight] = 120;
		} 
		case ITEM_TYPE_WATCH:
		{
			Item[itemid][item_weight] = 75;
		}
		case ITEM_TYPE_FOOD:
		{
			Item[itemid][item_weight] = 50;
		}  
		case ITEM_TYPE_CIGGY:
		{
			Item[itemid][item_weight] = 35;
		} 
		case ITEM_TYPE_CUBE:
		{
			Item[itemid][item_weight] = 5;
		} 
		case ITEM_TYPE_CLOTH:
		{
			Item[itemid][item_weight] = 250;
		} 
		case ITEM_TYPE_CANISTER:
		{
			Item[itemid][item_weight] = 900;
		} 
		case ITEM_TYPE_LINE:
		{
			Item[itemid][item_weight] = 350;
		} 
		case ITEM_TYPE_TUNING:
		{
			Item[itemid][item_weight] = 3500;
		} 
		case ITEM_TYPE_BAG:
		{
			Item[itemid][item_weight] = 500;
		} 
		case ITEM_TYPE_DRINK:
		{
			Item[itemid][item_weight] = 50;
		} 
		case ITEM_TYPE_DRUG:
		{
			Item[itemid][item_weight] = Item[itemid][item_value2];
		} 
		case ITEM_TYPE_ATTACH:
		{
			Item[itemid][item_weight] = 225;
		} 
		case ITEM_TYPE_ACCESS:
		{
			Item[itemid][item_weight] = 750;
		} 
		case ITEM_TYPE_PLATE:
		{
			Item[itemid][item_weight] = 35;
		} 
		case ITEM_TYPE_ARMOUR:
		{
			Item[itemid][item_weight] = 1000;
		} 
		case ITEM_TYPE_GLOVES:
		{
			Item[itemid][item_weight] = 250;
		} 
		case ITEM_TYPE_CORPSE:
		{
			Item[itemid][item_weight] = 5000;
		}
		case ITEM_TYPE_HOWITZER:
		{
			Item[itemid][item_weight] = 180;
		}
		case ITEM_TYPE_LEK:
		{
			Item[itemid][item_weight] = 70;
		}
		case ITEM_TYPE_GYM_KARNET:
		{
			Item[itemid][item_weight] = 20;
		}
		case ITEM_TYPE_BOMB:
		{
			Item[itemid][item_weight] = 2800;
		}
		case ITEM_TYPE_WEAPON:
		{
			Item[itemid][item_weight] = 850;
		}
		case ITEM_TYPE_EMPTY_PLATE:	Item[itemid][item_weight] = 35;

	}
	mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_weight = %d WHERE item_uid = %d", Item[itemid][item_weight], Item[itemid][item_uid]));
	return 1;
}

stock SetupPlayerItemWeight(playerid, itemid)
{
	if(itemid == -1) return 1;
	new type = PlayerItem[playerid][itemid][player_item_type];

	if(PlayerItem[playerid][itemid][player_item_weight] == 0)
	{
		switch(type)
		{
			case ITEM_TYPE_AMMO: PlayerItem[playerid][itemid][player_item_weight] = 250;

			case ITEM_TYPE_WEAPON: 
			{
				switch(PlayerItem[playerid][itemid][player_item_value1])
				{
					case 1: PlayerItem[playerid][itemid][player_item_weight] = 150;
					case 2: PlayerItem[playerid][itemid][player_item_weight] = 350;
					case 3: PlayerItem[playerid][itemid][player_item_weight] = 280;
					case 4: PlayerItem[playerid][itemid][player_item_weight] = 90;
					case 5: PlayerItem[playerid][itemid][player_item_weight] = 400;
					case 6: PlayerItem[playerid][itemid][player_item_weight] = 420;
					case 7: PlayerItem[playerid][itemid][player_item_weight] = 180;
					case 8: PlayerItem[playerid][itemid][player_item_weight] = 220;
					case 9: PlayerItem[playerid][itemid][player_item_weight] = 2400;
					case 10: PlayerItem[playerid][itemid][player_item_weight] = 90;
					case 11: PlayerItem[playerid][itemid][player_item_weight] = 90;
					case 12: PlayerItem[playerid][itemid][player_item_weight] = 90;
					case 13: PlayerItem[playerid][itemid][player_item_weight] = 90;
					case 14: PlayerItem[playerid][itemid][player_item_weight] = 110;
					case 15: PlayerItem[playerid][itemid][player_item_weight] = 170;
					case 16: PlayerItem[playerid][itemid][player_item_weight] = 80;
					case 17: PlayerItem[playerid][itemid][player_item_weight] = 75;
					case 18: PlayerItem[playerid][itemid][player_item_weight] = 225;
					case 22: PlayerItem[playerid][itemid][player_item_weight] = 870;
					case 23: PlayerItem[playerid][itemid][player_item_weight] = 970;
					case 24: PlayerItem[playerid][itemid][player_item_weight] = 1140;
					case 25: PlayerItem[playerid][itemid][player_item_weight] = 1800;
					case 26: PlayerItem[playerid][itemid][player_item_weight] = 900;
					case 27: PlayerItem[playerid][itemid][player_item_weight] = 2900;
					case 28: PlayerItem[playerid][itemid][player_item_weight] = 660;
					case 29: PlayerItem[playerid][itemid][player_item_weight] = 1600;
					case 30: PlayerItem[playerid][itemid][player_item_weight] = 3100;
					case 31: PlayerItem[playerid][itemid][player_item_weight] = 2900;
					case 32: PlayerItem[playerid][itemid][player_item_weight] = 700;
					case 33: PlayerItem[playerid][itemid][player_item_weight] = 2200;
					case 34: PlayerItem[playerid][itemid][player_item_weight] = 2500;
					case 35: PlayerItem[playerid][itemid][player_item_weight] = 5500;
					case 36: PlayerItem[playerid][itemid][player_item_weight] = 5500;
					case 37: PlayerItem[playerid][itemid][player_item_weight] = 4900;
					case 38: PlayerItem[playerid][itemid][player_item_weight] = 5200;
					case 41: PlayerItem[playerid][itemid][player_item_weight] = 550;
					case 42: PlayerItem[playerid][itemid][player_item_weight] = 1550;
					case 43: PlayerItem[playerid][itemid][player_item_weight] = 850;
					case 46: PlayerItem[playerid][itemid][player_item_weight] = 650;
				}
			}

			case ITEM_TYPE_MASKA: PlayerItem[playerid][itemid][player_item_weight] = 500;
			case ITEM_TYPE_PHONE: PlayerItem[playerid][itemid][player_item_weight] = 120;
			case ITEM_TYPE_WATCH: PlayerItem[playerid][itemid][player_item_weight] = 75;
			case ITEM_TYPE_FOOD: PlayerItem[playerid][itemid][player_item_weight] = 50;
			case ITEM_TYPE_CIGGY: PlayerItem[playerid][itemid][player_item_weight] = 35;
			case ITEM_TYPE_CUBE: PlayerItem[playerid][itemid][player_item_weight] = 5;
			case ITEM_TYPE_CLOTH: PlayerItem[playerid][itemid][player_item_weight] = 250;
			case ITEM_TYPE_CANISTER: PlayerItem[playerid][itemid][player_item_weight] = 900;
			case ITEM_TYPE_LINE: PlayerItem[playerid][itemid][player_item_weight] = 350;
			case ITEM_TYPE_TUNING: PlayerItem[playerid][itemid][player_item_weight] = 3500;
			case ITEM_TYPE_BAG: PlayerItem[playerid][itemid][player_item_weight] = 500; 
			case ITEM_TYPE_DRINK: PlayerItem[playerid][itemid][player_item_weight] = 50;
			case ITEM_TYPE_DRUG: PlayerItem[playerid][itemid][player_item_weight] = PlayerItem[playerid][itemid][player_item_value2];
			case ITEM_TYPE_ATTACH: PlayerItem[playerid][itemid][player_item_weight] = 225;
			case ITEM_TYPE_ACCESS: PlayerItem[playerid][itemid][player_item_weight] = 750;
			case ITEM_TYPE_PLATE: PlayerItem[playerid][itemid][player_item_weight] = 35;
			case ITEM_TYPE_EMPTY_PLATE: PlayerItem[playerid][itemid][player_item_weight] = 35;
			case ITEM_TYPE_ARMOUR: PlayerItem[playerid][itemid][player_item_weight] = 1000;
			case ITEM_TYPE_GLOVES: PlayerItem[playerid][itemid][player_item_weight] = 250;
			case ITEM_TYPE_CORPSE: PlayerItem[playerid][itemid][player_item_weight] = 5000;
			case ITEM_TYPE_HOWITZER: PlayerItem[playerid][itemid][player_item_weight] = 180;
			case ITEM_TYPE_LEK: PlayerItem[playerid][itemid][player_item_weight] = 70;
			case ITEM_TYPE_GYM_KARNET: PlayerItem[playerid][itemid][player_item_weight] = 20;
			case ITEM_TYPE_BOMB: PlayerItem[playerid][itemid][player_item_weight] = 2800;
			case ITEM_TYPE_DOOR_ACCESS: PlayerItem[playerid][itemid][player_item_weight] = 700;
			case ITEM_TYPE_MOLOTOV: PlayerItem[playerid][itemid][player_item_weight] = 450;
			case ITEM_TYPE_DEMOLITION: PlayerItem[playerid][itemid][player_item_weight] = 380;
		}

		mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_weight = %d WHERE item_uid = %d", PlayerItem[playerid][itemid][player_item_weight], PlayerItem[playerid][itemid][player_item_uid]));
	}
	return 1;
}