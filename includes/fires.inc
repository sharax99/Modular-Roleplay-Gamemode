new Float:FirePoints[16][3] = {
{2110.9216,-1244.1956,25.8516},
{2579.6021,-1033.1991,69.5798},
{2673.1675,-2020.2915,14.1682},
{2336.8188,-1899.1926,14.3245},
{1873.5493,-2070.7568,15.4971},
{1762.3302,-2101.9927,13.8570},
{2016.2004,-1716.9417,14.1250},
{2409.0859,-1674.7617,14.3593},
{768.0783,-1655.8044,5.6094},
{827.6642,-858.0377,70.330},
{2023.6371,-1053.3273,25.5961},
{2015.5984,-977.6002,36.9531},
{2467.7795,-1200.4070,36.8117},
{657.2266,-1652.6771,15.4063},
{2243.4834,-1397.2430,24.5738},
{2308.8540,-1714.3331,14.9801}
};

stock SpreadFire()
{
	if( Fire[fire_is_burning] )
	{
		foreach(new fsid : FireSources)
		{
			if( Iter_Free(FireSources) == -1 ) break;
			if( FireSource[fsid][fs_start_time] == -1 ) continue;
			
			if( gettime() - FireSource[fsid][fs_start_time] > FIRE_SPREAD_DELAY )
			{
				FireSource[fsid][fs_start_time] = -1;
				new Float:pos[3];
				GetDynamicObjectPos(FireSource[fsid][fs_object], pos[0], pos[1], pos[2]);
				
				if( random(2) == 0 ) pos[0] -= 2.0;
				else pos[0] += 2.0;
				
				if( random(2) == 0 ) pos[1] -= 2.0;
				else pos[1] += 2.0;
				
				CreateFireSource(pos[0], pos[1], pos[2]+2.5);
			}
		}
	}
}

stock SetupFires()
{	
	if(Fire[fire_is_burning]) return 1;

	if(!IsAnyFirefighterOnline()) return 1;

	Fire[fire_is_burning] = true;
	
	new fid;
    fid = random(sizeof(FirePoints));
	Fire[fire_map_icon] = CreateDynamicMapIcon(FirePoints[fid][0],FirePoints[fid][1], FirePoints[fid][2], 20, 0, -1, -1, 0, 100000.0);
	Streamer_SetIntData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_STYLE, 1);
	Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, 0);
	
	foreach(new p : Player)
	{
		Streamer_UpdateEx(p, FirePoints[fid][0], FirePoints[fid][1], FirePoints[fid][2], 0, 0);
	}
	
	CreateFireSource(FirePoints[fid][0], FirePoints[fid][1], FirePoints[fid][2]);
	

	foreach(new p : Player)
	{
		if( IsPlayerInAnyGroup(p) )
		{
			new gid = pInfo[p][player_duty_gid];
			if(gid != -1)
			{
				if( Group[gid][group_flags] & GROUP_FLAG_MEDIC)
				{
					SendClientMessage(p, COLOR_GOLD, "[911 report]: Detected fire, positon marked on map!");
					
					Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, p);
				}
			}
		}
	}
	return 1;
}

stock SetupScriptedFire(source)
{	
	if(Fire[fire_is_burning]) return 1;

	if(!IsAnyFirefighterOnline()) return 1;

	Fire[fire_is_burning] = true;
	
	Fire[fire_map_icon] = CreateDynamicMapIcon(FireSource[source][fs_pos][0],FireSource[source][fs_pos][1], FireSource[source][fs_pos][2], 20, 0, -1, -1, 0, 100000.0);
	Streamer_SetIntData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_STYLE, 1);
	Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, 0);
	
	foreach(new p : Player)
	{
		Streamer_UpdateEx(p, FireSource[source][fs_pos][0], FireSource[source][fs_pos][1], FireSource[source][fs_pos][2], 0, 0);
	}

	foreach(new p : Player)
	{
		if( IsPlayerInAnyGroup(p) )
		{
			new gid = pInfo[p][player_duty_gid];
			if(gid != -1)
			{
				if( Group[gid][group_flags] & GROUP_FLAG_MEDIC)
				{
					SendClientMessage(p, COLOR_GOLD, "Detected fire, posiont marked on map!");
					Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, p);
				}
			}
		}
	}
	return 1;
}

stock CreateFireSource(Float:x, Float:y, Float:z)
{
	new fsid = Iter_Free(FireSources);
	if( fsid == -1 ) return;
	
	FireSource[fsid][fs_object] = CreateDynamicObject(18688, x, y, z-2.5, 0.0, 0.0, 0.0);
	FireSource[fsid][fs_health] = 100.0;
	FireSource[fsid][fs_label] = CreateDynamic3DTextLabel("100.00%", 0xF07800FF, x, y, z, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1);
	FireSource[fsid][fs_start_time] = gettime();

	new a_id = CreateDynamicCircle(x, y, 15.0, 0, -1);
	Area[a_id][area_type] = AREA_TYPE_FIRE;

	Iter_Add(FireSources, fsid);
}

stock CreateScriptedSource(did, Float:x, Float:y, Float:z)
{
	new fsid = Iter_Free(FireSources);
	if( fsid == -1 ) return -1;
	
	FireSource[fsid][fs_object] = CreateDynamicObject(18688, x, y, z-2.5, 0.0, 0.0, 0.0);
	FireSource[fsid][fs_health] = 5.0;
	FireSource[fsid][fs_label] = CreateDynamic3DTextLabel("5.00%", 0xF07800FF, x, y, z, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1);
	FireSource[fsid][fs_start_time] = gettime();
	FireSource[fsid][fs_pos][0] = x;
	FireSource[fsid][fs_pos][1] = y;
	FireSource[fsid][fs_pos][2] = z;
	FireSource[fsid][fs_door] = did;

	new a_id = CreateDynamicCircle(x, y, 15.0, 0, -1);
	Area[a_id][area_type] = AREA_TYPE_FIRE;
	Iter_Add(Areas, a_id);

	Iter_Add(FireSources, fsid);

	return fsid;
}

stock StopFireSource(fsid)
{
	DestroyDynamicObject(FireSource[fsid][fs_object]);
	DestroyDynamic3DTextLabel(FireSource[fsid][fs_label]);
	
	for(new z=0; e_fire_sources:z != e_fire_sources; z++)
    {
		FireSource[fsid][e_fire_sources:z] = 0;
    }
	
	Iter_Remove(FireSources, fsid);

	if( Iter_Count(FireSources) == 0 )
	{
		GiveMoneyForFireFighters();
		StopFire();
	}
}

stock GiveMoneyForFireFighters()
{
	new price = 75 + random(50);
	foreach(new p : Player)
	{
		if( IsPlayerInAnyGroup(p) )
		{
			new gid = pInfo[p][player_duty_gid];
			if( Group[gid][group_flags] & GROUP_FLAG_MEDIC)
			{
				if(GetPVarInt(p, "fire") >0 )
				{
					GivePlayerMoney(p, price);
					SendClientMessage(p, COLOR_GOLD, sprintf("Received %d for rescue action.", price));
				}
			}
		}
	}
}

stock StopFire()
{
	DestroyDynamicMapIcon(Fire[fire_map_icon]);
				
	foreach(new p : Player)
	{
		if( IsPlayerInAnyGroup(p) )
		{
			new gid = pInfo[p][player_duty_gid];
			if( Group[gid][group_flags] & GROUP_FLAG_MEDIC)
			{
				SendClientMessage(p, COLOR_GOLD, "Fire stopped.");
				DestroyDynamicArea(GetPVarInt(p, "fire"));
			}
		}
	}
	Fire[fire_is_burning] = false;

	foreach(new areaid: Areas)
	{
		if(Area[areaid][area_type] == AREA_TYPE_FIRE)
		{
			new next;
		    Iter_SafeRemove(Areas, areaid, next);
		    areaid = next;
		}
	}
}

stock IsAnyFirefighterOnline()
{
	foreach(new p : Player)
	{
		if( IsPlayerInAnyGroup(p) )
		{
			new gid = pInfo[p][player_duty_gid];
			if(gid != -1 )
			{
				if( Group[gid][group_flags] & GROUP_FLAG_MEDIC)
				{
					return 1;
				}
			}
		}
	}
	
	return 0;
}

Float:DistanceCameraTargetToLocation(Float:CamX, Float:CamY, Float:CamZ,   Float:ObjX, Float:ObjY, Float:ObjZ,   Float:FrX, Float:FrY, Float:FrZ) {

	new Float:TGTDistance;
	TGTDistance = floatsqroot((CamX - ObjX) * (CamX - ObjX) + (CamY - ObjY) * (CamY - ObjY) + (CamZ - ObjZ) * (CamZ - ObjZ));
	new Float:tmpX, Float:tmpY, Float:tmpZ;
	tmpX = FrX * TGTDistance + CamX;
	tmpY = FrY * TGTDistance + CamY;
	tmpZ = FrZ * TGTDistance + CamZ;
	return floatsqroot((tmpX - ObjX) * (tmpX - ObjX) + (tmpY - ObjY) * (tmpY - ObjY) + (tmpZ - ObjZ) * (tmpZ - ObjZ));
}

stock PlayerFaces(playerid, Float:x, Float:y, Float:z, Float:radius)
{
        new Float:cx,Float:cy,Float:cz,Float:fx,Float:fy,Float:fz;
        GetPlayerCameraPos(playerid, cx, cy, cz);
        GetPlayerCameraFrontVector(playerid, fx, fy, fz);
        return (radius >= DistanceCameraTargetToLocation(cx, cy, cz, x, y, z, fx, fy, fz));
}