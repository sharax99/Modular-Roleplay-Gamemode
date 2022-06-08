stock SetOffer(sellerid, buyerid, type, price, extraid = -1)
{
	if( sellerid == buyerid )
	{
		SendGuiInformation(sellerid, "Information", "You can't send offer to yourself.");
		return 0;
	}

	if( sellerid != INVALID_PLAYER_ID )
	{
		if( pOffer[sellerid][offer_type] > 0 )
		{
			SendGuiInformation(sellerid, "Information", "You can't send more than 1 offer.");
			return 0;
		}
	}
	
	if( pOffer[buyerid][offer_type] > 0 )
	{
		SendGuiInformation(sellerid, "Information", "That players has already offer to accept.");
		return 0;
	}
	
	if( pInfo[buyerid][player_afk] )
	{
		SendGuiInformation(sellerid, "Information", "That player is AFK.");
		return 0;
	}
	
	switch( type )
	{
		case OFFER_TYPE_VCARD:
		{
			if(extraid == -1) return 0;
			
			new itemid = GetPlayerUsedItem(buyerid, ITEM_TYPE_PHONE);
			if( GetPlayerUsedItem(buyerid, ITEM_TYPE_PHONE) == -1 )
			{
				SendGuiInformation(sellerid, "Information", "This player's phone is off.");
				return 0;
			}
			
			new rows, fields;
			mysql_query(mySQLconnection, sprintf("SELECT contact_uid FROM `ipb_contacts` WHERE `contact_owner` = %d AND `contact_number` = %d AND `contact_deleted` = 0", Item[itemid][item_uid], Item[extraid][item_value1]));
			cache_get_data(rows, fields);
			
			if( rows )
			{
				SendGuiInformation(sellerid, "Information", "This player has already got your vCard.");
				return 0;
			}
		}
		
		case OFFER_TYPE_ITEM:
		{
			if(extraid == -1) return 0;

			new itemid = extraid;

			if(PlayerItem[sellerid][itemid][player_item_used])
			{
				SendGuiInformation(sellerid, "Information", "You can't offer used item.\nHide it and try again.");
				return 0;
			}

			if(pInfo[buyerid][player_capacity] < PlayerItem[sellerid][itemid][player_item_weight])
			{
				SendGuiInformation(sellerid, "Information", "This item is too heavy for him.");
				SendGuiInformation(buyerid, "Information", "This item is too heavy for you.");
				return 0;
			} 
		}

		case OFFER_TYPE_INTERVIEW:
		{
			if(pInfo[buyerid][player_interview] > 0)
			{
				SendGuiInformation(sellerid, "Information", "This player is already in invterview mode. Type /interview again to end.");
				return 0;
			}
		}
	}
	
	pOffer[buyerid][offer_type] = type;
	pOffer[buyerid][offer_sellerid] = sellerid;
	pOffer[buyerid][offer_buyerid] = INVALID_PLAYER_ID;
	pOffer[buyerid][offer_price] = price;

	if(extraid != -1)
	{
		pOffer[buyerid][offer_extraid] = extraid;
	}
	
	if( sellerid != INVALID_PLAYER_ID )
	{
		pOffer[sellerid][offer_type] = type;
		pOffer[sellerid][offer_sellerid] = INVALID_PLAYER_ID;
		pOffer[sellerid][offer_buyerid] = buyerid;
		pOffer[sellerid][offer_price] = price;

		if(extraid != -1)
		{
			pOffer[sellerid][offer_extraid] = extraid;
		}
		
		pOffer[sellerid][offer_start] = gettime();
	}
	pOffer[buyerid][offer_start] = gettime();
	
	return 1;
}

stock ShowPlayerOffer(playerid, fromid, header[], name[], price, bool:show_details = false)
{
	new tmp[MAX_PLAYER_NAME+1];
	if( fromid != INVALID_PLAYER_ID )
	{
		strcopy(tmp, pInfo[fromid][player_name], MAX_PLAYER_NAME+1);
	}
	else strcopy(tmp, "System");
	PlayerTextDrawSetString(playerid, OfferTD[2], sprintf("~y~~h~ Offer from %s ~>~ %s~n~~n~~b~~h~ Name:~w~ %s~n~~g~ Cost:~w~ $%d~n~~n~~n~~n~", tmp, header, name, price));
	
	HidePlayerDialog(playerid);
	
	PlayerTextDrawShow(playerid, OfferTD[2]);
	PlayerTextDrawShow(playerid, OfferTD[4]);
	PlayerTextDrawShow(playerid, OfferTD[5]);
	
	if( show_details ) PlayerTextDrawShow(playerid, OfferTD[3]);
	
	SelectTextDraw(playerid, COLOR_WHITE);
	
	if( fromid != INVALID_PLAYER_ID ) SendClientMessage(fromid, COLOR_LIGHTER_ORANGE, "Offer sent. Wait for player reaction.");
	
	return 1;
}

stock OnPlayerOfferResponse(playerid, response)
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	for(new i;i<6;i++) PlayerTextDrawHide(playerid, OfferTD[i]);
	CancelSelectTextDraw(playerid);
	
	new sellerid = pOffer[playerid][offer_sellerid];
	new type = pOffer[playerid][offer_type];

	if( !response )
	{
		if( sellerid != INVALID_PLAYER_ID ) GameTextForPlayer(sellerid, "~w~offer ~r~rejected", 3000, 3);
		
		GameTextForPlayer(playerid, "~w~offer ~r~rejected", 3000, 3);

		if(type == OFFER_TYPE_TRIBUTE)
		{
			new gid = pInfo[playerid][player_duty_gid];
			new gidd = pInfo[sellerid][player_duty_gid];
			if(gid == -1 || gidd == -1) return 1;
			SendClientMessage(sellerid, COLOR_GOLD, sprintf("Leader of %s (UID: %d) - %s (UID: %d) rejected to pay tribute for %s (UID %d).", pInfo[playerid][player_name], pInfo[playerid][player_id], Group[gid][group_name], Group[gid][group_uid], Group[gidd][group_name], Group[gidd][group_uid]));
		}

		for(new x=0; e_player_offer:x != e_player_offer; x++)
		{
			pOffer[playerid][e_player_offer:x] = 0;
			if( sellerid != INVALID_PLAYER_ID ) pOffer[sellerid][e_player_offer:x] = 0;
		}
	}
	else
	{
		pOffer[playerid][offer_accepted] = true;
		if( pOffer[playerid][offer_price] == 0 ) OnPlayerOfferFinish(playerid, 0, 1);
		else
		{
			ShowPlayerPayment(playerid);
		}
	}
	return 1;
}

stock OnPlayerOfferFinish(playerid, type, response)
{
	new sellerid = pOffer[playerid][offer_sellerid];
	type = pOffer[playerid][offer_type];
	
	if( !response )
	{
		if( sellerid != INVALID_PLAYER_ID )  GameTextForPlayer(sellerid, "~w~offer ~r~rejected", 3000, 3);
	}
	else
	{
		logprintf(LOG_PLAYER, "[OFFER] %s (UID: %d) accepted offer from %s (UID %d). Type: %d, price: $%d", pInfo[playerid][player_name], pInfo[playerid][player_id], pInfo[sellerid][player_name], pInfo[sellerid][player_id], type, pOffer[playerid][offer_price]);
		if( sellerid != INVALID_PLAYER_ID && type != OFFER_TYPE_REPAIR && type != OFFER_TYPE_REFUEL) 
		{
			if(type == OFFER_TYPE_PRODUCT || type == OFFER_TYPE_GYM || type == OFFER_TYPE_DOCUMENT || type == OFFER_TYPE_MANDAT || type == OFFER_TYPE_MEDIC || type == OFFER_TYPE_ADVERTISE || type == OFFER_TYPE_RP || type == OFFER_TYPE_DOOR_ACCESS || type == OFFER_TYPE_GYM_FIGHT)
			{
				new gid = pInfo[sellerid][player_duty_gid];
				if(gid == -1) return 1;
				Group[gid][group_activity_points] += pOffer[playerid][offer_price];
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_game_groups SET group_activity = %d WHERE group_uid = %d", Group[gid][group_activity_points], Group[gid][group_uid]));

				if(pOffer[playerid][offer_price] >= 40)
				{
					GiveGroupMoney(gid, pOffer[playerid][offer_price]-10);
					GivePlayerMoney(sellerid, 10);
				}
				else
				{
					GiveGroupMoney(gid, pOffer[playerid][offer_price]);
				}

				if(Group[gid][group_tributers] > 0)
				{
					if(Group[gid][group_tribute] < 3500)
					{
						Group[gid][group_tribute] += 10;
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_game_groups SET group_tribute = %d WHERE group_uid = %d", Group[gid][group_tribute], Group[gid][group_uid]));
					}
				}
			}
			else
			{
				if(type != OFFER_TYPE_TAXI)
				{
					GivePlayerMoney(sellerid, pOffer[playerid][offer_price]);
				}
			}		
			GameTextForPlayer(sellerid, "~w~offer ~g~accepted", 3000, 3);
			GameTextForPlayer(playerid, "~w~offer ~g~accepted", 3000, 3);
		}
		
		switch( type )
		{
			case OFFER_TYPE_VCARD:
			{
				new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
				if( itemid > -1 )
				{
					new number[64];
					valstr(number, PlayerItem[sellerid][pOffer[playerid][offer_extraid]][player_item_value1]);
					mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_contacts` VALUES (null, %d, '%s', %d, 0)", PlayerItem[sellerid][pOffer[playerid][offer_extraid]][player_item_value1], number, PlayerItem[playerid][itemid][player_item_uid]));
					ProxMessage(sellerid, sprintf("sends vCard %s", pInfo[playerid][player_name]), PROX_ME);
				}	
			}

			case OFFER_TYPE_PASS:
			{
				SetPlayerSpecialAction(sellerid, SPECIAL_ACTION_NONE);
				SetPlayerSpecialAction(playerid, pOffer[playerid][offer_extraid]);
			}

			case OFFER_TYPE_TOW:
			{
				new v = GetPlayerVehicleID(playerid);
				new vid = pOffer[playerid][offer_extraid];
				AttachTrailerToVehicle(vid, v);
				SendClientMessage(sellerid, COLOR_GOLD, "To finish type /tow.");
				SetPVarInt(sellerid, "holowanie", 1);
			}

			case OFFER_TYPE_TRIBUTE:
			{
				new gid = pInfo[playerid][player_duty_gid];
				if(gid == -1) return SendGuiInformation(sellerid, "Information", "You've left group duty.");
				Group[gid][group_tributers] = pOffer[playerid][offer_extraid];
				new tgid = pInfo[sellerid][player_duty_gid];
				SendClientMessage(sellerid, COLOR_GOLD, sprintf("Group %s (UID: %d) accepted your tribute (%s, UID: %d).", Group[gid][group_name], Group[gid][group_uid], Group[tgid][group_name], Group[tgid][group_uid]));
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_game_groups SET group_tributers = %d WHERE group_uid = %d", pOffer[playerid][offer_extraid], Group[gid][group_uid]));
			}

			case OFFER_TYPE_MEDIC:
			{
				cmd_ame(sellerid, sprintf("gives pills for %s.", pInfo[playerid][player_name]));
				SetPlayerHealth(playerid, 50);
			}

			case OFFER_TYPE_ADVERTISE:
			{
				SendClientMessage(sellerid, COLOR_GOLD, "Player accepted your offer, use /advertise to place his advert.");
				SendClientMessage(playerid, COLOR_GOLD, "Accepted advertise offer, reporter have to place it now.");
			}

			case OFFER_TYPE_GYM:
            {
            	new gid = pInfo[sellerid][player_duty_gid];
            	if(gid == -1) return 1;
            	
                Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_GYM_KARNET, 2709, 10, Group[gid][group_uid], "Gym ticket");
                SendGuiInformation(playerid, "Information", "Gym ticket has been added to your inventory.");
            }

            case OFFER_TYPE_GYM_FIGHT:
            {
            	new gid = pInfo[sellerid][player_duty_gid];
            	if(gid == -1) return 1;
            	
                SendGuiInformation(playerid, "Information", "You can start training at punchbag.");

                new style = pOffer[playerid][offer_extraid];

                SetPlayerFightingStyle(playerid, style);
                TextDrawSetString(Tutorial[playerid], "Hit punchbag to start ~p~training~w~.");
                TextDrawShowForPlayer(playerid, Tutorial[playerid]);

                pInfo[playerid][player_fight_training] = true;
            }

            case OFFER_TYPE_INTERVIEW:
            {
            	pInfo[sellerid][player_interview] = playerid;
            	pInfo[playerid][player_interview] = 1;
                SendClientMessage(playerid, COLOR_GOLD, "Invterview started. Everyting u say now will be visible on radio textdraw.");
                SendClientMessage(sellerid, COLOR_GOLD, "Invterview started. Everyting u say now will be visible on radio textdraw.");
            }

			case OFFER_TYPE_REPAIR:
			{				
				new vehid = pOffer[playerid][offer_extraid];

				pInfo[sellerid][player_repair_car] = pOffer[playerid][offer_extraid];
				pInfo[sellerid][player_repair_time] = 120;
				pInfo[sellerid][player_repairing] = 1;
				Attach3DTextLabelToVehicle(Vehicle[vehid][vehicle_state_label], vehid, 0.0, 1.1, 0.0);
			}

			case OFFER_TYPE_REFUEL:
			{
				new str[64];
				new vehid = GetPlayerVehicleID(playerid);
				new amount = pOffer[playerid][offer_extraid];
				new model = Vehicle[vehid][vehicle_model];
				Vehicle[vehid][vehicle_fuel_current] = Vehicle[vehid][vehicle_fuel_current] + float(amount);

				format(str, sizeof(str), "refuels %s.", VehicleNames[model-400]);
				ProxMessage(sellerid, str, PROX_ME);
				ApplyAnimation(sellerid, "INT_HOUSE", "wash_up",4.1, 0, 0, 0, 0, 0, 1);

				new price = pInfo[sellerid][player_dialog_tmp1];
				GivePlayerMoney(playerid, price);
			}

			case OFFER_TYPE_MONTAGE:
			{
				new vehid = GetPlayerVehicleID(playerid);

				pInfo[sellerid][player_montage_car] = vehid;
				pInfo[sellerid][player_montage_time] = 120;
				pInfo[sellerid][player_montage_comp] = pOffer[playerid][offer_extraid];
				Attach3DTextLabelToVehicle(Vehicle[vehid][vehicle_state_label], vehid, 0.0, 1.1, 0.0);
				pInfo[sellerid][player_tuning] = 1;
			}

			case OFFER_TYPE_DOOR_ACCESS:
			{
				new itemid = pOffer[playerid][offer_extraid];
				new access = PlayerItem[sellerid][itemid][player_item_value1];

				new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));

				if(d_id != -1)
				{
					Door[d_id][door_access] += access;
					mysql_query(mySQLconnection, sprintf("UPDATE ipb_doors SET door_access = %d WHERE door_uid = %d", Door[d_id][door_access], Door[d_id][door_uid]));
					SendClientMessage(sellerid, COLOR_GOLD, "Montage complete.");
					SendClientMessage(playerid, COLOR_GOLD, "Montage complete.");
					DeleteItem(itemid, true, playerid);
				}
				else
				{
					printf("Door access error, d_id -1.");
				}
			}

			case OFFER_TYPE_RESPRAY:
			{
				new vehid = GetPlayerVehicleID(playerid);

				pInfo[sellerid][player_spraying_car] = vehid;
				pInfo[sellerid][player_spraying_time] = 0;
				pInfo[sellerid][player_spraying_color2] = pOffer[playerid][offer_extraid];
				Attach3DTextLabelToVehicle(Vehicle[vehid][vehicle_state_label], vehid, 0.0, 1.1, 0.0);
				pInfo[sellerid][player_spraying] = 1;
			}

			case OFFER_TYPE_PAINTJOB:
			{
				new vehid = GetPlayerVehicleID(playerid);

				pInfo[sellerid][player_spraying_car] = vehid;
				pInfo[sellerid][player_spraying_time] = 0;
				pInfo[sellerid][player_spraying_color] = pOffer[playerid][offer_extraid];
				Attach3DTextLabelToVehicle(Vehicle[vehid][vehicle_state_label], vehid, 0.0, 1.1, 0.0);
				pInfo[sellerid][player_spraying] = 2;
			}

			case OFFER_TYPE_ACCESS:
			{
				new vehid = GetPlayerVehicleID(playerid);

				pInfo[sellerid][player_montage_car] = vehid;
				pInfo[sellerid][player_montage_time] = 120;
				pInfo[sellerid][player_montage_comp] = pOffer[playerid][offer_extraid];
				Attach3DTextLabelToVehicle(Vehicle[vehid][vehicle_state_label], vehid, 0.0, 1.1, 0.0);
				pInfo[sellerid][player_tuning] = 2;
			}

			case OFFER_TYPE_VEHICLE:
			{
				new vehid = pOffer[playerid][offer_extraid];
				new vehidd = GetPlayerVehicleID(sellerid);

				if( Vehicle[vehid][vehicle_uid] != Vehicle[vehidd][vehicle_uid])
				{
					KickAc(sellerid, -1, "Bug abusing try (o: veh)");
					return 1;
				}
				
				Vehicle[vehid][vehicle_owner] = pInfo[playerid][player_id];

				mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_owner = %d WHERE vehicle_uid = %d", Vehicle[vehid][vehicle_owner], Vehicle[vehid][vehicle_uid]));

				SendGuiInformation(playerid, "Information", "You've bought new car.");	
				SendGuiInformation(sellerid, "Information", "Vehicle has been sold.");
				RemovePlayerFromVehicle(sellerid);	
			}

			case OFFER_TYPE_ITEM:
			{
				new itemid = pOffer[playerid][offer_extraid];
				if( PlayerItem[sellerid][itemid][player_item_used] )
				{
					KickAc(sellerid, -1, "Bug abusing try (item use)");
					return 1;
				}

				if(!Iter_Contains(PlayerItems[sellerid], itemid))
				{
					KickAc(sellerid, -1, "Bug abusing try (item drop)");
					return 1;
				}

				mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_owner = %d WHERE item_uid = %d", pInfo[playerid][player_id], PlayerItem[sellerid][itemid][player_item_uid]));
				new uid = PlayerItem[sellerid][itemid][player_item_uid];
				DeleteItem(itemid, false, sellerid);
				new item = LoadPlayerItem(playerid, sprintf("WHERE item_uid = %d", uid));

				pInfo[sellerid][player_capacity] += PlayerItem[playerid][item][player_item_weight];
				pInfo[playerid][player_capacity] -= PlayerItem[playerid][item][player_item_weight];
			}

			case OFFER_TYPE_DOOR:
			{
				new d_id = pOffer[playerid][offer_extraid];

				Door[d_id][door_owner] = pInfo[playerid][player_id];
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_doors SET door_owner = %d WHERE door_uid = %d", Door[d_id][door_owner], Door[d_id][door_uid]));
				SendClientMessage(playerid, COLOR_GOLD, sprintf("Door %s has been bought.", Door[d_id][door_name]));
				SendClientMessage(sellerid, COLOR_GOLD, sprintf("Door %s has been sold.", Door[d_id][door_name]));
			}

			case OFFER_TYPE_PRODUCT:
			{
				new itemid = pOffer[playerid][offer_extraid];
				new gid = pInfo[sellerid][player_duty_gid];
				new itemname[40];
				format(itemname, sizeof(itemname), Item[itemid][item_name]);

				Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, Item[itemid][item_type], Item[itemid][item_model], Item[itemid][item_value1], Item[itemid][item_value2], itemname, Item[itemid][item_extraid]);

				printf("[OFFER %s (%d)] %s (UID %d) sold product %s (UID %d) for %s (UID %d). Price: $%d.", Group[gid][group_name], Group[gid][group_uid], pInfo[sellerid][player_name], pInfo[sellerid][player_id], Item[itemid][item_name], Item[itemid][item_uid], pInfo[playerid][player_name], pInfo[playerid][player_id], Item[itemid][item_price]);	
				
				pInfo[sellerid][player_capacity] += Item[itemid][item_weight];
				pInfo[playerid][player_capacity] -= Item[itemid][item_weight];

				if(Item[itemid][item_count] <= 1)
				{
					Item[itemid][item_group] = 0;
					Item[itemid][item_count] = 0;
					DeleteItem(itemid, true);
				}
				else
				{
					Item[itemid][item_count]--;
					mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_count = %d WHERE item_uid = %d", Item[itemid][item_count], Item[itemid][item_uid]));
				}

				SendGuiInformation(playerid, "Information", "Item has been added to your inventory.");	
				SendGuiInformation(sellerid, "Information", "Product has been sold");
			}

			case OFFER_TYPE_TAXI:
			{
				pInfo[sellerid][player_taxi_veh] = GetPlayerVehicleID(sellerid);
				pInfo[playerid][player_taxi_veh] = GetPlayerVehicleID(sellerid);
				pInfo[sellerid][player_taxi_price] = pOffer[playerid][offer_extraid];
				pInfo[sellerid][player_taxi_distance] = 0;
				pInfo[sellerid][player_taxi_passenger] = playerid;
			
				pInfo[sellerid][player_taxi_drive] = true;
			}

			case OFFER_TYPE_YO:
			{
				new welcome_type = pOffer[playerid][offer_extraid],
		    	Float:PosX, Float:PosY, Float:PosZ, Float:PosA;

		    	new offererid = pOffer[playerid][offer_sellerid];

				GetPlayerPos(offererid, PosX, PosY, PosZ);
				
				if(welcome_type <= 6)
				{
					GetXYInFrontOfPlayer(offererid, PosX, PosY, 1.0);
				}
				else
				{
		            GetXYInFrontOfPlayer(offererid, PosX, PosY, 0.5);
				}
				
				GetPlayerFacingAngle(offererid, PosA);

				SetPlayerPos(playerid, PosX, PosY, PosZ);
				SetPlayerFacingAngle(playerid, (floatabs(PosA) - 180.0));
				
				switch(welcome_type)
				{
				    case 1:
				    {
		  				ApplyAnimation(offererid, "GANGS", "hndshkfa", 4.1, 0, 0, 0, 0, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkfa", 4.1, 0, 0, 0, 0, 0, 1);
				    }
				    case 2:
				    {
		  				ApplyAnimation(offererid, "GANGS", "hndshkba", 4.1, 0, 0, 0, 0, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkba", 4.1, 0, 0, 0, 0, 0, 1);
				    }
				    case 3:
				    {
				    	ApplyAnimation(offererid, "GANGS", "hndshkaa", 4.1, 0, 0, 0, 0, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkaa", 4.1, 0, 0, 0, 0, 0, 1);
				    }
				    case 4:
				    {
		  				ApplyAnimation(offererid, "GANGS", "hndshkda", 4.1, 0, 0, 0, 0, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkda", 4.1, 0, 0, 0, 0, 0, 1);
				    }
				    case 5:
				    {
		  				ApplyAnimation(offererid, "GANGS", "hndshkca", 4.1, 0, 0, 0, 0, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkca", 4.1, 0, 0, 0, 0, 0, 1);
				    }
				    case 6:
				    {
		  				ApplyAnimation(offererid, "GANGS", "hndshkcb", 4.1, 0, 0, 0, 0, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkcb", 4.1, 0, 0, 0, 0, 0, 1);
				    }
				    case 7:
				    {
						ApplyAnimation(offererid, "KISSING", "Playa_Kiss_01", 4.1, 0, 0, 0, 0, 0, 1);
						ApplyAnimation(playerid, "KISSING", "Playa_Kiss_01", 4.1, 0, 0, 0, 0, 0, 1);
					}
					case 8:
					{
						ApplyAnimation(offererid, "KISSING", "Playa_Kiss_02", 4.1, 0, 0, 0, 0, 0, 1);
						ApplyAnimation(playerid, "KISSING", "Playa_Kiss_02", 4.1, 0, 0, 0, 0, 0, 1);
					}
					case 9:
					{
						ApplyAnimation(offererid, "KISSING", "Playa_Kiss_03", 4.1, 0, 0, 0, 0, 0, 1);
						ApplyAnimation(playerid, "KISSING", "Playa_Kiss_03", 4.1, 0, 0, 0, 0, 0, 1);
					}
				}
			}
			
			case OFFER_TYPE_DOCUMENT:
			{
				pInfo[playerid][player_documents] += pOffer[playerid][offer_extraid];
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_documents` = %d WHERE `char_uid` = %d", pInfo[playerid][player_documents], pInfo[playerid][player_id]));
			}
		}
	}
	
	for(new x=0; e_player_offer:x != e_player_offer; x++)
	{
		pOffer[playerid][e_player_offer:x] = 0;
		if( sellerid != INVALID_PLAYER_ID ) pOffer[sellerid][e_player_offer:x] = 0;
	}
	return 1;
}