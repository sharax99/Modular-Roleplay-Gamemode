stock AddGroupLog(guid, text[])
{
 	mysql_query(mySQLconnection, sprintf("INSERT INTO `gamecp_group_logs` (`log_group`, `log_datetime`,  `log_action`) VALUES ('%d', '%d', '%s')", guid, gettime(), text));
	return 1;
}

stock CountGroupPlayers(groupid)
{
	new count;
	foreach(new playerid: Player)
	{
		if(pInfo[playerid][player_duty_gid] == groupid)
		{
			count++;
		}
	}
	return count;
}

stock GetGroupByUid(groupUid)
{
	foreach (new g : Groups)
	{
		if( Group[g][group_uid] == groupUid ) return g;
	}
	
	return -1;
}

stock IsPlayerInAnyGroup(playerid)
{
	if( pGroup[playerid][0][pg_id] > -1 ) return 1;
	
	return 0;
}

stock IsAnyGastroOpen()
{
	foreach(new pid: Player)
	{
		new gid = pInfo[pid][player_duty_gid];
		if(gid != -1)
		{
			if(Group[gid][group_type] == GROUP_TYPE_GASTRO)
			{
				return 1;
			}
		}
	}
	return 0;
}

stock IsAnyWorkshopOpen()
{
	foreach(new pid: Player)
	{
		new gid = pInfo[pid][player_duty_gid];
		if(gid != -1)
		{
			if(Group[gid][group_type] == GROUP_TYPE_WORKSHOP)
			{
				return 1;
			}
		}
	}
	return 0;
}

stock IsAnyGovOnline()
{
	foreach(new pid: Player)
	{
		new gid = pInfo[pid][player_duty_gid];
		if(gid != -1)
		{
			if(Group[gid][group_type] == GROUP_TYPE_GOV)
			{
				return 1;
			}
		}
	}
	return 0;
}

stock SherifsOnline()
{
	new count;
	foreach(new pid: Player)
	{
		new gid = pInfo[pid][player_duty_gid];
		if(gid != -1)
		{
			if(Group[gid][group_type] == GROUP_TYPE_LSPD)
			{
				count++;
			}
		}
	}
	if(count > 2) return 1;
	return 0;
}

stock IsAnyTaxiOnDuty()
{
	foreach(new pid: Player)
	{
		new gid = pInfo[pid][player_duty_gid];
		if(gid != -1)
		{
			if(Group[gid][group_type] == GROUP_TYPE_TAXI)
			{
				return 1;
			}
		}
	}
	return 0;
}

stock IsCorrectGroup(gid)
{
	if( !(gid >= 0 && gid < MAX_GROUPS) ) return 0;
	if( !Iter_Contains(Groups, gid) ) return 0;
	
	return 1;
}

stock GetPlayerGroupSlot(playerid, gid)
{
	for(new i;i<5;i++)
	{
		if( pGroup[playerid][i][pg_id] == gid ) return i;
	}
	return -1;
}

stock GetPlayerDutySlot(playerid)
{
	for(new i;i<5;i++)
	{
		if( pGroup[playerid][i][pg_duty] ) return i;
	}
	
	return -1;
}

stock GetPlayerGroupFreeSlot(playerid)
{
	for(new i;i<5;i++)
	{
		if( pGroup[playerid][i][pg_id] == -1 ) return i;
	}
	
	return -1;
}

stock GetGroupType(gid)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT group_type FROM `ipb_game_groups` WHERE `group_uid` = %d", Group[gid][group_uid]));
	cache_get_data(rows, fields);
	
	new gtype = cache_get_row_int(0, 0);
	
	return gtype;
}

stock GetGroupDotation(gid)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT group_dotation FROM `ipb_game_groups` WHERE `group_uid` = %d", Group[gid][group_uid]));
	cache_get_data(rows, fields);
	
	new dotation = cache_get_row_int(0, 0);
	
	return dotation;
}

stock _:GetGroupTag(gid)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT group_tag FROM `ipb_game_groups` WHERE `group_uid` = %d", Group[gid][group_uid]));
	cache_get_data(rows, fields);
	
	new tag[15];
	cache_get_row(0, 0, tag);
	
	return tag;
}

stock GetGroupColor(gid)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT group_color FROM `ipb_game_groups` WHERE `group_uid` = %d", Group[gid][group_uid]));
	cache_get_data(rows, fields);

	new color = cache_get_row_int(0, 0);
	return color;
}

stock GetPlayerSubgroupByOwner(playerid, g_uid)
{
	for(new i;i<5;i++)
	{
		if( pGroup[playerid][i][pg_id] != -1 )
		{
			if( Group[pGroup[playerid][i][pg_id]][group_parent_uid] == g_uid ) return i;
		}
	}
	
	return -1;
}

stock GiveGroupMoney(gid, amount)
{
	Group[gid][group_bank_money] += amount;
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_game_groups` SET `group_cash` = %d WHERE `group_uid` = %d", Group[gid][group_bank_money], Group[gid][group_uid]));
}

stock GiveGroupCapital(gid, amount)
{
	Group[gid][group_capital] += amount;
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_game_groups` SET `group_capital` = %d WHERE `group_uid` = %d", Group[gid][group_capital], Group[gid][group_uid]));
}

stock SendGroupOOC(playerid, slot, text[], bool:to_subs = false)
{
	slot -= 1;
	if( pGroup[playerid][slot][pg_id] == -1 ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_OOC) ) return SendGuiInformation(playerid, "Informacja", "This group has no OOC chat flag.");
	if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_CHAT) ) return GameTextForPlayer(playerid, "~r~You have no chat access", 4000, 3);
	if(Group[gid][group_chat] == true) return SendGuiInformation(playerid, "Informacja", "Leader of this group blocked chat.");
	
	new string[160];
	format(string, sizeof(string), BeautifyString(text, true, false, false));
	
	new str[200], gcolor = MakeColorDarker(GetGroupColor(gid), 30);
	foreach(new p : Player)
	{
		new pslot = GetPlayerGroupSlot(p, gid);
		if( to_subs && pslot == -1 )
		{
			if( Group[gid][group_parent_uid] == 0 ) pslot = GetPlayerSubgroupByOwner(p, Group[gid][group_uid]);
			else 
			{
				pslot = GetPlayerGroupSlot(p, GetGroupByUid(Group[gid][group_parent_uid]));
				if( pslot == -1 ) pslot = GetPlayerSubgroupByOwner(p, Group[gid][group_parent_uid]);
			}
		}
		if( pslot == -1 ) continue;
		
		switch( pslot )
		{
			case 0: if( PlayerHasTog(p, TOG_G1) ) continue;
			case 1: if( PlayerHasTog(p, TOG_G2) ) continue;
			case 2: if( PlayerHasTog(p, TOG_G3) ) continue;
			case 3: if( PlayerHasTog(p, TOG_G4) ) continue;
			case 4: if( PlayerHasTog(p, TOG_G5) ) continue;
		}
		
		if( to_subs ) format(str, sizeof(str), "[@@%d %s]: (( %s [%d]: %s ))", ((pGroup[p][pslot][pg_id] == gid) ? (pslot+1) : (0)), GetGroupTag(gid), pInfo[playerid][player_name], playerid, string);
		else format(str, sizeof(str), "[@%d %s]: (( %s [%d]: %s ))", pslot+1, GetGroupTag(gid), pInfo[playerid][player_name], playerid, string);
		SendClientMessage(p, gcolor, str);
	}
	
	pInfo[playerid][player_last_group_slot_chat] = slot+1;
	
	return 1;
}


stock SendGroupIC(playerid, slot, text[])
{
	slot -= 1;
	if( pGroup[playerid][slot][pg_id] == -1 ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
	
	new gid = pGroup[playerid][slot][pg_id];

	if( !GroupHasFlag(gid, GROUP_FLAG_IC) ) return SendGuiInformation(playerid, "Informacja", "This group has no radio flag.");
	if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_CHAT) ) return GameTextForPlayer(playerid, "~r~You have no access to chat", 4000, 3);
	if(strlen(text) > 100) return SendGuiInformation(playerid, "Informacja", "Max 100 chars.");
	
	new string[400];
	format(string, sizeof(string), BeautifyString(text, true, true, false));
	
	new str[500], gcolor = GetGroupColor(gid);
	foreach(new p : Player)
	{
		new pslot = GetPlayerGroupSlot(p, gid);
		if( pslot == -1 ) continue;
		
		switch( pslot )
		{
			case 0: if( PlayerHasTog(playerid, TOG_G1) ) continue;
			case 1: if( PlayerHasTog(playerid, TOG_G2) ) continue;
			case 2: if( PlayerHasTog(playerid, TOG_G3) ) continue;
			case 3: if( PlayerHasTog(playerid, TOG_G4) ) continue;
			case 4: if( PlayerHasTog(playerid, TOG_G5) ) continue;
		}
		
		format(str, sizeof(str), "!%d ** [%s]: %s: %s **", pslot+1, GetGroupTag(gid), pInfo[playerid][player_name], string);
		SendClientMessage(p, gcolor, str);
	}
	
	ProxMessage(playerid, text, PROX_RADIO);
	pInfo[playerid][player_last_group_slot_chat] = slot+1;
	
	return 1;
}

stock ShowPlayerGroups(playerid)
{
	TextDrawShowForPlayer(playerid, GroupsListStaticHeader);
	
	for(new i=0;i<5;i++)
	{
		if( pGroup[playerid][i][pg_id] > -1 )
		{
			new gid = pGroup[playerid][i][pg_id];
			PlayerTextDrawSetString(playerid, GroupsListRow[i], sprintf("%d       %s (%d)", i+1, Group[gid][group_name], Group[gid][group_uid]));
			PlayerTextDrawShow(playerid, GroupsListRow[i]);
			for(new y=0;y<5;y++) PlayerTextDrawShow(playerid, GroupsListStaticButtons[i][y]);
		}
	}
	
	SelectTextDraw(playerid, 0x750000FF);
	
	pInfo[playerid][player_group_list_showed] = true;
}

stock CountPlayerGroups(playerid)
{
	new gcount;
	for(new i=0;i<5;i++)
	{
		if( pGroup[playerid][i][pg_id] > -1 )
		{
			gcount++;
		}
	}
	return gcount;
}

stock GetPlayerPayday(playerid)
{
	new payday;
	for(new i=0;i<5;i++)
	{
		if( pGroup[playerid][i][pg_id] > -1 )
		{
			payday += pGroup[playerid][i][pg_rank_payment];
		}
	}
	return payday;
}

stock HideGroupsList(playerid)
{
	TextDrawHideForPlayer(playerid, GroupsListStaticHeader);
	for(new i=0;i<5;i++)
	{
		PlayerTextDrawHide(playerid, GroupsListRow[i]);
		for(new y=0;y<5;y++) PlayerTextDrawHide(playerid, GroupsListStaticButtons[i][y]);
	}
	
	CancelSelectTextDraw(playerid);
	
	pInfo[playerid][player_group_list_showed] = false;
}

stock ShowGroupDutyPane(playerid, slot)
{
	new hour, minute, second;
	gettime(hour, minute, second);
	
	new gid = pGroup[playerid][slot][pg_id];
	
	TextDrawSetString(DashBoardPane[playerid], sprintf("~b~group                    ~y~%s (UID: %d)~n~~n~~w~Started: ~g~%02d:%02d               ~w~Title: ~r~%s", Group[gid][group_name], Group[gid][group_uid], hour, minute, pGroup[playerid][slot][pg_rank_title]));
	TextDrawShowForPlayer(playerid, DashBoardPane[playerid]);
	TextDrawShowForPlayer(playerid, DashBoardPane2[playerid]);
	
	defer HideGroupDutyPane[4000](playerid);
}