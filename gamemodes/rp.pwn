/*

	Society Roleplay

	Last update: 26.01.2017

	Author: Raydex
	
	Credits:
	- Promsters
	- Mario

*/

// Include
#include <a_samp>
#include <dini>
#include <a_mysql>
#include <md5>
#include <streamer>
#include <timestamptodate>
#include <sscanf2>
#include <kickfix>
#include <sprintf>
#include <YSI\y_iterate>
#include <YSI\y_timers>
#include <YSI\y_va>
#include <physics>
#include <zones>
#include <zcmd>
#include <geoip>

#pragma tabsize 0
#pragma dynamic 8196

// Moduly
#include "rp/color_management.inc"
#include "rp/config.inc"
#include "rp/code_timer.inc"
#include "rp/misc.inc"
#include "rp/dynamicgui.inc"
#include "rp/robbery.inc"
#include "rp/buses.inc"
#include "rp/penalties.inc"
#include "rp/functions.inc"
#include "rp/areas.inc"
#include "rp/groups.inc"
#include "rp/vehicles.inc"
#include "rp/offers.inc"
#include "rp/items.inc"
#include "rp/labels.inc"
#include "rp/player.inc"
#include "rp/textdraws.inc"
#include "rp/objects.inc"
#include "rp/gates.inc"
#include "rp/gym.inc"
#include "rp/acmd.inc"
#include "rp/cmd.inc"
#include "rp/materials.inc"
#include "rp/timers.inc"
#include "rp/doors.inc"
#include "rp/fires.inc"
#include "rp/actors.inc"
#include "rp/products.inc"
#include "rp/works.inc"
#include "rp/special_places.inc"

main() {}

public OnGameModeInit()
{
    Code_ExTimer_Begin(GameModeInit);

    ShowPlayerMarkers(0);
    ShowNameTags(0);
    DisableInteriorEnterExits();
    EnableStuntBonusForAll(0);
    ManualVehicleEngineAndLights();

    Streamer_SetVisibleItems(STREAMER_TYPE_OBJECT, MAX_VISIBLE_OBJECTS); 

    Iter_Init(PlayerItems);
    Iter_Init(PlayerVehicles); 

    CreateTextdraws();

    new
	Float:x1 = 786.7335,
	Float:y1 = -1331.0755,
	Float:x2 = 648.1239,
	Float:y2 = -1384.4113;
	PHY_CreateWall(x1, y1, x1, y2);
	PHY_CreateWall(x1, y2, x2, y2);
	PHY_CreateWall(x2, y2, x2, y1);
	PHY_CreateWall(x2, y1, x1, y1);

    for(new i;i<MAX_PLAYERS;i++)
    {
        pInfo[i][player_label] = Create3DTextLabel("", 0xFFFFFF60, 0.0, 0.0, 0.0, 12.0, 0, 1);
        pInfo[i][player_description_label] = Create3DTextLabel("", LABEL_DESCRIPTION, 0.0, 0.0, 0.0, 4.0, 0, 1);
    }

    FerrisWheelObjects[10]=CreateObject(18877,389.7734,-2028.4688,22,0,0,90,300);
	FerrisWheelObjects[11]=CreateObject(18878,389.7734,-2028.4688,22,0,0,90,300);
	forEx((sizeof FerrisWheelObjects)-2,x){
		FerrisWheelObjects[x]=CreateObject(18879,389.7734,-2028.4688,22,0,0,90,300);
		AttachObjectToObject(FerrisWheelObjects[x], FerrisWheelObjects[10],gFerrisCageOffsets[x][0],gFerrisCageOffsets[x][1],gFerrisCageOffsets[x][2],0.0, 0.0, 90, 0 );}
	SetTimer("RotateFerrisWheel", 3000,false);

    job_pickup = CreatePickup(1210, 2, 1495.9677,-1749.3802,15.4453);

    LoadConfiguration();
    if( !ConnectMysql() ) return 1;

    LoadGlobalSpawns();
    LoadGroups();
    LoadAreas();
    LoadDoors();
    LoadBusStops();
    LoadLabels();
    LoadObjects();
    LoadVehicles();
    LoadItems();
    LoadAnims();
    LoadActors();
    LoadProducts();
    LoadGates();
    LoadMaterials();
    LoadSpecialPlaces();
    LoadSkins();
    LoadAccess();

    mysql_query(mySQLconnection, "DELETE FROM `ipb_logged_players`");
    mysql_query(mySQLconnection, "UPDATE `ipb_characters` SET char_online = 0");
    
    printf("Society Roleplay started. [time: %d ms]", Code_ExTimer_End(GameModeInit));
    return 1;
}

public OnGameModeExit()
{
	foreach(new p : Player)
	{
		SavePlayer(p);
	}
	
	foreach(new v : Vehicles)
	{
		SaveVehicle(v);
	}
	
	mysql_close(mySQLconnection);
	return 1;
}

public OnPlayerConnect(playerid)
{
	if( IsPlayerNPC(playerid) )
	{
		return 1;
	}

	SetPlayerVirtualWorld(playerid, playerid+900);
	SetPlayerColor(playerid, 0x00000000);

	ResetPlayerWeapons(playerid);
	ResetPlayerWeapons(playerid);

	CleanGlobalData(playerid);
	CleanPlayerData(playerid);

	CreatePlayerTextdraws(playerid);	
	TextDrawShowForPlayer(playerid, Textdraw2);

	TogglePlayerSpectating(playerid, 1);

	GetPlayerName(playerid, pInfo[playerid][player_name], 60);
	pInfo[playerid][player_name][0] = chrtoupper(pInfo[playerid][player_name][0]);
	strreplace(pInfo[playerid][player_name], '_', ' ');

	new name_escaped[MAX_PLAYER_NAME+1];
	strcopy(name_escaped, pInfo[playerid][player_name], MAX_PLAYER_NAME+1);
	strreplace(name_escaped, ' ', '_');

	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT ch.char_uid, ch.char_gid, m.name, m.member_id, m.members_pass_salt, m.members_pass_hash, m.member_game_points, m.member_game_ban, m.member_game_admin_perm, m.member_premium_time FROM ipb_characters ch INNER JOIN ipb_members m ON ch.char_gid = m.member_id WHERE ch.char_name = '%s'", name_escaped));
	cache_get_data(rows, fields);

	cache_get_row(0, 4, gInfo[playerid][global_salt], mySQLconnection, 20);
	cache_get_row(0, 5, gInfo[playerid][global_password], mySQLconnection, 80);
	cache_get_row(0, 2, gInfo[playerid][global_name], mySQLconnection, MAX_PLAYER_NAME+1);
	cache_get_row(0, 2, pGlobal[playerid][glo_name], mySQLconnection, MAX_PLAYER_NAME+1);
	
	pInfo[playerid][player_id] = cache_get_row_int(0, 0);	
	gInfo[playerid][global_id] = cache_get_row_int(0, 3);

	pGlobal[playerid][glo_id] 		=  cache_get_row_int(0, 3);
	pGlobal[playerid][glo_score] 	=  cache_get_row_int(0, 6);
	pGlobal[playerid][glo_ban] 		=  cache_get_row_int(0, 7);
	pGlobal[playerid][glo_perm] 	=  cache_get_row_int(0, 8);
	pGlobal[playerid][glo_premium] 	=  cache_get_row_int(0, 9);

	new serial[128];
	gpci(playerid, serial, sizeof(serial));

	format(pInfo[playerid][player_serial], sizeof(serial), "%s", serial);

	if( pGlobal[playerid][glo_ban] > 0 )
	{
		SendGuiInformation(playerid, ""guiopis"Alert", "This account has been banned. Please file an appeal.");
		Kick(playerid);	
		return 1;
	}

	RemoveBuildingsForPlayer(playerid);

	connect_timer[playerid] = SetTimerEx("WelcomeTimer", 60000, false, "i", playerid);

	if( !rows )
	{
		new string[512];
		format(string, sizeof(string), "Welcome to Society Roleplay!\n\nA Character with that name doesn't exist.\nYou can create a new character on our website.\n: societyroleplay.com");
		ShowPlayerDialog(playerid, DIALOG_LOGIN_NO_ACCOUNT, DIALOG_STYLE_MSGBOX, ""guiopis"Login panel", string, "Exit", "" );
		return 1;
	}
	
	for(new i = 1; i < 40; i++)
 	{
  		SendClientMessage(playerid, -1, " ");
 	}
 	
	new string[512];
	format(string, sizeof(string), "Welcome to Society Roleplay!\nCharacter {AFAFAF}%s {a9c4e4}has been found in our database.\nPlease input your password to login.", pInfo[playerid][player_name]);
    ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, ""guiopis"Login panel", string, "Login", "Exit");
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	if( IsPlayerNPC(playerid) ) return 1;
	mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_logged_players` WHERE `char_uid` = %d", pInfo[playerid][player_id]));
	Update3DTextLabelText(pInfo[playerid][player_description_label], LABEL_DESCRIPTION, "");
	KillTimer(connect_timer[playerid]);
	if( !pInfo[playerid][player_logged] ) return 1;

	SavePlayer(playerid);

	if(pInfo[playerid][player_golf])
	{
		golf = 0;
	}

	if(reason == 0)
	{
		new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);
	
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_online`='0', `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d, `char_quittime`=%d WHERE `char_uid`=%d", x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), gettime(), pInfo[playerid][player_id]));

		if(GetPlayerVehicleID(playerid) != INVALID_VEHICLE_ID)
		{
			new vd = GetPlayerVehicleID(playerid);
			Vehicle[vd][vehicle_engine] = false;
			UpdateVehicleVisuals(vd);
		}
	}

	if(reason == 1 || reason == 2)
	{
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_online`='0' WHERE `char_uid`=%d", pInfo[playerid][player_id]));
	}

	if( IsValidDynamicObject(pInfo[playerid][player_edited_object]) )
	{
		OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_CANCEL, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	}

	if( pInfo[playerid][player_creating_area] )
	{
		if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
		if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
		
		GangZoneDestroy(pInfo[playerid][player_carea_zone]);
	}

	if( pOffer[playerid][offer_type] > 0 )
	{
		if( pOffer[playerid][offer_sellerid] == INVALID_PLAYER_ID )
		{
			new buyerid = pOffer[playerid][offer_buyerid];
			for(new x=0; e_player_offer:x != e_player_offer; x++)
			{
				pOffer[buyerid][e_player_offer:x] = 0;
			}
			for(new i;i<6;i++) PlayerTextDrawHide(buyerid, OfferTD[i]);
			CancelSelectTextDraw(buyerid);
			SendGuiInformation(buyerid, ""guiopis"Alert", "Player, who made you an offer has left the server.");
		}
		else OnPlayerOfferResponse(playerid, 0);
	}

	if( pInfo[playerid][player_phone_call_started] )
	{
		if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID )
		{
			new targetid = -1;
			if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
			else targetid = pInfo[playerid][player_phone_caller];
			
			SendClientMessage(targetid, COLOR_YELLOW, "Call ended.");
			pInfo[targetid][player_phone_call_started] = false;
			pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
			pInfo[targetid][player_phone_caller] = INVALID_PLAYER_ID;
			
			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
			SetPlayerSpecialAction(targetid, SPECIAL_ACTION_STOPUSECELLPHONE);
			if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
			if( pInfo[targetid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(targetid, pInfo[targetid][player_phone_object_index]);
		}
	}
	else
	{
		if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID && pInfo[playerid][player_phone_receiver] != INVALID_PLAYER_ID )
		{
			new targetid = -1;
			if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
			else targetid = pInfo[playerid][player_phone_caller];
			
			SendClientMessage(targetid, COLOR_YELLOW, "Call ended.");
			pInfo[targetid][player_phone_call_started] = false;
			pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
			pInfo[targetid][player_phone_caller] = INVALID_PLAYER_ID;
			
			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
			SetPlayerSpecialAction(targetid, SPECIAL_ACTION_STOPUSECELLPHONE);
			if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
			if( pInfo[targetid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(targetid, pInfo[targetid][player_phone_object_index]);
		}
	}

	if( pInfo[playerid][player_lookup_area] )
	{
		cmd_area(playerid, "check");
	}
	
	if( pInfo[playerid][player_admin_duty] )
	{
		cmd_duty(playerid, "");
	}

	new slot = GetPlayerDutySlot(playerid);
	if( slot > -1 )
	{
		cmd_g(playerid, sprintf("%d duty", slot+1));
	}

	for(new i;i<13;i++)
	{
		if( pWeapon[playerid][i][pw_itemid] > -1 ) Item_Use(pWeapon[playerid][i][pw_itemid], playerid);
	}

	for(new item;item<MAX_PLAYER_ITEMS;item++)
	{
		if( PlayerItem[playerid][item][player_item_uid] < 1 ) continue;
		
		DeleteItem(item, false, playerid);
	}

	new Text3D:EndLabel, str[64], left_reason[32];
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);

	switch(reason)
	{
		case 0:
		{
			format(left_reason, sizeof(left_reason), "timeout");
		}
		case 1:
		{
			format(left_reason, sizeof(left_reason), "/q");
		}
		case 2:
		{
			format(left_reason, sizeof(left_reason), "/qs");
		}
	}

	format(str, sizeof(str), "(( %s - %s ))", pInfo[playerid][player_name], left_reason);
	EndLabel = CreateDynamic3DTextLabel(str, COLOR_GREY, x, y, z, 25.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID);
	defer DestroyQuitText[15000](EndLabel);

	return 1;
}

public OnQueryError(errorid, error[], callback[], query[], connectionHandle)
{
	switch(errorid)
	{
		case CR_SERVER_GONE_ERROR:
		{
			printf("[Society MySQL] Lost connection to MySQL, reconnecting...");
			mysql_reconnect(connectionHandle);
		}
		case ER_SYNTAX_ERROR:
		{
			printf("[Society MySQL]: Syntax error: %s",query);
		}
	}
	return 1;
}

stock OnPlayerWeaponChange(playerid, newweapon, oldweapon)
{
	if( oldweapon > -1 )
	{
		new slot = GetWeaponSlot(oldweapon), wid, wammo;
		GetPlayerWeaponData(playerid, slot, wid, wammo);

		if( pWeapon[playerid][slot][pw_itemid] > -1 && wid > 0 && wammo == 0 )
		{
			new itemid = pWeapon[playerid][slot][pw_itemid];
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				pWeapon[playerid][slot][pw_ammo] = 0;
				Item_Use(pWeapon[playerid][slot][pw_itemid], playerid);
			}
		}
	}
	
	new wslot;
	if( newweapon > 1 )
	{
		wslot = GetWeaponSlot(newweapon);
		if( pWeapon[playerid][wslot][pw_object_index] > -1 )
		{
			RemovePlayerAttachedObject(playerid, ATTACH_SLOT_WEAPON);
			pWeapon[playerid][wslot][pw_object_index] = -1;
		}
	}
	
	if( oldweapon > -1 )
	{
		wslot = GetWeaponSlot(oldweapon);
		if( pWeapon[playerid][wslot][pw_id] != oldweapon ) return 1;
		if( pWeapon[playerid][wslot][pw_id] != oldweapon ) return 1;
		if( WeaponVisualModel[oldweapon] > -1 )
		{
			new index = ATTACH_SLOT_WEAPON;
			if(IsPlayerAttachedObjectSlotUsed(playerid, index)) return RemovePlayerAttachedObject(playerid, index);
			
			new itemid = pWeapon[playerid][wslot][pw_itemid], ow = oldweapon;

			if(ao[playerid][index][ao_inserted]==false)
			{
				if( PlayerItem[playerid][itemid][player_item_group] > 0 ) SetPlayerAttachedObject(playerid, index, WeaponVisualModel[ow], WeaponVisualBone[ow], FWeaponVisualPos[ow][0], FWeaponVisualPos[ow][1], FWeaponVisualPos[ow][2], FWeaponVisualPos[ow][3], FWeaponVisualPos[ow][4], FWeaponVisualPos[ow][5], FWeaponVisualPos[ow][6], FWeaponVisualPos[ow][7], FWeaponVisualPos[ow][8]);
				else SetPlayerAttachedObject(playerid, index, WeaponVisualModel[ow], WeaponVisualBone[ow], WeaponVisualPos[ow][0], WeaponVisualPos[ow][1], WeaponVisualPos[ow][2], WeaponVisualPos[ow][3], WeaponVisualPos[ow][4], WeaponVisualPos[ow][5], WeaponVisualPos[ow][6], WeaponVisualPos[ow][7], WeaponVisualPos[ow][8]);
			}
			else
			{
				SetPlayerAttachedObject(playerid, index, WeaponVisualModel[ow], WeaponVisualBone[ow], ao[playerid][index][ao_x], ao[playerid][index][ao_y], ao[playerid][index][ao_z], ao[playerid][index][ao_rx], ao[playerid][index][ao_ry], ao[playerid][index][ao_rz], WeaponVisualPos[ow][6], WeaponVisualPos[ow][7], WeaponVisualPos[ow][8]);
			}
			pWeapon[playerid][wslot][pw_object_index] = index;
		}
	}
	return 1;
}

public OnPlayerUpdate(playerid)
{
	if( IsPlayerNPC(playerid) ) return 1;

	if(GetPlayerCameraMode(playerid) == 53)  
    {  
        new Float:kLibPos[3];  
        GetPlayerCameraPos(playerid, kLibPos[0], kLibPos[1], kLibPos[2]); 
        if ( kLibPos[2] < -50000.0 || kLibPos[2] > 50000.0 )  
        {  
            BanAc(playerid, -1, "Invalid aim data");  
            return 0;  
        }  
    }   

    if(pGlobal[playerid][glo_run])
    {
    	if(GetPlayerSpeed(playerid) > 1)
    	{
    		new keysa, uda, lra;
			GetPlayerKeys(playerid, keysa, uda, lra);
			if(!(keysa & KEY_WALK))
			{
	    		new skin = GetPlayerSkin(playerid);
	    		SetPlayerSkin(playerid, skin);
	    		TogglePlayerControllable(playerid, 0);
	    		TogglePlayerControllable(playerid, 1);
	    	}
    	}
    }

	if(pInfo[playerid][player_skin_changing] == true)
    {
		new Keys, ud, lr;
  		GetPlayerKeys(playerid, Keys, ud, lr);
        if(lr < 0 || lr > 0)
        {
            new action = lr < 0 ? 1 : -1,
				uid = pInfo[playerid][player_skin_id],
				str[ 20 ];

            uid = uid + action < 0 ? MAX_SKINS - 1: (uid + action >= MAX_SKINS ? 0: uid + action);

            if(ClothSkin[uid][skin_model] != 0)
            {
	            pInfo[playerid][player_skin_id] = uid;
	            SetPlayerSkin(playerid, ClothSkin[uid][skin_model]);

			    if(ClothSkin[uid][skin_price] <= pInfo[playerid][player_money])
					format(str, sizeof str, "~g~$%d", ClothSkin[uid][skin_price]);
				else
					format(str, sizeof str, "~r~$%d", ClothSkin[uid][skin_price]);
	            GameTextForPlayer(playerid, str, 2000, 6);
	        }
	        else
	        {
	        	return 1;
	        }
		}
	}

	if(pInfo[playerid][player_access_changing] == true)
    {
		new Keys, ud, lr;
  		GetPlayerKeys(playerid, Keys, ud, lr);
        if(lr < 0 || lr > 0)
        {
            new action = lr < 0 ? 1 : -1,
				uid = pInfo[playerid][player_access_id],
				str[ 20 ];

            uid = uid + action < 0 ? MAX_ACCESS - 1: (uid + action >= MAX_ACCESS ? 0: uid + action);

            if(ClothAccess[uid][access_model] != 0)
            {
	            pInfo[playerid][player_access_id] = uid;
	            RemovePlayerAttachedObject(playerid, ATTACH_SLOT_VICTIM);
	            SetPlayerAttachedObject(playerid, ATTACH_SLOT_VICTIM, ClothAccess[uid][access_model], ClothAccess[uid][access_bone], ClothAccess[uid][access_pos][0], ClothAccess[uid][access_pos][1], ClothAccess[uid][access_pos][2], ClothAccess[uid][access_pos][3], ClothAccess[uid][access_pos][4],ClothAccess[uid][access_pos][5]);

			    if(ClothAccess[uid][access_price] <= pInfo[playerid][player_money])
					format(str, sizeof str, "~g~$%d", ClothAccess[uid][access_price]);
				else
					format(str, sizeof str, "~r~$%d", ClothAccess[uid][access_price]);
	            GameTextForPlayer(playerid, str, 2000, 6);
	        }
	        else
	        {
	        	return 1;
	        }
		}
	}

	if(pInfo[playerid][player_training] == true)
	{
		new Keys,ud,lr;
    	GetPlayerKeys(playerid,Keys,ud,lr);

    	if(ud == KEY_UP)
    	{
    		if(pInfo[playerid][player_can_train] == 1)
    		{
    			UseGymDumb(playerid);
    		}
    	}
    	else if(ud == KEY_DOWN)
    	{
    		if(pInfo[playerid][player_can_train] == 2)
    		{
				LeaveDumb(playerid);
    		}
    	}
	}

	// Gaszenie pozaru
	if(GetPlayerWeapon(playerid) == 42)
	{
		new newkeys,l,u;
		GetPlayerKeys(playerid, newkeys, l, u);
		if(HOLDING(KEY_FIRE))
		{
			new Float:pos[3];
			foreach(new fsid : FireSources)	
			{
				GetDynamicObjectPos(FireSource[fsid][fs_object], pos[0], pos[1], pos[2]);
				if(!IsPlayerInRangeOfPoint(playerid, 4, pos[0], pos[1], pos[2])) continue;
				
				if(PlayerFaces(playerid, pos[0], pos[1], pos[2], 3.0))
				{
					if(FireSource[fsid][fs_health]>0)
					{
						new str[10];
						FireSource[fsid][fs_health] -= 0.1;
						format(str, sizeof(str), "%.2f%%", FireSource[fsid][fs_health]);
						UpdateDynamic3DTextLabelText(FireSource[fsid][fs_label], 0xF07800FF, str);
					}
					else
					{
						StopFireSource(fsid);
					}
				}
			}
		}
	}

	// Malowanie furki
	if(GetPlayerWeapon(playerid) == 41)
	{
		new newkeys,l,u;
		GetPlayerKeys(playerid, newkeys, l, u);
		if(HOLDING(KEY_FIRE))
		{
			pInfo[playerid][player_can_spray] = true;
		}
	}

	if( pInfo[playerid][player_logged] )
	{
		new wid = GetPlayerWeapon(playerid);
		if( pInfo[playerid][player_held_weapon] != wid )
		{
			OnPlayerWeaponChange(playerid, wid, pInfo[playerid][player_held_weapon]);
			pInfo[playerid][player_held_weapon] = wid;
		}
	
		if( pInfo[playerid][player_afk] )
		{
			RemovePlayerStatus(playerid, PLAYER_STATUS_AFK);
			
			pInfo[playerid][player_afk_time] += gettime() - pInfo[playerid][player_last_activity];
			
			if( GetPlayerDutySlot(playerid) > -1 ) pInfo[playerid][player_onduty_afk] += gettime() - pInfo[playerid][player_last_activity]; 
			if( pInfo[playerid][player_admin_duty] ) pInfo[playerid][player_admin_duty_afk_time] += gettime() - pInfo[playerid][player_last_activity];
			
			pInfo[playerid][player_afk] = false;
		}
		
		pInfo[playerid][player_last_activity] = gettime();
	}
	return 1;
}

public OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart)
{
	if(pGlobal[playerid][glo_dmg]) return 0;
	if(pGlobal[playerid][glo_score] < 10)
	{
		pInfo[playerid][player_hits] ++;
		SendClientMessage(playerid, COLOR_GOLD, "You are new player, you can't fight with other players until you get 1 hour online. Next punches will be punished by system.");
		if(pInfo[playerid][player_hits] > 4)
		{
			AdminJail(playerid, -1, "Deathmatch", 30);
		}
		return 1;
	}
	return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float: amount, weaponid, bodypart)
{
	if( IsPlayerNPC(playerid) ) return 0;
	if( !pInfo[playerid][player_logged] ) return 0;
	if( pInfo[playerid][player_bw] > 0 ) return 0;
	if( pGlobal[issuerid][glo_dmg]) return 0;
	
	pInfo[playerid][player_taken_damage] = gettime();

	UpdatePlayerLabel(playerid);

	EncountDamage(playerid, amount, bodypart, weaponid);

	if(issuerid != INVALID_PLAYER_ID)
	{
	  	if(pInfo[playerid][player_parachute] == 0 && GetPlayerWeapon(issuerid) != 0 && GetPlayerWeapon(issuerid) != GetPVarInt(issuerid, "weaping") || pInfo[playerid][player_parachute] == 0 && GetPVarInt(issuerid, "weaping") == 0 && GetPlayerWeapon(issuerid) != 0)
	    {
	    	if(GetPlayerWeaponAmmo(issuerid, weaponid)==0) 
	    	{
	    		SetPVarInt(issuerid, "weaping", 0);
				SetPVarInt(issuerid, "taser", 0);
				return 0; 
	    	}
	    	new String[64];
	    	format(String, sizeof(String), "Invalid weapon damage (w: %d)", GetPlayerWeapon(issuerid));
	    	KickAc(issuerid, -1, String);
	    	return 0;
	    }

		if(GetPlayerVehicleSeat(issuerid) == 1 || GetPlayerVehicleSeat(issuerid) == 2 || GetPlayerVehicleSeat(issuerid) == 3)
		{
			new wslots = GetWeaponSlot(weaponid);
			if(wslots != -1)
			{
				if(issuerid != INVALID_PLAYER_ID)
				{
					if(pWeapon[issuerid][wslots][pw_itemid] == -1 )
					{
						new String[64];
			    		format(String, sizeof(String), "No item DB (w: %d, seat: %d)", GetPlayerWeapon(issuerid), GetPlayerVehicleSeat(issuerid));
						KickAc(issuerid, -1, String);
						return 0;
					}
				}
			}
		}
	}

	new Float:HP;
	GetPlayerHealth(playerid, HP);
	if(amount+3 >= HP)
	{
		for(new i;i<13;i++)
		{
			if( pWeapon[playerid][i][pw_itemid] > -1 ) Item_Use(pWeapon[playerid][i][pw_itemid], playerid);
		}
		
		if( pInfo[playerid][player_bw] == 0 )
		{
			pInfo[playerid][player_bw] = 300;
			pInfo[playerid][player_bw_end_time] = pInfo[playerid][player_bw] + gettime();  
			SetPVarInt(playerid, "AnimHitPlayerGun", 0);
		}
		
		new
			Float:x,
			Float:y,
			Float:z,
			Float:a;
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);
				
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_bw`=%d, `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d WHERE `char_uid`=%d", pInfo[playerid][player_bw], x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), pInfo[playerid][player_id]));
		
		ApplyAnimation(playerid,"CRACK","crckdeth3",4.1,0,0,0,1,0);
		pInfo[playerid][player_quit_pos][0] = x;
		pInfo[playerid][player_quit_pos][1] = y;
		pInfo[playerid][player_quit_pos][2] = z;
		pInfo[playerid][player_quit_pos][3] = a;
		pInfo[playerid][player_quit_vw] = GetPlayerVirtualWorld(playerid);
		pInfo[playerid][player_quit_int] = GetPlayerInterior(playerid);
		pInfo[playerid][player_health] = 5.0;
		pInfo[playerid][player_death] = weaponid;

		if(issuerid != INVALID_PLAYER_ID)
		{
			pInfo[playerid][player_killer] = issuerid;
		}

		SetPlayerCameraPos(playerid, pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2] + 6.0);
		SetPlayerCameraLookAt(playerid, pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2]);
		SetPlayerHealth(playerid, 5);
		TogglePlayerControllable(playerid, false);
		AddPlayerStatus(playerid, PLAYER_STATUS_BW);
		SetPlayerChatBubble(playerid, sprintf("((To get more informations about player damages, use /damages %d.))", playerid), COLOR_LIGHTER_RED, 7.0, 300000);
		defer ApplyAnim[2000](playerid, ANIM_TYPE_BW);
		return 0;
	}

	if(GetPVarInt(issuerid, "taser") == 1)
	{
		ApplyAnimation(playerid,"CRACK","crckdeth2", 4.1, 0, 1, 1, 1, 0);
		defer AnimHitPlayer[15000](playerid);
	}

	if( pInfo[playerid][player_armour] > 0 && GetPlayerWeapon(issuerid) !=0 )
	{
		new armor = GetPlayerUsedItem(playerid, ITEM_TYPE_ARMOUR);
		new Float:Armour;
		GetPlayerArmour(playerid, Armour);

		switch(bodypart)
		{
			case BODY_PART_TORSO:
			{
				PlayerItem[playerid][armor][player_item_value1] -= 25;
				SetPlayerArmour(playerid, floatround(Armour) - 25);
			}
			case BODY_PART_GROIN:
			{
				PlayerItem[playerid][armor][player_item_value1] -= 30;
				SetPlayerArmour(playerid, floatround(Armour) - 30);
			}
			case BODY_PART_LEFT_ARM:
			{
				PlayerItem[playerid][armor][player_item_value1] -= 35;
				SetPlayerArmour(playerid, floatround(Armour) - 35);
			}
			case BODY_PART_RIGHT_ARM:
			{
				PlayerItem[playerid][armor][player_item_value1] -= 35;
				SetPlayerArmour(playerid, floatround(Armour) - 35);
			}
			case BODY_PART_HEAD:
			{
				PlayerItem[playerid][armor][player_item_value1] -= 25;
				SetPlayerArmour(playerid, floatround(Armour) - 25);
			}
			case BODY_PART_LEFT_LEG:
			{
				PlayerItem[playerid][armor][player_item_value1] -= 40;
				SetPlayerArmour(playerid, floatround(Armour) - 40);
			}
			case BODY_PART_RIGHT_LEG:
			{
				PlayerItem[playerid][armor][player_item_value1] -= 40;
				SetPlayerArmour(playerid, floatround(Armour) - 40);
			}
		}

		if(PlayerItem[playerid][armor][player_item_value1] < 1)
		{
			Item_Use(armor, playerid);
		}

		pInfo[playerid][player_health] += amount;
		SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
	}
	
	if( (pInfo[playerid][player_health] - amount) <= 0.0 )
	{
		if( issuerid != INVALID_PLAYER_ID )
		{
			pInfo[playerid][player_bw] = 60 * 5;
		}
		else pInfo[playerid][player_bw] = 60 * 2;
		
		pInfo[playerid][player_bw_end_time] = pInfo[playerid][player_bw] + gettime();  
		
		SetPlayerHealth(playerid, 5);
	}
	else SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health] - amount));
	
	// Animacja postrza³u 	
	if(pInfo[playerid][player_bw] == 0 && amount > 5.0 && issuerid != INVALID_PLAYER_ID && GetPlayerWeapon(issuerid) != 0 && pInfo[playerid][player_health] < 50 && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
	{
		if(GetPVarInt(playerid, "AnimHitPlayerGun") == 1) return 1;
		SetPVarInt(playerid, "AnimHitPlayerGun", 1);
		defer AnimHitPlayer[15000](playerid);

		switch(bodypart)
		{
			case BODY_PART_TORSO:
			{
				ApplyAnimation(playerid, "PED", "KO_shot_stom", 4.1,0,0,0,1,0);
			}
			case BODY_PART_GROIN:
			{
				ApplyAnimation(playerid, "PED", "KO_shot_stom", 4.1,0,0,0,1,0);
			}
			case BODY_PART_LEFT_ARM:
			{
				ApplyAnimation(playerid, "PED", "KO_shot_stom", 4.1,0,0,0,1,0);
			}
			case BODY_PART_RIGHT_ARM:
			{
				ApplyAnimation(playerid, "PED", "KO_shot_stom", 4.1,0,0,0,1,0);
			}
			case BODY_PART_HEAD:
			{
				if(weaponid == 30 || weaponid == 31 || weaponid == 34 || weaponid == 33)
				{
					SetPlayerHealth(playerid, 0);
					return 0;
				}

				ApplyAnimation(playerid, "PED", "KO_shot_face",4.1,0,0,0,1,0);
			}
			case BODY_PART_LEFT_LEG:
			{
				ApplyAnimation(playerid, "CRACK","crckdeth2", 4.1,0,0,0,1,0);
			}
			case BODY_PART_RIGHT_LEG:
			{
				ApplyAnimation(playerid, "CRACK","crckdeth2", 4.1,0,0,0,1,0);
			}
		}
	}
	return 0;
}

public OnPlayerDeath(playerid, killerid, reason)
{	
	RemovePlayerFromVehicle(playerid);
	pInfo[playerid][player_last_skin] = GetPlayerSkin(playerid);

	for(new i;i<13;i++)
	{
		if( pWeapon[playerid][i][pw_itemid] > -1 ) Item_Use(pWeapon[playerid][i][pw_itemid], playerid);
	}
	
	if( pInfo[playerid][player_bw] == 0 )
	{
		SetPVarInt(playerid, "AnimHitPlayerGun", 0);
		pInfo[playerid][player_bw] = 300;
		pInfo[playerid][player_bw_end_time] = pInfo[playerid][player_bw] + gettime();
		TogglePlayerControllable(playerid, 0);
	}
	
	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
			
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_bw`=%d, `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d WHERE `char_uid`=%d", pInfo[playerid][player_bw], x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), pInfo[playerid][player_id]));
	
	pInfo[playerid][player_quit_pos][0] = x;
	pInfo[playerid][player_quit_pos][1] = y;
	pInfo[playerid][player_quit_pos][2] = z;
	pInfo[playerid][player_quit_pos][3] = a;
	pInfo[playerid][player_quit_vw] = GetPlayerVirtualWorld(playerid);
	pInfo[playerid][player_quit_int] = GetPlayerInterior(playerid);
	pInfo[playerid][player_health] = 5.0;
	pInfo[playerid][player_death] = reason;
	pInfo[playerid][player_killer] = killerid;
	SetPlayerChatBubble(playerid, sprintf("((To get more information about player damage, use /damages %d.))", playerid), COLOR_LIGHTER_RED, 7.0, 300000);

	scrp_SpawnPlayer(playerid);
	return 1;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
	if(pInfo[playerid][player_admin_duty] == true)
	{
		SetPlayerPosFindZ(playerid, fX, fY, fZ);
	}
    return 1;
}

public OnPlayerClickTextDraw(playerid, Text:clickedid)
{
	if( pInfo[playerid][player_group_list_showed] )
	{
		HideGroupsList(playerid);
	}
	else if( pOffer[playerid][offer_type] > 0 && !pOffer[playerid][offer_accepted] )
	{
		OnPlayerOfferResponse(playerid, 0);
	}
	
    return 1;
}

public OnPlayerPickUpDynamicPickup(playerid, pickupid)
{
	ShowPlayerDoorTextdraw(playerid, pickupid);

	new d_id = pickupid;

	if(Door[d_id][door_rentable] == 1)
	{
		SendPlayerInformation(playerid, sprintf("~w~House for rent.~n~Price: ~p~$%d~w~~n~/~p~door rent", Door[d_id][door_rent]), 4000);
	}

	if(Door[d_id][door_buyable] == 1)
	{
		SendPlayerInformation(playerid, sprintf("~w~House for sale.~n~Price: ~p~$%d~w~~n~/~p~door buy", Door[d_id][door_price]), 4000);
	}

	return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
	if(pickupid == job_pickup)
	{
		if( pInfo[playerid][player_job] == 0 )
		{
			DynamicGui_Init(playerid);
			
			DynamicGui_AddRow(playerid, WORK_TYPE_LUMBERJACK);
			DynamicGui_AddRow(playerid, WORK_TYPE_FISHER);
			DynamicGui_AddRow(playerid, WORK_TYPE_TRUCKER);
			
			ShowPlayerDialog(playerid, DIALOG_WORKS, DIALOG_STYLE_TABLIST_HEADERS, ""guiopis"Available jobs:", "Job\tRequirements\tLocalization\nLumberjack\tDriver license\tDillimore\nFisher\tnone\tEast Beach LS\nTruck driver\tDriver license\tPlace", "Choose", "Close");
		}
		else
		{
			SendClientMessage(playerid, 0xD8D8D8FF, "Tip: Sorry, you already have a job. Use /job leave to leave your current job.");
		}
	}
	return 1;
}

public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
	if( pInfo[playerid][player_group_list_showed] )
	{
		for(new i=0;i<5;i++)
		{
			if( playertextid == GroupsListStaticButtons[i][0] ) cmd_g(playerid, sprintf("%d info", i+1));
			else if( playertextid == GroupsListStaticButtons[i][1] ) cmd_g(playerid, sprintf("%d vehicles", i+1));
			else if( playertextid == GroupsListStaticButtons[i][2] ) cmd_g(playerid, sprintf("%d duty", i+1));
			else if( playertextid == GroupsListStaticButtons[i][3] ) cmd_g(playerid, sprintf("%d magazine", i+1));
			else if( playertextid == GroupsListStaticButtons[i][4] ) cmd_g(playerid, sprintf("%d online", i+1));
		}
	
		HideGroupsList(playerid);
	}
	else if( pOffer[playerid][offer_type] > 0 )
	{
		if( playertextid == OfferTD[4] ) OnPlayerOfferResponse(playerid, 1);
		else if( playertextid == OfferTD[5] ) OnPlayerOfferResponse(playerid, 0);
	}
    return 1;
}

public OnPlayerSelectDynamicObject(playerid, objectid, modelid, Float:x, Float:y, Float:z)
{
	pInfo[playerid][player_edited_object_no_action] = true;
	if( !CanPlayerEditObject(playerid, objectid) )
	{
		SendClientMessage(playerid, COLOR_GREY, "You don't have access to edit this object.");
		EditDynamicObject(playerid, objectid);
		CancelEdit(playerid);
		return 1;
	}
	if( IsObjectEdited(objectid) ) return SendClientMessage(playerid, COLOR_GREY, "This object is already used by someone else."), EditDynamicObject(playerid, objectid), CancelEdit(playerid);
	pInfo[playerid][player_edited_object_no_action] = false;
	
	EditDynamicObject(playerid, objectid);
	Object[objectid][object_is_edited] = true;
	pInfo[playerid][player_edited_object] = objectid;
	
	GetDynamicObjectPos(objectid, pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]);
	GetDynamicObjectRot(objectid, pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5]);
	
	Object[objectid][object_pos][0] = pInfo[playerid][player_edited_object_pos][0];
	Object[objectid][object_pos][1] = pInfo[playerid][player_edited_object_pos][1];
	Object[objectid][object_pos][2] = pInfo[playerid][player_edited_object_pos][2];
	Object[objectid][object_pos][3] = pInfo[playerid][player_edited_object_pos][3];
	Object[objectid][object_pos][4] = pInfo[playerid][player_edited_object_pos][4];
	Object[objectid][object_pos][5] = pInfo[playerid][player_edited_object_pos][5];
	
	UpdateObjectInfoTextdraw(playerid, objectid);
	TextDrawShowForPlayer(playerid, Dashboard[playerid]);
    return 1;
}

public OnPlayerEditDynamicObject(playerid, objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
	if( !IsValidDynamicObject(objectid) ) return 1;
	if( Object[objectid][object_uid] == 0 && !pInfo[playerid][player_esel_edited_label]) return 1;
	
	if( pInfo[playerid][player_edited_object_no_action] )
	{
		pInfo[playerid][player_edited_object_no_action] = false;
		return 1;
	}
	
	if( objectid == pInfo[playerid][player_esel_edited_object] && pInfo[playerid][player_esel_edited_label] > 0 )
	{
		if( response == EDIT_RESPONSE_FINAL )
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_3dlabels` SET `label_posx` = %f, `label_posy` = %f, `label_posz` = %f WHERE `label_uid` = %d", x, y, z, pInfo[playerid][player_esel_edited_label]));
			
			new l_id = LoadLabel(sprintf("WHERE `label_uid` = %d", pInfo[playerid][player_esel_edited_label]), true);
			
			SendGuiInformation(playerid, ""guiopis"Alert", sprintf("You have changed 3d label position [UID: %d, ID: %d].", Label[Text3D:l_id][label_uid], l_id));

		}
		
		if( response == EDIT_RESPONSE_CANCEL )
		{
			SendGuiInformation(playerid, ""guiopis"Alert", "Edition canceled. Label is going back to original place.");
			
			LoadLabel(sprintf("WHERE `label_uid` = %d", pInfo[playerid][player_esel_edited_label]));
		}
		
		if( response == EDIT_RESPONSE_CANCEL || response == EDIT_RESPONSE_FINAL )
		{
			DestroyDynamicObject(objectid);
			
			pInfo[playerid][player_esel_edited_label] = 0;
			pInfo[playerid][player_esel_edited_object] = -1;
			
			SendPlayerInformation(playerid, "", 0);
			
			TextDrawHideForPlayer(playerid, Dashboard[playerid]);
		}
		return 1;
	}
	
	if( response == EDIT_RESPONSE_FINAL || response == EDIT_RESPONSE_CANCEL )
	{
		new o_id = pInfo[playerid][player_edited_object];
		TextDrawHideForPlayer(playerid, Dashboard[playerid]);
		if(o_id == -1) return 1;
		Object[o_id][object_is_edited] = false;
		pInfo[playerid][player_edited_object] = -1;
	}
	
	if( response == EDIT_RESPONSE_CANCEL )
	{
		SetDynamicObjectPos(objectid, pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]);
		SetDynamicObjectRot(objectid, pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5]);
		
		if(Object[objectid][object_gate] == 0)
		{
			new str[400];
			strcat(str, sprintf("UPDATE `ipb_objects` SET `object_posx` = %f, `object_posy` = %f, `object_posz` = %f,", pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]));
			strcat(str, sprintf(" `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5], Object[objectid][object_uid]));
			mysql_query(mySQLconnection, str);
		}
	}
	
	if( response == EDIT_RESPONSE_FINAL )
	{		
		if( Object[objectid][object_owner_type] == OBJECT_OWNER_TYPE_AREA )
		{
			if( !IsPointInDynamicArea(GetAreaByUid(Object[objectid][object_owner]), x, y, z) )
			{
				pInfo[playerid][player_edited_object] = -1;
				Object[objectid][object_is_edited] = false;
				
				SetDynamicObjectPos(objectid, pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]);
				SetDynamicObjectRot(objectid, pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5]);
				
				SendGuiInformation(playerid, ""guiopis"Alert", "Object is out of area borders.\nIts going back to the original place.");
				
				return 1;
			}
		}
	
		SetDynamicObjectPos(objectid, x, y, z);
		SetDynamicObjectRot(objectid, rx, ry, rz);

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_objects` SET `object_posx` = %f, `object_posy` = %f, `object_posz` = %f, `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", x, y, z, rx, ry, rz, Object[objectid][object_uid]));
		
		new uid = Object[objectid][object_uid];
		DeleteObject(objectid, false);
		
		LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
		RefreshPlayer(playerid);
	}
	else if( response == EDIT_RESPONSE_UPDATE )
	{
		Object[objectid][object_pos][0] = x;
		Object[objectid][object_pos][1] = y;
		Object[objectid][object_pos][2] = z;
		Object[objectid][object_pos][3] = rx;
		Object[objectid][object_pos][4] = ry;
		Object[objectid][object_pos][5] = rz;
		
		UpdateObjectInfoTextdraw(playerid, objectid);
	}
	return 1;
}

public OnPlayerText(playerid, text[])
{
	if( text[0] == '.' && text[1] != ' ' )
	{
		if(GetPVarInt(playerid, "AnimHitPlayerGun")==1)
		{
			if( strfind(text, "/me", true) == -1 && strcmp(text, "/admins") != 0 && strcmp(text, "/kill") != 0 && strcmp(text, "/a") != 0 && strfind(text, "/do", true) == -1 && strfind(text, "/w", true) == -1 && strfind(text, "/bw", true) == -1 && strfind(text, "/report", true) == -1 && strfind(text, "/b", true) == -1 && strfind(text, "/p", true) == -1 )
			{
				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "You can't use animations when you are shot.", "Okay", "");
				return 0;
			}
		}
		
		if(pInfo[playerid][player_bw] != 0)
		{
			SendGuiInformation(playerid, "Information", "You can't use animations when you are brutally wounded.");
			return 0;
		}

		new bool: found = false;
	    foreach(new anim_id: Anims)
	    {
			if(!isnull(AnimInfo[anim_id][aCommand]))
			{
	        	if(!strcmp(text, AnimInfo[anim_id][aCommand], true))
	        	{
	        	    if(AnimInfo[anim_id][aAction] == 0)
	        	    {
	        	    	ApplyAnimation(playerid, AnimInfo[anim_id][aLib], AnimInfo[anim_id][aName], AnimInfo[anim_id][aSpeed], AnimInfo[anim_id][aOpt1], AnimInfo[anim_id][aOpt2], AnimInfo[anim_id][aOpt3], AnimInfo[anim_id][aOpt4], AnimInfo[anim_id][aOpt5], 1);
					}
					else
					{
	                    SetPlayerSpecialAction(playerid, AnimInfo[anim_id][aAction]);
					}
					pInfo[playerid][player_looped_anim] = true;
					found = true;
	        	}
	        }
	    }
		if(!found) PlayerPlaySound(playerid, 1085, 0.0, 0.0, 0.0);
		
		return 0;
	}
	
	if( text[0] == '@' && strlen(text) > 3)
	{
		if(pGlobal[playerid][glo_ooc])
		{
			SendGuiInformation(playerid, "Information", "You have active OOC blockade.");
			return 0;
		}

		new input[128], slot;
		if( text[1] != ' ' && text[2] == ' ' )
		{
			sscanf(text, "'@'ds[128]", slot, input);
			if(isnull(input)) return 0;
			if( slot >= 1 && slot <= 5 )
			{
				SendGroupOOC(playerid, slot, input);
			}
		}
		return 0;
	}
	
	if( text[0] == '!' && strlen(text) > 3)
	{
		new input[128], slot;
		if( text[1] != ' ' && text[2] == ' ' )
		{
			sscanf(text, "'!'ds[128]", slot, input);
			if(isnull(input)) return 0;
			if( slot >= 1 && slot <= 5 )
			{
				SendGroupIC(playerid, slot, input);
			}
		}
		return 0;
	}

	if( pInfo[playerid][player_bw] > 0)
	{
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "You are brutally wounded, you have to wait.\nYou can only use: /kill, /me, /do, /w, /p.", "OK", "");
		return 0;
	}
	
	if( !strcmp(text, ":D", true) )
	{
		cmd_ame(playerid, "is laughing.");
		return 0;
	}

	if( !strcmp(text, "XD", true) )
	{
		cmd_ame(playerid, "is laughing.");
		return 0;
	}

	if( !strcmp(text, ":O", true) )
	{
		cmd_ame(playerid, "is shocked.");
		return 0;
	}
	
	if( !strcmp(text, ":)", true) )
	{
		cmd_ame(playerid, "is smiling.");
		return 0;
	}

	if( !strcmp(text, ":(", true) )
	{
		cmd_ame(playerid, "is sad.");
		return 0;
	}

	if( !strcmp(text, ":/", true) )
	{
		cmd_ame(playerid, "is unhappy.");
		return 0;
	} 

	if( !strcmp(text, ":P", true) )
	{
		cmd_ame(playerid, "shows his tounge.");
		return 0;
	}

	if( !strcmp(text, ":*", true) )
	{
		cmd_ame(playerid, "is sending a kiss.");
		return 0;
	}	
	
	if( pInfo[playerid][player_phone_call_started] )
	{
		ProxMessage(playerid, text, PROX_PHONE);	
		return 0;
	}

	if( pInfo[playerid][player_interview] > -1 )
	{
		new input[128];
		sscanf(text, "s[128]", input);
		format(input, sizeof(input), "~r~~h~~h~Weazel~w~ (~y~LIVE~w~ - %s): %s", pInfo[playerid][player_name], text);
		TextDrawSetString(TextDrawSanNews, input);
		return 0;
	}
	
	if(pGlobal[playerid][glo_ooc])
	{
		if( strfind(text, "((", true) != -1 && strfind(text, "))", true) != -1 )
		{
			AdminJail(playerid, -1, "Trying to bypass OOC blockade", 20);
			return 0;
		}
	}

	ProxMessage(playerid, text, PROX_LOCAL);	

	return 0;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(pInfo[playerid][player_race_phase] == 1)
	{
	 	if(newkeys & 4)
	  	{
			if(pInfo[playerid][player_race_point] < MAX_RACE_CP - 1)
			{
			    new vehicleid = GetPlayerVehicleID(playerid);
   				new checkpoint = pInfo[playerid][player_race_point], string[256];
       			GetVehiclePos(vehicleid, RaceCheckpoint[checkpoint][0], RaceCheckpoint[checkpoint][1], RaceCheckpoint[checkpoint][2]);

				GameTextForPlayer(playerid, "~n~~n~~n~~n~~n~~n~~n~~w~Checkpoint ~y~added", 3000, 3);
				pInfo[playerid][player_race_point] ++;

				format(string, sizeof(string), "~y~Race ~w~creator.~w~~n~~n~~y~~k~~VEHICLE_FIREWEAPON~ ~w~- setting checkpoint~n~~y~PPM ~w~- setting finish line~n~~n~Checkpoints: ~y~%d/%d", pInfo[playerid][player_race_point], MAX_RACE_CP);

				TextDrawSetString(Tutorial[playerid], string);
				TextDrawShowForPlayer(playerid, Tutorial[playerid]);
			}
			else
			{
   				GameTextForPlayer(playerid, "~n~~n~~n~~n~~n~~n~~n~~r~Checkpoint limits exceeded! Set up finish line!", 3000, 3);
			}
   		}

		if(newkeys & 128)
  		{
  			if(pInfo[playerid][player_race_point] <= 2)
	    	{
      			GameTextForPlayer(playerid, "~n~~n~~n~~n~~n~~n~~n~~r~You have to add at least 3 checkpoints!", 3000, 3);
	        	return 1;
		    }
		    new vehicleid=GetPlayerVehicleID(playerid);
      		new checkpoint = pInfo[playerid][player_race_point];
        	GetVehiclePos(vehicleid, RaceCheckpoint[checkpoint][0], RaceCheckpoint[checkpoint][1], RaceCheckpoint[checkpoint][2]);

			GameTextForPlayer(playerid, "~n~~n~~n~~n~~n~~n~~n~~w~Finish line ~y~set", 3000, 3);
			pInfo[playerid][player_race_phase] = 2;

			pInfo[playerid][player_race_checkpoints] = pInfo[playerid][player_race_point];

			SendPlayerInformation(playerid, "You have set up ~y~finish line~w~. Now invite players by ~y~/race invite~w~.~n~~n~~y~/race start ~w~is starting race.", 6000);
		}
	}

	new vidd = GetPlayerVehicleID(playerid);

	// Rowerek
	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		new model = GetVehicleModel(vidd);
		if(model == 509 || model == 510 || model == 481)
  		{
  		    if(newkeys == 1 || newkeys == 9)
			{
				new a_id = pInfo[playerid][player_area];
				if(a_id < 1 || !AreaHasFlag(a_id, AREA_FLAG_BMX))
				{
					if(!PlayerHasFlag(playerid, PLAYER_FLAG_BMX))
					{
	                	ClearAnimations(playerid);
	                	SendPlayerInformation(playerid, "~w~You can't jump on bike in this ~y~area~w~.", 5000);
	                }
				}
			}
		}
	}

	//Okno informacyjne
	if(newkeys & KEY_NO)
	{
		TextDrawHideForPlayer(playerid, Tutorial[playerid]);
	}

	//Praca rybaka
	if(pInfo[playerid][player_working] == WORK_TYPE_FISHER)
	{
		if(newkeys & KEY_YES)
		{
			if(pInfo[playerid][player_job] == WORK_TYPE_FISHER )
			{
				if(pInfo[playerid][player_carry_fish])
				{
					new Float:x, Float:y, Float:z;
					GetActorPos(Fisher, x, y, z);

					if(IsPlayerInRangeOfPoint(playerid, 3.0, x, y, z))
					{
						new cash, type;

						new item = GetPlayerFish(playerid);
						if (item == -1 ) return SendGuiInformation(playerid, "Information", "You don't have any fishes in your inventory.");

						type = PlayerItem[playerid][item][player_item_value2];

						switch(type)
						{
							case FISH_TYPE_SALMON: cash = 25;
							case FISH_TYPE_RARE: cash = 85;
							case FISH_TYPE_GARFISH: cash = 15;
							case FISH_TYPE_COD: cash = 11;
							case FISH_TYPE_SPRAT: cash = 7;
						}

						pInfo[playerid][player_job_cash] += cash;
						SendClientMessage(playerid, COLOR_GOLD, sprintf("Added $%d to your job paycheck. You can pick up cash at bank. Current state: $%d/$350", cash, pInfo[playerid][player_job_cash]));
						RemovePlayerAttachedObject(playerid, 7);
						RemovePlayerAttachedObject(playerid, 8);
						SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);

						if(type == FISH_TYPE_RARE)
						{
							ActorProx(Fisher, "Richard Bait", "That fish is illegal! I can buy it, but watch out!", PROX_LOCAL);
							DeleteItem(item, true, playerid);
							pInfo[playerid][player_carry_fish] = false;
							return 1;
						}

						new randd = random(4);
						switch(randd)
						{
							case 0: ActorProx(Fisher, "Richard Bait", "Damn, that fish is huge.", PROX_LOCAL);
							case 1: ActorProx(Fisher, "Richard Bait", "Not bad. I heard there are more at east.", PROX_LOCAL);
							case 2: ActorProx(Fisher, "Richard Bait", "I need more, sushi bar just ordered 50 kilos.", PROX_LOCAL);
							case 3: ActorProx(Fisher, "Richard Bait", "That fish will be looking good on my plate.", PROX_LOCAL);
						}

						DeleteItem(item, true, playerid);
						pInfo[playerid][player_carry_fish] = false;
					}
				}
				else
				{
					new veh = GetNearestVehicle(playerid);
					if(veh > -1 && Vehicle[veh][vehicle_model] == 453)
					{
						new Float:xx, Float:yy, Float:zz;
						GetVehicleBoot(veh, xx, yy, zz);
						if(!IsPlayerInRangeOfPoint(playerid, 3.0, xx, yy, zz)) return 1;

						if(GetVehicleFishCount(veh) < 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: There are no fishes in this reefer.");

						Vehicle[veh][vehicle_fish_object] -= 1;

						SetPlayerSpecialAction(playerid, SPECIAL_ACTION_CARRY);
						SetPlayerAttachedObject(playerid, 7, 19630, 6, 0.024000, 0.052000, -0.199000);
						SetPlayerAttachedObject(playerid, 8, 1355, 6, -0.024000, 0.193000, -0.240999, -114.300041, 0.000000, 78.000000);
						pInfo[playerid][player_carry_fish] = true;
					}
				}
			}
		}

		if(newkeys & KEY_FIRE)
		{
			if(pInfo[playerid][player_job] == WORK_TYPE_FISHER )
			{
				if(GetPlayerVehicleID(playerid) != INVALID_VEHICLE_ID)
				{
					if(Vehicle[GetPlayerVehicleID(playerid)][vehicle_model] == 453)
					{
						new object_id = GetClosestObjectType(playerid, OBJECT_FISH);

						if(object_id != INVALID_OBJECT_ID)
						{
							if(GetVehicleFishCount(GetPlayerVehicleID(playerid)) >= 10)
							{
								SendClientMessage(playerid, COLOR_GREY, "Tip: You can't load more fishes. Go to the bay and try to sell it.");
								return 1;
							}

							if(Object[object_id][object_logs] < 1)
							{
								DeleteObject(object_id, false);
								return 1;
							}
							
							SendClientMessage(playerid, COLOR_GOLD, "Fishing in progress, please wait.");

							defer Fish_Get[5000](playerid, GetPlayerVehicleID(playerid));
							pInfo[playerid][player_fishing] = true;
							Object[object_id][object_logs]--;
						}
					}
				}
			}
		}
	}

	//Praca drwala
	if(pInfo[playerid][player_working] == WORK_TYPE_LUMBERJACK)
	{
		if(newkeys & KEY_YES)
		{
			if(pInfo[playerid][player_job] == WORK_TYPE_LUMBERJACK )
			{
				if(pInfo[playerid][player_carry_log])
				{
					new Float:x, Float:y, Float:z;
					GetActorPos(Lumberjack, x, y, z);

					if(IsPlayerInRangeOfPoint(playerid, 3.0, x, y, z))
					{
						new cash = 5 + random(10);
						pInfo[playerid][player_job_cash] += cash;
						SendClientMessage(playerid, COLOR_GOLD, sprintf("Added $%d to your job paycheck. You can pick it up in bank. Current state: $%d/$350", cash, pInfo[playerid][player_job_cash]));
						RemovePlayerAttachedObject(playerid, 7);
						SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
						pInfo[playerid][player_carry_log] = false;
					}
					else
					{
						new veh = GetNearestVehicle(playerid);
						if(veh > -1 && Vehicle[veh][vehicle_model] == 422)
						{
							new Float:xx, Float:yy, Float:zz;
							GetVehicleBoot(veh, xx, yy, zz);
							if(!IsPlayerInRangeOfPoint(playerid, 3.0, xx, yy, zz)) return 1;
							if(GetVehicleLogCount(veh) >= 10)
							{
								SendClientMessage(playerid, COLOR_GREY, "Tip: You can't load more wood.");
								RemovePlayerAttachedObject(playerid, 7);
								SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
								pInfo[playerid][player_carry_log] = false;
								return 1;
							}

							for(new i; i < 10; i++)
					    	{
					    	    if(!IsValidDynamicObject(Vehicle[veh][vehicle_log_object][i]))
					    	    {
					    	        Vehicle[veh][vehicle_log_object][i] = CreateDynamicObject(19793, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
					    			AttachDynamicObjectToVehicle(Vehicle[veh][vehicle_log_object][i], veh, LogAttachOffsets[i][0], LogAttachOffsets[i][1], LogAttachOffsets[i][2], 0.0, 0.0, LogAttachOffsets[i][3]);
					    			break;
					    	    }
					    	}

					    	RemovePlayerAttachedObject(playerid, 7);
							SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
							pInfo[playerid][player_carry_log] = false;
						}
					}
				}
				else
				{
					new object_id = GetClosestObjectType(playerid, OBJECT_TREE);

					if(object_id != INVALID_OBJECT_ID)
					{
						new Float:rx, Float:ry, Float:rz;
						GetDynamicObjectRot(object_id, rx, ry, rz);
						if(ry == -80.0)
						{
							if(Object[object_id][object_logs] < 1) return SendClientMessage(playerid, COLOR_GREY, "There is no more wood.");
							pInfo[playerid][player_carry_log] = true;
							SetPlayerSpecialAction(playerid, SPECIAL_ACTION_CARRY);
							SetPlayerAttachedObject(playerid, 7, 19793, 6, 0.077999, 0.043999, -0.170999, -13.799953, 79.70, 0.0);
							Object[object_id][object_logs]--;
						}
					}
					else
					{
						new veh = GetNearestVehicle(playerid);
						if(veh > -1)
						{
							new Float:x, Float:y, Float:z;
							GetVehicleBoot(veh, x, y, z);
							if(!IsPlayerInRangeOfPoint(playerid, 3.0, x, y, z)) return 1;
							if(GetVehicleLogCount(veh) < 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: There isn't any wood in this vehicle.");

							for(new i = (10 - 1); i >= 0; i--)
					    	{
					    	    if(IsValidDynamicObject(Vehicle[veh][vehicle_log_object][i]))
					    	    {
					    	        DestroyDynamicObject(Vehicle[veh][vehicle_log_object][i]);
					    	        Vehicle[veh][vehicle_log_object][i] = -1;
					    			break;
					    	    }
					    	}

							SetPlayerSpecialAction(playerid, SPECIAL_ACTION_CARRY);
							SetPlayerAttachedObject(playerid, 7, 19793, 6, 0.077999, 0.043999, -0.170999, -13.799953, 79.70, 0.0);
							pInfo[playerid][player_carry_log] = true;
						}
					}
				}
			}
		}

		if(newkeys & KEY_FIRE)
		{
			if(pInfo[playerid][player_job] == WORK_TYPE_LUMBERJACK )
			{
				if(IsPlayerAttachedObjectSlotUsed(playerid, 8) && !pInfo[playerid][player_cutting_tree])
				{
					new object_id = GetClosestObjectType(playerid, OBJECT_TREE);
					if(object_id != INVALID_OBJECT_ID)
					{
						new Float:rx, Float:ry, Float:rz;
						GetDynamicObjectRot(object_id, rx, ry, rz);

						if(ry == 0)
						{
							new Float:x, Float:y, Float:z;
							GetDynamicObjectPos(object_id, x, y, z);
							SetPlayerLookAt(playerid, x, y);
							defer Tree_Cut[5000](playerid, object_id);
							ApplyAnimation(playerid, "CHAINSAW", "WEAPON_csaw", 4.1, 1, 0, 0, 1, 0, 1);
							pInfo[playerid][player_cutting_tree] = true;
						}
					}
				}
			}
		}
	}

	if(pInfo[playerid][player_golf])
	{
 		if(newkeys & KEY_NO) // N - mocne uderzenie
		{
			if(GetPlayerWeapon(playerid)==2)
			{
				new Float:x, Float:y, Float:z, Float:ang;
				GetObjectPos(ballid, x, y, z);
				if(IsPlayerInRangeOfPoint(playerid, 1.5, x, y, z))
				{
		    		#define BALL_SPEED (7.0) // Prêdkoœæ pi³ki
		    		GetPlayerFacingAngle(playerid, ang);
		    		x = BALL_SPEED * floatsin(-ang, degrees);
		    		y = BALL_SPEED * floatcos(-ang, degrees);
		    		PHY_SetObjectVelocity(ballid, x, y);
		    		PlayerPlaySound(playerid,1130,0.0,0.0,0.0);
					ApplyAnimation(playerid,"BASEBALL","Bat_3",4.1,0,1,1,0,0);
				}
			}
		}
		if(newkeys & KEY_YES) // Y - delikatne uderzenie
		{
			if(GetPlayerWeapon(playerid)==2)
			{
				new Float:x, Float:y, Float:z, Float:ang;
				GetObjectPos(ballid, x, y, z);
				if(IsPlayerInRangeOfPoint(playerid, 1.5, x, y, z))
				{
		    		#define BALLL_SPEED (3.0) // Prêdkoœæ pi³ki
		    		GetPlayerFacingAngle(playerid, ang);
		    		x = BALLL_SPEED * floatsin(-ang, degrees);
		    		y = BALLL_SPEED * floatcos(-ang, degrees);
		    		PHY_SetObjectVelocity(ballid, x, y);
		    		PlayerPlaySound(playerid,1130,0.0,0.0,0.0);
					ApplyAnimation(playerid,"BASEBALL","Bat_3",4.1,0,1,1,0,0);
				}
			}
		}
	}

	//Interakcja ze stref¹
	if(newkeys & KEY_YES)
	{
		new a_id = pInfo[playerid][player_area];
		if(a_id > 0)
		{
			if(AreaHasFlag(a_id, AREA_FLAG_DRIVE))
			{
				if(!IsAnyGastroOpen())
				{
					ShowPlayerDialog(playerid, DIALOG_CBELL, DIALOG_STYLE_TABLIST_HEADERS, "Drive Thru", "Menu\tPrice\nHuge meal\t$30\nBig Meal\t$20\nSmall meal\t$15\nSalad meal\t$21", "Choose", "Close");
				}
			}
			else if(AreaHasFlag(a_id, AREA_FLAG_SERWIS))
			{
				if(!IsAnyWorkshopOpen() && GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
				{
					new price = 1250 - floatround(Vehicle[GetPlayerVehicleID(playerid)][vehicle_health], floatround_ceil);
					new str[64];
					format(str, sizeof(str), "Do you really want to fix your car for $%d?", price);

					pInfo[playerid][player_dialog_tmp2] = price;
					ShowPlayerDialog(playerid, DIALOG_AUTO_FIX, DIALOG_STYLE_MSGBOX, "Automatic fix", str, "Yes", "No");
				}
			}
			else if(AreaHasFlag(a_id, AREA_FLAG_ZLOM))
			{
				if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
				{
					new vid = GetPlayerVehicleID(playerid);
					if(vid != INVALID_VEHICLE_ID)
					{
						if(CanPlayerUseVehicle(playerid, vid))
						{
							new rows, fields;
							mysql_query(mySQLconnection, sprintf("SELECT dealer_price FROM ipb_veh_dealer WHERE dealer_model = %d", Vehicle[vid][vehicle_model]));
							cache_get_data(rows, fields);

							if(rows)
							{
								new price = cache_get_row_int(0, 0);
								new allprice = price/3;

								new str[64];
								format(str, sizeof(str), "Do you realy want to leave your car on scrap yard? (for $%d)", allprice);

								pInfo[playerid][player_dialog_tmp2] = allprice;
								ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Scrap yard", str, "Yes", "No");
							}
						}
					}
				}
			}
		}	
	}

	//Spec
	if(pInfo[playerid][player_spec] != INVALID_PLAYER_ID)
 	{
  		if(newkeys == KEY_SPRINT) // spacja id+1
	  	{
	  		new id = pInfo[playerid][player_spec];
	  		return cmd_spec(playerid, sprintf("%d", Iter_Next(Player, id)));
	  	}
	  	else if(newkeys == KEY_WALK) // lalt id-1
	  	{
	  		new id = pInfo[playerid][player_spec];
	  		return cmd_spec(playerid, sprintf("%d", Iter_Prev(Player, id)));
	  	} 
	  	else if(newkeys == KEY_JUMP) // odswiezanie jesli wejdzie do intku, wsiadzie do auta
        {
            return cmd_spec(playerid, sprintf("%d", pInfo[playerid][player_spec]));
        }
 	}

 	// Malowanie furki
	if(GetPlayerWeapon(playerid) == 41)
	{
		if(RELEASED(KEY_FIRE))
		{
			pInfo[playerid][player_can_spray] = false;
		}
	}

 	//Animacja chodzenia
	if(pInfo[playerid][player_walking_anim])
	{
		if(newkeys & KEY_WALK )
		{
			if(GetPVarInt(playerid, "AnimHitPlayerGun")==1) return 1;
			ApplyAnimation(playerid, pInfo[playerid][player_walking_lib], pInfo[playerid][player_walking_name], 4.1, 1, 1, 1, 1, 1, 0);
            pInfo[playerid][player_looped_anim] = true;
		}
		else if(oldkeys & KEY_WALK)
		{
			if(GetPVarInt(playerid, "AnimHitPlayerGun")==1) return 1;
			ApplyAnimation(playerid, "CARRY", "crry_prtial", 4.1, 0, 0, 0, 0, 0);
			pInfo[playerid][player_looped_anim] = false;
		}
	}

	//Wy³¹czenie animacji
	if(newkeys & KEY_FIRE)
	{
		if(pInfo[playerid][player_looped_anim] == true)
		{
			pInfo[playerid][player_looped_anim] = false;
		}
	}

	//Victim
	if(newkeys & KEY_JUMP)
	{
	    if(pInfo[playerid][player_skin_changing])
	    {
			TogglePlayerControllable(playerid, true);
			pInfo[playerid][player_skin_changing] = false;
			GameTextForPlayer(playerid, "_", 0, 6);
			TextDrawHideForPlayer(playerid, Tutorial[playerid]);
			
			SetCameraBehindPlayer(playerid);
			SetPlayerSkin(playerid, pInfo[playerid][player_skin]);
			
			return 1;
		}

		if(pInfo[playerid][player_access_changing])
	    {
			TogglePlayerControllable(playerid, true);
			pInfo[playerid][player_access_changing] = false;
			GameTextForPlayer(playerid, "_", 0, 6);
			TextDrawHideForPlayer(playerid, Tutorial[playerid]);
			
			SetCameraBehindPlayer(playerid);
			RemovePlayerAttachedObject(playerid, ATTACH_SLOT_VICTIM);
			
			return 1;
		}
	}

    // Victim
    if(pInfo[playerid][player_skin_changing])
    {
    	if(newkeys & KEY_SECONDARY_ATTACK)
    	{
	        new skin_id = pInfo[playerid][player_skin_id];
	        if(pInfo[playerid][player_money] < ClothSkin[skin_id][skin_price])
	        {
				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "You don't have enough money.", "OK", "");
	            return 1;
	        }
	        GivePlayerMoney(playerid, -ClothSkin[skin_id][skin_price]);
	        
	        new skin_nam[40];
	        sscanf(ClothSkin[skin_id][skin_name], "s[40]", skin_nam);
	        Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_CLOTH, 2384, ClothSkin[skin_id][skin_model], 0, skin_nam);

			SetCameraBehindPlayer(playerid);
			SetPlayerSkin(playerid, pInfo[playerid][player_skin]);
			
			TogglePlayerControllable(playerid, true);
			pInfo[playerid][player_skin_changing] = false;
			
			TextDrawHideForPlayer(playerid, Tutorial[playerid]);

	       	SendGuiInformation(playerid, ""guiopis"Alert", "Good choice.\nItem has been added to your inventory.");
			return 1;
		}
    }

    if(pInfo[playerid][player_access_changing])
    {
    	if(newkeys & KEY_SECONDARY_ATTACK)
    	{
	        new access_id = pInfo[playerid][player_access_id];
	        if(pInfo[playerid][player_money] < ClothAccess[access_id][access_price])
	        {
				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "You don't have enough money.", "OK", "");
	            return 1;
	        }
	        GivePlayerMoney(playerid, -ClothAccess[access_id][access_price]);
	        
	        new access_nam[40];
	        sscanf(ClothAccess[access_id][access_name], "s[40]", access_nam);
	        Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_ATTACH, ClothAccess[access_id][access_model], ClothAccess[access_id][access_bone], 0, access_nam);

			SetCameraBehindPlayer(playerid);
			RemovePlayerAttachedObject(playerid, ATTACH_SLOT_VICTIM);
			
			TogglePlayerControllable(playerid, true);
			pInfo[playerid][player_access_changing] = false;
			
			TextDrawHideForPlayer(playerid, Tutorial[playerid]);

	       	SendGuiInformation(playerid, ""guiopis"Alert", "Good choice.\nItem has been added to your inventory.");
			return 1;
		}
    }

    //Silnik
	if( IsPlayerInAnyVehicle(playerid) )
	{
		new vid = GetPlayerVehicleID(playerid);
		if( !CanPlayerUseVehicle(playerid, vid) ) return 1;
		if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER ) return 1;

		if( PRESSED(KEY_ACTION)  )
		{
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
		else if( PRESSED(KEY_FIRE) )
		{
			if( Vehicle[vid][vehicle_lights] )
			{
				// Gaszenie swiatel
				Vehicle[vid][vehicle_lights] = false;

				UpdateVehicleVisuals(vid);
			}
			else
			{
				// Odpalanie swiatel
				Vehicle[vid][vehicle_lights] = true;

				UpdateVehicleVisuals(vid);
			}

			return 1;
		}
	}
	else
	{
		if( PRESSED(KEY_SECONDARY_ATTACK) || PRESSED(KEY_HANDBRAKE) )
		{
			if( pInfo[playerid][player_looped_anim] == true ) 
			{
				if(GetPVarInt(playerid, "AnimHitPlayerGun")==1) return 1;
				new skin = GetPlayerSkin(playerid);
				SetPlayerSkin(playerid, skin);
				TogglePlayerControllable(playerid, 0);
				TogglePlayerControllable(playerid, 1);
				pInfo[playerid][player_looped_anim] = false;
			}
		}
		if( PRESSED( KEY_SPRINT | KEY_WALK ) )
		{
			new vir = GetPlayerVirtualWorld(playerid);
			new d_id = -1;
			new ds_id = -1;

			foreach(new d : Doors)
			{
				if(vir == Door[d][door_vw] && IsPlayerInRangeOfPoint(playerid, 3.0,  Door[d][door_pos][0],  Door[d][door_pos][1], Door[d][door_pos][2]))
				{
					d_id = d;
				}
				else if(vir == Door[d][door_spawn_vw] && IsPlayerInRangeOfPoint(playerid, 3.0,  Door[d][door_spawn_pos][0],  Door[d][door_spawn_pos][1], Door[d][door_spawn_pos][2]))
				{
					ds_id = d;
				}
			}

			if( ds_id != -1 )
			{
				if( Door[ds_id][door_closed] ) return SendClientMessage(playerid, COLOR_GREY, "These doors are locked.");
				
				FreezePlayer(playerid, 2500);
				
				RP_PLUS_SetPlayerPos(playerid, Door[ds_id][door_pos][0], Door[ds_id][door_pos][1], Door[ds_id][door_pos][2]);
				SetPlayerFacingAngle(playerid, Door[ds_id][door_pos][3]+180.0);
				
				SetCameraBehindPlayer(playerid);
				
				SetPlayerVirtualWorld(playerid, Door[ds_id][door_vw]);
				SetPlayerInterior(playerid, Door[ds_id][door_int]);
				SetPlayerTime(playerid, WorldTime+2, 0);
				SetPlayerWeather(playerid, WorldWeather);

				new slot = GetPlayerDutySlot(playerid);

				if(slot != -1)
				{
					new grid = pInfo[playerid][player_duty_gid];
					if( GroupHasFlag(grid, GROUP_FLAG_DUTY) )
					{
						cmd_g(playerid, sprintf("%d duty", slot+1));
					}
				}
				return 1;
			}
			else if( d_id != -1 )
			{
				if( Door[d_id][door_destroyed])	return SendClientMessage(playerid, COLOR_GREY, "This bulding is destroyed.");
				if( Door[d_id][door_burned])	return SendClientMessage(playerid, COLOR_GREY, "This building is burned.");
				if( Door[d_id][door_closed] ) return SendClientMessage(playerid, COLOR_GREY, "These doors are locked.");
				
				if( Door[d_id][door_payment] > 0 )
				{
					if( Door[d_id][door_payment] > pInfo[playerid][player_money] ) return SendClientMessage(playerid, COLOR_GREY, "You don't have enough money.");
					
					new g_uid = Door[d_id][door_owner];
					new gid = GetGroupByUid(g_uid);
					if(gid == -1 ) return SendClientMessage(playerid, COLOR_GREY, "This doors are not assigned to a group, payment cannot be accepted.");
					GivePlayerMoney(playerid, -Door[d_id][door_payment]);
					GiveGroupMoney(gid, Door[d_id][door_payment]);
				}
				
				FreezePlayer(playerid, 2500);
				
				RP_PLUS_SetPlayerPos(playerid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
				SetPlayerFacingAngle(playerid, Door[d_id][door_spawn_pos][3]);
				
				SetCameraBehindPlayer(playerid);
				
				SetPlayerVirtualWorld(playerid, Door[d_id][door_spawn_vw]);
				SetPlayerInterior(playerid, Door[d_id][door_spawn_int]);
				SetPlayerTime(playerid, Door[d_id][door_time], 0);
				SetPlayerWeather(playerid, 0);
				return 1;
			}
			
		}
		if( pInfo[playerid][player_creating_area] )
		{
			if( PRESSED(KEY_HANDBRAKE) )
			{
				if( pInfo[playerid][player_carea_point1][0] == 0.0 && pInfo[playerid][player_carea_point1][1] == 0.0 && pInfo[playerid][player_carea_point1][2] == 0.0 )
				{
					GetPlayerPos(playerid, pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2]);
					
					pInfo[playerid][player_carea_label][0] = CreateDynamic3DTextLabel(sprintf("First point\n(%f, %f, %f)", pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2]), COLOR_LIGHTER_RED, pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
					
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "You've made first point.", "OK", "");
				}
				else if( pInfo[playerid][player_carea_point2][0] == 0.0 && pInfo[playerid][player_carea_point2][1] == 0.0 && pInfo[playerid][player_carea_point2][2] == 0.0 )
				{
					GetPlayerPos(playerid, pInfo[playerid][player_carea_point2][0], pInfo[playerid][player_carea_point2][1], pInfo[playerid][player_carea_point2][2]);
					
					pInfo[playerid][player_carea_label][1] = CreateDynamic3DTextLabel(sprintf("Second point\n(%f, %f, %f)", pInfo[playerid][player_carea_point2][0], pInfo[playerid][player_carea_point2][1], pInfo[playerid][player_carea_point2][2]), COLOR_LIGHTER_RED, pInfo[playerid][player_carea_point2][0], pInfo[playerid][player_carea_point2][1], pInfo[playerid][player_carea_point2][2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
					
					pInfo[playerid][player_carea_zone] = GangZoneCreate(Min(pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point2][0]), Min(pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point2][1]), Max(pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point2][0]), Max(pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point2][1]));
					GangZoneShowForPlayer(playerid, pInfo[playerid][player_carea_zone], 0xFF3C3C80);
									
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "You've made second point.", "OK", "");
				}
				else
				{
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "Points added, to delete last press SHIFT or type /area create again to finish process.", "OK", "");
				}
			}
			
			if( PRESSED(KEY_FIRE) )
			{
				if( pInfo[playerid][player_carea_point2][0] != 0.0 && pInfo[playerid][player_carea_point2][1] != 0.0 && pInfo[playerid][player_carea_point2][2] != 0.0 )
				{
					pInfo[playerid][player_carea_point2][0] = 0.0;
					pInfo[playerid][player_carea_point2][1] = 0.0;
					pInfo[playerid][player_carea_point2][2] = 0.0;
					
					if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
					
					GangZoneDestroy(pInfo[playerid][player_carea_zone]);
					
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "You've deleted second point.", "OK", "");
				}
				else if( pInfo[playerid][player_carea_point1][0] != 0.0 && pInfo[playerid][player_carea_point1][1] != 0.0 && pInfo[playerid][player_carea_point1][2] != 0.0 )
				{
					pInfo[playerid][player_carea_point1][0] = 0.0;
					pInfo[playerid][player_carea_point1][1] = 0.0;
					pInfo[playerid][player_carea_point1][2] = 0.0;
					
					if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
					
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "You've deleted first point.", "OK", "");
				}
			}
			
			if( PRESSED(KEY_WALK | KEY_SPRINT) )
			{
				pInfo[playerid][player_carea_point1][0] = 0.0;
				pInfo[playerid][player_carea_point1][1] = 0.0;
				pInfo[playerid][player_carea_point1][2] = 0.0;
				
				pInfo[playerid][player_carea_point2][0] = 0.0;
				pInfo[playerid][player_carea_point2][1] = 0.0;
				pInfo[playerid][player_carea_point2][2] = 0.0;
				
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
				
				GangZoneDestroy(pInfo[playerid][player_carea_zone]);
				
				pInfo[playerid][player_creating_area] = false;
				
				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "Area creations mode is now off.", "OK", "");
				SendPlayerInformation(playerid, "", 0);
			}
		}
	}
	return 1;
}

public OnPlayerCommandReceived(playerid, cmdtext[])
{
	if( strfind(cmdtext, "|", true) != -1)
	{
		SendGuiInformation(playerid, "Error", "Detected unacceptable symbols.");
		return 0;
	}

	if( !pInfo[playerid][player_logged] ) return 0;

	if( pInfo[playerid][player_bw] > 0 )
	{
		if( strfind(cmdtext, "/me", true) == -1 && strcmp(cmdtext, "/admins") != 0 && strcmp(cmdtext, "/kill") != 0 && strcmp(cmdtext, "/as") != 0 && strcmp(cmdtext, "/a") != 0 && strfind(cmdtext, "/do", true) == -1 && strfind(cmdtext, "/w", true) == -1 && strfind(cmdtext, "/bw", true) == -1 && strfind(cmdtext, "/report", true) == -1 && strfind(cmdtext, "/b", true) == -1)
		{
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "You are brutally wounded, you have to wait.\nYou can only use: /kill, /me, /do, /w, /p.", "Okay", "");
			return 0;
		}
	}
	
	return 1;
}

public OnPlayerCommandPerformed(playerid, cmdtext[], success)
{
	if( !success ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
	printf("[CMD] %s - %s", pInfo[playerid][player_name], cmdtext);
	return 1;
}

public OnPlayerEnterRaceCheckpoint(playerid)
{
	pInfo[playerid][player_race_point] ++;
	PlayerPlaySound(playerid, 1139, 0.0, 0.0, 0.0);
	new checkpoint = pInfo[playerid][player_race_point];

	if(checkpoint < pInfo[playerid][player_race_checkpoints])
	{
	    SetPlayerRaceCheckpoint(playerid, 0, RaceCheckpoint[checkpoint][0], RaceCheckpoint[checkpoint][1], RaceCheckpoint[checkpoint][2], RaceCheckpoint[checkpoint + 1][0], RaceCheckpoint[checkpoint + 1][1], RaceCheckpoint[checkpoint + 1][2], 5.0);
	}
	else
	{
		SetPlayerRaceCheckpoint(playerid, 1, RaceCheckpoint[checkpoint][0], RaceCheckpoint[checkpoint][1], RaceCheckpoint[checkpoint][2], 0.0, 0.0, 0.0, 5.0);
	}

	if(checkpoint > pInfo[playerid][player_race_checkpoints])
	{
	    new string[128];
	    format(string, sizeof(string), "~w~Race ended!~n~~y~Winner is ~g~%s", pInfo[playerid][player_name]);

		foreach(new p: Player)
	    {
	        if(pInfo[p][player_logged])
	        {
	            if(pInfo[p][player_race_phase] == 3)
	            {
	                GameTextForPlayer(p, string, 5000, 3);
	                DisablePlayerRaceCheckpoint(p);

	                pInfo[p][player_race_phase] = 0;
	                pInfo[p][player_race_point] = 0;

	                pInfo[p][player_race_checkpoints] = 0;
	            }
	        }
	    }
	}
	return 1;
}

public OnPlayerEnterCheckpoint(playerid)
{
	if(pInfo[playerid][player_truck] == 1)
	{
	    if(IsPlayerInAnyVehicle(playerid))
	    {
	        new carid = GetPlayerVehicleID(playerid);

	        if(IsTrailerAttachedToVehicle(carid))
	        {
		        if(Vehicle[carid][vehicle_model] == 403)
		        {
		        	new cash = random(80)+100;
		            PlayerPlaySound(playerid, 1056, 0.0, 0.0, 0.0);
		            SendGuiInformation(playerid, ""guiopis"Alert", sprintf("Package delivered. Payout -  $%d.\nGo back to your base to get another delivery.", cash));
		            GivePlayerMoney(playerid, 170);
		            DisablePlayerCheckpoint(playerid);
		            pInfo[playerid][player_truck]=0;
		            new trailerid = GetVehicleTrailer(carid);
		            SetVehicleToRespawn(trailerid);
		        }
		    }
	    }
	    return 1;
	}
	return 1;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if(pInfo[playerid][player_hours] < 1)
	{
		if(GetVehicleDriver(vehicleid) == INVALID_PLAYER_ID)
		{
			ClearAnimations(playerid);
			SendGuiInformation(playerid, "Information", "You can't enter vehicle without driver if you are under 1 hour online.");
			return 1;
		}
	}

	if(GetPVarInt(playerid, "AnimHitPlayerGun") == 1 )
	{
		pInfo[playerid][player_looped_anim] = true;
		GameTextForPlayer(playerid, "~r~You are shot~n~Cant enter vehicle.", 2000, 3);
		SetPVarInt(playerid, "AnimHitPlayerGun", 1);
		ApplyAnimation(playerid, "PED", "KO_shot_face", 4.1,0,0,0,1,0);
		return 1;
	}

	if( Vehicle[vehicleid][vehicle_locked] )
	{
		ClearAnimations(playerid, 1);
		GameTextForPlayer(playerid, "~w~Vehicle ~r~locked", 2500, 3);
		return 1;
	}

	/*if(pInfo[playerid][player_job] == WORK_TYPE_TRUCKER)
	{
		if( Vehicle[vehicleid][vehicle_model] == 403)
		{
			SendPlayerInformation(playerid, "~w~Doczep przyczepe do ciezarowki i wpisz /truck.", 6000);
		}
	}*/

	if( Vehicle[vehicleid][vehicle_destroyed] && !ispassenger )
	{
		ClearAnimations(playerid, 1);
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Information", "Vehicle is totally destroyed.", "OK", "");	
		return 1;
	}

	pInfo[playerid][player_entering_vehicle] = vehicleid;
	
	return 1;
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
    StopAudioStreamForPlayer(playerid);
    return 1;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	if( (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER) && (oldstate != PLAYER_STATE_DRIVER && oldstate != PLAYER_STATE_PASSENGER) )
	{
		if( pInfo[playerid][player_entering_vehicle] != GetPlayerVehicleID(playerid) )
		{
			KickAc(playerid, -1, "Vehicle hack");
		}
		else
		{
			pInfo[playerid][player_entering_vehicle] = -1;
			
			new vid = GetPlayerVehicleID(playerid);
			pInfo[playerid][player_occupied_vehicle] = vid;
			Vehicle[vid][vehicle_occupants] += 1;
			Vehicle[vid][vehicle_last_used] = gettime();
			pInfo[playerid][player_parachute] = 1;

			// Wylaczamy namierzanie
			if( pInfo[playerid][player_vehicle_target] == vid )
			{
				Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
				Streamer_UpdateEx(playerid, Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2]);

				pInfo[playerid][player_vehicle_target] = -1;
				SendGuiInformation(playerid, ""guiopis"Alert", "Vehicle tracking off.");
			}
			
			// Uruchamiamy stream
            if(Vehicle[vid][vehicle_streaming] == 1)
		    {
		    	PlayAudioStreamForPlayer(playerid, Vehicle[vid][vehicle_stream]);
		    }
		    
			if( newstate == PLAYER_STATE_DRIVER )
			{
				//Sprawdzamy czy nie ma blokady
				if(pGlobal[playerid][glo_veh])
			    {
		    		ClearAnimations(playerid);
		    		SendGuiInformation(playerid, "Information", "You have active car block.");
		    		return 1;
			    }

				// Sprawdzamy czy ma uprawnienia
				if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP && !CanPlayerUseVehicle(playerid, vid))
				{
					SendGuiInformation(playerid, ""guiopis"", "You don't have access to this vehicle.");
					ClearAnimations(playerid);
					return 1;	
				}

				if(Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP)
				{
					new fuel = floatround(Vehicle[vid][vehicle_fuel_current]);
					logprintf(LOG_VEHICLE, "[ENTER %d] [GROUP %d], player: %s, current hp: %0.2f, fuel: %d", Vehicle[vid][vehicle_uid], Vehicle[vid][vehicle_owner], pInfo[playerid][player_name], Vehicle[vid][vehicle_health], fuel);
				}

				// Ustawiamy kierowce
				Vehicle[vid][vehicle_driver] = playerid;

				// Rowerki
				new model = GetVehicleModel(vid);
		  		if(model == 509 || model == 510 || model == 481) return 1;

				// Sprawdzamy czy silnik nie jest juz czasem odpalony
				if( !Vehicle[vid][vehicle_engine] && CanPlayerUseVehicle(playerid, vid) ) TextDrawShowForPlayer(playerid, vehicleInfo);
			}
		}
	}
	if( oldstate == PLAYER_STATE_DRIVER && newstate != PLAYER_STATE_DRIVER )
	{
		new vid = GetPlayerVehicleID(playerid);
		if(Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP)
		{
			new fuel = floatround(Vehicle[vid][vehicle_fuel_current]);
			logprintf(LOG_VEHICLE, "[EXIT %d] [GROUP %d], player: %s, current hp: %0.2f, fuel: %d", Vehicle[vid][vehicle_uid], Vehicle[vid][vehicle_owner], pInfo[playerid][player_name], Vehicle[vid][vehicle_health], fuel);
		}
		
		TextDrawHideForPlayer(playerid, vehicleInfo);
        StopAudioStreamForPlayer(playerid);
	}

	if(oldstate == PLAYER_STATE_PASSENGER && newstate == PLAYER_STATE_ONFOOT)
	{
		if(pInfo[playerid][player_taxi_veh] != INVALID_VEHICLE_ID)
		{
			new driver = GetVehicleDriver(pInfo[playerid][player_taxi_veh]);
			new price = pInfo[driver][player_taxi_cost];

			if(price > 0)
			{
				GivePlayerMoney(playerid, -price);
				new gid = pInfo[driver][player_duty_gid];
				if(gid != -1)
				{
					if(price >= 20)
					{
						GiveGroupMoney(gid, price-10);
						GivePlayerMoney(driver, 10);
					}
					else
					{
						GiveGroupMoney(gid, price);
					}

					SendGuiInformation(playerid, "Information", sprintf("You've paid %d for taxi drive.", pInfo[driver][player_taxi_cost]));

					pInfo[driver][player_taxi_price] = 0;
					pInfo[driver][player_taxi_cost] = 0;
					pInfo[driver][player_taxi_distance] = 0;
					pInfo[driver][player_taxi_drive] = false;
	 				pInfo[playerid][player_taxi_veh] = INVALID_VEHICLE_ID;
	 				pInfo[driver][player_taxi_veh] = INVALID_VEHICLE_ID;
	 			}
			}
		}
	}

	if( newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER)
	{
		if(pInfo[playerid][player_hours] < 1 && pInfo[playerid][player_duty_gid] == -1)
		{
			new vid = GetPlayerVehicleID(playerid);
			if(GetVehicleDriver(vid) == INVALID_PLAYER_ID || GetVehicleDriver(vid) == playerid)
			{
				KickAc(playerid, -1, "Non authorized entrance (force, 0h)");
			}
		}
	}
	
	if( (oldstate == PLAYER_STATE_DRIVER || oldstate == PLAYER_STATE_PASSENGER) && newstate != PLAYER_STATE_DRIVER && newstate != PLAYER_STATE_PASSENGER )
	{
		if(pInfo[playerid][player_occupied_vehicle] != -1)
		{
			Vehicle[pInfo[playerid][player_occupied_vehicle]][vehicle_occupants] -= 1;
		}

		pInfo[playerid][player_occupied_vehicle] = -1;

		StopAudioStreamForPlayer(playerid);

		if( pInfo[playerid][player_belt] )
		{
			RemovePlayerStatus(playerid, PLAYER_STATUS_BELT);
			pInfo[playerid][player_belt] = false;
			
			SendPlayerInformation(playerid, "~w~You left car without seatbelt ~r~off~w~. You have to wait 2 seconds.", 3000);
			TogglePlayerControllable(playerid, 0);
			pInfo[playerid][player_freeze] = 2;
		}
	}
	
	return 1;
}

/*public OnUnoccupiedVehicleUpdate(vehicleid, playerid, passenger_seat, Float:new_x, Float:new_y, Float:new_z)
{
	if(GetVehicleDistanceFromPoint(vehicleid, Vehicle[vehicleid][vehicle_last_pos][0], Vehicle[vehicleid][vehicle_last_pos][1], Vehicle[vehicleid][vehicle_last_pos][2]) < 20)
	{
		if(GetVehicleDistanceFromPoint(vehicleid, Vehicle[vehicleid][vehicle_last_pos][0], Vehicle[vehicleid][vehicle_last_pos][1], Vehicle[vehicleid][vehicle_last_pos][2]) > 5)
		{
	    	SetVehiclePos(vehicleid, Vehicle[vehicleid][vehicle_last_pos][0], Vehicle[vehicleid][vehicle_last_pos][1], Vehicle[vehicleid][vehicle_last_pos][2]);
	    }
	}
    return 1;
}*/

public OnVehicleStreamIn(vehicleid, forplayerid)
{
    Iter_Add(PlayerVehicles[forplayerid], vehicleid);
    return 1;
}

public OnVehicleStreamOut(vehicleid, forplayerid)
{
    Iter_Remove(PlayerVehicles[forplayerid], vehicleid);
    return 1;
}

public OnVehicleMod(playerid, vehicleid, componentid)
{
    switch(componentid)
    {
        case 1008..1010: 
        {
        	if(IsPlayerInInvalidNosVehicle(playerid))
        	{
        		RemoveVehicleComponent(vehicleid, componentid);
        		BanAc(playerid, -1, sprintf("Invalid NOS (compid:%d, vid: %d)", componentid, vehicleid));
        	}
        }
    }
    if(!IsComponentidCompatible(GetVehicleModel(vehicleid), componentid))
    {
    	RemoveVehicleComponent(vehicleid, componentid);
    	BanAc(playerid, -1, sprintf("Invalid component (compid:%d, vid: %d)", componentid, vehicleid));
    }

    BanAc(playerid, -1, "Force mod shop tune");
    return 0;
}

public OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
    BanAc(playerid, -1, "Force paintjob");
    DeleteVehicle(vehicleid, false);
    return 1;
}

public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
	BanAc(playerid, -1, "Force color change");
    DeleteVehicle(vehicleid, false);
    return 1;
}

public OnVehicleDamageStatusUpdate(vehicleid, playerid)
{
	/*new log[64];
	format(log, sizeof(log), "[CAR] %s zespawnowal pojazd %s [UID: %d]", pInfo[playerid][player_name], VehicleNames[Vehicle[vid][vehicle_model]-400], Vehicle[vid][vehicle_uid]);
	AddGroupLog(Vehicle[vid][vehicle_owner], log);*/
	new Float:carhp;
	GetVehicleHealth(vehicleid, carhp);

	if(Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP)
	{
		logprintf(LOG_VEHICLE, "[DAMAGE %d] [GROUP %d], player: %s, current hp: %0.2f", Vehicle[vehicleid][vehicle_uid], Vehicle[vehicleid][vehicle_owner], pInfo[playerid][player_name], carhp);
	}

	Vehicle[vehicleid][vehicle_damaged] = true;

	if(carhp > 900.0)
	{
		Vehicle[vehicleid][vehicle_damage][0] = 0;
		Vehicle[vehicleid][vehicle_damage][1] = 0;
		Vehicle[vehicleid][vehicle_damage][2] = 0;
		Vehicle[vehicleid][vehicle_damage][3] = 0;
		UpdateVehicleDamageStatus(vehicleid, Vehicle[vehicleid][vehicle_damage][0], Vehicle[vehicleid][vehicle_damage][1], Vehicle[vehicleid][vehicle_damage][2], Vehicle[vehicleid][vehicle_damage][3]);
	}

    return 1;
}

public OnVehicleSpawn(vehicleid)
{
	if(Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_JOB)
	{
		SetVehicleHealth(vehicleid, 1000.0);
		Vehicle[vehicleid][vehicle_damage][0] = 0;
		Vehicle[vehicleid][vehicle_damage][1] = 0;
		Vehicle[vehicleid][vehicle_damage][2] = 0;
		Vehicle[vehicleid][vehicle_damage][3] = 0;
		Vehicle[vehicleid][vehicle_fuel_current] = 40.0;
		RepairVehicle(vehicleid);
		SaveVehicle(vehicleid);

		LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", Vehicle[vehicleid][vehicle_uid]), true);
	}
	return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
	if(killerid != INVALID_PLAYER_ID)
	{
		logprintf(LOG_VEHICLE, "[DAMAGE %d] [GROUP %d], player: %s, TOTAL DESTROYED.", Vehicle[vehicleid][vehicle_uid], Vehicle[vehicleid][vehicle_owner], pInfo[killerid][player_name]);
	}

	if(Vehicle[vehicleid][vehicle_damaged] == false && Vehicle[vehicleid][vehicle_occupants] == 0 && Vehicle[vehicleid][vehicle_last_used] == 0)
	{
		if(killerid != INVALID_PLAYER_ID)
		{
			KickAc(killerid, -1, "Vehicle killer");
			Vehicle[vehicleid][vehicle_health] = 1000.0;
			SetVehicleHealth(vehicleid, 1000);
			return 1;
		}
	}

	/*GetVehiclePos(vehicleid, Vehicle[vehicleid][vehicle_park][0], Vehicle[vehicleid][vehicle_park][1], Vehicle[vehicleid][vehicle_park][2]);
	GetVehicleZAngle(vehicleid, Vehicle[vehicleid][vehicle_park][3]);
	Vehicle[vehicleid][vehicle_park_world] = GetVehicleVirtualWorld(vehicleid);
	Vehicle[vehicleid][vehicle_park_interior] = Vehicle[vehicleid][vehicle_interior];
					
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_posx` = %f, `vehicle_posy` = %f, `vehicle_posz` = %f, `vehicle_posa` = %f, `vehicle_world` = %d, `vehicle_interior` = %d WHERE `vehicle_uid` = %d", Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2], Vehicle[vid][vehicle_park][3], Vehicle[vid][vehicle_park_world], Vehicle[vid][vehicle_park_interior], Vehicle[vid][vehicle_uid]));

	new v_uid = Vehicle[vehicleid][vehicle_uid];

	LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", v_uid), true);*/

	Vehicle[vehicleid][vehicle_destroyed] = true;
	DeleteVehicle(vehicleid, false);
    return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
    return 1;
}

public OnPlayerRequestSpawn(playerid)
{
    return 1;
}

public OnPlayerSpawn(playerid)
{
	if( IsPlayerNPC(playerid) )
	{
		return 1;
	}

	PlayerTextDrawShow(playerid, ZoneName);
	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
	Update3DTextLabelText(pInfo[playerid][player_description_label], LABEL_DESCRIPTION, "");

	pInfo[playerid][player_spawned] = 1;
	pInfo[playerid][player_bomb_car] = INVALID_VEHICLE_ID;
	pInfo[playerid][player_repair_car] = INVALID_VEHICLE_ID;
	pInfo[playerid][player_montage_car] = INVALID_VEHICLE_ID;

	SetPlayerTeam(playerid, 10);
	TextDrawShowForPlayer(playerid, TextDrawSanNews);
	defer PreloadAllAnimLibs[2000](playerid);
	
	// BW
	if( pInfo[playerid][player_bw] > 0 )
	{
		TogglePlayerControllable(playerid, 0);

		SetPlayerHealth(playerid, 1);
		
		SetPlayerVirtualWorld(playerid, pInfo[playerid][player_quit_vw]);
		SetPlayerInterior(playerid, pInfo[playerid][player_quit_int]);

		SetPlayerCameraPos(playerid, pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2] + 6.0);
		SetPlayerCameraLookAt(playerid, pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2]);
		
		TogglePlayerControllable(playerid, 0);
		
		defer ApplyAnim[2000](playerid, ANIM_TYPE_BW);
		
		UpdatePlayerBWTextdraw(playerid);

		RP_PLUS_SetPlayerPos(playerid, pInfo[playerid][player_quit_pos][0],  pInfo[playerid][player_quit_pos][1],  pInfo[playerid][player_quit_pos][2]);
		SetPlayerSkin(playerid, pInfo[playerid][player_last_skin]);
	}
	else
	{
		new health = floatround(pInfo[playerid][player_health]);
		if( health == 0 ) health = 5;
		SetPlayerHealth(playerid, health);
		FreezePlayer(playerid, 3000);
	}

	if(pInfo[playerid][player_aj] > 0)
	{
		RP_PLUS_SetPlayerPos(playerid, 154.0880,-1951.6383,47.8750);
		SetPlayerVirtualWorld(playerid, pInfo[playerid][player_id]);
	}

	return 1;
}

public OnPlayerEnterDynamicArea(playerid, areaid)
{
	if(IsPlayerNPC(playerid))
	{
		return 1;
	}

	pInfo[playerid][player_area] = areaid;

	if( !PlayerHasTog(playerid, TOG_HUD) )
	{
		TextDrawSetString(AreaText[playerid], sprintf("~b~~h~ Area:~w~ %d", areaid));
		TextDrawShowForPlayer(playerid, AreaText[playerid]);

		if(Area[areaid][area_flags] != 0)
		{
			ShowAreaFlags(playerid, areaid);
		}
	}

	if(AreaHasFlag(areaid, AREA_FLAG_LS))
	{
		new vid = GetPlayerVehicleID(playerid);
		if(vid != INVALID_VEHICLE_ID)
		{
			if(Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_JOB)
			{
				CarUnspawn(playerid, vid, -1, "Job vehicle abuse");
			}
		}
	}

	if(AreaHasFlag(areaid, AREA_FLAG_SERWIS))
	{
		if(!IsAnyWorkshopOpen())
		{
			SendPlayerInformation(playerid, "~w~Press ~y~Y~w~ to interact with area.", 4000);
		}
	}

	if(AreaHasFlag(areaid, AREA_FLAG_DRIVE))
	{
		if(!IsAnyGastroOpen())
		{
			SendPlayerInformation(playerid, "~w~Press ~y~Y~w~ to interact with area.", 4000);
		}
		else
		{
			SendPlayerInformation(playerid, "~w~Drive thru is closed. There are opened ~y~businesses~w~.", 4000);
		}
	}
	
	if(AreaHasFlag(areaid, AREA_FLAG_WORK))
	{
		if(pInfo[playerid][player_job] == WORK_TYPE_LUMBERJACK)
		{
			pInfo[playerid][player_working] = WORK_TYPE_LUMBERJACK;
			TextDrawSetString(Tutorial[playerid], "~p~Y~w~ - moving wood~n~~p~LPM~w~ - cutting~n~~p~Y~w~ - bot selling");
			TextDrawShowForPlayer(playerid, Tutorial[playerid]);
		}
	}

	if(AreaHasFlag(areaid, AREA_FLAG_WORK_FISH))
	{
		if(pInfo[playerid][player_job] == WORK_TYPE_FISHER)
		{
			pInfo[playerid][player_working] = WORK_TYPE_FISHER;
			TextDrawSetString(Tutorial[playerid], "~p~Y~w~ - moving fishes~n~~p~LPM~w~ - fishing~n~~p~Y~w~ - bot selling");
			TextDrawShowForPlayer(playerid, Tutorial[playerid]);
		}
	}

	switch( Area[areaid][area_type] )
	{
		case AREA_TYPE_NORMAL:
		{
			if( strcmp(Area[areaid][area_audio], "-", true) )
			{
				PlayAudioStreamForPlayer(playerid, Area[areaid][area_audio]);
			}
		}

		case AREA_TYPE_FIRE:
		{
			CreateExplosion(Area[areaid][area_pos][0], Area[areaid][area_pos][1], Area[areaid][area_pos][2], 2, 5.0);
			if( IsPlayerInAnyGroup(playerid) )
			{
				new gid = pInfo[playerid][player_duty_gid];
				if(gid == -1) return 1;

				if( Group[gid][group_flags] & GROUP_FLAG_MEDIC)
				{
					SetPVarInt(playerid, "fire", areaid);
				}
			}
		}

		case AREA_TYPE_ARMDEALER:
		{
			ApplyActorAnimation(ArmDealer, "DEALER", "DEALER_IDLE_01", 4.1, 0, 0, 0, 1, 0);

			if(IsPlayerCop(playerid))
			{
				ActorProx(ArmDealer, "Marcus Bradford", "Im not talking with cops.", PROX_LOCAL);
				return 1;
			}
			else
			{
				new gid = pInfo[playerid][player_duty_gid];
				if(gid == -1)
				{
					if(PlayerHasFlag(playerid, PLAYER_FLAG_ORDER) )
					{
						new loss = random(3);
						switch(loss)
						{
							case 0:
							{
								ActorProx(ArmDealer, "Marcus Bradford", "Take a look at my offer.", PROX_LOCAL);
							}
							case 1:
							{
								ActorProx(ArmDealer, "Marcus Bradford", "Let's make it real quick.", PROX_LOCAL);
							}
							case 2:
							{
								ActorProx(ArmDealer, "Marcus Bradford", "I have something for you.", PROX_LOCAL);
							}
						}

						new string[1024], count;
		                DynamicGui_Init(playerid);

		                format(string, sizeof(string), "%sProduct\tPrice\tWeek limit\n", string);

		                foreach (new prod: Products)
		                {
		                    if( Product[prod][product_player] != pInfo[playerid][player_id] ) continue;

		                    format(string, sizeof(string), "%s %s\t$%d\t%d/%d \n", string, Product[prod][product_name], Product[prod][product_price], Product[prod][product_limit_used], Product[prod][product_limit]);
		                    DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
		                    count++;
		                }
		                if( count == 0 ) SendGuiInformation(playerid, "Information", "This bot has no offer for you.");
		                else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ILLEGAL_ADD, DIALOG_STYLE_TABLIST_HEADERS, "Marcus Bradford - offer", string, "Buy", "Close");
					}

					return 1;
				}
				
				if(GroupHasFlag(gid, GROUP_FLAG_BOT) )
				{
					new slot = GetPlayerDutySlot(playerid);
					if(slot == -1) return 1;
					if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_ORDER) ) return SendGuiInformation(playerid, "Information", "You don't have access to order products. Ask group leader for it.");
					
					new loss = random(3);

					switch(loss)
					{
						case 0:
						{
							ActorProx(ArmDealer, "Marcus Bradford", "Take a look at my offer.", PROX_LOCAL);
						}
						case 1:
						{
							ActorProx(ArmDealer, "Marcus Bradford", "Let's make it real quick.", PROX_LOCAL);
						}
						case 2:
						{
							ActorProx(ArmDealer, "Marcus Bradford", "I have something for you.", PROX_LOCAL);
						}
					}
					new string[1024], count;
	                DynamicGui_Init(playerid);

	                format(string, sizeof(string), "%sProduct\tPrice\tWeek limit\n", string);

	                foreach (new prod: Products)
	                {
	                    if( Product[prod][product_group] != Group[gid][group_uid] ) continue;

	                    format(string, sizeof(string), "%s %s\t$%d\t%d/%d \n", string, Product[prod][product_name], Product[prod][product_price], Product[prod][product_limit_used], Product[prod][product_limit]);
	                    DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                    count++;
	                }
	                if( count == 0 ) SendGuiInformation(playerid, "Information", "These bot has no products for your group.");
	                else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ILLEGAL_ADD, DIALOG_STYLE_TABLIST_HEADERS, "Marcus Bradford - offer", string, "Buy", "Close");
				}
			}
		}
	}
	return 1;
}

public OnPlayerLeaveDynamicArea(playerid, areaid)
{
	if(IsPlayerNPC(playerid)) return 1;

	pInfo[playerid][player_area] = 0;

	TextDrawHideForPlayer(playerid, AreaText[playerid]);
	TextDrawHideForPlayer(playerid, AreaFlags[playerid]);

	if(pInfo[playerid][player_dealing])
	{
		SendGuiInformation(playerid, "Information", "You have left dealing zone. Deal ended.");
		RemovePlayerStatus(playerid, PLAYER_STATUS_DEALING);
		TextDrawHideForPlayer(playerid, Tutorial[playerid]);
		pInfo[playerid][player_dealing] = false;
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

	if(AreaHasFlag(areaid, AREA_FLAG_WORK) || AreaHasFlag(areaid, AREA_FLAG_WORK_FISH))
	{
		pInfo[playerid][player_working] = 0;
		TextDrawHideForPlayer(playerid, Tutorial[playerid]);
	}

	if(AreaHasFlag(areaid, AREA_FLAG_GOLF))
	{
		pInfo[playerid][player_golf] = false;
		golf = 0;
	}

	switch( Area[areaid][area_type] )
	{
		case AREA_TYPE_FIRE:
		{
			SetPVarInt(playerid, "fire", 0);
		}
		case AREA_TYPE_NORMAL:
		{
			if( strcmp(Area[areaid][area_audio], "-", true) )
			{
				StopAudioStreamForPlayer(playerid);
			}
		}
	}
	return 1;
}

public OnObjectMoved(objectid)
{
	if(objectid==FerrisWheelObjects[10]) SetTimer("RotateFerrisWheel", 3000,false);
	return 1;
}

forward RotateFerrisWheel();
public RotateFerrisWheel()
{
	FerrisWheelAngle+=36;
	if(FerrisWheelAngle>=360)FerrisWheelAngle=0;
	if(FerrisWheelAlternate)FerrisWheelAlternate=0;
	else FerrisWheelAlternate=1;
	new Float:FerrisWheelModZPos=0.0;
	if(FerrisWheelAlternate)FerrisWheelModZPos=0.05;
	MoveObject(FerrisWheelObjects[10],389.7734,-2028.4688,22.0+FerrisWheelModZPos, 0.005, 0, FerrisWheelAngle,90);
}

public OnPlayerShootDynamicObject(playerid, weaponid, objectid, Float:x, Float:y, Float:z)
{
	new wslot = GetWeaponSlot(weaponid);
	pWeapon[playerid][wslot][pw_ammo] -= 1;
	
	if( pWeapon[playerid][wslot][pw_ammo] == 0 )
	{
		Item_Use(pWeapon[playerid][wslot][pw_itemid], playerid);
	}
	return 1;
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	new String[64];
	if( hittype != BULLET_HIT_TYPE_NONE && hittype != BULLET_HIT_TYPE_PLAYER_OBJECT)
	{
		if( !( -20.0 <= fX <= 20.0 ) || !( -20.0 <= fY <= 20.0 ) || !( -20.0 <= fZ <= 20.0 ) )
		{
		    KickAc(playerid, -1, "Invalid bullet");
            return 0; 
		}
	 	if( !( -1000.0 <= fX <= 1000.0 ) || !( -1000.0 <= fY <= 1000.0 ) || !( -1000.0 <= fZ <= 1000.0 ) )
        {
            KickAc(playerid, -1, "Invalid bullet (second)");
            return 0; 
        }
	}

	new wslot = GetWeaponSlot(weaponid);

	if( hittype == BULLET_HIT_TYPE_VEHICLE )
	{
		Vehicle[hitid][vehicle_damaged] = true;
		pWeapon[playerid][wslot][pw_ammo] -= 1;
	}

	if(weaponid == 38 || weaponid == 37 || weaponid == 36 || weaponid == 39 || weaponid == 35)
    {
    	format(String, sizeof(String), "Restricted weap shot (w: %d)", weaponid);
    	BanAc(playerid, -1, String);
		return 1;
    }
	
	pWeapon[playerid][wslot][pw_ammo] -= 1;
	
	if( pWeapon[playerid][wslot][pw_ammo] == 0 )
	{
		Item_Use(pWeapon[playerid][wslot][pw_itemid], playerid);
		return 1;
	}

	if(!IsPlayerInAnyVehicle(playerid))
    {
	    if(weaponid != 0 && GetPlayerWeapon(playerid) != GetPVarInt(playerid, "weaping") && !pInfo[playerid][player_shooting] || GetPVarInt(playerid, "weaping") == 0 && weaponid != 0 && !pInfo[playerid][player_shooting])
	    {
	    	format(String, sizeof(String), "No item shot (w: %d)", weaponid);
	    	KickAc(playerid, -1, String);
	    	return 1;
	    }
	}

	if( pInfo[playerid][player_howitzer] > 0 )
	{
    	CreateExplosion(fX, fY, fZ, 12, 1); 
    	pInfo[playerid][player_howitzer]--;
    }
    
    if( weaponid != 0 && GetPlayerWeapon(playerid) == GetPVarInt(playerid, "weaping"))
    {
    	if(GetPlayerWeaponAmmo(playerid, weaponid) <= 1) 
    	{
    		for(new i;i<13;i++)
			{
				if( pWeapon[playerid][i][pw_itemid] > -1 ) Item_Use(pWeapon[playerid][i][pw_itemid], playerid);
			}

			GivePlayerWeapon(playerid, 0, 0);
    		SetPVarInt(playerid, "weaping", 0);
			SetPVarInt(playerid, "taser", 0);
			return 1;
    	}
    }

    if(hittype == BULLET_HIT_TYPE_PLAYER_OBJECT && pInfo[playerid][player_shooting] > 0)
    {
    	if(Object[hitid][object_model] == 1586 || Object[hitid][object_model] == 1585)
    	{
    		pInfo[playerid][player_weapon_skill] += 0.40;
    		/*new uid = Object[hitid][object_uid];
    		DeleteObject(hitid, false);
    		LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);*/
    		pWeapon[playerid][wslot][pw_ammo] -= 1;
    		return 1;
    	}
    }

    pInfo[playerid][player_weapon_skill] += 0.25;
	return 1;
}

public OnPlayerEditAttachedObject(playerid, response, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ)
{
    if(response)
    {
    	if(fOffsetX > 1 || fOffsetY > 1 || fOffsetZ > 1)
    	{
    		SendGuiInformation(playerid, "Information", "Object is too far from character.");
    		RemovePlayerAttachedObject(playerid, index);
    		return 1;
    	}

		RemovePlayerAttachedObject(playerid, index);
		SetPlayerAttachedObject(playerid, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ);
        
		ao[playerid][index][ao_x] = fOffsetX;
        ao[playerid][index][ao_y] = fOffsetY;
        ao[playerid][index][ao_z] = fOffsetZ;
        ao[playerid][index][ao_rx] = fRotX;
        ao[playerid][index][ao_ry] = fRotY;
        ao[playerid][index][ao_rz] = fRotZ;
        ao[playerid][index][ao_sx] = fScaleX;
        ao[playerid][index][ao_sy] = fScaleY;
        ao[playerid][index][ao_sz] = fScaleZ;

        if(ao[playerid][index][ao_inserted] == false)
        {
        	mysql_query(mySQLconnection, sprintf("INSERT INTO ipb_attached_objects (attach_uid, attach_owner, attach_index, attach_model, attach_bone, attach_x, attach_y, attach_z, attach_rx, attach_ry, attach_rz, attach_sx, attach_sy, attach_sz) VALUES (null, %d, %d, %d, %d, '%f', '%f', '%f', '%f', '%f', '%f', '%f', '%f', '%f')", pInfo[playerid][player_id], index, modelid, boneid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ));
        }
        else
        {
        	mysql_query(mySQLconnection, sprintf("UPDATE ipb_attached_objects SET attach_x = '%f', attach_y = '%f', attach_z = '%f', attach_rx = '%f', attach_ry = '%f', attach_rz = '%f', attach_sx = '%f', attach_sy = '%f', attach_sz = '%f' WHERE attach_owner = %d AND attach_model = %d", fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ, pInfo[playerid][player_id], modelid ));
        }
    }
    return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	printf("[DIAL] %s (UID: %d, GID: %d): [%s] (%d, %d, %d)", pInfo[playerid][player_name], pInfo[playerid][player_id], pGlobal[playerid][glo_id], inputtext, playerid, dialogid, response);
	if( strfind(inputtext, "|", true) != -1) return SendGuiInformation(playerid, "Error", "Detected not allowed characters.");
	DebugText(inputtext);

	switch( dialogid )
	{
		case DIALOG_LOGIN:
		{
			if( !response ) return Kick(playerid);

			if( isnull(inputtext) || strlen(inputtext) < 5 )
			{
				pGlobal[playerid][glo_bad_pass] += 1;

				if( pGlobal[playerid][glo_bad_pass] >= 3 )
				{
					SendClientMessage(playerid, COLOR_LIGHTER_RED, "Kicked because of too many bad login attempts.");
					Kick(playerid);
					return 1;
				}

				new bad_pass_info[60];
				if( pGlobal[playerid][glo_bad_pass] > 0 ) format(bad_pass_info, sizeof(bad_pass_info), "\nWrong password. Remaining attempts: {ADC7E7}%d", 3-pGlobal[playerid][glo_bad_pass]);
				
				new string[512];
				format(string, sizeof(string), "Welcome to Society Roleplay!\nCharacter {AFAFAF}%s {a9c4e4}has been found in our database.\nPlease input your password to login.\n%s", pInfo[playerid][player_name], bad_pass_info);
			    return ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, ""guiopis"Login panel", string, "Login", "Exit");
			}
			mysql_real_escape_string(inputtext, inputtext, mySQLconnection, 256);

			new Hash[80];
			format(Hash, sizeof(Hash), "%s%s", MD5_Hash(gInfo[playerid][global_salt]), MD5_Hash(inputtext));
			format(Hash, sizeof(Hash), "%s", MD5_Hash(Hash));


			if( !strcmp(Hash, gInfo[playerid][global_password], true) )
			{
				gInfo[playerid][global_logged] = true;
				OnPlayerLoginHere(playerid);
			}
			else
			{
				pGlobal[playerid][glo_bad_pass] += 1;

				if( pGlobal[playerid][glo_bad_pass] >= 3 )
				{
					SendClientMessage(playerid, COLOR_LIGHTER_RED, "Kicked because of too many bad login attempts.");
					Kick(playerid);
					return 1;
				}

				new bad_pass_info[60];
				if( pGlobal[playerid][glo_bad_pass] > 0 ) format(bad_pass_info, sizeof(bad_pass_info), "\nWrong password. Remaining attempts: {ADC7E7}%d", 3-pGlobal[playerid][glo_bad_pass]);
				
				new string[512];
				format(string, sizeof(string), "Welcome to Society Roleplay!\nCharacter {AFAFAF}%s {a9c4e4}has been found in our database.\nPlease input your password to login.\n%s", pInfo[playerid][player_name], bad_pass_info);
			    ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, ""guiopis"Login panel", string, "Login", "Exit");
			}
		}

		case DIALOG_LOGIN_NO_ACCOUNT:
		{
			return Kick(playerid);
		}

		case DIALOG_SALON_SELL:
		{
			if(!response) return 1;

			new dg_value = DynamicGui_GetValue(playerid, listitem), dg_data = DynamicGui_GetDataInt(playerid, listitem);
			if(dg_value == DG_PRODS_SALON)
			{
				new rows, fields;
				new model= dg_data;
				mysql_query(mySQLconnection, sprintf("SELECT dealer_price, dealer_fueltype, dealer_category FROM ipb_veh_dealer WHERE dealer_model = %d", model));
				cache_get_data(rows, fields);

				new price = cache_get_row_int(0, 0);
				new category = cache_get_row_int(0, 2);
				//new fueltype = cache_get_row_int(0, 1);

				if(pInfo[playerid][player_money] < price)
				{
					SendGuiInformation(playerid, "Information", "You don't have enough money to buy this vehicle.");
					return 1;
				}

				if(category == CATEGORY_PREMIUM)
				{
					mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET game_unique_vehicle = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
				}

				GivePlayerMoney(playerid, -price);

				new color = random(44);
				
				if(model == 511 || model == 519 || model == 593 || model == 512 || model == 553 || model == 487 || model == 563)
	            {
	                mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_vehicles` (vehicle_uid, vehicle_model, vehicle_posx, vehicle_posy, vehicle_posz, vehicle_posa, vehicle_world, vehicle_interior, vehicle_color1, vehicle_color2, vehicle_owner, vehicle_ownertype, vehicle_fuel) VALUES (null, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d, %d, %f)", model, 1938.9546,-2271.0830,13.1125, 176.0897, 0, 0, color, 1, pInfo[playerid][player_id], VEHICLE_OWNER_TYPE_PLAYER, 5.0));
	                new uid = cache_insert_id();

	                new vid = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);

	                SendGuiInformation(playerid, ""guiopis"Alert", sprintf("Flying vehicle has been bought - %s [UID: %d, ID: %d].\nPosition marked on the map.", VehicleNames[model-400], uid, vid));
	                SetPlayerMapIcon(playerid, 13, 1938.9546,-2271.0830,13.1125, 0, 0xFF0000AA, MAPICON_GLOBAL);
	                return 1;
	            }

	            if(model == 446 || model == 452 || model == 453 || model == 454 || model == 473 || model == 484 || model == 493)
	            {
	                mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_vehicles` (vehicle_uid, vehicle_model, vehicle_posx, vehicle_posy, vehicle_posz, vehicle_posa, vehicle_world, vehicle_interior, vehicle_color1, vehicle_color2, vehicle_owner, vehicle_ownertype, vehicle_fuel) VALUES (null, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d, %d, %f)", model, 733.0229,-1502.7858,-0.6217, 176.0897, 0, 0, color, 1, pInfo[playerid][player_id], VEHICLE_OWNER_TYPE_PLAYER, 5.0));
	                new uid = cache_insert_id();

	                new vid = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);

	                SendGuiInformation(playerid, ""guiopis"Alert", sprintf("Boat has been bought - %s [UID: %d, ID: %d].\nPosition marked on the map.", VehicleNames[model-400], uid, vid));
	                SetPlayerMapIcon(playerid, 13, 733.0229,-1502.7858,-0.6217, 0, 0xFF0000AA, MAPICON_GLOBAL);
	                return 1;
	            }

	            mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_vehicles` (vehicle_uid, vehicle_model, vehicle_posx, vehicle_posy, vehicle_posz, vehicle_posa, vehicle_world, vehicle_interior, vehicle_color1, vehicle_color2, vehicle_owner, vehicle_ownertype, vehicle_fuel) VALUES (null, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d, %d, %f)", model, 866.7350, -1210.2969, 16.6562, 176.0897, 0, 0, color, 1, pInfo[playerid][player_id], VEHICLE_OWNER_TYPE_PLAYER, 5.0));
	            new uid = cache_insert_id();
	            new vid = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);

	            SendGuiInformation(playerid, ""guiopis"Alert", sprintf("Car has been bought - %s [UID: %d, ID: %d].\nPosition marked on the map.", VehicleNames[model-400], uid, vid));
	            SetPlayerMapIcon(playerid, 13, 866.7350, -1210.2969, 16.6562, 0, 0xFF0000AA, MAPICON_GLOBAL);
			}
		}

		case DIALOG_LUMBERJACK:
		{
			if(!response) return 1;

			if(pInfo[playerid][player_money]<150)
			{
				SendGuiInformation(playerid, "Information", "You don't have enough money.");
			}
			else
			{
				GivePlayerMoney(playerid,-150);
				Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_DILDO_CHAINSAW, 361, 9, 1, "Chainsaw");
				SendGuiInformation(playerid, "Information", "Item has been added to your inventory.");
			}
		}

		case DIALOG_AUTO_FIX:
		{
			if(!response) return 1;

			if(pInfo[playerid][player_money] < pInfo[playerid][player_dialog_tmp2])
			{
				SendGuiInformation(playerid, "Information", "You don't have enough money.");
			}
			else
			{
				GivePlayerMoney(playerid, -pInfo[playerid][player_dialog_tmp2]);
				defer CarAutoFix[60000](playerid, GetPlayerVehicleID(playerid));
				TextDrawSetString(Tutorial[playerid], "Fixing your vehicle, dont leave this ~y~area~w~...~n~Wait a minute.");
				TextDrawShowForPlayer(playerid, Tutorial[playerid]);
				pInfo[playerid][player_dialog_tmp1] = pInfo[playerid][player_area];
			}
		}

		case DIALOG_SALON:
		{
			if(!response) return 1;
			switch(listitem)
			{
				//Trzydrzwiowe
				case 0:
				{
					ListDealership(playerid, CATEGORY_THREEDOORS, "Three-door vehicles");
				}

				//Piêciodrzwiowe
				case 1:
				{
					ListDealership(playerid, CATEGORY_FIVEDOORS, "Five-door vehicles");
				}

				//Ciê¿arowe
				case 2:
				{
					ListDealership(playerid, CATEGORY_TRUCKS, "Trucks");
				}

				//Jednoœlady
				case 3:
				{
					ListDealership(playerid, CATEGORY_BIKES, "Bikes");
				}

				//Sportowe
				case 4:
				{
					ListDealership(playerid, CATEGORY_SPORT, "Sport cars");
				}

				//£odzie
				case 5:
				{
					ListDealership(playerid, CATEGORY_BOATS, "Boats");
				}

				//Lataj¹ce
				case 6:
				{
					ListDealership(playerid, CATEGORY_PLANES, "Flying vehicles");
				}

				//Premium
				case 7:
				{
					new rows, fields;
					mysql_query(mySQLconnection, sprintf("SELECT game_unique_vehicle FROM ipb_members WHERE member_id = %d", pGlobal[playerid][glo_id]));
					cache_get_data(rows, fields);

					if(rows)
					{
						new premium = cache_get_row_int(0, 0);
						if(premium > 0)
						{
							ListDealership(playerid, CATEGORY_PREMIUM, "Premium vehicles");
						}
						else
						{
							SendGuiInformation(playerid, "Information", "You don't have access to this category.");
						}
					}
					else
					{
						SendGuiInformation(playerid, "Information", "You don't have access to this category.");
					}
				}
			}
		}

		case DIALOG_CLOTH:
		{
			if(!response) return 1;

			switch(listitem)
			{
				case 0:
				{
					new Float:PosX, Float:PosY, Float:PosZ;
					GetPlayerPos(playerid, PosX, PosY, PosZ);

					GetXYInFrontOfPlayer(playerid, PosX, PosY, 4.0);
					SetPlayerCameraPos(playerid, PosX, PosY, PosZ + 0.30);

					GetPlayerPos(playerid, PosX, PosY, PosZ);
					SetPlayerCameraLookAt(playerid, PosX, PosY, PosZ);

					TogglePlayerControllable(playerid, false);

					pInfo[playerid][player_skin_changing] = true;
					pInfo[playerid][player_skin_id] = 0;

					TextDrawSetString(Tutorial[playerid], "~w~Choose clothes by ~w~arrows ~y~~<~ ~>~~n~~k~~PED_JUMPING~ ~w~- cancel~n~~y~~k~~VEHICLE_ENTER_EXIT~ ~w~- buy");
					TextDrawShowForPlayer(playerid, Tutorial[playerid]);
				}
				case 1:
				{
					new Float:PosX, Float:PosY, Float:PosZ;
					GetPlayerPos(playerid, PosX, PosY, PosZ);

					GetXYInFrontOfPlayer(playerid, PosX, PosY, 4.0);
					SetPlayerCameraPos(playerid, PosX, PosY, PosZ + 0.30);

					GetPlayerPos(playerid, PosX, PosY, PosZ);
					SetPlayerCameraLookAt(playerid, PosX, PosY, PosZ);

					TogglePlayerControllable(playerid, false);

					pInfo[playerid][player_access_changing] = true;
					pInfo[playerid][player_access_id] = 0;

					TextDrawSetString(Tutorial[playerid], "~w~Choose access by ~w~arrows ~y~~<~ ~>~~n~~k~~PED_JUMPING~ ~w~- cancel~n~~y~~k~~VEHICLE_ENTER_EXIT~ ~w~- buy");
					TextDrawShowForPlayer(playerid, Tutorial[playerid]);
				}
			}
		}

		case DIALOG_MDC:
		{
			if(!response) return 1;
			switch(listitem)
			{
				//Znajdz osobe
				case 0:
				{
					ShowPlayerDialog(playerid, DIALOG_MDC_FIND_PERSON, DIALOG_STYLE_INPUT, "MDC - find person", "Please input name of target:", "Find", "Close");
				}
				//Baza DMV
				case 1:
				{
					ShowPlayerDialog(playerid, DIALOG_MDC_FIND_VEHICLE, DIALOG_STYLE_INPUT, "MDC - DMV database", "Please input target plates:", "Find", "Close");
				}
				//Zobacz poszukiwanych
				case 2:
				{
					new rows, fields, wanted_list[2048];
					mysql_query(mySQLconnection, "SELECT record_owner, record_reason FROM ipb_crime_records");
					cache_get_data(rows, fields);

					if(rows)
					{
						for(new row = 0; row != rows; row++)
						{
							new record_owner[64], record_reason[128];

							cache_get_row(row, 0, record_owner);
							cache_get_row(row, 1, record_reason);
							
							format(wanted_list, sizeof(wanted_list), "%s%s\t%s\n", wanted_list, record_owner, record_reason);
						}

						format(wanted_list, sizeof(wanted_list), "Suspect\tReason\n%s", wanted_list);
						ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST_HEADERS, "MDC - crime records", wanted_list, "OK", "");
					}
					else
					{
						SendGuiInformation(playerid, "MDC - crime records", "No suspects in database.");
					}
				}
				//Nadaj APB
				case 3:
				{
					ShowPlayerDialog(playerid, DIALOG_MDC_ADD_APB, DIALOG_STYLE_INPUT, "MDC - add APB", "Please input car plates or target name:", "Add", "");
				}
				//Anuluj APB
				case 4:
				{
					ShowPlayerDialog(playerid, DIALOG_MDC_REMOVE_APB, DIALOG_STYLE_INPUT, "MDC - remove APB", "Please input  car plates or target name.", "Remove", "");
				}
			}
		}

		case DIALOG_MDC_ADD_APB:
		{
			if(!response) return 1;

			if(strlen(inputtext) < 4 || strlen(inputtext) > 60)
			{
				SendGuiInformation(playerid, "MDC - add APB", "Wrong characters count. Minimum 4, max 60.");
				return 1;
			}

			format(pInfo[playerid][player_dialog_tmp], 64, inputtext);

			ShowPlayerDialog(playerid, DIALOG_MDC_ADD, DIALOG_STYLE_INPUT, "MDC - add APB", "Please input a reason.", "Add", "Cancel");
		}

		case DIALOG_MDC_ADD:
		{
			if(!response) return 1;

			if(strlen(inputtext) < 4 || strlen(inputtext) > 60)
			{
				SendGuiInformation(playerid, "MDC - add APB", "Wrong letters count. Minimum 4, max 60.");
				return 1;
			}

			mysql_escape_string(inputtext, inputtext, mySQLconnection, 64);
			mysql_escape_string(pInfo[playerid][player_dialog_tmp], pInfo[playerid][player_dialog_tmp], mySQLconnection, 64);

			mysql_query(mySQLconnection, sprintf("INSERT INTO ipb_crime_records (record_owner, record_reason) VALUES ('%s', '%s')", pInfo[playerid][player_dialog_tmp], inputtext));

			SendGuiInformation(playerid, "Information", "Record added.");
		}

		case DIALOG_MDC_REMOVE_APB:
		{
			if(!response) return 1;
			if(strval(inputtext) > 32) return ShowPlayerDialog(playerid, DIALOG_MDC_REMOVE_APB, DIALOG_STYLE_INPUT, "MDC - remove APB", "Please input  car plates or target name.", "Remove", "");
			mysql_escape_string(inputtext, inputtext, 64, mySQLconnection);
			mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_crime_records WHERE record_owner = '%s'", inputtext));
			SendGuiInformation(playerid, "Information", "Recored removed.");
		}

		case DIALOG_MDC_FIND_PERSON:
		{
			if(!response) return 1;

			new rows, fields, suspect[MAX_PLAYER_NAME+1];

			if(strlen(inputtext) < 4)
			{
				SendGuiInformation(playerid, "MDC - find person", "Minimum 4 letters.");
				return 1;
			}

			mysql_escape_string(inputtext, suspect, mySQLconnection, 256);
			mysql_query(mySQLconnection, sprintf("SELECT char_birth, char_documents, char_spawn, char_spawn_type, char_uid FROM ipb_characters WHERE `char_name` = '%s' LIMIT 1", suspect));
			cache_get_data(rows, fields);

			if(rows)
			{
				new adress[40], list_mdc[768], list_cars[256];

				strreplace(suspect, '_', ' ');

				new age = 2016 - cache_get_row_int(0, 0);
				new doc = cache_get_row_int(0, 1);
				new door = cache_get_row_int(0, 2);
				new spawntype = cache_get_row_int(0, 3);
				new jail = 0;
				new mdc_records = 0;
				new driver = cache_get_row_int(0, 4);
				new driverlic[5];

				if(spawntype > 2 && spawntype <=4)
				{
					new d_id = GetDoorByUid(door);
					format(adress, sizeof(adress), "%s", Door[d_id][door_name]);
				}
				else
				{
					format(adress, sizeof(adress), "none");
				}

				if((doc & DOCUMENT_DRIVE))
				{
					format(driverlic, sizeof(driverlic), "yes");
				}	
				else
				{
					format(driverlic, sizeof(driverlic), "none");
				}

				new carrows, carfields;
				mysql_query(mySQLconnection, sprintf("SELECT vehicle_model, vehicle_register FROM ipb_vehicles WHERE `vehicle_ownertype` = '1' AND `vehicle_owner` = '%d' ", driver));
				cache_get_data(carrows, carfields);

				if(carrows)
				{
					for(new row = 0; row != carrows; row++)
					{
						new register[10];
						new model = cache_get_row_int(row, 0);
						cache_get_row(row, 1, register);
						format(list_cars, sizeof(list_cars), "%s~g~~h~%s~w~ - %s~n~", list_cars, VehicleNames[model-400], register);
					}
				}
				else
				{
					format(list_cars, sizeof(list_cars), "~g~~h~ none~w~~n~");
				}

				format(list_mdc, sizeof(list_mdc), "~p~Mobile~w~ Data Computer~n~~n~Name: %s~n~Age: %d~n~Adress: %s~n~Arrest count: %d~n~Registry count: %d~n~Driver license: %s~n~~n~~b~~h~Vehicles:~n~%s", suspect, age, adress, jail, mdc_records, driverlic, list_cars);
				TextDrawSetString(Tutorial[playerid], list_mdc);
				TextDrawShowForPlayer(playerid, Tutorial[playerid]);
			}
			else
			{
				SendGuiInformation(playerid, "MDC - find person", "Cannot find anything about this person.");
			}
		}

		case DIALOG_MDC_FIND_VEHICLE:
		{
			if(!response) return 1;

			new rows, fields, register[10];

			if(strlen(inputtext) < 2)
			{
				SendGuiInformation(playerid, "MDC - find vehicle", "Minimum 2 letters.");
				return 1;
			}

			mysql_escape_string(inputtext, register, mySQLconnection, 256);
			mysql_query(mySQLconnection, sprintf("SELECT vehicle_model, vehicle_color1, vehicle_color2, vehicle_ownertype, vehicle_owner FROM ipb_vehicles WHERE `vehicle_register` = '%s' LIMIT 1", register));
			cache_get_data(rows, fields);

			if(rows)
			{
				new list_mdc[768], ownerdata[64];

				new model = cache_get_row_int(0, 0);
				new color1 = cache_get_row_int(0, 1);
				new color2 = cache_get_row_int(0, 2);
				new ownertype = cache_get_row_int(0, 3);
				new owner = cache_get_row_int(0, 4);
				new wanted[32];

				if(ownertype != 0)
				{
					if(ownertype == VEHICLE_OWNER_TYPE_GROUP)
					{
						new gid = GetGroupByUid(owner);
						if(gid == -1) return format(ownerdata, sizeof(ownerdata), "none");
						format(ownerdata, sizeof(ownerdata), "%s", Group[gid][group_name]);
					}
					else if(ownertype == VEHICLE_OWNER_TYPE_PLAYER)
					{
						new prows, pfields, ownername[32];
						mysql_query(mySQLconnection, sprintf("SELECT char_name FROM ipb_characters WHERE char_uid = '%d' LIMIT 1", owner));
						cache_get_data(prows, pfields);

						if(prows)
						{
							cache_get_row(0, 0, ownername);
							format(ownerdata, sizeof(ownerdata), "%s", ownername);
						}
						else
						{
							format(ownerdata, sizeof(ownerdata), "none");
						}
					}
				}
				else
				{
					format(ownerdata, sizeof(ownerdata), "brak");
				}

				format(wanted, sizeof(wanted), "nie");
				format(list_mdc, sizeof(list_mdc), "~p~Mobile~w~ Data Computer~n~~n~Car model: %s~n~Colors: %d/%d~n~Owner: %s~n~Wanted: %s", VehicleNames[model-400], color1, color2, ownerdata, wanted);
				TextDrawSetString(Tutorial[playerid], list_mdc);
				TextDrawShowForPlayer(playerid, Tutorial[playerid]);
			}
			else
			{
				SendGuiInformation(playerid, "MDC - find vehicle", "Cannot find any informations about this plates.");
			}
		}
		
		case DIALOG_SERVICES:
		{
			if(!response) return 1;
			switch(listitem)
			{
				case 0:
				{
					new rows, fields;
					mysql_query(mySQLconnection, sprintf("SELECT game_area_objects FROM ipb_members WHERE member_id = %d", pGlobal[playerid][glo_id]));
					cache_get_data(rows, fields);

					new objects = cache_get_row_int(0, 0);
					if(objects == 0) SendGuiInformation(playerid, "Information", "You don't have bought area objects service.");

					new a_id = pInfo[playerid][player_area];
					if(a_id > 0 )
					{
						if(Area[a_id][area_owner_type] != AREA_OWNER_TYPE_PLAYER)
						{
							SendGuiInformation(playerid, "Information", "This area is not yours.");
							return 1;
						}

						if(Area[a_id][area_owner] != pInfo[playerid][player_id])
						{
							SendGuiInformation(playerid, "Information", "This area is not yours.");
							return 1;
						}

						Area[a_id][area_objects_limit] += objects;
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_areas SET area_objects = %d WHERE area_uid = %d", Area[a_id][area_objects_limit], Area[a_id][area_uid]));
					}
					else
					{
						SendGuiInformation(playerid, "Information", "You are not in any area.");
					}
				}
				case 1:
				{
					SendGuiInformation(playerid, "Information", "Use /door managment to add objects to doors.");
				}
				case 2:
				{
					if(!IsPlayerVip(playerid)) return SendGuiInformation(playerid, "Information", "You don't have premium account.");

					new rows, fields;
					mysql_query(mySQLconnection, sprintf("SELECT char_visible FROM ipb_characters WHERE char_uid = %d", pInfo[playerid][player_id]));
					cache_get_data(rows, fields);

					new visible = cache_get_row_int(0, 0);

					if(visible == 0)
					{
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_visible = 1 WHERE char_uid = %d", pInfo[playerid][player_id]));
						SendGuiInformation(playerid, "Information", "Char is now hidden.");
					}
					else
					{
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_visible = 0 WHERE char_uid = %d", pInfo[playerid][player_id]));
						SendGuiInformation(playerid, "Information", "Char is now visible.");
					}
				}
				case 3:
				{
					new rows, fields;
					mysql_query(mySQLconnection, sprintf("SELECT game_char_block_three FROM ipb_members WHERE member_id = %d", pGlobal[playerid][glo_id]));
					cache_get_data(rows, fields);

					new block = cache_get_row_int(0, 0);

					if(block == 0)
					{
						SendGuiInformation(playerid, "Information", "You don't have blockade service.");
					}
					else
					{
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET game_char_block_three = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
						SendGuiInformation(playerid, "Information", "Char blocked.");
						CharacterKill(playerid, playerid, "Char block (cShop)");
					}
				}
				case 4: 
				{
					SendGuiInformation(playerid, "Information", "Function available in usercp (on website).");
				}
				case 5:
				{
					SendGuiInformation(playerid, "Information", "To buy premium vehicle go to dealership.");
				}
				case 6:
				{
					SendGuiInformation(playerid, "Information", "Ask administrator about this service.");
				}
			}
		}

		case DIALOG_STATS:
		{
			if(!response) return 1;
			switch(listitem)
			{
				case 16:
				{
					new rows, fields, list_anims[256];
					mysql_query(mySQLconnection, "SELECT anim_command, anim_uid FROM ipb_anim WHERE anim_command LIKE '.idz%' ORDER BY `anim_command` ASC");
					
					cache_get_data(rows, fields);
					
					for(new row = 0; row != rows; row++)
					{
						new tmp[30];
						new uid = cache_get_row_int(row, 1);
						cache_get_row(row, 0, tmp);
						
						format(list_anims, sizeof(list_anims), "%s%d\t%s\n", list_anims, uid, tmp);
					}

					if(strlen(list_anims) > 0)
					{
						ShowPlayerDialog(playerid, DIALOG_WALKING_ANIM, DIALOG_STYLE_LIST, "Walking animations", list_anims, "Choose", "Cancel");
					}
					else
					{
						SendGuiInformation(playerid, "Information", "Animations not loaded. Report to the administration.");
					}
					return 1;
				}
				case 17:
				{
					SendGuiInformation(playerid, ""guiopis"Alert", "Walking anim off.");

					pInfo[playerid][player_walking_anim]=0;
					mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_walking_anim = 0 WHERE char_uid = %d", pInfo[playerid][player_id]));
				}
				case 18:
				{
					if(!IsPlayerAttachedObjectSlotUsed(playerid, ATTACH_SLOT_WEAPON)) return SendGuiInformation(playerid, "Information", "You don't have attached weapon.");
					EditAttachedObject(playerid, ATTACH_SLOT_WEAPON);
				}
				case 19:
				{
					new opt1[5];
					new opt2[5];
					new opt3[5];
					new opt4[5];
					new opt5[5];

					new rows, fields;
					mysql_query(mySQLconnection, sprintf("SELECT game_door_objects, game_area_objects, game_unique_vehicle, game_area, game_char_block_three, game_char_name_change FROM ipb_members WHERE member_id = %d", pGlobal[playerid][glo_id]));
					cache_get_data(rows, fields);

					new objects = cache_get_row_int(0, 0);
					new area_objects = cache_get_row_int(0, 1);
					new uvehicle = cache_get_row_int(0, 2);
					new uarea = cache_get_row_int(0, 3);
					new cblock = cache_get_row_int(0, 4);
					new cchange = cache_get_row_int(0, 5);

					if(cblock > 0)
					{
						format(opt2, sizeof(opt1), "yes");
					}
					else
					{
						format(opt2, sizeof(opt1), "no");
					}

					if(cchange > 0)
					{
						format(opt3, sizeof(opt1), "yes");
					}
					else
					{
						format(opt3, sizeof(opt1), "no");
					}

					if(uvehicle > 0)
					{
						format(opt4, sizeof(opt1), "yes");
					}
					else
					{
						format(opt4, sizeof(opt1), "no");
					}

					if(uarea > 0)
					{
						format(opt5, sizeof(opt1), "yes");
					}
					else
					{
						format(opt5, sizeof(opt1), "no");
					}

					if(IsPlayerVip(playerid))
					{
						format(opt1, sizeof(opt1), "yes");
					}
					else
					{
						format(opt1, sizeof(opt1), "no");
					}

					new list_premium[256];
					format(list_premium, sizeof(list_premium), "Service\tState\nArea objects\t%d\nDoor objects\t%d\nHiding char\t%s\nChar block\t%s\nName change\t%s\nPremium vehicles\t%s\nOwn area\t%s", area_objects, objects, opt1, opt2, opt3, opt4, opt5);

					ShowPlayerDialog(playerid, DIALOG_SERVICES, DIALOG_STYLE_TABLIST_HEADERS, "Premium services", list_premium, "Use", "Close");
				}
			}
		}

		case DIALOG_WALKING_ANIM:
		{
			if(response)
		    {
		        new anim_uid = strval(inputtext), rows, fields;
		        mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_anim` WHERE `anim_uid` = '%d'", anim_uid));
				cache_get_data(rows, fields);
				
				if(rows)
				{
					pInfo[playerid][player_walking_anim]= cache_get_row_int(0, 0);
					cache_get_row(0, 2, pInfo[playerid][player_walking_lib], mySQLconnection, 32);
					cache_get_row(0, 3, pInfo[playerid][player_walking_name], mySQLconnection, 32);
					SendPlayerInformation(playerid, "~w~Walking anim ~p~choosed~w~.");
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_walking_anim` = %d, `char_walking_lib` = '%s', `char_walking_name`= '%s'  WHERE `char_uid` = %d", pInfo[playerid][player_walking_anim], pInfo[playerid][player_walking_lib], pInfo[playerid][player_walking_name], pInfo[playerid][player_id]));
				}
				else
				{
					PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
				}
		        return 1;
		    }
		    else
		    {
		        return 1;
		    }
		}
		
		case DIALOG_HELPER:
		{
			if( !response ) return 1;

			switch(listitem)
			{
				case 0:
				{
					SendGuiInformation(playerid, "Information", "To send question to helpers, use /help [text]");
				}
				case 1:
				{
					return cmd_helpers(playerid, "");
				}
			}
		}

		case DIALOG_HELP:
		{
			if( !response ) return 1;

			switch(listitem)
			{
				case 0:
				{
					ShowPlayerDialog(playerid, DIALOG_HELPER, DIALOG_STYLE_LIST, ""guiopis"Help center", "Ask helpers\nCheck helpers online", "Choose", "Exit");
				}
				case 1:
				{
					new str[1024];
	                format(str, sizeof(str), "/v - vehicle managment.\n/g - group managment\n/door - door managment\n/stats - player stats and options\n/anim - animations list\n/inv - inventory system\n/inv pickup - pickup item from ground\n/kill - kill your character (permanently)\n/login - relog, without need to quit game\n");
	                format(str, sizeof(str), "%s/qs - quit server, saves your last position\n/area - area managment\n/desc - set your description", str);
	                ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, ""guiopis"Commands", str, "OK", "");
				}
				case 2:
				{
					new str[256];
					format(str, sizeof(str), "You can get a job near City Hall (Pershing Square).\nYou can also join player's business.");
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Work", str, "OK", "");
				}
				case 3:
				{
					new str[256];
					format(str, sizeof(str), "You can buy vehicle in dealership at Market.\nUse /dealership command inside.\nYou can get it position by /v track.");
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Vehicles", str, "OK", "");
				}
				case 4:
				{
					new list_anims[1024];
					foreach(new anim_id: Anims)
					{
					    format(list_anims, sizeof(list_anims), "%s\n%s", list_anims, AnimInfo[anim_id][aCommand]);
					}
					if(strlen(list_anims))
					{
					    ShowPlayerDialog(playerid, DIALOG_ANIMATIONS, DIALOG_STYLE_LIST, "Animations list:", list_anims, "Start", "Close");
					}
					else
					{
					    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Alert", "Animations not laoded. Report it to administration", "OK", "");
					}
				}
				case 5:
				{
					new str[1024];
	                format(str, sizeof(str), "/av - vehicles managment.\n/ag - group managment\n/ad- door managment\n/stats [playerid] - player stats\n/area - area managment\n/ai - item managment\n/aproduct - product managment\n");
	                format(str, sizeof(str), "%s/block - blockades\n/gs - set score\n/sethours - set hours online\n/abus - bus stops managment\n/duty - admin duty\n/res - reset player position to spawn\n/apoint - save last position\n/glob - global chat\n/bw - brutally wounded managment", str);
	                ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, ""guiopis"Admin commands", str, "OK", "");
				}
			}
		}

		case DIALOG_WEAZEL:
		{
			new zgloszenie[MAX_PLAYERS];
			new number = pInfo[playerid][player_dialog_tmp1];
			if( !response )
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				return 1;
			}

			if(strlen(inputtext) < 4)
			{
				SendGuiInformation(playerid, "Information", "Input too short.");
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				return 1;
			}

			if(strlen(inputtext) > 110)
			{
				SendGuiInformation(playerid, "Information", "Input too short.");
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				return 1;
			}

			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
			RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);

			ProxMessage(playerid, inputtext, PROX_PHONE);

			foreach(new p : Player)
			{
				if(pInfo[p][player_duty_gid] >= 0)
				{
					if(Group[pInfo[p][player_duty_gid]][group_type] == GROUP_TYPE_SN)
					{
						zgloszenie[p]=1;
					}
				}
				if(zgloszenie[p]==1)
				{
					SendFormattedClientMessage(p, COLOR_GOLD, "Call from %d: %s", number, inputtext);
					zgloszenie[p]=0;
				}
			}
		}

		case DIALOG_CBELL:
		{
			if(!response)
			{
				return 1;
			}
			else if(response)
			{
				switch(listitem)
				{
					case 0:
					{
						if(pInfo[playerid][player_money]<30)
		                {
	                        SendGuiInformation(playerid, ""guiopis"Alert", "You don't have enough money.");
	                        return 1;
		                }
						GivePlayerMoney(playerid, -30);
						if(pInfo[playerid][player_health] <= 40)
						{
							pInfo[playerid][player_health]+=60;
						}
						else
						{
							pInfo[playerid][player_health]= 100;
						}

						SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
						ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Drive-thru", "You have bough a food, your has been HP increased.", "OK", "");
					}
					case 1:
					{
						if(pInfo[playerid][player_money]<20)
		                {
	                        SendGuiInformation(playerid, ""guiopis"Alert", "You don't have enough money.");
	                        return 1;
		                }
						GivePlayerMoney(playerid, -20);
						if(pInfo[playerid][player_health] <= 60)
						{
							pInfo[playerid][player_health]+=40;
						}
						else
						{
							pInfo[playerid][player_health]= 100;
						}
						
						SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
						ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Drive-thru", "You have bough a food, your has been HP increased.", "OK", "");
					}
					case 2:
					{
						if(pInfo[playerid][player_money]<15)
		                {
	                        SendGuiInformation(playerid, ""guiopis"Alert", "You don't have enough money.");
	                        return 1;
		                }
						GivePlayerMoney(playerid, -15);
						if(pInfo[playerid][player_health] <= 75)
						{
							pInfo[playerid][player_health]+=25;
						}
						else
						{
							pInfo[playerid][player_health]= 100;
						}
						
						SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
						ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Drive-thru", "You have bough a food, your has been HP increased.", "OK", "");
					}
					case 3:
					{
						if(pInfo[playerid][player_money]<21)
		                {
	                        SendGuiInformation(playerid, ""guiopis"Alert", "You don't have enough money.");
	                        return 1;
		                }
						GivePlayerMoney(playerid, -21);
						if(pInfo[playerid][player_health] <= 55)
						{
							pInfo[playerid][player_health] += 45;
						}
						else
						{
							pInfo[playerid][player_health]= 100;
						}
						
						SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
						ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, ""guiopis"Drive-thru", "You have bough a food, your has been HP increased.", "OK", "");
					}
				}
			}
		}

		case DIALOG_ANIMATIONS:
		{
			if(response)
		    {
		    	if(GetPVarInt(playerid, "AnimHitPlayerGun") == 1) return 1;
	    	 	new dg_value = DynamicGui_GetValue(playerid, listitem), dg_data = DynamicGui_GetDataInt(playerid, listitem);

            	if( dg_value == DG_ANIMS )
            	{
			        new anim_id = dg_data;
			        
			        if(!AnimInfo[anim_id][aAction])
			        {
			        	ApplyAnimation(playerid, AnimInfo[anim_id][aLib], AnimInfo[anim_id][aName], AnimInfo[anim_id][aSpeed], AnimInfo[anim_id][aOpt1], AnimInfo[anim_id][aOpt2], AnimInfo[anim_id][aOpt3], AnimInfo[anim_id][aOpt4], AnimInfo[anim_id][aOpt5], 1);
					}
					else
					{
					    SetPlayerSpecialAction(playerid, AnimInfo[anim_id][aAction]);
					}

					pInfo[playerid][player_looped_anim]= true;
				}
		    }
		    else
		    {
		        return 1;
		    }
		}

		case DIALOG_SELECT_BUSSTOP:
		{
			if(response)
		    {
	     		new busstop_id = pInfo[playerid][player_bus_start], busstop_id2, busstop_name[32], string[256];
		        sscanf(inputtext, "ds[32]", busstop_id2, busstop_name);

				new Float:distance = floatround(floatsqroot((BusStopData[busstop_id][bPosX] - BusStopData[busstop_id2][bPosX]) * (BusStopData[busstop_id][bPosX] - BusStopData[busstop_id2][bPosX]) + (BusStopData[busstop_id][bPosY] - BusStopData[busstop_id2][bPosY]) * (BusStopData[busstop_id][bPosY] - BusStopData[busstop_id2][bPosY])));

		        pInfo[playerid][player_bus_travel] = busstop_id2;
		        
		        pInfo[playerid][player_bus_time] = floatround(distance, floatround_floor) / 10;
		        pInfo[playerid][player_bus_price] = floatround(distance, floatround_floor) / 25;

		        format(string, sizeof(string), "Bus ride: %s > %s.\n\nTime: %ds\nCost: $%d\n\nAre you sure?", BusStopData[busstop_id][bName], BusStopData[busstop_id2][bName], pInfo[playerid][player_bus_time], pInfo[playerid][player_bus_price]);
		        ShowPlayerDialog(playerid, DIALOG_ACCEPT_TRAVEL, DIALOG_STYLE_MSGBOX, "Bus ride", string, "Go", "Exit");
		    	return 1;
			}
			else
			{
			    return 1;
			}
		}

		case DIALOG_ACCEPT_TRAVEL:
		{
			if(response)
		    {
	  			new price = pInfo[playerid][player_bus_price];
				if(pInfo[playerid][player_money] < price)
				{
					SendGuiInformation(playerid, ""guiopis"Alert", "You don't have enough money.");
				    return 1;
				}
				GivePlayerMoney(playerid, -price);

				FreezePlayer(playerid, 2500);
				RP_PLUS_SetPlayerPos(playerid, 2022.0273, 2235.2402, 2103.9936);
				SetPlayerFacingAngle(playerid, 0);
				SetCameraBehindPlayer(playerid);
				SetPlayerInterior(playerid, 1);
				SendPlayerInformation(playerid, "~w~Wait for destination. You can be ~y~AFK~w~.", 4000);
				
				PlayerPlaySound(playerid, 1076, 0.0, 0.0, 0.0);
				pInfo[playerid][player_bus_ride] = true;
		        return 1;
		    }
		    else
		    {
	   			pInfo[playerid][player_bus_start] = 0;
				pInfo[playerid][player_bus_travel] = 0;
				
				pInfo[playerid][player_bus_time] = 0;
				pInfo[playerid][player_bus_price] = 0;
		        return 1;
		    }
		}

		case DIALOG_AS:
		{
			if( !response ) return 1;

			CharacterKill(playerid, playerid, "Death");
			return 1;
		}

		case DIALOG_BANKOMAT:
		{
			if( !response ) return 1;

			switch(listitem)
			{
				case 0:
				{
					new str[78];
					format(str, sizeof(str), "Balance: $%d.\nAccount number: %d.", pInfo[playerid][player_bank_money], pInfo[playerid][player_bank_number]);
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Bank account", str, "OK", "");
				}
				case 1:
				{
					ShowPlayerDialog(playerid, DIALOG_BANK_DEPOSIT, DIALOG_STYLE_INPUT, "Deposit", "Please input amount of cash to deposit.", "Deposit", "");
				}
				case 2:
				{
					ShowPlayerDialog(playerid, DIALOG_BANK_WITHDRAW, DIALOG_STYLE_INPUT, "Withdraw", "Please input amount of cash to withdraw.", "Withdraw", "");
				}
				case 3:
				{
					if(pInfo[playerid][player_hours] < 1) return SendGuiInformation(playerid, ""guiopis"Alert", "You can access bank transfers after 1 hour online.");
					ShowPlayerDialog(playerid, DIALOG_BANK_PRZELEW, DIALOG_STYLE_INPUT, "Bank transfer", "Please enter amount for transfer.", "Next", "Exit");
				}
				case 4:
				{
					if(gettime() < pInfo[playerid][player_last_payday] + 12*3600 )
					{
						new nextpay = pInfo[playerid][player_last_payday] + 12*3600;
						new payHour, payMinute, temp;

						TimestampToDate(nextpay, temp, temp, temp, payHour, payMinute, temp, 1);

						if(payHour == 24)
						{
							SendGuiInformation(playerid, "Information", sprintf("You've already got payday today.\nNext payday at 01:%02d.", payMinute));
						}
						else
						{
							SendGuiInformation(playerid, "Information", sprintf("You've already got payday today.\nNext payday at %02d:%02d.", payHour+1, payMinute));
						}
						return 1;
					}

					new payday = GetPlayerPayday(playerid);

					if(payday <= 0)
					{
						SendGuiInformation(playerid, "Maze Bank - payday", "You don't have any setted paydays. Ask your group leader.");
						return 1;
					}

					new rows, fields, dutytime[200], start[200], end[200];

					mysql_query(mySQLconnection, sprintf("SELECT session_start, session_end FROM ipb_game_sessions WHERE session_owner = %d AND session_type = %d", pInfo[playerid][player_id], SESSION_TYPE_DUTY));
					cache_get_data(rows, fields);

					if(!rows) return SendGuiInformation(playerid, "Information", "Cannot find any working sessions.");

					new overall;
					for(new row = 0; row != rows; row++)
					{
						start[row] = cache_get_row_int(row, 0);
						end[row] = cache_get_row_int(row, 1);
						dutytime[row] = end[row] - start[row];
						overall += dutytime[row];
					}

					new count = CountPlayerGroups(playerid);

					new minutes = floatround(overall/60, floatround_floor);

					new payend = minutes/count;

					if(payend >= 15)
					{
						GivePlayerMoney(playerid, payday);

						new string[256], header[64];
						format(header, sizeof(header), "~w~Maze ~p~Bank~w~ - payday~n~~n~");

						for(new i=0;i<5;i++)
						{
							if( pGroup[playerid][i][pg_id] > -1 )
							{
								format(string, sizeof(string), "%s~>~%s - $%d~n~", string, GetGroupTag(pGroup[playerid][i][pg_id]), pGroup[playerid][i][pg_rank_payment]);
							}
						}

						if(pInfo[playerid][player_renting] > 0)
						{
							new d_id = GetDoorByUid(pInfo[playerid][player_renting]);
							if(d_id != -1)
							{
								if(Door[d_id][door_rent] >= payday)
								{
									format(string, sizeof(string), "%s~>~Rental: -$%d~n~", string, Door[d_id][door_rent]);
									payday = payday - Door[d_id][door_rent];
								}
							}
						}

						format(string, sizeof(string), "%s%s~n~Final amount: ~g~$%d", header, string, payday);

						SendPlayerInformation(playerid, string, 10000);
						SendClientMessage(playerid, COLOR_YELLOW, sprintf("> (SMS) [755] Maze Bank: New transfer to your account. Amount: $%d.", payday));

						pInfo[playerid][player_last_payday] = gettime();
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_last_payday = %d WHERE char_uid = %d", gettime(), pInfo[playerid][player_id]));
						mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_game_sessions WHERE session_owner = %d AND session_type = %d", pInfo[playerid][player_id], SESSION_TYPE_DUTY));
					}
					else
					{
						SendGuiInformation(playerid, "Maze Bank - payday", sprintf("You didnt made average (15 minutes for 1 group slot).\nYour average: %dmin.", payend));
					}
				}
				case 5:
				{
					if(pInfo[playerid][player_job] == 0) return SendGuiInformation(playerid, "Information", "You don't have any job.");
					if(pInfo[playerid][player_job_cash] <= 0) return SendGuiInformation(playerid, "Information", "You don't have any cash to payout.");
					if(gettime() < pInfo[playerid][player_last_work] + 12*3600 )
					{
						new nextpay = pInfo[playerid][player_last_work] + 12*3600;
						new payHour, payMinute, temp;

						TimestampToDate(nextpay, temp, temp, temp, payHour, payMinute, temp, 1);

						if(payHour == 24)
						{
							SendGuiInformation(playerid, "Information", sprintf("You've already got payday today.\nNext payday at 01:%02d.", payMinute));
						}
						else
						{
							SendGuiInformation(playerid, "Information", sprintf("You've already got payday today.\nNext payday at %02d:%02d.", payHour+1, payMinute));
						}
						return 1;
					}

					if(pInfo[playerid][player_job_cash] <= 350)
					{
						new string[128];
						GivePlayerMoney(playerid, pInfo[playerid][player_job_cash]);
						format(string, sizeof(string), "~w~Maze ~p~Bank~w~ - payday~n~~n~Extra job - $%d", pInfo[playerid][player_job_cash]);
						SendPlayerInformation(playerid, string, 10000);
						pInfo[playerid][player_last_work] = gettime();
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_last_work = %d WHERE char_uid = %d", gettime(), pInfo[playerid][player_id]));
					}
					else if(pInfo[playerid][player_job_cash] > 350)
					{
						GivePlayerMoney(playerid, 350);
						SendPlayerInformation(playerid, "~w~Maze ~p~Bank~w~ - payday~n~~n~Extra job - $350", 10000);
						pInfo[playerid][player_last_work] = gettime();
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_last_work = %d WHERE char_uid = %d", gettime(), pInfo[playerid][player_id]));
					}
				}
			}
		}

		case DIALOG_BANK_PRZELEW:
		{
			if( !response ) return 1;

			new kwota = strval(inputtext);
			if(kwota <= 0) return SendGuiInformation(playerid, ""guiopis"Alert", "Wrong input.");
			if(kwota > pInfo[playerid][player_bank_money]) return SendGuiInformation(playerid, ""guiopis"Alert", "You don't have enouh money on bank account");

			DynamicGui_Init(playerid);
			DynamicGui_SetDialogValue(playerid, kwota);
			ShowPlayerDialog(playerid, DIALOG_BANK_NUMER, DIALOG_STYLE_INPUT, "Bank transfer", "Please input target's account number.", "Transfer", "Exit");
		}

		case DIALOG_BANK_NUMER:
		{
			if( !response ) return 1;

			if(pInfo[playerid][player_hours] < 1) return SendGuiInformation(playerid, "Information", "You will be able to access transfers after 1 hour online.");
			new numer = strval(inputtext);
			new uid;
			new rows, fields;

			mysql_query(mySQLconnection, sprintf("SELECT char_uid FROM ipb_characters WHERE char_banknumb = %d", numer));
			cache_get_data(rows, fields);

			if(rows)
			{
				uid = cache_get_row_int(0, 0);

				if(uid == -1)
				{
					SendGuiInformation(playerid, ""guiopis"Alert", "Wrong account number.");
					return 1;
				}

				if(uid == pInfo[playerid][player_id])
				{
					SendGuiInformation(playerid, ""guiopis"Alert", "You can't transfer money to your own account.");
					return 1;
				}

				new kwota = DynamicGui_GetDialogValue(playerid);

				new player = GetPlayerByUid(uid);

				if(player == -1)
				{
					pInfo[playerid][player_bank_money] -= kwota;
					mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_bankcash = char_bankcash - %d WHERE char_uid = %d", kwota, pInfo[playerid][player_id]));

					mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_bankcash = char_bankcash + %d WHERE char_uid = %d", kwota, uid));
				}
				else
				{
					pInfo[playerid][player_bank_money] -= kwota;
					pInfo[player][player_bank_money] += kwota;

					SendClientMessage(player, COLOR_YELLOW, sprintf("> (SMS) [755] Maze Bank: New transfer for your account. Value: $%d.", kwota));
					mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_bankcash = char_bankcash + %d WHERE char_uid = %d", kwota, uid));
					mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_bankcash = char_bankcash - %d WHERE char_uid = %d", kwota, pInfo[playerid][player_id]));
				}

				SendGuiInformation(playerid, "Maze Bank", "Transfer done.");
			}
			else
			{
				SendGuiInformation(playerid, "Maze Bank", "Wrong account number.");
			}
		}

		case DIALOG_BANK_DEPOSIT:
		{
			if( !response ) return 1;

			new money=strval(inputtext);

			if(money<0) return KickAc(playerid, -1, "Bug abusing try (negative value)");


			if(pInfo[playerid][player_money]<money)
			{
				SendGuiInformation(playerid, ""guiopis"Alert", "You don't have enough money.");
			}
			else
			{
				pInfo[playerid][player_bank_money]+=money;
				GivePlayerMoney(playerid,-money);
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_bankcash = %d WHERE char_uid = %d", pInfo[playerid][player_bank_money], pInfo[playerid][player_id]));
				SendGuiInformation(playerid, ""guiopis"Alert", "Deposit succesfull.");
			}
		}

		case DIALOG_BANK_WITHDRAW:
		{
			if( !response ) return 1;

			new money=strval(inputtext);

			if(money<0)
			{
				KickAc(playerid, -1, "Bug abusing try (negative value)");
				return 1;
			}

			if(pInfo[playerid][player_bank_money]<money)
			{
				SendGuiInformation(playerid, ""guiopis"Alert", "You don't have enough money.");
			}
			else
			{
				pInfo[playerid][player_bank_money]-=money;
				GivePlayerMoney(playerid,money);
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_bankcash = %d WHERE char_uid = %d", pInfo[playerid][player_bank_money], pInfo[playerid][player_id]));
				SendGuiInformation(playerid, ""guiopis"Alert", "Withdraw succesfull.");
			}
		}

		case DIALOG_GIVE_CREW:
		{
			if( !response ) return 1;

			new targetid = DynamicGui_GetDialogValue(playerid);
			if( !IsPlayerConnected(targetid) || !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Alert", "Wrong player id.");

			new flag = DynamicGui_GetValue(playerid, listitem);

			if( HasCrewFlag(targetid, flag) )
			{
				// usuwamy flage
				pGlobal[targetid][glo_perm] -= flag;
			}
			else
			{
				// dodajemy flage
				if( flag == CREW_FLAG_GM || flag == CREW_FLAG_ADMIN || flag == CREW_FLAG_ADMIN_ROOT )
				{
					if( HasCrewFlag(targetid, CREW_FLAG_GM) || HasCrewFlag(targetid, CREW_FLAG_ADMIN) || HasCrewFlag(targetid, CREW_FLAG_ADMIN_ROOT) )
					{
						return SendGuiInformation(playerid, ""guiopis"Alert", "This player already has an admin rank.");
					}
				}

				pGlobal[targetid][glo_perm] += flag;
			}

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_game_admin_perm` = %d WHERE `member_id` = %d", pGlobal[targetid][glo_perm], pGlobal[targetid][glo_id]));


			return cmd_aflags(playerid, sprintf("%d", targetid));
		}

		case DIALOG_GIVE_FLAG:
		{
			if( !response ) return 1;

			new targetid = DynamicGui_GetDialogValue(playerid);
			if( !IsPlayerConnected(targetid) || !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Alert", "Wrong player id.");

			new flag = DynamicGui_GetValue(playerid, listitem);

			if( PlayerHasFlag(targetid, flag) )
			{
				// usuwamy flage
				pInfo[targetid][player_flags] -= flag;
			}
			else
			{
				pInfo[targetid][player_flags] += flag;
			}

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_flags` = %d WHERE `char_uid` = %d", pInfo[targetid][player_flags], pInfo[targetid][player_id]));


			return cmd_pflags(playerid, sprintf("%d", targetid));
		}

		case DIALOG_AREA_FLAGS:
		{
			if( !response ) return 1;

			new a_id = DynamicGui_GetDialogValue(playerid);

			new flag = DynamicGui_GetValue(playerid, listitem);

			if( AreaHasFlag(a_id, flag) )
			{
				// usuwamy flage
				Area[a_id][area_flags] -= flag;
			}
			else
			{
				// dodajemy flage
				Area[a_id][area_flags] += flag;
			}

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_areas` SET `area_flags` = %d WHERE `area_uid` = %d", Area[a_id][area_flags], Area[a_id][area_uid]));


			return cmd_area(playerid, sprintf("flags %d", a_id));
		}

		case DIALOG_GROUP_FLAGS:
		{
			if( !response ) return 1;

			new g_id = DynamicGui_GetDialogValue(playerid);

			new flag = DynamicGui_GetValue(playerid, listitem);

			if( GroupHasFlag(g_id, flag) )
			{
				// usuwamy flage
				Group[g_id][group_flags] -= flag;
			}
			else
			{
				// dodajemy flage
				Group[g_id][group_flags] += flag;
			}

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_game_groups` SET `group_flags` = %d WHERE `group_uid` = %d", Group[g_id][group_flags], Group[g_id][group_uid]));


			return cmd_agroup(playerid, sprintf("flags %d", Group[g_id][group_uid]));
		}

		case DIALOG_DRZWI:
		{
			TextDrawHideForPlayer(playerid, Tutorial[playerid]);

			if( !response )
			{
				return 1;
			}

			new d_id = DynamicGui_GetDialogValue(playerid);

			switch( DynamicGui_GetValue(playerid, listitem) )
			{
				case DG_DRZWI_NAME:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;

					ShowPlayerDialog(playerid, DIALOG_DRZWI_NAME, DIALOG_STYLE_INPUT, "Door name change", "Please input new door name:", "Change", "Close");
				}

				case DG_DRZWI_SPAWN:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;

					ShowPlayerDialog(playerid, DIALOG_DRZWI_SPAWN, DIALOG_STYLE_MSGBOX, "Door position (inside)", "Do you really want to change inside door position for your current one?", "Cnage", "Close");
				}

				case DG_DRZWI_SPAWN_COORDS:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;

					ShowPlayerDialog(playerid, DIALOG_DRZWI_SPAWN_COORDS, DIALOG_STYLE_INPUT, "Door position (inside)", "Please input x, y,z. Separate it by comma:", "Change", "Close");
				}

				case DG_DRZWI_AUDIO:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;

					ShowPlayerDialog(playerid, DIALOG_DRZWI_AUDIO, DIALOG_STYLE_INPUT, "Door audio", "Please input audio link (or leave empty to turn off music):", "Change", "Close");
				}

				case DG_DRZWI_PAYMENT:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;

					ShowPlayerDialog(playerid, DIALOG_DRZWI_PAYMENT, DIALOG_STYLE_INPUT, "Door payment", "Please input amount of payment for enter:", "Change", "Close");
				}

				case DG_DRZWI_TIME:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;

					ShowPlayerDialog(playerid, DIALOG_DRZWI_TIME, DIALOG_STYLE_INPUT, "Door time", "Please input an hour (0-23):", "Change", "Close");
				}

				case DG_DRZWI_CLEAR:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;

					ShowPlayerDialog(playerid, DIALOG_DRZWI_CLEAR, DIALOG_STYLE_INPUT, "Delete interior", "Do you really want to remoe your mapping?\nInput YES for accept.", "Delete", "Close");
				}

				case DG_DRZWI_CARS:
				{
					Door[d_id][door_car_crosing] = !Door[d_id][door_car_crosing];
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_garage` = %d WHERE `door_uid` = %d", Door[d_id][door_car_crosing], Door[d_id][door_uid]));

					return cmd_door(playerid, "managment");
				}

				case DG_DRZWI_CLOSING:
				{
					Door[d_id][door_auto_closing] = !Door[d_id][door_auto_closing];
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_lock` = %d WHERE `door_uid` = %d", Door[d_id][door_auto_closing], Door[d_id][door_uid]));

					return cmd_door(playerid, "managment");
				}

				case DG_DRZWI_BUY:
				{
					new rows, fields, objects;
					mysql_query(mySQLconnection, sprintf("SELECT game_door_objects FROM ipb_members WHERE member_id = %d", pGlobal[playerid][glo_id]));
					cache_get_data(rows, fields);

					if(rows)
					{
						objects = cache_get_row_int(0, 0);
						if(objects == 0) return SendGuiInformation(playerid, "Information", "You don't have any premium objects.");

						Door[d_id][door_objects_limit] += objects;
						mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_objects` = %d WHERE `door_uid` = %d", Door[d_id][door_objects_limit], Door[d_id][door_uid]));
						mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `game_door_objects` = 0 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
						SendGuiInformation(playerid, "Information", sprintf("Added %d objects to your door.", objects));
					}

					return cmd_door(playerid, "managment");
				}

				case DG_DRZWI_FIX_BURN:
				{
					new price = Door[d_id][door_burned] * 50;
					if(price > pInfo[playerid][player_money]) return SendGuiInformation(playerid, "Information", "You don't have enough money.");

					Door[d_id][door_burned] = 0;
					GivePlayerMoney(playerid, -price);

					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_burned` = 0 WHERE `door_uid` = %d", Door[d_id][door_uid]));

					return cmd_door(playerid, "managment");
				}

				case DG_DRZWI_FIX:
				{
					new price = Door[d_id][door_destroyed] * 25;
					if(price > pInfo[playerid][player_money]) return SendGuiInformation(playerid, "Information", "You don't have enough money.");

					Door[d_id][door_destroyed] = 0;
					GivePlayerMoney(playerid, -price);

					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_destroyed` = 0 WHERE `door_uid` = %d", Door[d_id][door_uid]));

					return cmd_door(playerid, "managment");
				}

				case DG_DRZWI_MAGAZYN:
				{
					if(Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GROUP)
					{
						new gid = GetGroupByUid(Door[d_id][door_owner]);
						if(gid == -1) return cmd_door(playerid, "managment");
						new slot = GetPlayerGroupSlot(playerid, gid);
						if(slot == -1) return cmd_door(playerid, "managment");
						return cmd_g(playerid, sprintf("%d magazine", slot+1));
					}
					else
					{
						return cmd_door(playerid, "managment");
					}
				}

				case DG_DRZWI_MAP_LOAD:
				{
					//LoadObject(sprintf("WHERE `object_world` = %d", Door[d_id][door_spawn_vw]));
					SendGuiInformation(playerid, "Information", "This option is currently unavailable.");
				}

				case DG_DRZWI_CAMERA:
				{
					new rows, fields, list_suspects[256];
					mysql_query(mySQLconnection, sprintf("SELECT camera_suspects FROM ipb_game_cameras WHERE camera_door = %d", Door[d_id][door_uid]));
			    	cache_get_data(rows, fields);

			    	if(!rows) return SendGuiInformation(playerid, "Information", "Cameras didnt registered anything strange.");

			    	cache_get_row(0, 0, list_suspects);

			    	if(strlen(list_suspects))
			    	{
			    		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Suspects:", list_suspects, "OK", "");
			    	}
				}

				case DG_DRZWI_SCHOWEK:
				{
					new rows, fields, header[32], bag_item_uid, bag_item_name[40], list_items[1024];
					mysql_query(mySQLconnection, sprintf("SELECT item_uid, item_name FROM ipb_items WHERE item_owner = %d AND item_ownertype = %d", Door[d_id][door_uid], ITEM_OWNER_TYPE_DOOR));
			    	cache_get_data(rows, fields);

			    	for(new row = 0; row != rows; row++)
					{
					    bag_item_uid = cache_get_row_int(row, 0);
		   				cache_get_row(row, 1, bag_item_name, mySQLconnection, 40);

						format(list_items, sizeof(list_items), "%s\n%d\t%s", list_items, bag_item_uid, bag_item_name);
					}

					format(header, sizeof(header), "UID\tName\n");

					if(strlen(list_items))
					{
						format(list_items, sizeof(list_items), "%s%s", header, list_items);
						ShowPlayerDialog(playerid, DIALOG_SCHOWEK_TAKE, DIALOG_STYLE_TABLIST_HEADERS, "Items in doors:", list_items, "Take", "Cancel");
			      	}
			       	else
			       	{
			        	SendGuiInformation(playerid, ""guiopis"Information", "There are no items in these doors.");
					}
				}
			}
		}

		case DIALOG_DRZWI_NAME:
		{
			if( !response ) return cmd_door(playerid, "managment");

			if( strlen(inputtext) < 6 ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_NAME, DIALOG_STYLE_INPUT, "Door name change", "Please input new door name:\n\n"HEX_COLOR_LIGHTER_RED"Minimum 6 characters.", "Change", "Close");
			if( strlen(inputtext) > 30 ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_NAME, DIALOG_STYLE_INPUT, "Door name change", "Please input new door name:\n\n"HEX_COLOR_LIGHTER_RED"Max 30 characters.", "Change", "Close");
			if( strfind(inputtext, "~~") != -1 ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_NAME, DIALOG_STYLE_INPUT, "Door name change", "Please input new door name:\n\n"HEX_COLOR_LIGHTER_RED"Detected invalid characters.", "Change", "Close");

			new d_id = pInfo[playerid][player_dialog_tmp1];

			mysql_real_escape_string(inputtext, inputtext, mySQLconnection, 256);
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_name` = '%s' WHERE `door_uid` = %d", inputtext, Door[d_id][door_uid]));

			strcopy(Door[d_id][door_name], inputtext, 30);

			SendFormattedClientMessage(playerid, COLOR_GREY, "Door name has been changed to: %s.", inputtext);
			cmd_door(playerid, "managment");
		}

		case DIALOG_DRZWI_SPAWN:
		{
			if( !response ) return cmd_door(playerid, "managment");

			new d_id = pInfo[playerid][player_dialog_tmp1];

			GetPlayerPos(playerid, Door[d_id][door_spawn_pos][0],Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
			GetPlayerFacingAngle(playerid, Door[d_id][door_spawn_pos][3]);

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_exitx` = %f, `door_exity` = %f, `door_exitz` = %f, `door_exita` = %f WHERE `door_uid` = %d", Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], Door[d_id][door_spawn_pos][3], Door[d_id][door_uid]));

			SendClientMessage(playerid, COLOR_GOLD, "Inside position changed.");

			return cmd_door(playerid, "managment");
		}

		case DIALOG_DRZWI_SPAWN_COORDS:
		{
			if( !response ) return cmd_door(playerid, "managment");

			new d_id = pInfo[playerid][player_dialog_tmp1];

			if( sscanf(inputtext, "p<,>a<f>[4]", Door[d_id][door_spawn_pos]) ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_SPAWN_COORDS, DIALOG_STYLE_INPUT, "Door position change (inside)", "Please input x, y,z. Separate it by comma:\n\n"HEX_COLOR_LIGHTER_RED"Wrong format.", "Change", "Close");

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_exitx` = %f, `door_exity` = %f, `door_exitz` = %f, `door_exita` = %f WHERE `door_uid` = %d", Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], Door[d_id][door_spawn_pos][3], Door[d_id][door_uid]));

			SendClientMessage(playerid, COLOR_GOLD, "Inside position changed.");

			return cmd_door(playerid, "managment");
		}

		case DIALOG_DRZWI_AUDIO:
		{
			if( !response ) return cmd_door(playerid, "managment");

			new d_id = pInfo[playerid][player_dialog_tmp1];

			if( strlen(inputtext) < 3 )
			{
				SendClientMessage(playerid, COLOR_GOLD, "Audio stream off.");
				Door[d_id][door_audio] = EOS;
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_audiourl` = '-' WHERE `door_uid` = %d", Door[d_id][door_uid]));
				StopAudioStreamForPlayer(playerid);
				return 1;
			}

			sscanf(inputtext, "s[100]", Door[d_id][door_audio]);

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_audiourl` = '%s' WHERE `door_uid` = %d", Door[d_id][door_audio], Door[d_id][door_uid]));

			SendClientMessage(playerid, COLOR_GOLD, "New audio stream has been set up.");

			foreach(new p : Player)
			{
				if( GetPlayerVirtualWorld(p) == Door[d_id][door_spawn_vw] )
				{
					if( !isnull(Door[d_id][door_audio]) ) PlayAudioStreamForPlayer(playerid, Door[d_id][door_audio], 0);
					else StopAudioStreamForPlayer(playerid);
				}
			}

			return cmd_door(playerid, "managment");
		}

		case DIALOG_DRZWI_PAYMENT:
		{
			if( !response ) return cmd_door(playerid, "managment");

			new payment;
			if( sscanf(inputtext, "d", payment) ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_PAYMENT, DIALOG_STYLE_INPUT, "Door payment", "Please input amount of entrance fee:\n\nYou've typed wrong fee before.", "Change", "Close");
			if( payment < 0 ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_PAYMENT, DIALOG_STYLE_INPUT, "Door payment", "Please input amount of entrance fee:\n\nYou've typed wrong fee before.", "Change", "Close");

			new d_id = pInfo[playerid][player_dialog_tmp1];

			Door[d_id][door_payment] = payment;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_enterpay` = %d WHERE `door_uid` = %d", Door[d_id][door_payment], Door[d_id][door_uid]));

			return cmd_door(playerid, "managment");
		}

		case DIALOG_DRZWI_TIME:
		{
			if( !response ) return cmd_door(playerid, "managment");

			new time;
			if( sscanf(inputtext, "d", time) ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_PAYMENT, DIALOG_STYLE_INPUT, "Door time", "Please input an hour (0-23).\n\nYou've typed wrong hour before.", "Change", "Close");
			if( time > 24 ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_PAYMENT, DIALOG_STYLE_INPUT, "Door time", "Please input an hour (0-23).\n\nYou've typed wrong hour before.", "Change", "Close");
			if(time == 0) time = 24;

			new d_id = pInfo[playerid][player_dialog_tmp1];

			Door[d_id][door_time] = time;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_time` = %d WHERE `door_uid` = %d", Door[d_id][door_time], Door[d_id][door_uid]));

			SendClientMessage(playerid, COLOR_GOLD, "Door time has been changed. Enter building again.");

			return cmd_door(playerid, "managment");
		}

		case DIALOG_DRZWI_CLEAR:
		{
			if( !response ) return cmd_door(playerid, "managment");

			if( !strcmp(inputtext, "YES", true) )
			{ 
				new d_id = pInfo[playerid][player_dialog_tmp1];
				new o_id = INVALID_OBJECT_ID;

			 	for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
				{
					if(IsValidPlayerObject(playerid, player_object))
					{
						o_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);
						if( Object[o_id][object_owner_type] != OBJECT_OWNER_TYPE_DOOR ) continue;
						if( Object[o_id][object_owner] == Door[d_id][door_uid] )
						{
							mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_objects` WHERE `object_uid` = %d", Object[o_id][object_uid]));

							DestroyDynamicObject(o_id);

							for(new z=0; e_objects:z != e_objects; z++)
							{
						  		Object[o_id][e_objects:z] = 0;
						    }
						}
					}
				}
				SendClientMessage(playerid, COLOR_GOLD, "Your interior has been permanently removed.");
			}
			return cmd_door(playerid, "managment");
		}

		case DIALOG_ADRZWI_CHANGE_INTERIOR:
		{
			new d_id = DynamicGui_GetDialogValue(playerid);

			if( !response ) return 1;

			switch( DynamicGui_GetValue(playerid, listitem) )
			{
				case DG_DRZWI_CHANGE_INTERIOR_PREV:
				{
					DoorsDefaultInteriorsList(playerid, d_id, pInfo[playerid][player_dialog_tmp1]-1);
				}

				case DG_DRZWI_CHANGE_INTERIOR_NEXT:
				{
					DoorsDefaultInteriorsList(playerid, d_id, pInfo[playerid][player_dialog_tmp1]+1);
				}

				case DG_DRZWI_CHANGE_INTERIOR_ROW:
				{
					foreach(new p : Player)
					{
						if( GetPlayerVirtualWorld(p) == Door[d_id][door_spawn_vw] && Door[d_id][door_spawn_vw] != 0)
						{
							SetPlayerVirtualWorld(p, Door[d_id][door_vw]);
							SetPlayerInterior(p, Door[d_id][door_int]);

							RP_PLUS_SetPlayerPos(p, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
							SetPlayerFacingAngle(p, Door[d_id][door_pos][3]);

							SendClientMessage(p, COLOR_LIGHTER_RED, "Teleported to the door entrance due to administration changes inside.");
						}
					}

					if( DynamicGui_GetDataInt(playerid, listitem) == -1 )
					{
						Door[d_id][door_spawn_int] = 0;
						Door[d_id][door_spawn_pos][0] = Door[d_id][door_pos][0];
						Door[d_id][door_spawn_pos][1] = Door[d_id][door_pos][1];
						Door[d_id][door_spawn_pos][2] = Door[d_id][door_pos][2];
						Door[d_id][door_spawn_pos][3] = Door[d_id][door_pos][3];
					}
					else
					{
						new rows, fields;
						mysql_query(mySQLconnection, sprintf("SELECT interior, x, y, z, a FROM `ipb_default_interiors` WHERE `id` = %d", DynamicGui_GetDataInt(playerid, listitem)));
						cache_get_data(rows, fields);

						Door[d_id][door_spawn_int] = cache_get_row_int(0, 0);
						Door[d_id][door_spawn_pos][0] = cache_get_row_float(0, 1);
						Door[d_id][door_spawn_pos][1] = cache_get_row_float(0, 2);
						Door[d_id][door_spawn_pos][2] = cache_get_row_float(0, 3);
						Door[d_id][door_spawn_pos][3] = cache_get_row_float(0, 4);
					}

					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_exitint` = %d, `door_exitx` = %f, `door_exity` = %f, `door_exitz` = %f, `door_exita` = %f WHERE `door_uid` = %d", Door[d_id][door_spawn_int], Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], Door[d_id][door_spawn_pos][3], Door[d_id][door_uid]));

					SendFormattedClientMessage(playerid, COLOR_GOLD, "Interior has been changed [INTERIOR: %d, UID: %d, ID: %d].", Door[d_id][door_spawn_int], Door[d_id][door_uid], d_id);
				}
			}
		}

		case DIALOG_ADRZWI_PICKUP:
		{
			if( !response ) return 1;

			new d_id = DynamicGui_GetDialogValue(playerid);

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_pickupid` = %d WHERE `door_uid` = %d", DynamicGui_GetDataInt(playerid, listitem), Door[d_id][door_uid]));

			new uid = Door[d_id][door_uid];
			DeleteDoor(d_id, false);

			new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);
			SendFormattedClientMessage(playerid, COLOR_GOLD, "Pickup has been changed. [PICKUP: %d, UID: %d, ID: %d]", DynamicGui_GetDataInt(playerid, listitem), uid, did);
		}

		case DIALOG_AGRUPA_TYP:
		{
			if( !response ) return 1;

			new gid = DynamicGui_GetDialogValue(playerid), type = DynamicGui_GetDataInt(playerid, listitem);

			Group[gid][group_type] = type;
			Group[gid][group_flags] = GroupDefaultFlags[type];

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_game_groups` SET `group_type` = %d, `group_flags` = %d WHERE `group_uid` = %d", Group[gid][group_type], Group[gid][group_flags], Group[gid][group_uid]));

			SendGuiInformation(playerid, ""guiopis"Alert", sprintf("Group type has been changed. Also added standard flags. [TYPE: %d, FLAG: %d, UID: %d, ID: %d].", Group[gid][group_type], Group[gid][group_flags], Group[gid][group_uid], gid));
		}

		case DIALOG_ADMIN_FLAGS:
		{
			if(!response) return 1;
			new str[500];

			new targetid = DynamicGui_GetValue(playerid, listitem);

			if( !IsPlayerConnected(targetid) ) return 1;
			if( !pInfo[targetid][player_logged] ) return 1;

			format(str, sizeof(str), "%s%s01\tDoor managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_DOORS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
			
			format(str, sizeof(str), "%s%s02\tVehicle managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_VEHICLES)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
			
			format(str, sizeof(str), "%s%s03\tGroup managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_GROUPS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
			
			format(str, sizeof(str), "%s%s04\tArea managament\n", str, ((HasCrewFlag(targetid, CREW_FLAG_AREAS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
			
			format(str, sizeof(str), "%s%s05\tObject managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_EDITOR)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
			
			format(str, sizeof(str), "%s%s06\tItem managment\n", str, ((HasCrewFlag(targetid, CREW_FLAG_ITEMS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
			
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("Privileges of %s", pInfo[targetid][player_name]), str, "Close", "");
		}

		case DIALOG_CHAR_DESCRIPTION:
		{
			if( response == 0 ) return 1;
			new dg_value = DynamicGui_GetValue(playerid, listitem);

			if( dg_value == DG_CHAR_DESC_DELETE )
			{
				Update3DTextLabelText(pInfo[playerid][player_description_label], LABEL_DESCRIPTION, "");
				pInfo[playerid][player_description][0] = EOS;
				SendGuiInformation(playerid, "Information", "Your description has been removed.");
			}
			else if( dg_value == DG_CHAR_DESC_ADD)
			{
				ShowPlayerDialog(playerid, DIALOG_CHAR_DESCRIPTION_ADD, DIALOG_STYLE_INPUT, "Char description", "Please input content of your description (max. 110 chars)", "Set", "Close");
			}
			else if( dg_value == DG_CHAR_DESC_OLD )
			{
				new rows, fields;
				mysql_query(mySQLconnection ,sprintf("SELECT * FROM `ipb_descriptions` WHERE `uid` = %d", DynamicGui_GetDataInt(playerid, listitem)));
				cache_get_data(rows, fields);

				new oldDesc[256];
				cache_get_row(0, 1, oldDesc);

				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_descriptions` SET `last_used` = '%d' WHERE `uid`='%d'", gettime(), DynamicGui_GetDataInt(playerid, listitem)));

				strcopy(pInfo[playerid][player_description], oldDesc);

				Attach3DTextLabelToPlayer(pInfo[playerid][player_description_label], playerid, 0.0, 0.0, -0.7);
				Update3DTextLabelText(pInfo[playerid][player_description_label], LABEL_DESCRIPTION, BreakLines(oldDesc, "\n", 32));
				SendGuiInformation(playerid, "Information", "Your description has been changed.");
			}
		}

		case DIALOG_CHAR_DESCRIPTION_ADD:
		{
			if( response == 0 ) return cmd_desc(playerid, "");

			if(strlen(inputtext) > 110) return SendGuiInformation(playerid, "Information", "Input too long.");

			new inputOpis[256], rows, fields;
			strcopy(inputOpis, inputtext, 256);

			mysql_real_escape_string(inputOpis, inputOpis, mySQLconnection, 128);
			mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_descriptions` WHERE `text`='%s' AND `owner`='%d'", inputOpis, pInfo[playerid][player_id]));
			cache_get_data(rows, fields);

			if( rows )
			{
				new descUid = cache_get_row_int(0, 0);

				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_descriptions` SET `last_used`='%d' WHERE `uid`='%d'", gettime(), descUid));
			}
			else
			{
				mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_descriptions` (uid, owner, text, last_used) VALUES (null, '%d', '%s', '%d')", pInfo[playerid][player_id], inputOpis, gettime()));
			}

			strcopy(pInfo[playerid][player_description], inputOpis);

			Attach3DTextLabelToPlayer(pInfo[playerid][player_description_label], playerid, 0.0, 0.0, -0.7);
			Update3DTextLabelText(pInfo[playerid][player_description_label], LABEL_DESCRIPTION, BreakLines(inputOpis, "\n", 32));
			SendGuiInformation(playerid, "Information", "Your description has been changed.");
		}

		case DIALOG_GROUP_MAGAZYN:
		{
			new dg_value = DynamicGui_GetValue(playerid, listitem), dg_data = DynamicGui_GetDataInt(playerid, listitem);
			if( !response)
			{
				TextDrawHideForPlayer(playerid, Tutorial[playerid]);
				return 1;
			}

			if( response && dg_value == DG_ITEMS_ITEM_ROW )
			{
				TextDrawHideForPlayer(playerid, Tutorial[playerid]);
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, dg_data);
				ShowPlayerDialog(playerid, DIALOG_GROUP_MAGAZYN_ID, DIALOG_STYLE_INPUT, "Offering a product", "Please input player id.", "Offer", "Cancel");
			}
		}

		case DIALOG_GROUP_MAGAZYN_ID:
		{
			if( !response ) return 1;

			new customerid = strval(inputtext);
			new itemid = DynamicGui_GetDialogValue(playerid);
			new gid = pInfo[playerid][player_duty_gid];
			if(gid == -1) return SendGuiInformation(playerid, "Information", "You are not on group duty.");

			if( !pInfo[customerid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Alert", "Invalid player id.");

			new Float:dist;
			dist = GetDistanceBetweenPlayers(playerid, customerid);
			if(dist>3.0) return SendGuiInformation(playerid, ""guiopis"Alert", "This player is too far.");

			if(GetPlayerVirtualWorld(playerid) == 0)
			{
				new a_id = pInfo[playerid][player_area];
				if(a_id > 0)
				{
					if( !AreaHasFlag(a_id, AREA_FLAG_OFFER) ) return SendGuiInformation(playerid, ""guiopis"Alert", "You are not in area with flag to offer products.");
					if(Area[a_id][area_owner_type] != AREA_OWNER_TYPE_GROUP) return SendGuiInformation(playerid, ""guiopis"Alert", "This area is not belong to you, or you are not on duty.");
				    if(Area[a_id][area_owner] != Group[gid][group_uid]) return SendGuiInformation(playerid, ""guiopis"Alert", "This area is not belong to you, or you are not on duty.");

				    new resp = SetOffer(playerid, customerid, OFFER_TYPE_PRODUCT, Item[itemid][item_price], itemid);
				    if( resp ) ShowPlayerOffer(customerid, playerid, "Product", sprintf("Product: %s [%d]", Item[pOffer[customerid][offer_extraid]][item_name], Item[pOffer[customerid][offer_extraid]][item_uid]), Item[pOffer[customerid][offer_extraid]][item_price]);
				}
				else
				{
					SendGuiInformation(playerid, ""guiopis"Alert", "You are not in area with flag to offer products.");
				}
			}
			else
			{
				new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
				if (d_id != -1)
				{
					if(Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP) return SendGuiInformation(playerid, ""guiopis"Alert", "These doors are not assigned to any group."); 
					if(Door[d_id][door_owner] != Group[gid][group_uid]) return SendGuiInformation(playerid, ""guiopis"Alert", "You are not on group duty or these doors are not assigned to your group.");

					new resp = SetOffer(playerid, customerid, OFFER_TYPE_PRODUCT, Item[itemid][item_price], itemid);
				    if( resp ) ShowPlayerOffer(customerid, playerid, "Product", sprintf("Product: %s [%d]", Item[pOffer[customerid][offer_extraid]][item_name], Item[pOffer[customerid][offer_extraid]][item_uid]), Item[pOffer[customerid][offer_extraid]][item_price]);
				}
			}
		}

		case DIALOG_GROUP_VEHICLES:
		{
			if( !response ) return 1;

			new v_uid = DynamicGui_GetValue(playerid, listitem), vid = GetVehicleByUid(v_uid);
			if( vid != INVALID_VEHICLE_ID )
			{
				DeleteVehicle(vid);
				GameTextForPlayer(playerid, "~w~veh ~r~unspawned", 3000, 6);
			}
			else
			{
				LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", v_uid), true);

				GameTextForPlayer(playerid, "~w~veh ~g~spawned", 3000, 6);
			}
		}

		case DIALOG_PLAYER_VEHICLES:
		{
			if( !response ) return 1;

			new v_uid = DynamicGui_GetValue(playerid, listitem), vid = GetVehicleByUid(v_uid);
			if( vid != INVALID_VEHICLE_ID )
			{
				DeleteVehicle(vid);
				GameTextForPlayer(playerid, "~w~veh ~r~unspawned", 3000, 6);
			}
			else
			{
				new count = 0;
				foreach(new v_id : Vehicles)
				{
					if( Vehicle[v_id][vehicle_owner_type] == VEHICLE_OWNER_TYPE_PLAYER && Vehicle[v_id][vehicle_owner] == pInfo[playerid][player_id] ) count++;
				}

				if( IsPlayerVip(playerid) && count >= 5 ) return SendGuiInformation(playerid, ""guiopis"Alert", "You can't spawn more than 5 vehicles.");
				else if( !IsPlayerVip(playerid) && count >= 3 ) return SendGuiInformation(playerid, ""guiopis"Alert", "You can't spawn more than 3 vehicles without premium account.");

				LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", v_uid), true);

				GameTextForPlayer(playerid, "~w~veh ~g~spawned", 3000, 6);
			}
		}

		case DIALOG_TUNE:
		{
			if( !response ) return 1;
			new vid = GetPlayerVehicleID(playerid);
			if(vid == INVALID_VEHICLE_ID) return 1;
			new componentid = DynamicGui_GetValue(playerid, listitem);
			new slot = GetVehicleComponentType(componentid);
			if(slot != -1)
			{
			    Vehicle[vid][vehicle_component][slot] = 0;
			    RemoveVehicleComponent(vid , componentid);

			    new comp0 = Vehicle[vid][vehicle_component][0];
				new comp1 = Vehicle[vid][vehicle_component][1];
				new comp2 = Vehicle[vid][vehicle_component][2];
				new comp3 = Vehicle[vid][vehicle_component][3];
				new comp4 = Vehicle[vid][vehicle_component][4];
				new comp5 = Vehicle[vid][vehicle_component][5];
				new comp6 = Vehicle[vid][vehicle_component][6];
				new comp7 = Vehicle[vid][vehicle_component][7];
				new comp8 = Vehicle[vid][vehicle_component][8];
				new comp9 = Vehicle[vid][vehicle_component][9];
				new comp10 = Vehicle[vid][vehicle_component][10];
				new comp11 = Vehicle[vid][vehicle_component][11];
				new comp12 = Vehicle[vid][vehicle_component][12];
				new comp13 = Vehicle[vid][vehicle_component][13];

			    new visual_tuning[128];
				format(visual_tuning, sizeof(visual_tuning), "%d %d %d %d %d %d %d %d %d %d %d %d %d %d", comp0, comp1, comp2, comp3, comp4, comp5, comp6, comp7,comp8, comp9, comp10, comp11, comp12, comp13);
		    	mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_component = '%s' WHERE vehicle_uid = %d", visual_tuning, Vehicle[vid][vehicle_uid]));

		    	new it_name[40];
		    	format(it_name, sizeof(it_name), "%s", GetComponentName(componentid));

		    	Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_TUNING, componentid, componentid, 0, it_name);
		    	SendGuiInformation(playerid, "Information", "Component removed.");
			}
		}

		case DIALOG_PLAYER_VEHICLE_PANEL:
		{
			if( !response ) return 1;

			new vid = GetPlayerVehicleID(playerid);
			if( vid == INVALID_VEHICLE_ID ) return 1;

			new selected = DynamicGui_GetValue(playerid, listitem);

			switch( selected )
			{
				case DG_PLAYER_VEHICLE_PANEL_LIGHTS:
				{
					Vehicle[vid][vehicle_lights] = !Vehicle[vid][vehicle_lights];
				}

				case DG_PLAYER_VEHICLE_PANEL_BOOT:
				{
					Vehicle[vid][vehicle_boot] = !Vehicle[vid][vehicle_boot];
				}

				case DG_PLAYER_VEHICLE_PANEL_BONNET:
				{
					Vehicle[vid][vehicle_bonnet] = !Vehicle[vid][vehicle_bonnet];
				}

				case DG_PLAYER_VEHICLE_PANEL_WIN_DRIVER:
				{
					Vehicle[vid][vehicle_win_driver] = !Vehicle[vid][vehicle_win_driver];
				}

				case DG_PLAYER_VEHICLE_PANEL_WIN_PP:
				{
					Vehicle[vid][vehicle_win_pp] = !Vehicle[vid][vehicle_win_pp];
				}

				case DG_PLAYER_VEHICLE_PANEL_WIN_LT:
				{
					Vehicle[vid][vehicle_win_lt] = !Vehicle[vid][vehicle_win_lt];
				}

				case DG_PLAYER_VEHICLE_PANEL_WIN_PT:
				{
					Vehicle[vid][vehicle_win_pt] = !Vehicle[vid][vehicle_win_pt];
				}
			}

			if( selected == DG_PLAYER_VEHICLE_PANEL_LIGHTS || selected == DG_PLAYER_VEHICLE_PANEL_BONNET || selected == DG_PLAYER_VEHICLE_PANEL_BOOT ) UpdateVehicleVisuals(vid);
			else if( selected == DG_PLAYER_VEHICLE_PANEL_WIN_DRIVER || selected == DG_PLAYER_VEHICLE_PANEL_WIN_PP || selected == DG_PLAYER_VEHICLE_PANEL_WIN_LT || selected == DG_PLAYER_VEHICLE_PANEL_WIN_PT ) UpdateWindowVisuals(vid);

			cmd_vehicle(playerid, "");
		}

		case DIALOG_TAKE_BAG:
		{
			if( response)
			{
				new iuid = strval(inputtext);
				if(iuid == -1) return 1;

				new bagid = DynamicGui_GetDialogValue(playerid);
				if(bagid == -1) return 1;

				ProxMessage(playerid, "takes item from his bag.", PROX_SERWERME);
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_ownertype` = %d, `item_owner` = %d WHERE `item_uid` = %d", ITEM_OWNER_TYPE_PLAYER, pInfo[playerid][player_id], iuid));
				new itemid = LoadPlayerItem(playerid, sprintf("WHERE `item_uid` = %d", iuid), true);
				PlayerItem[playerid][bagid][player_item_weight] -= PlayerItem[playerid][itemid][player_item_weight];
			}

			else if( !response ) return 1;
		}

		case DIALOG_PLAYER_ITEMS:
		{
			new dg_value = DynamicGui_GetValue(playerid, listitem), dg_data = DynamicGui_GetDataInt(playerid, listitem);
			if( !response && dg_value == DG_NO_ACTION ) return 1;

			if( response && dg_value == DG_ITEMS_ITEM_ROW )
			{
				Item_Use(dg_data, playerid);
			}

			if( !response && dg_value == DG_ITEMS_ITEM_ROW )
			{
				new itemid = dg_data;

				DynamicGui_Init(playerid);
				new string[200];

				format(string, sizeof(string), "%s01\tItem details\n", string);
				DynamicGui_AddRow(playerid, DG_ITEMS_MORE_INFO, itemid);

				if( IsPlayerInAnyVehicle(playerid) ) format(string, sizeof(string), "%s02\tPut in vehicle\n", string);
				else format(string, sizeof(string), "%s02\tDrop on ground\n", string);

				DynamicGui_AddRow(playerid, DG_ITEMS_MORE_DROPG, itemid);

				format(string, sizeof(string), "%s03\tOffer to someone\n", string);
				DynamicGui_AddRow(playerid, DG_ITEMS_MORE_SELL, itemid);

				format(string, sizeof(string), "%s04\tPut in bag\n", string);
				DynamicGui_AddRow(playerid, DG_ITEMS_MORE_PUT_IN_BAG, itemid);

				format(string, sizeof(string), "%s05\tDestroy\n", string);
				DynamicGui_AddRow(playerid, DG_ITEMS_MORE_DELETE, itemid);

				if( GetPlayerVirtualWorld(playerid) > 0 )
				{
					new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
					if( d_id > -1 )
					{
						if( CanPlayerUseDoor(playerid, d_id) )
						{
							format(string, sizeof(string), "%s06\tPut in doors\n", string);
							DynamicGui_AddRow(playerid, DG_ITEMS_MORE_PUT_IN_DOOR, itemid);

							format(string, sizeof(string), "%s07\tPut in magazine\n", string);
							DynamicGui_AddRow(playerid, DG_ITEMS_MORE_PUT_IN_STORAGE, itemid);
						}
					}
				}

				ShowPlayerDialog(playerid, DIALOG_ITEM_MORE, DIALOG_STYLE_LIST, sprintf("%s [UID: %d] Options", PlayerItem[playerid][itemid][player_item_name], PlayerItem[playerid][itemid][player_item_uid]), string, "Wybierz", "Zamknij");
			}
		}

		case DIALOG_ITEM_MORE:
		{
			new dg_value = DynamicGui_GetValue(playerid, listitem), itemid = DynamicGui_GetDataInt(playerid, listitem);
			if( !response ) return 1;

			if( dg_value == DG_ITEMS_MORE_DROPG )
			{
				Item_Drop(itemid, playerid);
			}

			if( dg_value == DG_ITEMS_MORE_INFO)
			{
				new header[64], info[128];

				format(header, sizeof(header), "Details about: %s", PlayerItem[playerid][itemid][player_item_name]);
				format(info, sizeof(info), "UID:\t%d\nValue1:\t%d\nValue2:\t%d\nType:\t%d\nExtra id:\t%d\nModel:\t%d", PlayerItem[playerid][itemid][player_item_uid], PlayerItem[playerid][itemid][player_item_value1], PlayerItem[playerid][itemid][player_item_value2], PlayerItem[playerid][itemid][player_item_type], PlayerItem[playerid][itemid][player_item_extraid], PlayerItem[playerid][itemid][player_item_model]);
				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST, header, info, "OK", "");
			}


			if( dg_value == DG_ITEMS_MORE_SELL )
			{
				if( !response ) return 1;
				pInfo[playerid][player_dialog_tmp1] = itemid;
				ShowPlayerDialog(playerid, DIALOG_ITEMS_OFFER_PRICE, DIALOG_STYLE_INPUT, "Offering an item", "Please input a price.", "Offer", "Close");
			}

			if( dg_value == DG_ITEMS_MORE_PUT_IN_DOOR )
			{
				if(PlayerItem[playerid][itemid][player_item_used]) return SendGuiInformation(playerid, "Information", "This item is used.");

				new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
				if( d_id > -1 )
				{
					ProxMessage(playerid, "puts item inside locker.", PROX_SERWERME);

					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_ownertype` = %d, `item_owner` = %d WHERE `item_uid` = %d", ITEM_OWNER_TYPE_DOOR, Door[d_id][door_uid], PlayerItem[playerid][itemid][player_item_uid]));

					DeleteItem(itemid, false, playerid);
					ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
				}
			}

			if( dg_value == DG_ITEMS_MORE_PUT_IN_BAG )
			{	
				if(PlayerItem[playerid][itemid][player_item_used]) return SendGuiInformation(playerid, "Information", "This item is used.");
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
				new count, string[200];
				foreach(new item : PlayerItems[playerid])
				{
					if( PlayerItem[playerid][item][player_item_type] == ITEM_TYPE_BAG )
					{
						format(string, sizeof(string), "%s%d\t\t%s\n", string, PlayerItem[playerid][item][player_item_uid], PlayerItem[playerid][item][player_item_name]);
						DynamicGui_AddRow(playerid, item);
						count++;
					}
				}
				
				if( count == 0 ) return SendGuiInformation(playerid, ""guiopis"Alert", "You don't have any bag.");
				else ShowPlayerDialog(playerid, DIALOG_USE_BAG, DIALOG_STYLE_LIST, "Choose bag:", string, "Choose", "Close");
			}

			if( dg_value == DG_ITEMS_MORE_DELETE )
			{	
				if(PlayerItem[playerid][itemid][player_item_type] != ITEM_TYPE_CORPSE && PlayerItem[playerid][itemid][player_item_type] != ITEM_TYPE_DRUG && PlayerItem[playerid][itemid][player_item_type] != ITEM_TYPE_WEAPON && PlayerItem[playerid][itemid][player_item_type] != ITEM_TYPE_BAG)
				{
					DynamicGui_Init(playerid);
					DynamicGui_SetDialogValue(playerid, itemid);
					SendGuiInformation(playerid, ""guiopis"Alert","Item has been destroyed.");
					ProxMessage(playerid, sprintf("destroys %s.", PlayerItem[playerid][itemid][player_item_name]), PROX_SERWERME);
					pInfo[playerid][player_capacity] += PlayerItem[playerid][itemid][player_item_weight];
					DeleteItem(itemid, true, playerid);
				}
				else
				{
					SendGuiInformation(playerid, "Information", "This type of item cannot be destroyed.");
				}
			}

			if( dg_value == DG_ITEMS_MORE_PUT_IN_STORAGE )
			{
				new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
				if( d_id > -1 )
				{
					if(Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GROUP)
					{
						DynamicGui_Init(playerid);
						DynamicGui_SetDialogValue(playerid, itemid);

						ShowPlayerDialog(playerid, DIALOG_PUTTING_ITEM, DIALOG_STYLE_INPUT, "Put in storage/magazine", "Please input product price for which you want to sell it.", "Put", "Close");
					}
					else
					{
						SendGuiInformation(playerid, ""guiopis"Alert", "These doors aren't assigned to any group.");
					}
				}
			}
		}

		case DIALOG_PUTTING_ITEM:
		{
			if( !response ) return 1;

			new itemid = DynamicGui_GetDialogValue(playerid);
			new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
			new price = strval(inputtext);

			new uid = PlayerItem[playerid][itemid][player_item_uid];

			if(PlayerItem[playerid][itemid][player_item_used]) return SendGuiInformation(playerid, "Information", "This item is used.");
			if(price <= 0) return SendGuiInformation(playerid, ""guiopis"Alert", "Invalid price.");

			DeleteItem(itemid, false, playerid);

			ProxMessage(playerid, "puts item inside storage.", PROX_SERWERME);

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_ownertype` = %d, `item_owner` = %d, `item_price` = %d, `item_count` = '1' WHERE `item_uid` = %d", ITEM_OWNER_TYPE_GROUP, Door[d_id][door_owner], price, uid));

			ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
		}

		case DIALOG_ITEMS_PICKUP:
		{
			new dg_value = DynamicGui_GetValue(playerid, listitem), itemuid = DynamicGui_GetDataInt(playerid, listitem);
			if( !response ) return 1;

			if( dg_value == DG_ITEMS_PICKUP_ROW )
			{
				Item_Pickup(itemuid, playerid);
			}
		}

		case DIALOG_SCHOWEK_TAKE:
		{
			if( !response ) return 1;
			new itemuid = strval(inputtext);

			Item_Pickup(itemuid, playerid);
		}

		case DIALOG_USE_BAG:
		{
			if( !response ) return 1;

			new itemid = DynamicGui_GetDialogValue(playerid), bagid = DynamicGui_GetValue(playerid, listitem);

			if(PlayerItem[playerid][itemid][player_item_type]  == ITEM_TYPE_BAG) return SendGuiInformation(playerid, ""guiopis"Alert", "You can't put bag in bag.");

			PlayerItem[playerid][bagid][player_item_weight] += PlayerItem[playerid][itemid][player_item_weight];
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_ownertype` = %d, `item_owner` = %d WHERE `item_uid` = %d", ITEM_OWNER_TYPE_ITEM, PlayerItem[playerid][bagid][player_item_uid], PlayerItem[playerid][itemid][player_item_uid]));
			DeleteItem(itemid, false, playerid);

			ProxMessage(playerid, "puts item in bag.", PROX_SERWERME);
		}

		case DIALOG_USE_AMMO:
		{
			if( !response ) return 1;

			new ammoid = DynamicGui_GetDialogValue(playerid), itemid = DynamicGui_GetValue(playerid, listitem);

			PlayerItem[playerid][itemid][player_item_value2] += PlayerItem[playerid][ammoid][player_item_value2];
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value2` = %d WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_value2], PlayerItem[playerid][itemid][player_item_uid]));

			SendGuiInformation(playerid, ""guiopis"Alert", sprintf("Loaded %d bullet to %s [UID: %d].", PlayerItem[playerid][ammoid][player_item_value2], PlayerItem[playerid][itemid][player_item_name], PlayerItem[playerid][itemid][player_item_uid]));

			DeleteItem(ammoid, true, playerid);
		}

		case DIALOG_USE_DRUG:
		{
			if( !response ) return 1;

			new itemid = DynamicGui_GetDialogValue(playerid);
			new str[40];
			new type = PlayerItem[playerid][itemid][player_item_value1];

			switch(listitem)
			{
				case 0: // Uzywanie narkotyku
				{
					format(str, sizeof(str), "takes %s", PlayerItem[playerid][itemid][player_item_name]);
					ProxMessage(playerid, str, PROX_SERWERME);

					if(PlayerItem[playerid][itemid][player_item_value2] == 1)
					{
						DeleteItem(itemid, true, playerid);
					}
					else
					{
						PlayerItem[playerid][itemid][player_item_value2]--;
						PlayerItem[playerid][itemid][player_item_weight] = PlayerItem[playerid][itemid][player_item_value2];
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_value2 = %d, item_weight = %d WHERE item_uid = %d", PlayerItem[playerid][itemid][player_item_value2], PlayerItem[playerid][itemid][player_item_weight], PlayerItem[playerid][itemid][player_item_uid]));
					}

					switch(type)
					{
						case DRUG_TYPE_COCAINE:
						{
							AddPlayerStatus(playerid, PLAYER_STATUS_DRUGS);
							//Efekt here
						}
						case DRUG_TYPE_CRACK:
						{
							AddPlayerStatus(playerid, PLAYER_STATUS_DRUGS);
							//Efekt here
						}
						case DRUG_TYPE_AMFA:
						{
							AddPlayerStatus(playerid, PLAYER_STATUS_DRUGS);
							//Efekt here
						}
						case DRUG_TYPE_HEROINE:
						{
							AddPlayerStatus(playerid, PLAYER_STATUS_DRUGS);
							//Efekt here
						}
						case DRUG_TYPE_WEED:
						{
							AddPlayerStatus(playerid, PLAYER_STATUS_WEED);
							SetPlayerSpecialAction(playerid, SPECIAL_ACTION_SMOKE_CIGGY);
						}
						case DRUG_TYPE_METH:
						{
							AddPlayerStatus(playerid, PLAYER_STATUS_DRUGS);
							//Efekt here
						}
						case DRUG_TYPE_EXTASY:
						{
							AddPlayerStatus(playerid, PLAYER_STATUS_DRUGS);
							//Efekt here
						}
					}
				}
				case 1:
				{
					new hour, minute, second;
					gettime(hour, minute, second);
					if(hour<16) return SendGuiInformation(playerid, ""guiopis"Alert", "You can deal only between 4:00 PM and 12:00 AM.");
					new aid = pInfo[playerid][player_area];
					if(aid == -1) return SendGuiInformation(playerid, ""guiopis"Alert", "You aren't or corner.");
					if(!AreaHasFlag(aid, AREA_FLAG_CORNER)) return SendGuiInformation(playerid, ""guiopis"Alert", "You aren't or corner.");
					if(pInfo[playerid][player_dealing] > 0) return SendGuiInformation(playerid, ""guiopis"Alert", "You are already dealing. Wait for client.");

					new p_count;
					foreach(new pid: Player)
					{
						if(pInfo[pid][player_dealing] > 0 && pInfo[pid][player_area] == pInfo[playerid][player_area])
						{
							p_count++;
						}
					}

					if(p_count >= 3) return SendGuiInformation(playerid, ""guiopis"Alert", "This corner is full (3 dealers).");

					if(PlayerItem[playerid][itemid][player_item_value2] <= 0)
					{
						SendGuiInformation(playerid, ""guiopis"Alert", "Wrong item value, report it to administration.");
						DeleteItem(itemid, true, playerid);
						return 1;
					} 
					
					if(IsValidDrugType(playerid, itemid))
					{
						pInfo[playerid][player_dealing] = 120;
						pInfo[playerid][player_dialog_tmp4] = itemid;

						TextDrawSetString(Tutorial[playerid], sprintf("~w~Waiting for client.~n~Remaining time: ~y~%ds", pInfo[playerid][player_dealing]));
						TextDrawShowForPlayer(playerid, Tutorial[playerid]);

						AddPlayerStatus(playerid, PLAYER_STATUS_DEALING);
						ApplyAnimation(playerid, "DEALER", "DEALER_IDLE_02", 4.1, 0, 0, 0, 1, 0, 0);
						pInfo[playerid][player_looped_anim] = true;
					}
					else
					{
						SendGuiInformation(playerid, ""guiopis"Alert", "Wrong item type, report it to administration.");
						return 1;
					}
				}
				case 2:
				{
					DynamicGui_Init(playerid);
					DynamicGui_SetDialogValue(playerid, itemid);
					ShowPlayerDialog(playerid, DIALOG_PAKOWANIE, DIALOG_STYLE_INPUT, "Drug split", "Please input how many grams you want in one piece.", "Split", "Cancel");
				}
				case 3:
				{
					if(PlayerItem[playerid][itemid][player_item_value1] != DRUG_TYPE_COCAINE) return SendGuiInformation(playerid, ""guiopis"Alert", "Crack can be cooked only from cocaine.");
					new object_id = GetClosestObjectType(playerid, OBJECT_CRACK);
					if(object_id == INVALID_OBJECT_ID) return SendGuiInformation(playerid, ""guiopis"Alert", "You are not near stove.");

					DeleteItem(itemid, true, playerid);
					Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_DRUG, 1575, DRUG_TYPE_CRACK, 3, "Crack");

					SendGuiInformation(playerid, ""guiopis"Alert", "You've cooked 3 grams of crack from 1 gram of cocaine.");
				}
			}
		}

		case DIALOG_FINGERPRINTS:
		{
			if( !response ) return 1;

			new finger = strval(inputtext);
			new name[MAX_PLAYER_NAME+1], str[64];
			GetPlayerNameByUid(finger, name);
			
			format(str, sizeof(str), "Fingerprint belongs to %s.", name);
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Research results", str, "OK", "");
		}

		case DIALOG_PAKOWANIE:
		{
			if( !response ) return 1;
			if(isnull(inputtext)) return 1;
			new itemid = DynamicGui_GetDialogValue(playerid);
			new ilosc = strval(inputtext);
			if( strfind(inputtext, ",", true) != -1 || strfind(inputtext, ".", true) != -1 ) return SendGuiInformation(playerid, "Information", "Amount must be integer.");

			if(ilosc >= PlayerItem[playerid][itemid][player_item_value2]) return 1;
			if(ilosc < 1) return 1;

			new second = PlayerItem[playerid][itemid][player_item_value2] - ilosc;
			if(second < 1) return 1;

			new string[40];
			format(string, sizeof(string), "%s", PlayerItem[playerid][itemid][player_item_name]);
			new drugtype = PlayerItem[playerid][itemid][player_item_value1];

			Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_DRUG, 1575, drugtype, ilosc, string);

			Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_DRUG, 1575, drugtype, second, string);

			SendGuiInformation(playerid, "Information", "Item has been splitted.");
			DeleteItem(itemid, true, playerid);
		}

		case DIALOG_PHONE:
		{
			if( !response ) return 1;

			new dg_value = DynamicGui_GetValue(playerid, listitem), itemid = DynamicGui_GetDialogValue(playerid);

			if( dg_value == DG_PHONE_TURNOFF )
			{
				PlayerItem[playerid][itemid][player_item_used] = false;
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_used` = 0 WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_uid]));

				GameTextForPlayer(playerid, "~w~Phone ~r~off", 3000, 3);
			}
			else if( dg_value == DG_PHONE_CALL )
			{
				ShowPlayerDialog(playerid, DIALOG_PHONE_CALL_NUMBER, DIALOG_STYLE_INPUT, "Phone call", "Please input phone number you want to call:", "Call", "Close");
			}
			else if( dg_value == DG_PHONE_SMS )
			{
				ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_NUMBER, DIALOG_STYLE_INPUT, "Text message", "Please input phone nember you want to send text message:", "Next", "Close");
			}
			else if( dg_value == DG_PHONE_CONTACTS )
			{
				DynamicGui_Init(playerid);
				new string[1024];

				format(string, sizeof(string), "%s911\tEmergency Number\n", string);
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_BASE, 911);

				format(string, sizeof(string), "%s444\tWeazel News\n", string);
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_BASE, 444);

				format(string, sizeof(string), "%s333\tWholesale\n", string);
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_BASE, 333);

				format(string, sizeof(string), "%s---\tBusinesses\n", string);
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_BASE, 668);

				format(string, sizeof(string), "%s-----\n", string);
				DynamicGui_AddBlankRow(playerid);

				new rows, fields;
				mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_contacts` WHERE `contact_owner` = %d AND `contact_deleted` = 0", PlayerItem[playerid][itemid][player_item_uid]));
				cache_get_data(rows, fields);

				if( !rows ) SendGuiInformation(playerid, ""guiopis"Alert", "No contacts.");
				else
				{
				  	for(new row = 0; row != rows; row++)
					{
						new tmp[MAX_PLAYER_NAME+1];
						cache_get_row(row, 2, tmp);

						format(string, sizeof(string), "%s%d\t%s\n", string, cache_get_row_int(row, 1), tmp);
						DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_ROW, cache_get_row_int(row, 0));
					}
				}

				ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACTS, DIALOG_STYLE_LIST, sprintf("%s [%d]: Contacts", PlayerItem[playerid][itemid][player_item_name], PlayerItem[playerid][itemid][player_item_value1]), string, "Wybierz", "Zamknij");
			}
			else if( dg_value == DG_PHONE_ADD_CONTACT )
			{
				ShowPlayerDialog(playerid, DIALOG_PHONE_ADD_CONTACT, DIALOG_STYLE_INPUT, sprintf("%s [%d]: Add contact", PlayerItem[playerid][itemid][player_item_name], PlayerItem[playerid][itemid][player_item_value1]), "Wpisz numer telefonu, który chcesz dodaæ do kontaktów.", "Dodaj", "Zamknij");
			}
			else if( dg_value == DG_PHONE_VCARD )
			{
				DynamicGui_Init(playerid);
				new string[2048], count;

				new Float:p_pos[3];
				GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);

				foreach(new p : Player)
				{
					if( !pInfo[p][player_logged] ) continue;
					if( pInfo[p][player_spec] != INVALID_PLAYER_ID) continue;
					if( p == playerid ) continue;
					if( GetPlayerDistanceFromPoint(p, p_pos[0], p_pos[1], p_pos[2]) <= 10.0 )
					{
						if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASKA) > -1 ) format(string, sizeof(string), "%s##\t\t%s\n", string, pInfo[p][player_name]);
						else format(string, sizeof(string), "%s%d\t\t%s\n", string, p, pInfo[p][player_name]);

						DynamicGui_AddRow(playerid, p);
						count++;
					}
				}

				if( count == 0 ) SendGuiInformation(playerid, ""guiopis"Alert", "There are no people in this area.");
				else ShowPlayerDialog(playerid, DIALOG_PHONE_VCARD, DIALOG_STYLE_LIST, "People in your arrea:", string, "Offer", "Close");
			}
		}

		case DIALOG_PHONE_SMS_NUMBER:
		{
			if( !response ) return 1;

			new number;
			if( sscanf(inputtext, "d", number) ) return ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_NUMBER, DIALOG_STYLE_INPUT, "Text message", "Please input phone number you want to send text message:\n\n"HEX_COLOR_LIGHTER_RED"Invalid number.", "Next", "Close");

			pInfo[playerid][player_dialog_tmp1] = number;

			ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_TEXT, DIALOG_STYLE_INPUT, "Text message", "Please input a content of message:", "Send", "Close");
		}

		case DIALOG_PHONE_SMS_TEXT:
		{
			if( !response ) return 1;

			if( isnull(inputtext) ) return ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_TEXT, DIALOG_STYLE_INPUT, "Text Message", "Please input a content of message:\n\n"HEX_COLOR_LIGHTER_RED"You didn't typed anything.", "Send", "Close");

			cmd_sms(playerid, sprintf("%d %s", pInfo[playerid][player_dialog_tmp1], inputtext));
		}

		case DIALOG_PHONE_CALL_NUMBER:
		{
			if( !response) return 1;
			new number;
			if( sscanf(inputtext, "d", number) ) return ShowPlayerDialog(playerid, DIALOG_PHONE_CALL_NUMBER, DIALOG_STYLE_INPUT, "Phone call", "Please input phone number you want to call:\n\n"HEX_COLOR_LIGHTER_RED"Wrong number.", "Call", "Close");

			cmd_call(playerid, sprintf("%d", number));
		}

		case DIALOG_PHONE_CONTACTS:
		{
			if( !response ) return 1;

			new dg_value = DynamicGui_GetValue(playerid, listitem), dg_data = DynamicGui_GetDataInt(playerid, listitem);

			if( dg_value == DG_PHONE_CONTACTS_BASE )
			{
				if(dg_data == 668)
				{
					new list_business[1024];
					DynamicGui_Init(playerid);
					foreach(new gid: Groups)
					{
						if( !GroupHasFlag(gid, GROUP_FLAG_BUSINESS) ) continue;
						new count = CountGroupPlayers(gid);
						if(count == 0) continue;

						format(list_business, sizeof(list_business), "%s\n%s\t%d", list_business, Group[gid][group_name], count);
						DynamicGui_AddRow(playerid, gid);
					}

					if(!strlen(list_business)) return SendGuiInformation(playerid, "Information", "No active businesses at this moment.");

					format(list_business, sizeof(list_business), "Name\tActive workers\n%s", list_business);
					ShowPlayerDialog(playerid, DIALOG_PHONE_CALL_GROUP, DIALOG_STYLE_TABLIST_HEADERS, "Active businesses", list_business, "Call", "Cancel");
				}
				else
				{
					cmd_call(playerid, sprintf("%d", dg_data));
				}
			}
			else if( dg_value == DG_PHONE_CONTACTS_ROW )
			{
				new rows, fields;
				mysql_query(mySQLconnection, sprintf("SELECT contact_name, contact_number FROM `ipb_contacts` WHERE `contact_uid` = %d", dg_data));
				cache_get_data(rows, fields);

				new tmp[MAX_PLAYER_NAME+1];
				cache_get_row(0, 0, tmp);

				pInfo[playerid][player_dialog_tmp1] = dg_data;
				ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACTS_ROW, DIALOG_STYLE_LIST, sprintf("Kontakt %s [%d]", tmp, cache_get_row_int(0, 1)), "01\tCall\n02\tSMS\n03\tEdit contact\n04\tDelete contact", "Choose", "Close");
			}
		}

		case DIALOG_PHONE_CALL_GROUP:
		{
			if( !response ) return 1;

			new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
			if( itemid == -1 ) return SendGuiInformation(playerid, "Information", "You don't have active phone.");
			
			new gid = DynamicGui_GetValue(playerid, listitem);

			ShowPlayerDialog(playerid, DIALOG_CALL_GROUP, DIALOG_STYLE_INPUT, "Call to business:", "Please input content of your order:", "Call", "");
			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
			pInfo[playerid][player_dialog_tmp1] = PlayerItem[playerid][itemid][player_item_value1];
			pInfo[playerid][player_dialog_tmp2] = gid;
		}

		case DIALOG_CALL_GROUP:
		{
			if( !response )
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				return 1;
			}

			if(strlen(inputtext) < 4)
			{
				SendGuiInformation(playerid, "Information", "Input too short.");
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				return 1;
			}

			if(strlen(inputtext) > 110)
			{
				SendGuiInformation(playerid, "Information", "Input too long.");
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				return 1;
			}

			new number = pInfo[playerid][player_dialog_tmp1];
			new gid = pInfo[playerid][player_dialog_tmp2];

			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
			RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);

			ProxMessage(playerid, inputtext, PROX_PHONE);

			foreach(new p : Player)
			{
				if(pInfo[p][player_duty_gid] == gid)
				{
					SendClientMessage(p, COLOR_GOLD, sprintf("[Order from %d]: %s", number, inputtext));
				}
			}
		}

		case DIALOG_911:
		{
			new zgloszenie[MAX_PLAYERS];
			new number = pInfo[playerid][player_dialog_tmp1];

			if( !response )
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				return 1;
			}

			if(strlen(inputtext) < 4)
			{
				SendGuiInformation(playerid, "Information", "Input too short.");
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				return 1;
			}

			if(strlen(inputtext) > 110)
			{
				SendGuiInformation(playerid, "Information", "Input too long.");
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				return 1;
			}

			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
			RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);

			ProxMessage(playerid, inputtext, PROX_PHONE);

			foreach(new p : Player)
			{
				if(pInfo[p][player_duty_gid] >= 0)
				{
					if(Group[pInfo[p][player_duty_gid]][group_flags] & GROUP_FLAG_DEP)
					{
						zgloszenie[p]=1;
					}
				}
				if(zgloszenie[p]==1)
				{
					SendFormattedClientMessage(p, COLOR_LIGHTER_RED, "[911] Call from %d: %s", number, inputtext);
					zgloszenie[p]=0;
				}
			}
		}

		case DIALOG_PHONE_VCARD:
		{
			if( !response ) return 1;

			new targetid = DynamicGui_GetValue(playerid, listitem);

			if( !IsPlayerConnected(targetid) ) return 1;
			if( !pInfo[targetid][player_logged] ) return 1;

			new resp = SetOffer(playerid, targetid, OFFER_TYPE_VCARD, 0, GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE));

			if( resp ) ShowPlayerOffer(targetid, playerid, "vCard", sprintf("vCard %s [%d]", pInfo[playerid][player_name], PlayerItem[playerid][pOffer[playerid][offer_extraid]][player_item_value1]), 0);
		}

		case DIALOG_ITEMS_OFFER:
		{
			if( !response ) return 1;

			new targetid = DynamicGui_GetValue(playerid, listitem);
			new itemid = pInfo[playerid][player_dialog_tmp1];
			new price = pInfo[playerid][player_item_price];
			if(price < 0) return SendGuiInformation(playerid, ""guiopis"Alert", "Invalid price.");

			if( !IsPlayerConnected(targetid) ) return 1;
			if( !pInfo[targetid][player_logged] ) return 1;

			new resp = SetOffer(playerid, targetid, OFFER_TYPE_ITEM, price, itemid);

			if( resp ) ShowPlayerOffer(targetid, playerid, "Item", sprintf("Item %s [%d]", PlayerItem[playerid][pOffer[targetid][offer_extraid]][player_item_name], PlayerItem[playerid][pOffer[targetid][offer_extraid]][player_item_uid]), price);	
		}

		case DIALOG_ITEMS_OFFER_PRICE:
		{
			if( !response ) return 1;
			new price = strval(inputtext);
			DynamicGui_Init(playerid);

			new string[2048], count;
			pInfo[playerid][player_item_price] = price;

			new Float:p_pos[3];
			GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
			foreach(new p : Player)
			{
				if( !pInfo[p][player_logged] ) continue;
				if( p == playerid ) continue;
				if( pInfo[p][player_spec] != INVALID_PLAYER_ID) continue;
				if( GetPlayerDistanceFromPoint(p, p_pos[0], p_pos[1], p_pos[2]) <= 10.0 )
				{
					if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASKA) > -1 ) format(string, sizeof(string), "%s##\t\t%s\n", string, pInfo[p][player_name]);
					else format(string, sizeof(string), "%s%d\t\t%s\n", string, p, pInfo[p][player_name]);

					DynamicGui_AddRow(playerid, p);
					count++;
				}
			}

			if( count == 0 ) SendGuiInformation(playerid, ""guiopis"Alert", "There are no people in your area.");
			else ShowPlayerDialog(playerid, DIALOG_ITEMS_OFFER, DIALOG_STYLE_LIST, "People around:", string, "Offer", "Close");
		}

		case DIALOG_PHONE_CONTACTS_ROW:
		{
			if( !response )
			{
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE));

				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS);
				OnDialogResponse(playerid, DIALOG_PHONE, 1, 0, "");

				return 1;
			}

			if( listitem == 0 )
			{
				new rows, fields;
				mysql_query(mySQLconnection, sprintf("SELECT contact_number FROM `ipb_contacts` WHERE `contact_uid` = %d", pInfo[playerid][player_dialog_tmp1]));
				cache_get_data(rows, fields);

				new number = cache_get_row_int(0, 0);

				cmd_call(playerid, sprintf("%d", number));
			}
			else if( listitem == 1 )
			{
				new rows, fields;
				mysql_query(mySQLconnection, sprintf("SELECT contact_number FROM `ipb_contacts` WHERE `contact_uid` = %d", pInfo[playerid][player_dialog_tmp1]));
				cache_get_data(rows, fields);
				pInfo[playerid][player_dialog_tmp1] = cache_get_row_int(0, 0);
				ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_TEXT, DIALOG_STYLE_INPUT, "Text message", "Please input content of message:", "Send", "Close");
			}
			else if( listitem == 2 )
			{
				ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACTS_ROW_NAME, DIALOG_STYLE_INPUT, "Contact name change", "Please input new contact name (max 24 chars):", "Done", "Close");
			}
			else
			{
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_contacts` SET `contact_deleted` = 1 WHERE `contact_uid` = %d", pInfo[playerid][player_dialog_tmp1]));
				SendPlayerInformation(playerid, "Contact ~r~deleted~w~.", 5000);

				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE));

				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS);
				OnDialogResponse(playerid, DIALOG_PHONE, 1, 0, "");
			}
		}

		case DIALOG_PHONE_CONTACTS_ROW_NAME:
		{
			if( !response )
			{
				DynamicGui_Init(playerid);

				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_ROW, pInfo[playerid][player_dialog_tmp1]);
				OnDialogResponse(playerid, DIALOG_PHONE_CONTACTS, 1, 0, "");
				return 1;
			}

			if( strlen(inputtext) < 2 ) return ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACTS_ROW_NAME, DIALOG_STYLE_INPUT, "Contact name change", "Please input new contact name (max 24 chars):\n\n"HEX_COLOR_LIGHTER_RED"Input too short.", "Change", "Closed");
			if( strlen(inputtext) > 24 ) return ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACTS_ROW_NAME, DIALOG_STYLE_INPUT, "Contact name change", "Please input new contact name (max 24 chars):\n\n"HEX_COLOR_LIGHTER_RED"Input too long.", "Change", "Closed");
			mysql_real_escape_string(inputtext, inputtext, mySQLconnection, 256);

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_contacts` SET `contact_name` = '%s' WHERE `contact_uid` = %d", inputtext, pInfo[playerid][player_dialog_tmp1]));
			SendPlayerInformation(playerid, "Contact name ~y~changed~w~.", 5000);

			DynamicGui_Init(playerid);

			DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_ROW, pInfo[playerid][player_dialog_tmp1]);
			OnDialogResponse(playerid, DIALOG_PHONE_CONTACTS, 1, 0, "");
		}

		case DIALOG_PHONE_ADD_CONTACT:
		{
			if( !response )	return 1;
			
			if( strlen(inputtext) < 4 ) return ShowPlayerDialog(playerid, DIALOG_PHONE_ADD_CONTACT, DIALOG_STYLE_INPUT, "Adding contact", "Please input phone number you want add to your contact list:\n\n"HEX_COLOR_LIGHTER_RED"Number too short.", "Add", "Close");
			if( strlen(inputtext) > 7 ) return ShowPlayerDialog(playerid, DIALOG_PHONE_ADD_CONTACT, DIALOG_STYLE_INPUT, "Adding contact", "Please input phone number you want add to your contact list:\n\n"HEX_COLOR_LIGHTER_RED"Number too long.", "Add", "Close");

			new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);

			if(itemid == - 1) return SendGuiInformation(playerid, "Information", "You don't have active phone.");

			mysql_real_escape_string(inputtext, inputtext, mySQLconnection, 256);

			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_contacts` VALUES (null, %d, 'Nowy kontakt', %d, 0)", strval(inputtext), PlayerItem[playerid][itemid][player_item_uid]));

			SendPlayerInformation(playerid, "Contact ~y~added~w~.", 5000);

			return 1;
		}

		case DIALOG_WORKS:
		{
			if( !response ) return 1;

			new wvalue = DynamicGui_GetValue(playerid, listitem);

			if(wvalue == WORK_TYPE_LUMBERJACK || wvalue == WORK_TYPE_TRUCKER)
			{
				if(!(pInfo[playerid][player_documents] & DOCUMENT_DRIVE)) return SendGuiInformation(playerid, "Information", "You don't have driver license.");
			}

			pInfo[playerid][player_job] = wvalue;

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_job` = %d WHERE `char_uid` = %d", pInfo[playerid][player_job], pInfo[playerid][player_id]));

			SendClientMessage(playerid, COLOR_GOLD, "Congratulations, you have new job.");
		}

		case DIALOG_DOCUMENTS:
		{
			if( !response ) return 1;

			new dg_value = DynamicGui_GetValue(playerid, listitem);
			switch( dg_value )
			{
				case DOCUMENT_ID:
				{
					new resp = SetOffer(INVALID_PLAYER_ID, playerid, OFFER_TYPE_DOCUMENT, 50, DOCUMENT_ID);

					if( resp ) ShowPlayerOffer(playerid, INVALID_PLAYER_ID, "Document", "ID Card", 50);
				}

				case DOCUMENT_DRIVE:
				{
					new resp = SetOffer(INVALID_PLAYER_ID, playerid, OFFER_TYPE_DOCUMENT, 150, DOCUMENT_DRIVE);

					if( resp ) ShowPlayerOffer(playerid, INVALID_PLAYER_ID, "Document", "Driver license", 150);
				}
			}
		}

		case DIALOG_PAYMENT:
		{
			if( !response ) return OnPlayerPaymentResponse(playerid, 0, 0);

			if( listitem == 1 )
			{
				new price = pOffer[playerid][offer_price];
				if( pInfo[playerid][player_bank_number] == 0 )
				{
					SendPlayerInformation(playerid, "You don't have ~r~bank~w~ account.", 4000);
					return ShowPlayerDialog(playerid, DIALOG_PAYMENT, DIALOG_STYLE_LIST, ""guiopis"Payment method", "Cash\nCredit card", "Choose", "Cancel");
				}

				if( pInfo[playerid][player_bank_money] < price )
				{
					SendPlayerInformation(playerid, "You don't have enough ~r~money~w~ on your credit card.", 4000);
					return ShowPlayerDialog(playerid, DIALOG_PAYMENT, DIALOG_STYLE_LIST, ""guiopis"Payment method", "Cash\nCredit card", "Choose", "Cancel");
				}

				AddPlayerBankMoney(playerid, -price);

				OnPlayerPaymentResponse(playerid, 1, 1);
			}
			else
			{
				if( pInfo[playerid][player_money] < pOffer[playerid][offer_price] )
				{
					SendPlayerInformation(playerid, "You don't have enough ~r~cash~w~.", 4000);
					return ShowPlayerDialog(playerid, DIALOG_PAYMENT, DIALOG_STYLE_LIST, ""guiopis"Payment method", "Cash\nCredit card", "Choose", "Cancel");
				}

				GivePlayerMoney(playerid, -pOffer[playerid][offer_price]);

				OnPlayerPaymentResponse(playerid, 0, 1);
			}
		}

		case DIALOG_HURTOWNIA_ILLEGAL:
		{
			if( !response )
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				SendClientMessage(playerid, COLOR_YELLOW, "Call ended.");
				return 1;
			} 

			switch(listitem)
			{
				case 0:
				{
					ProxMessage(playerid, "What you think about Market?", PROX_PHONE);
					SendClientMessage(playerid, COLOR_YELLOW, "[Phone]: I will be waiting near verona mall, try to be in 15 minutes.");
					SetActorPos(ArmDealer, 1081.0221,-1667.5089,13.6265);
					SetActorFacingAngle(ArmDealer, 301.9660);

					foreach(new a: Areas)
					{
						if(Area[a][area_type] == AREA_TYPE_ARMDEALER)
						{
							DestroyDynamicArea(a);
							for(new z=0; e_areas:z != e_areas; z++)
						    {
								Area[a][e_areas:z] = 0;
						    }
						}
					}

					new a_id = CreateDynamicCircle(1081.0221,-1667.5089, 2.0, 0, 0);
					Area[a_id][area_type] = AREA_TYPE_ARMDEALER;
					Iter_Add(Areas, a_id);

					new vid = LoadVehicle("WHERE `vehicle_uid` = 33", true);

					SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
					RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
					SendClientMessage(playerid, COLOR_YELLOW, "Call ended.");
					defer HideActor[540000](ArmDealer, vid);
				}
				case 1:
				{
					ProxMessage(playerid, "What you think about Rodeo?", PROX_PHONE);
					SendClientMessage(playerid, COLOR_YELLOW, "[Phone]: I will be waiting at parking, W.Broadway. Try to be in 15 minutes.");
					SetActorPos(ArmDealer, 198.3572,-1433.5908,13.1116);
					SetActorFacingAngle(ArmDealer, 314.5871);

					foreach(new a: Areas)
					{
						if(Area[a][area_type] == AREA_TYPE_ARMDEALER)
						{
							DestroyDynamicArea(a);
							for(new z=0; e_areas:z != e_areas; z++)
						    {
								Area[a][e_areas:z] = 0;
						    }
						}
					}

					new a_id = CreateDynamicCircle(198.3572,-1433.5908, 2.0, 0, 0);
					Area[a_id][area_type] = AREA_TYPE_ARMDEALER;
					Iter_Add(Areas, a_id);

					new vid = LoadVehicle("WHERE `vehicle_uid` = 55", true);

					SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
					RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
					SendClientMessage(playerid, COLOR_YELLOW, "Call ended.");
					defer HideActor[540000](ArmDealer, vid);
				}	
				case 2:
				{
					ProxMessage(playerid, "What you think about Mullholand?", PROX_PHONE);
					SendClientMessage(playerid, COLOR_YELLOW, "[Phone]: I will be waiting behind 24/7, try to be in 15 minutes.");

					SetActorPos(ArmDealer, 1305.4092,-873.1508,39.5781);
					SetActorFacingAngle(ArmDealer, 283.0870);

					foreach(new a: Areas)
					{
						if(Area[a][area_type] == AREA_TYPE_ARMDEALER)
						{
							DestroyDynamicArea(a);
							for(new z=0; e_areas:z != e_areas; z++)
						    {
								Area[a][e_areas:z] = 0;
						    }
						}
					}

					new a_id = CreateDynamicCircle(1305.4092,-873.1508, 2.0, 0, 0);
					Area[a_id][area_type] = AREA_TYPE_ARMDEALER;
					Iter_Add(Areas, a_id);

					new vid = LoadVehicle("WHERE `vehicle_uid` = 57", true);

					SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
					RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
					SendClientMessage(playerid, COLOR_YELLOW, "Call ended.");
					defer HideActor[540000](ArmDealer, vid);
				}
				case 3:
				{
					ProxMessage(playerid, "What you think about East Los Santos?", PROX_PHONE);
					SendClientMessage(playerid, COLOR_YELLOW, "[Phone]: I will be waiting behind Cluckin Bell, try to be in 15 minutes.");
					SetActorPos(ArmDealer, 2408.7490,-1469.6664,24.0000);
					SetActorFacingAngle(ArmDealer, 172.0870);

					foreach(new a: Areas)
					{
						if(Area[a][area_type] == AREA_TYPE_ARMDEALER)
						{
							DestroyDynamicArea(a);
							for(new z=0; e_areas:z != e_areas; z++)
						    {
								Area[a][e_areas:z] = 0;
						    }
						}
					}

					new a_id = CreateDynamicCircle(2408.7490,-1469.6664, 2.0, 0, 0);
					Area[a_id][area_type] = AREA_TYPE_ARMDEALER;
					Iter_Add(Areas, a_id);

					new vid = LoadVehicle("WHERE `vehicle_uid` = 38", true);

					SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
					RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
					SendClientMessage(playerid, COLOR_YELLOW, "Call ended.");
					defer HideActor[540000](ArmDealer, vid);
				}
				case 4:
				{
					ProxMessage(playerid, "What you think about Ocean Docks?", PROX_PHONE);
					SendClientMessage(playerid, COLOR_YELLOW, "[Phone]: I will be waiting near railroads, next to the bridge. Try to be in 15 minutes.");
					SetActorPos(ArmDealer, 2240.5803,-2152.7539,13.5538);
					SetActorFacingAngle(ArmDealer, 227.0870);

					foreach(new a: Areas)
					{
						if(Area[a][area_type] == AREA_TYPE_ARMDEALER)
						{
							DestroyDynamicArea(a);
							for(new z=0; e_areas:z != e_areas; z++)
						    {
								Area[a][e_areas:z] = 0;
						    }
						}
					}

					new a_id = CreateDynamicCircle(2240.5803,-2152.7539, 2.0, 0, 0);
					Area[a_id][area_type] = AREA_TYPE_ARMDEALER;
					Iter_Add(Areas, a_id);

					new vid = LoadVehicle("WHERE `vehicle_uid` = 34", true);

					SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
					RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
					SendClientMessage(playerid, COLOR_YELLOW, "Call ended.");
					defer HideActor[540000](ArmDealer, vid);
				}
			}

			bot_taken = gettime() + 900;
		}

		case DIALOG_HURTOWNIA_LEGAL:
		{
			if( !response )
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				return 1;
			}

			new gid = pInfo[playerid][player_duty_gid];

			switch(listitem)
			{
				case 0: // Gastronomia
                {
                    if(IsPlayerInAnyGroup(playerid))
                    {
                        if(Group[gid][group_type] == GROUP_TYPE_GASTRO)
                        {
                            new string[1024], count;
                            DynamicGui_Init(playerid);

                            format(string, sizeof(string), "%sProduct\tPrice\n", string);

                            foreach (new prod: Products)
                            {
                                if( Product[prod][product_owner] == PRODUCT_OWNER_GASTRONOMY )
                                {
	                                format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }

	                            if( Product[prod][product_group] == Group[gid][group_uid])
	                            {
	                            	format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }
                            }

                            if( count == 0 ) 
                            {
                            	SendGuiInformation(playerid, "Information", "This type of group has not added products. Report it to administration.");
                            	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                            }
                            else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADDPROD, DIALOG_STYLE_TABLIST_HEADERS, "Hurtownia gastronomiczna", string, "Kup", "WyjdŸ");
                        }
                        else
                        {
                            SendGuiInformation(playerid, ""guiopis"Alert", "You are not on group duty, which has access to this category.\nUse /g slot duty and try again.");
                            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
							RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                        }

                    }
                    else
                    {
                        SendGuiInformation(playerid, ""guiopis"Alert", "You are not belong to any group.");
                        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
						RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                    }
                }
				case 1: // Warsztat
                {
                    if(IsPlayerInAnyGroup(playerid))
                    {
                        if(Group[gid][group_type] == GROUP_TYPE_WORKSHOP)
                        {
                            new string[1024], count;
                            DynamicGui_Init(playerid);

                            format(string, sizeof(string), "%sProduct\tPrice\n", string);

                            foreach (new prod: Products)
                            {
                                if( Product[prod][product_owner] == PRODUCT_OWNER_WORKSHOP )
                                {
	                                format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }

	                            if( Product[prod][product_group] == Group[gid][group_uid])
	                            {
	                            	format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }
                            }

                            if( count == 0 ) 
                            {
                            	SendGuiInformation(playerid, "Information", "This type of group has not added products. Report it to administration.");
                            	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                            }
                            else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADDPROD, DIALOG_STYLE_TABLIST_HEADERS, "Hurtownia warsztatu", string, "Kup", "WyjdŸ");
                        }
                        else
                        {
                            SendGuiInformation(playerid, ""guiopis"Alert", "You are not on group duty, which has access to this category.\nUse /g slot duty and try again.");
                            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
							RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                        }

                    }
                    else
                    {
                        SendGuiInformation(playerid, ""guiopis"Alert", "You are not belong to any group.");
                        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
						RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                    }
                }

				case 2: // Porzadkowe
                {
                    if(IsPlayerInAnyGroup(playerid))
                    {
                        if(Group[gid][group_type] == GROUP_TYPE_LSPD)
                        {
                            new string[1024], count;
                            DynamicGui_Init(playerid);

                            format(string, sizeof(string), "%sProduct\tPrice\n", string);

                            foreach (new prod: Products)
                            {
                                if( Product[prod][product_owner] == PRODUCT_OWNER_LSPD )
                                {
	                                format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }

	                            if( Product[prod][product_group] == Group[gid][group_uid])
	                            {
	                            	format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }
                            }

                            if( count == 0 ) 
                            {
                            	SendGuiInformation(playerid, "Information", "This type of group has not added products. Report it to administration.");
                            	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                            }
                            else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADDPROD, DIALOG_STYLE_TABLIST_HEADERS, "Hurtownia LSPD", string, "Kup", "WyjdŸ");
                        }
                        else
                        {
                            SendGuiInformation(playerid, ""guiopis"Alert", "You are not on group duty, which has access to this category.\nUse /g slot duty and try again.");
                            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
							RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                        }

                    }
                    else
                    {
                        SendGuiInformation(playerid, ""guiopis"Alert", "You are not belong to any group.");
                        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
						RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                    }
                }
                case 3: // Weazel
                {
                    if(IsPlayerInAnyGroup(playerid))
                    {
                        if(Group[gid][group_type] == GROUP_TYPE_SN)
                        {
                            new string[1024], count;
                            DynamicGui_Init(playerid);

                            format(string, sizeof(string), "%sProduct\tPrice\n", string);

                            foreach (new prod: Products)
                            {
                                if( Product[prod][product_owner] == PRODUCT_OWNER_SNEWS )
                                {
	                                format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }

	                            if( Product[prod][product_group] == Group[gid][group_uid])
	                            {
	                            	format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }
                            }

                            if( count == 0 ) 
                            {
                            	SendGuiInformation(playerid, "Information", "This type of group has not added products. Report it to administration.");
                            	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                            }
                            else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADDPROD, DIALOG_STYLE_TABLIST_HEADERS, "Hurtownia Weazel", string, "Kup", "WyjdŸ");
                        }
                        else
                        {
                            SendGuiInformation(playerid, ""guiopis"Alert", "You are not on group duty, which has access to this category.\nUse /g slot duty and try again.");
                            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
							RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                        }

                    }
                    else
                    {
                        SendGuiInformation(playerid, ""guiopis"Alert", "You are not belong to any group.");
                        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
						RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                    }
                }
                case 4: // Ochrona
                {
                    if(IsPlayerInAnyGroup(playerid))
                    {
                        if(Group[gid][group_type] == GROUP_TYPE_SECURITY)
                        {
                            new string[1024], count;
                            DynamicGui_Init(playerid);

                            format(string, sizeof(string), "%sProduct\tPrice\n", string);

                            foreach (new prod: Products)
                            {
                                if( Product[prod][product_owner] == PRODUCT_OWNER_SECURITY )
                                {
	                                format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }

	                            if( Product[prod][product_group] == Group[gid][group_uid])
	                            {
	                            	format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }
                            }

                            if( count == 0 ) 
                            {
                            	SendGuiInformation(playerid, "Information", "This type of group has not added products. Report it to administration.");
                            	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                            }
                            else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADDPROD, DIALOG_STYLE_TABLIST_HEADERS, "Hurtownia ochroniarska", string, "Kup", "WyjdŸ");
                        }
                        else
                        {
                            SendGuiInformation(playerid, ""guiopis"Alert", "You are not on group duty, which has access to this category.\nUse /g slot duty and try again.");
                            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
							RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                        }

                    }
                    else
                    {
                        SendGuiInformation(playerid, ""guiopis"Alert", "You are not belong to any group.");
                        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
						RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                    }
                }
                case 5: // Silownia
                {
                    if(IsPlayerInAnyGroup(playerid))
                    {
                        if(Group[gid][group_type] == GROUP_TYPE_GYM)
                        {
                            new string[1024], count;
                            DynamicGui_Init(playerid);

                            format(string, sizeof(string), "%sProduct\tPrice\n", string);

                            foreach (new prod: Products)
                            {
                                if( Product[prod][product_owner] == PRODUCT_OWNER_GYM )
                                {
	                                format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }

	                            if( Product[prod][product_group] == Group[gid][group_uid])
	                            {
	                            	format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }
                            }

                            if( count == 0 ) 
                            {
                            	SendGuiInformation(playerid, "Information", "This type of group has not added products. Report it to administration.");
                            	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                            }
                            else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADDPROD, DIALOG_STYLE_TABLIST_HEADERS, "Hurtownia si³ownii", string, "Kup", "WyjdŸ");
                        }
                        else
                        {
                            SendGuiInformation(playerid, ""guiopis"Alert", "You are not on group duty, which has access to this category.\nUse /g slot duty and try again.");
                            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
							RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                        }

                    }
                    else
                    {
                        SendGuiInformation(playerid, ""guiopis"Alert", "You are not belong to any group.");
                        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
						RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                    }
                }
                case 6: // Przestêpcze
                {
                    if(IsPlayerInAnyGroup(playerid))
                    {
                        if(Group[gid][group_type] == GROUP_TYPE_GANG)
                        {
                            new string[1024], count;
                            DynamicGui_Init(playerid);

                            format(string, sizeof(string), "%sProduct\tPrice\n", string);

                            foreach (new prod: Products)
                            {
                                if( Product[prod][product_owner] != PRODUCT_OWNER_CRIME ) continue;

                                format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
                                count++;
                            }

                            if( count == 0 ) 
                            {
                            	SendGuiInformation(playerid, "Information", "This type of group has not added products. Report it to administration.");
                            	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                            }
                            else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADDPROD, DIALOG_STYLE_TABLIST_HEADERS, "Hurtownia przestêpczych", string, "Kup", "WyjdŸ");
                        }
                        else
                        {
                            SendGuiInformation(playerid, ""guiopis"Alert", "You are not on group duty, which has access to this category.\nUse /g slot duty and try again.");
                            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
							RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                        }

                    }
                    else
                    {
                        SendGuiInformation(playerid, ""guiopis"Alert", "You are not belong to any group.");
                        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
						RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                    }
                }
                case 7: // ERU
                {
                    if(IsPlayerInAnyGroup(playerid))
                    {
                        if(Group[gid][group_type] == GROUP_TYPE_MEDIC)
                        {
                            new string[1024], count;
                            DynamicGui_Init(playerid);

                            format(string, sizeof(string), "%sProduct\tPrice\n", string);

                            foreach (new prod: Products)
                            {
                                if( Product[prod][product_owner] == PRODUCT_OWNER_ERU )
                                {
	                                format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }

	                            if( Product[prod][product_group] == Group[gid][group_uid])
	                            {
	                            	format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }
                            }

                            if( count == 0 ) 
                            {
                            	SendGuiInformation(playerid, "Information", "This type of group has not added products. Report it to administration.");
                            	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                            }
                            else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADDPROD, DIALOG_STYLE_TABLIST_HEADERS, "Hurtownia si³ownii", string, "Kup", "WyjdŸ");
                        }
                        else
                        {
                            SendGuiInformation(playerid, ""guiopis"Alert", "You are not on group duty, which has access to this category.\nUse /g slot duty and try again.");
                            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
							RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                        }

                    }
                    else
                    {
                        SendGuiInformation(playerid, ""guiopis"Alert", "You are not belong to any group.");
                        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
						RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                    }
                }	
                case 8: // Nieokreœlone
                {
                    if(IsPlayerInAnyGroup(playerid))
                    {
                        if(Group[gid][group_type] == GROUP_TYPE_NONE)
                        {
                            new string[1024], count;
                            DynamicGui_Init(playerid);

                            format(string, sizeof(string), "%sProduct\tPrice\n", string);

                            foreach (new prod: Products)
                            {
	                            if( Product[prod][product_group] == Group[gid][group_uid])
	                            {
	                            	format(string, sizeof(string), "%s %s\t$%d \n", string, Product[prod][product_name], Product[prod][product_price]);
	                                DynamicGui_AddRow(playerid, DG_PRODS_ITEM_ROW, prod);
	                                count++;
	                            }
                            }

                            if( count == 0 ) 
                            {
                            	SendGuiInformation(playerid, "Information", "This type of group has not added products. Report it to an administrator.");
                            	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                            }
                            else ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADDPROD, DIALOG_STYLE_TABLIST_HEADERS, "Warehouse - nieokreœlone", string, "Kup", "WyjdŸ");
                        }
                        else
                        {
                            SendGuiInformation(playerid, ""guiopis"Alert", "You are not on on duty, which has access to this category.\nUse /g slot duty and try again.");
                            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
							RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                        }

                    }
                    else
                    {
                        SendGuiInformation(playerid, ""guiopis"Alert", "You do not belong to any specific group.");
                        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
						RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                    }
                }							
			}
		}

		case DIALOG_HURTOWNIA_ADD:
        {
            new dg_value = DynamicGui_GetValue(playerid, listitem), dg_data = DynamicGui_GetDataInt(playerid, listitem);
            if( !response ) return 1;

            if( response && dg_value == DG_PRODS_ITEM_ROW )
            {
                new prod_id = dg_data;
                new prod_name[40];
                format(prod_name, sizeof(prod_name), "%s", Product[prod_id][product_name]);

                if(pInfo[playerid][player_money] < Product[prod_id][product_price]) return SendGuiInformation(playerid, "Information", "You don't have enough money.");

                GivePlayerMoney(playerid,-Product[prod_id][product_price]);
                new iid = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, Product[prod_id][product_type], Product[prod_id][product_model], Product[prod_id][product_value1], Product[prod_id][product_value2], prod_name);
                if(Product[prod_id][product_extra] > 0)
                {
	                PlayerItem[playerid][iid][player_item_extraid] = Product[prod_id][product_extra];
	                mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_extraid = %d WHERE item_uid =%d", PlayerItem[playerid][iid][player_item_extraid], PlayerItem[playerid][iid][player_item_uid]));
	            }
                SendGuiInformation(playerid, "Information", "Product has been added to your inventory.");
            }
        }

        case DIALOG_HURTOWNIA_ADDPROD_COUNT:
        {
            if( !response )
            {
            	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
            	return 1;
            }

            new dg_value = pInfo[playerid][player_dialog_tmp1];
            new dg_data = pInfo[playerid][player_dialog_tmp2];

            if( response && dg_value == DG_PRODS_ITEM_ROW )
            {
                new count = strval(inputtext);
            	new prod_id = dg_data;
                
                new gid = pInfo[playerid][player_duty_gid];
                if(gid == -1 )
                {
                	SendGuiInformation(playerid, "Information", "You are not on group duty.");
                	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
					RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
					return 1;
				}

                if(Group[gid][group_capital] < Product[prod_id][product_price]*count)
                {
                	SendGuiInformation(playerid, "Information", "You don't have enough money.");
                	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
					RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
                	return 1;
                }

                GiveGroupCapital(gid, -Product[prod_id][product_price]*count);
               	
               	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
               	RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
               	defer DeliverProduct[540000](gid, prod_id, count);
                SendGuiInformation(playerid, "Information", "Product has been ordered and will be devilered to your group storage in 10 minutes.");
            }
        }

        case DIALOG_HURTOWNIA_ADDPROD:
        {
            new dg_value = DynamicGui_GetValue(playerid, listitem), dg_data = DynamicGui_GetDataInt(playerid, listitem);
            if( !response )
            {
            	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
            	return 1;
            }

            pInfo[playerid][player_dialog_tmp1] = dg_value;
            pInfo[playerid][player_dialog_tmp2] = dg_data;
            
            ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ADDPROD_COUNT, DIALOG_STYLE_INPUT, "Wholesale", "Please input amount of products you want to order:", "Order", "Close");
        }

        case DIALOG_HURTOWNIA_ILLEGAL_ADD:
        {
            new dg_value = DynamicGui_GetValue(playerid, listitem), dg_data = DynamicGui_GetDataInt(playerid, listitem);
            if( !response )
            {
            	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
            	return 1;
            }

            pInfo[playerid][player_dialog_tmp1] = dg_value;
            pInfo[playerid][player_dialog_tmp2] = dg_data;

            ShowPlayerDialog(playerid, DIALOG_HURTOWNIA_ILLEGAL_COUNT, DIALOG_STYLE_INPUT, "Marcus Bradford - offer", "Please input amount of products you want to buy:", "Buy", "Close");
        }

        case DIALOG_HURTOWNIA_ILLEGAL_COUNT:
        {
            if( !response )
            {
            	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
            	return 1;
            }
            new dg_value = pInfo[playerid][player_dialog_tmp1];
            new dg_data = pInfo[playerid][player_dialog_tmp2];

            if( dg_value == DG_PRODS_ITEM_ROW )
            {
            	new count = strval(inputtext);
            	new prod_id = dg_data;

        	 	if(count <= 0) return SendGuiInformation(playerid, "Information", "Invalid amount of product.");

        	 	if(pInfo[playerid][player_money] < Product[prod_id][product_price] * count)
        	 	{
        	 		SendGuiInformation(playerid, "Information", "You don't have enough money.");
	            	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
					RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
	            	return 1;
        	 	}

        	 	if(Product[prod_id][product_limit_used] == Product[prod_id][product_limit])
        	 	{
        	 		SendGuiInformation(playerid, "Information", "Weekly limit has been exceeded.");
        	 		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
					RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
	            	return 1;
	            }

        	 	if(count + Product[prod_id][product_limit_used] > Product[prod_id][product_limit])
        	 	{
        	 		SendGuiInformation(playerid, "Information", "This amount exceeds the weekly limit.");
        	 		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
					RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
	            	return 1;
	            }

        	 	new prod_name[40];
            	format(prod_name, sizeof(prod_name), "%s", Product[prod_id][product_name]);

            	GivePlayerMoney(playerid, -Product[prod_id][product_price]*count);

            	if(Product[prod_id][product_type] == ITEM_TYPE_DRUG)
            	{
            		Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, Product[prod_id][product_type], Product[prod_id][product_model], Product[prod_id][product_value1], count, prod_name);
            	}
            	else
            	{
	            	for(new c;c<count;c++)
	            	{
	        			Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, Product[prod_id][product_type], Product[prod_id][product_model], Product[prod_id][product_value1], Product[prod_id][product_value2], prod_name);
	            	}
	            }
	            
            	Product[prod_id][product_limit_used] += count;
            	mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_limit_used = %d WHERE product_uid = %d", Product[prod_id][product_limit_used], Product[prod_id][product_id]));
            	SendGuiInformation(playerid, "Information", "Transaction complete.");
            }
        }
		
      	case DIALOG_CD_LINK:
        {
            if( !response ) return 1;

            new link[256], query[400];
            strmid(link, inputtext, 0, 256);
            
            new itemid = DynamicGui_GetDialogValue(playerid);

            new cdid = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_PLATE, 1962, 0, 0, "Audio CD");
            mysql_format(mySQLconnection, query, sizeof(query), "INSERT INTO ipb_cds (`cd_uid`,`cd_link`,`cd_item`) VALUES (null, '%e', %d)", link, PlayerItem[playerid][cdid][player_item_uid]);

            mysql_query(mySQLconnection, query);
            new val1 = cache_insert_id();

            mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_value1 = %d  WHERE item_uid = %d", val1, PlayerItem[playerid][cdid][player_item_uid]));
            PlayerItem[playerid][cdid][player_item_value1] = val1;

            SendGuiInformation(playerid, "Information", "Audio CD has been made.");
            DeleteItem(itemid, true, playerid);
        }
	}
	return 1;
}
