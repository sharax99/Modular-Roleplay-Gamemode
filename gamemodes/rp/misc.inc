stock GetDayName(d=0,m=0,y=0) {
    /*
    0=Invalid date
    1=Sunday
    2=Monday
    ...
    7=Saturday
    */
    if(d==0&&m==0&&y==0) { 
        getdate(y, m, d);
    }

    new month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        i;

    if (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))
        month[1] = 29;

    if (y < 1900 || m < 1 || m > 12 || d < 1 || d > month[m - 1])
        return 0;

    for (i = 1900; i < y; i++) {
        if (i % 4 == 0 && (i % 100 != 0 || i % 400 == 0))
            d += 366;
        else
            d += 365;
    }

    for (i = 0; i < m - 1; i++) {
        d += month[i];
    }

    return d%7+1;
}

stock GetFormattedDate(timestamp, dest[])
{
	new date[40], year, month, day, hour, minute, second;
	TimestampToDate(timestamp, year, month, day, hour, minute, second, 1, 0);
	
	format(date, sizeof(date), "%02d.%02d.%d %02d:%02d:%02d", day, month, year, hour, minute, second);
	
	strcopy(dest, date);
}

forward Float:GetDistanceBetweenPlayers(playerid,targetplayerid);
public Float:GetDistanceBetweenPlayers(playerid,targetplayerid)
{
    new Float:x1,Float:y1,Float:z1,Float:x2,Float:y2,Float:z2;
    if(!IsPlayerConnected(playerid) || !IsPlayerConnected(targetplayerid)) {
        return -1.00;
    }
    GetPlayerPos(playerid,x1,y1,z1);
    GetPlayerPos(targetplayerid,x2,y2,z2);
    return floatsqroot(floatpower(floatabs(floatsub(x2,x1)),2)+floatpower(floatabs(floatsub(y2,y1)),2)+floatpower(floatabs(floatsub(z2,z1)),2));
}

stock srp_AddVehicleComponent(vehicleid, componentid)
{
	new slot = GetVehicleComponentType(componentid);
	if(slot != -1)
	{
	    AddVehicleComponent(vehicleid, componentid);
	    Vehicle[vehicleid][vehicle_component][slot] = componentid - 999;
	}
	return 1;
}

stock randomString() 
{
    return (random(1000) %2 ==0 ) ? (65 + random(26)) : (65+random(26));
}

stock randomNumber() 
{
    return (random(1000) %2 ==0 ) ? (48 + random(9)) : (48+random(9));
}

stock GetPlayerSpeed(playerid)
{
    new Float:ST[4];
    if(IsPlayerInAnyVehicle(playerid))
    GetVehicleVelocity(GetPlayerVehicleID(playerid),ST[0],ST[1],ST[2]);
    else GetPlayerVelocity(playerid,ST[0],ST[1],ST[2]);
    ST[3] = floatsqroot(floatpower(floatabs(ST[0]), 2.0) + floatpower(floatabs(ST[1]), 2.0) + floatpower(floatabs(ST[2]), 2.0)) * 179.28625;
    return floatround(ST[3]);
}

stock ShowPlayerStats(playerid, giveplayer_id)
{
    new list_stats[600], string[128], IP[16], praca[20], animacja[10];

    new Float:arhrs =  floatdiv((pInfo[playerid][player_arrest] - gettime()), 3600);
    new Float:armins = floatdiv((pInfo[playerid][player_arrest] - gettime()), 60);

    GetPlayerIp(giveplayer_id, IP, sizeof(IP));

    format(list_stats, sizeof(list_stats), "%sTime online:\t%dh %dm\n", list_stats, pInfo[giveplayer_id][player_hours], pInfo[giveplayer_id][player_minutes]);
    format(list_stats, sizeof(list_stats), "%sOOC name:\t%s\n", list_stats, pGlobal[giveplayer_id][glo_name]);
    if(pInfo[giveplayer_id][player_health] <= 40)
    {
        format(list_stats, sizeof(list_stats), "%sHealth:\t{FB5006}%.2f{FFFFFF} HP, %dmin BW\n", list_stats, pInfo[giveplayer_id][player_health], pInfo[giveplayer_id][player_bw]);
    }
    else
    {
        format(list_stats, sizeof(list_stats), "%sHealth:\t%.2f HP, %dmin BW\n", list_stats, pInfo[giveplayer_id][player_health], pInfo[giveplayer_id][player_bw]/60);
    }
    format(list_stats, sizeof(list_stats), "%sVirtual World:\t%d\n", list_stats, GetPlayerVirtualWorld(giveplayer_id));
    format(list_stats, sizeof(list_stats), "%sCash:\t$%d\n", list_stats, pInfo[giveplayer_id][player_money]);
 
    format(list_stats, sizeof(list_stats), "%sBank:\t$%d\n", list_stats, pInfo[giveplayer_id][player_bank_money]);
    format(list_stats, sizeof(list_stats), "%sBank number:\t%d\n", list_stats, pInfo[giveplayer_id][player_bank_number]);

    format(list_stats, sizeof(list_stats), "%sWeapon skill:\t%0.2f\n", list_stats, pInfo[giveplayer_id][player_weapon_skill]);
   
   	if(pInfo[playerid][player_arrest] > 0)
   	{
    	format(list_stats, sizeof(list_stats), "%sArrest time:\t%0.2fh %0.2fmin\n", list_stats, arhrs, armins);
   	}
   	else
   	{
   		format(list_stats, sizeof(list_stats), "%sArrest time:\t0h 0min\n", list_stats);
   	}
   	
    format(list_stats, sizeof(list_stats), "%sAge:\t%d\n", list_stats, 2016-pInfo[giveplayer_id][player_age]);
 
 
    format(list_stats, sizeof(list_stats), "%sSex:\t%s\n", list_stats, GetPlayerGender(giveplayer_id));
    format(list_stats, sizeof(list_stats), "%sSkin:\t%d\n", list_stats, GetPlayerSkin(giveplayer_id));
    format(list_stats, sizeof(list_stats), "%sStrenght:\t%.2f%%\n", list_stats, pInfo[giveplayer_id][player_strength]);
   
    format(praca, sizeof(praca), "%s", GetPlayerJob(giveplayer_id));
    format(list_stats, sizeof(list_stats), "%sJob:\t%s\n", list_stats, praca);

    format(list_stats, sizeof(list_stats), "%s------------------------------\n", list_stats);
    format(list_stats, sizeof(list_stats), "%s{a9c4e4}Options:{FFFFFF}\n", list_stats);
 
    if(pInfo[giveplayer_id][player_walking_anim]!=0)
    {
        format(animacja, sizeof(animacja), "On");
    }
    else
    {
        format(animacja, sizeof(animacja), "Off");
    }
 
    format(list_stats, sizeof(list_stats), "%sWalking animation:\t%s\n", list_stats, animacja);
    format(list_stats, sizeof(list_stats), "%sTurn off walking animation\t\n", list_stats);
    format(list_stats, sizeof(list_stats), "%sEdit weapon position\t\n", list_stats);
    format(list_stats, sizeof(list_stats), "%sPremium services\t\n", list_stats);
    format(list_stats, sizeof(list_stats), "%s------------------------------\n", list_stats);
    format(list_stats, sizeof(list_stats), "%s{a9c4e4}Groups:{FFFFFF}\n", list_stats);

    for(new i;i<5;i++)
	{
		new gid = pGroup[giveplayer_id][i][pg_id];
		if( gid != -1 )
		{
			format(list_stats, sizeof(list_stats), "%s%d. %s (%d)\n", list_stats, i+1, Group[gid][group_name], Group[gid][group_uid]);
		}
	}
 
    format(string, sizeof(string), "{BEBEBE}%s [ID: %d | GID: %d | UID: %d | IP: %s]\t", pInfo[giveplayer_id][player_name], giveplayer_id, gInfo[giveplayer_id][global_id], pInfo[giveplayer_id][player_id], IP);
    ShowPlayerDialog(playerid, DIALOG_STATS, DIALOG_STYLE_TABLIST, string, list_stats, "Close", "");
    return 1;
}

stock GetVehicleSpeed(vehicleid) 
{ 
    if(vehicleid != INVALID_VEHICLE_ID) 
    { 
        new Float:Pos[3],Float:VS ; 
        GetVehicleVelocity(vehicleid, Pos[0], Pos[1], Pos[2]); 
        VS = floatsqroot(Pos[0]*Pos[0] + Pos[1]*Pos[1] + Pos[2]*Pos[2])*200; 
        return floatround(VS,floatround_round); 
    } 
    return INVALID_VEHICLE_ID; 
} 

stock GetXYInFrontOfPlayer(playerid, &Float:x, &Float:y, Float:distance) //by Y_Less
{
 	new Float:a;
 	GetPlayerPos(playerid, x, y, a);
 	GetPlayerFacingAngle(playerid, a);
 	
 	x += (distance * floatsin(-a, degrees));
 	y += (distance * floatcos(-a, degrees));
}

stock mysql_query_format(format_query[], va_args<>)
{
	new query[1024];
	
	va_format(query, sizeof(query), format_query, va_start<1>);
	mysql_query(mySQLconnection, query);
	
	return 1;
}

stock IsObjectInSphere(objectid,Float:x,Float:y,Float:z,Float:radius2)
{
    new Float:x1,Float:y1,Float:z1,Float:tmpdis;
    GetObjectPos(objectid,x1,y1,z1);
    tmpdis = floatsqroot(floatpower(floatabs(floatsub(x,x1)),2)+ floatpower(floatabs(floatsub(y,y1)),2)+ floatpower(floatabs(floatsub(z,z1)),2));
    if(tmpdis < radius2) return 1;
    return 0;
}

stock IsPlayerInMotel(playerid)
{
	foreach(new place: Places)
	{
		if(Place[place][place_type] == PLACE_TYPE_HOTEL)
		{
			if(IsPlayerInRangeOfPoint(playerid, 5.0, Place[place][place_pos][0], Place[place][place_pos][1], Place[place][place_pos][2]) && GetPlayerVirtualWorld(playerid) == Place[place][place_world] )
			{
				return 1;
			}
		}
	}
	return 0;
}


stock IsPlayerInPharmacy(playerid)
{
	foreach(new place: Places)
	{
		if(Place[place][place_type] == PLACE_TYPE_PHARMACY)
		{
			if(IsPlayerInRangeOfPoint(playerid, 10.0, Place[place][place_pos][0], Place[place][place_pos][1], Place[place][place_pos][2]) && GetPlayerVirtualWorld(playerid) == Place[place][place_world])
			{
				return 1;
			}
		}
	}
	return 0;
}

stock IsPlayerInVictim(playerid)
{
	foreach(new place: Places)
	{
		if(Place[place][place_type] == PLACE_TYPE_VICTIM)
		{
			if(IsPlayerInRangeOfPoint(playerid, 10.0, Place[place][place_pos][0], Place[place][place_pos][1], Place[place][place_pos][2]) && GetPlayerVirtualWorld(playerid) == Place[place][place_world])
			{
				return 1;
			}
		}
	}
	return 0;
}


stock IsPlayerInBank(playerid)
{
	foreach(new place: Places)
	{
		if(Place[place][place_type] == PLACE_TYPE_BANK)
		{
			if(IsPlayerInRangeOfPoint(playerid, 8.0, Place[place][place_pos][0], Place[place][place_pos][1], Place[place][place_pos][2]) && GetPlayerVirtualWorld(playerid) == Place[place][place_world])
			{
				return 1;
			}
		}
	}
	return 0;
}

stock IsPlayerInStore(playerid)
{
	foreach(new place: Places)
	{
		if(Place[place][place_type] == PLACE_TYPE_SHOP)
		{
			if(IsPlayerInRangeOfPoint(playerid, 5.0, Place[place][place_pos][0], Place[place][place_pos][1], Place[place][place_pos][2]) && GetPlayerVirtualWorld(playerid) == Place[place][place_world])
			{
				return 1;
			}
		}
	}
	return 0;
}

stock IsPlayerInAmmunation(playerid)
{
	foreach(new place: Places)
	{
		if(Place[place][place_type] == PLACE_TYPE_AMMUNATION)
		{
			if(IsPlayerInRangeOfPoint(playerid, 5.0, Place[place][place_pos][0], Place[place][place_pos][1], Place[place][place_pos][2]) && GetPlayerVirtualWorld(playerid) == Place[place][place_world])
			{
				return 1;
			}
		}
	}
	return 0;
}

stock IsPlayerInToolShop(playerid)
{
	foreach(new place: Places)
	{
		if(Place[place][place_type] == PLACE_TYPE_TOOLSHOP)
		{
			if(IsPlayerInRangeOfPoint(playerid, 5.0, Place[place][place_pos][0], Place[place][place_pos][1], Place[place][place_pos][2]) && GetPlayerVirtualWorld(playerid) == Place[place][place_world])
			{
				return 1;
			}
		}
	}
	return 0;
}

stock IsPlayerInCarDealer(playerid)
{
	foreach(new place: Places)
	{
		if(Place[place][place_type] == PLACE_TYPE_CARDEALER)
		{
			if(IsPlayerInRangeOfPoint(playerid, 5.0, Place[place][place_pos][0], Place[place][place_pos][1], Place[place][place_pos][2]) && GetPlayerVirtualWorld(playerid) == Place[place][place_world])
			{
				return 1;
			}
		}
	}
	return 0;
}

stock GetBodyPartName(bodypart)
{
	new part[20];
	switch(bodypart)
	{
		case BODY_PART_TORSO:
		{
			format(part, sizeof(part), "TORSO");
		}
		case BODY_PART_GROIN:
		{
			format(part, sizeof(part), "GROIN");
		}
		case BODY_PART_LEFT_ARM:
		{
			format(part, sizeof(part), "LEFT ARM");
		}
		case BODY_PART_RIGHT_ARM:
		{
			format(part, sizeof(part), "RIGHT ARM");
		}
		case BODY_PART_LEFT_LEG:
		{
			format(part, sizeof(part), "LEFT LEG");
		}
		case BODY_PART_RIGHT_LEG:
		{
			format(part, sizeof(part), "RIGHT LEG");
		}
		case BODY_PART_HEAD:
		{
			format(part, sizeof(part), "HEAD");
		}
		default:
		{
			format(part, sizeof(part), "NONE");
		}
	}
	return part;
}


stock IsPlayerInWater(playerid)
{
        new Float:Z;
        GetPlayerPos(playerid,Z,Z,Z);
        if(Z < 0.7) switch(GetPlayerAnimationIndex(playerid)) { case 1543,1538,1539: return 1; }
        if(GetPlayerDistanceFromPoint(playerid,-965,2438,42) <= 700 && Z < 45)return 1;
        new Float:water_places[][] =
        {
                {25.0,  2313.0, -1417.0,        23.0},
                {15.0,  1280.0, -773.0,         1082.0},
                {15.0,  1279.0, -804.0,         86.0},
                {20.0,  1094.0, -674.0,         111.0},
                {26.0,  194.0,  -1232.0,        76.0},
                {25.0,  2583.0, 2385.0,         15.0},
                {25.0,  225.0,  -1187.0,        73.0},
                {50.0,  1973.0, -1198.0,        17.0}
        };
        for(new t=0; t < sizeof water_places; t++)
                if(GetPlayerDistanceFromPoint(playerid,water_places[t][1],water_places[t][2],water_places[t][3]) <= water_places[t][0]) return 1;
        return 0;
}

stock HexToInt(string[])
{
	if( string[0] == 0 ) return 0;
	new i, cur = 1, res = 0;
	for(i = strlen(string);i > 0;i--) 
	{
		if (string[i-1]<58) res=res+cur*(string[i-1]-48); 
		else res=res+cur*(string[i-1]-65+10);
		cur=cur*16;
	}
	return res;
}

stock GetWeaponSlot(weaponid)
{
	new slot = -1;
	switch(weaponid)
	{
		case 0,1: slot = 0;
		case 2 .. 9: slot = 1;
		case 10 .. 15: slot = 10;
		case 16 .. 18, 39: slot = 8;
		case 22 .. 24: slot =2;
		case 25 .. 27: slot = 3;
		case 28, 29, 32: slot = 4;
		case 30, 31: slot = 5;
		case 33, 34: slot = 6;
		case 35 .. 38: slot = 7;
		case 40: slot = 12;
		case 41 .. 43: slot = 9;
		case 44 .. 46: slot = 11;
	}
	return slot;
}

stock GetWeaponType(weaponid)
{
	new type = -1;
	
	switch(weaponid)
	{
		case 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15: type = WEAPON_TYPE_MELEE;
		case 22,23,24,28,32: type = WEAPON_TYPE_SHORT;
		case 25,26,27,29,30,31,33,34: type = WEAPON_TYPE_LONG;
		case 16,17,18,39,40: type = WEAPON_TYPE_GRENADE;
		
		default: type = WEAPON_TYPE_SPECIAL;
	}
	
	return type;
}

stock Float:GetPlayerDistanceFromPlayer(playerid, targetid)
{
	new Float:fDist[3];
	GetPlayerPos(playerid, fDist[0], fDist[1], fDist[2]);
	return GetPlayerDistanceFromPoint(targetid, fDist[0], fDist[1], fDist[2]);
}

/*
/   distance - odleglosc od gracza
/   angle - +30.0 gdy chcemy przesunac wzglednie w lewo oraz -30.0 gdy wzglednie prawo
/
*/
stock GetPointInAngleOfPlayer(playerid, &Float:x, &Float:y, &Float:z, Float:distance, Float:angle)
{
	new Float:current_angle;
	GetPlayerPos(playerid, x, y, z);
	
	GetPlayerFacingAngle(playerid, current_angle);

	if( IsPlayerInAnyVehicle(playerid) ) 
	{
        GetVehicleZAngle(GetPlayerVehicleID(playerid), current_angle);
    }
	
	new Float:a = current_angle + angle;
	
	x += (distance * floatsin(-a, degrees));
	y += (distance * floatcos(-a, degrees));
}

stock GetRelativeDate(timestamp, dest[], length = sizeof dest)
{
	if( timestamp == 0 ) strcopy(dest, "Never");
	else
	{
		new 
			year, month, day, hour, minute, second,
			t_year, t_month, t_day, t_hour, t_minute, t_second;
		
		// Pobieramy date ostatniego logowania
		TimestampToDate(timestamp, t_year, t_month, t_day, t_hour, t_minute, t_second, 1, 0);
		
		// Na poczatku sprawdzamy czy to czasem nie bylo dzisiaj ;)
		TimestampToDate(gettime(), year, month, day, hour, minute, second, 1, 0);
		
		if( t_year == year && t_month == month && t_day == day )
		{
			format(dest, length, "today, %02d:%02d", t_hour, t_minute);
			return;
		}
		
		// Jesli to nie bylo dzisiaj to sprawdzamy czy wczoraj
		TimestampToDate(gettime()-86400, year, month, day, hour, minute, second, 1, 0);
		
		if( t_year == year && t_month == month && t_day == day )
		{
			format(dest, length, "yesterday, %02d:%02d", t_hour, t_minute);
			return;
		}
		
		// Ani dzisiaj, ani wczoraj - wyswietlamy date
		format(dest, length, "%02d.%02d.%02d, %02d:%02d", t_day, t_month, t_year, t_hour, t_minute);
		return;
	}
}

stock IsValidSkin(skinid)
{
    if( skinid == 74 || skinid > 299 || skinid < 0 ) return 0;
        
    return 1;
}

stock strreplace(string[], find, replace)
{
    for(new i=0; string[i]; i++)
    {
        if(string[i] == find)
        {
            string[i] = replace;
        }
    }
}

stock str_replace (newstr [], oldstr [], srcstr [], deststr [], bool: ignorecase = false, size = sizeof (deststr))
{
    new
        newlen = strlen (newstr),
        oldlen = strlen (oldstr),
        srclen = strlen (srcstr),
        idx,
        rep;

    for (new i = 0; i < srclen; ++i)
    {
        if ((i + oldlen) <= srclen)
        {
            if (!strcmp (srcstr [i], oldstr, ignorecase, oldlen))
            {
                deststr [idx] = '\0';
                strcat (deststr, newstr, size);
                ++rep;
                idx += newlen;
                i += oldlen - 1;
            }
            else
            {
                if (idx < (size - 1))
                    deststr [idx++] = srcstr [i];
                else
                    return rep;
            }
        }
        else
        {
            if (idx < (size - 1))
                deststr [idx++] = srcstr [i];
            else
                return rep;
        }
    }
    deststr [idx] = '\0';
    return rep;
}


stock BreakLines(string[], delimiter[], limit)
{
	new inserts, tempLimit = limit, pos[50], string2[150], lastEmptyPos;
	format(string2, 150, string);
	
	for(new i; i < strlen(string); i++)
	{
		if( string[i] == ' ' ) lastEmptyPos = i;
		if( string[i] == '~' && string[i+1] == 'n' && string[i+2] == '~' ) tempLimit = i + limit;
		if( i >= tempLimit )
		{
			inserts += 1;
			tempLimit = i + limit;
			
			pos[inserts-1] = lastEmptyPos + ((inserts-1) * strlen(delimiter));
			if( inserts > 1 ) pos[inserts-1] -= (inserts-1);
		}
	}
	
	for(new d; d < 50; d++)
	{
		if( pos[d] == 0 ) break;
		strdel(string2, pos[d], pos[d]+1);
		strins(string2, delimiter, pos[d]);
	}
	
	return _:string2;
}

stock sortIntegers(array[], left, right)
{
    new
        tempLeft = left,
        tempRight = right-1,
        pivot = array[(left + right) / 2],
        tempVar
    ;
    while(tempLeft <= tempRight)
    {
        while(array[tempLeft] < pivot) tempLeft++;
        while(array[tempRight] > pivot) tempRight--;
        
        if(tempLeft <= tempRight)
        {
            tempVar = array[tempLeft], array[tempLeft] = array[tempRight], array[tempRight] = tempVar;
            tempLeft++, tempRight--;
        }
    }
    if(left < tempRight) sortIntegers(array, left, tempRight);
    if(tempLeft < right) sortIntegers(array, tempLeft, right);
}

stock BeautifyString(string[], capitalize = true, add_dot = true, add_shout_mark = false)
{
	new beauty[400];
	format(beauty, sizeof(beauty), string);
	
	if( capitalize ) beauty[0] = toupper(beauty[0]);	
	if( add_dot )
	{
		if( beauty[strlen(string)-1] != '.' && beauty[strlen(string)-1] != '!' && beauty[strlen(string)-1] != '?' && beauty[strlen(string)-1] != ':' && beauty[strlen(string)-1] != '-' )
		{
			strins(beauty, ".", strlen(string));
		}
	}
	if( add_shout_mark )
	{
		if( beauty[strlen(string)-1] != '.' )
		{
			strins(beauty, "!!", strlen(string));
		}
		else
		{
			strdel(beauty, strlen(beauty)-1, strlen(beauty));
			strins(beauty, "!!", strlen(string)-1);
		}
	}
	
	return _:beauty;
}

stock strsearch(const string[], sub, len, maxlen)
{
	if( len > maxlen ) len = maxlen;
	for( new i = len; i >= 0; i-- )
	{
		if( string[i] == sub )
		{
			return i;
		}
	}
	
	return -1;
}

stock Float:Min(Float:x1, Float:x2)
{
	if( floatcmp(x1, x2) == 1 ) return x2;
	else return x1;
}

stock Float:Max(Float:x1, Float:x2)
{
	if( floatcmp(x1, x2) == 1 ) return x1;
	else return x2;
}

stock IsNumeric(const string[])
{
    for (new i = 0, j = strlen(string); i < j; i++)
    {
        if (string[i] > '9' || string[i] < '0') return 0;
    }
    return 1;
}

stock ActorProx(actorid, actrname[], text[], type)
{
	// Some config
	new Float:ProxConfigDistance[][5] =
		{
			{ 10.0, 8.0, 6.0, 4.0, 2.0 }, // PROX_NORMAL
			{ 18.0, 14.0, 10.0, 6.0, 2.0 }, // PROX_SHOUT
			{ 6.0, 4.0, 3.0, 2.0, 1.0 }, // PROX_QUIET
			{ 12.0, 12.0, 12.0, 12.0, 12.0}, // PROX_ME
			{ 12.0, 12.0, 12.0, 12.0, 12.0},  // PROX_DO
			{ 10.0, 8.0, 6.0, 4.0, 2.0 }, // PROX_LOCAL_L
			{ 10.0, 10.0, 10.0, 10.0, 10.0 },// PROX_OOC
			{ 9.0, 7.0, 5.5, 4.0, 2.0 }, // PROX_RADIO
			{ 7.0, 5.0, 4.5, 3.0, 1.5 }, // PROX_PHONE
			{ 18.0, 14.0, 10.0, 6.0, 2.0 } // PROX_MEGAPHONE
		};
		
	new ProxConfigColor[][5] =
		{
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_LOCAL
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_SHOUT
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xD8D8D8FF }, // PROX_QUIET
			{ 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF }, // PROX_ME
			{ 0x9B91ECFF, 0x9B91ECFF, 0x9B91ECFF, 0x9B91ECFF, 0x9B91ECFF }, // PROX_DO
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_LOCAL_L
			{ COLOR_GREY, COLOR_GREY, COLOR_GREY, COLOR_GREY, COLOR_GREY }, // PROX OOC	
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_RADIO
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_PHONE
			{ 0xFFE229FF, 0xFFE229FF, 0xFFE229FF, 0xFFE229FF, 0xFFE229FF } // PROX_MEGAPHONE
		};
	
	new ProxConfigText[][32] =
		{
			"says", // PROX_LOCAL
			"shouts", // PROX_SHOUT
			"whispers", // PROX_QUIET
			"",
			"",
			"says", // PROX_LOCAL_L
			"",
			"(radio)", // PROX_RADIO
			"(phone)",
			"(megaphone)"
		};
	new
		Float:x,
		Float:y,
		Float:z,
		vw = GetActorVirtualWorld(actorid);
		
	GetActorPos(actorid, x, y, z);
	
	new beautyText[300];
		
	switch( type )
	{
		case PROX_SHOUT:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", actrname, ProxConfigText[type], BeautifyString(text, true, false, true));
		}
		
		case PROX_QUIET:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", actrname, ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_ME:
		{
			format(beautyText, sizeof(beautyText), "** %s %s", actrname, BeautifyString(text, false, true));
		}
		
		case PROX_DO:
		{
			format(beautyText, sizeof(beautyText), "** %s (( %s ))", BeautifyString(text, true, true), actrname);
		}
		
		case PROX_LOCAL_L:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", actrname, ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_OOC:
		{
			format(beautyText, sizeof(beautyText), "(( [%d] %s: %s ))", actorid, actrname, text);
			//SetPlayerChatBubble(playerid, beautyText, COLOR_GREY, 10.0, strlen(beautyText) * 100);
		}
		
		case PROX_RADIO:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", actrname, ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_LOCAL:
		{
			/*if( GetPlayerState(playerid) == PLAYER_STATE_ONFOOT )
			{
				if( !pInfo[playerid][player_looped_anim] )
				{
					new time = strlen(text) * 50;
					ApplyAnimation(playerid, "PED", "IDLE_CHAT", 4.1, 1, 0, 0, 1, 50, 1);
					defer StopPlayerAnimation[time](playerid);
				}
			}*/
			
			format(beautyText, sizeof(beautyText), "%s %s: %s", actrname, ProxConfigText[type], BeautifyString(text, true, true));
		}
		

		case PROX_MEGAPHONE:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", actrname, ProxConfigText[type], BeautifyString(text, true, true));
		}
	}
	
	foreach (new p : Player)
	{
		// Some checks
		if( GetPlayerVirtualWorld(p) != vw ) continue;
		
		new Float:distance = GetPlayerDistanceFromPoint(p, x, y, z);
		if( distance > ProxConfigDistance[type][0] ) continue;

		// Color formatting
		new color;
		if( distance <= ProxConfigDistance[type][0] ) color = ProxConfigColor[type][0];
		if( distance <= ProxConfigDistance[type][1] ) color = ProxConfigColor[type][1];
		if( distance <= ProxConfigDistance[type][2] ) color = ProxConfigColor[type][2];
		if( distance <= ProxConfigDistance[type][3] ) color = ProxConfigColor[type][3];
		if( distance <= ProxConfigDistance[type][4] ) color = ProxConfigColor[type][4];
		
		SendClientMessage(p, color, beautyText);
	}
	
	return 0;
}

stock str_zamien(str[], const find[], const repl[], bool: ignorecase=true, size = sizeof str)
{
    new zn = strfind(str, find, ignorecase), len = strlen(find), len2 = strlen(repl);
    while(zn != -1)
    {
        strdel(str, zn, zn + len);
        if(repl[0])
            strins(str, repl, zn, size);
        zn = strfind(str, find, ignorecase, zn + len2);
    }
}

stock ProxMessage(playerid, text[], type)
{
	// Some config
	new Float:ProxConfigDistance[][5] =
		{
			{ 10.0, 8.0, 6.0, 4.0, 2.0 }, // PROX_NORMAL
			{ 18.0, 14.0, 10.0, 6.0, 2.0 }, // PROX_SHOUT
			{ 2.50, 2.20, 2.0, 1.50, 1.0 }, // PROX_QUIET
			{ 12.0, 12.0, 12.0, 12.0, 12.0}, // PROX_ME
			{ 12.0, 12.0, 12.0, 12.0, 12.0},  // PROX_DO
			{ 10.0, 8.0, 6.0, 4.0, 2.0 }, // PROX_LOCAL_L
			{ 10.0, 10.0, 10.0, 10.0, 10.0 },// PROX_OOC
			{ 9.0, 7.0, 5.5, 4.0, 2.0 }, // PROX_RADIO
			{ 7.0, 5.0, 4.5, 3.0, 1.5 }, // PROX_PHONE
			{ 18.0, 14.0, 10.0, 6.0, 2.0 }, // PROX_MEGAPHONE
			{ 12.0, 12.0, 12.0, 12.0, 12.0} // PROX_SERWERME
		};
		
	new ProxConfigColor[][5] =
		{
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_LOCAL
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_SHOUT
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xD8D8D8FF }, // PROX_QUIET
			{ 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF }, // PROX_ME
			{ 0x9B91ECFF, 0x9B91ECFF, 0x9B91ECFF, 0x9B91ECFF, 0x9B91ECFF }, // PROX_DO
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_LOCAL_L
			{ COLOR_GREY, COLOR_GREY, COLOR_GREY, COLOR_GREY, COLOR_GREY }, // PROX OOC	
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_RADIO
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_PHONE
			{ 0xFFE229FF, 0xFFE229FF, 0xFFE229FF, 0xFFE229FF, 0xFFE229FF }, // PROX_MEGAPHONE
			{ 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF }  // PROX_SERWERME
		};
	
	new ProxConfigText[][32] =
		{
			"says", // PROX_LOCAL
			"shouts", // PROX_SHOUT
			"whispers", // PROX_QUIET
			"",
			"",
			"says", // PROX_LOCAL_L
			"",
			"(radio)", // PROX_RADIO
			"(phone)",
			"(megaphone)",
			""
		};
	new
		Float:x,
		Float:y,
		Float:z,
		vw = GetPlayerVirtualWorld(playerid),
		int = GetPlayerInterior(playerid);
		
	GetPlayerPos(playerid, x, y, z);
	
	new beautyText[300];
		
	switch( type )
	{
		case PROX_SHOUT:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, false, true));
		}
		
		case PROX_QUIET:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_ME:
		{
			format(beautyText, sizeof(beautyText), "** %s %s", pInfo[playerid][player_name], BeautifyString(text, false, true));
		}

		case PROX_SERWERME:
		{
			format(beautyText, sizeof(beautyText), "* %s %s", pInfo[playerid][player_name], BeautifyString(text, false, true));
		}
		
		case PROX_DO:
		{
			format(beautyText, sizeof(beautyText), "** %s (( %s ))", BeautifyString(text, true, true), pInfo[playerid][player_name]);
		}
		
		case PROX_LOCAL_L:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_OOC:
		{
			format(beautyText, sizeof(beautyText), "(( [%d] %s: %s ))", playerid, pInfo[playerid][player_name], text);
			//SetPlayerChatBubble(playerid, beautyText, COLOR_GREY, 10.0, strlen(beautyText) * 100);
		}
		
		case PROX_RADIO:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_LOCAL:
		{
			/*if( GetPlayerState(playerid) == PLAYER_STATE_ONFOOT )
			{
				if( !pInfo[playerid][player_looped_anim] )
				{
					new time = strlen(text) * 50;
					ApplyAnimation(playerid, "PED", "IDLE_CHAT", 4.1, 1, 0, 0, 1, 50, 1);
					defer StopPlayerAnimation[time](playerid);
				}
			}*/
			
			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_PHONE:
		{
			new targetid = -1;
			if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
			else targetid = pInfo[playerid][player_phone_caller];
			if(targetid == -1) return 1;
			
			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
			SendFormattedClientMessage(targetid, COLOR_YELLOW, "[Phone]: %s", BeautifyString(text, true, true));
		}

		case PROX_MEGAPHONE:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
		}
	}
	
	foreach (new p : Player)
	{
		// Some checks
		if( !pInfo[p][player_logged] ) continue;
		if( type == PROX_RADIO && p == playerid ) continue;
		if( GetPlayerVirtualWorld(p) != vw || GetPlayerInterior(p) != int ) continue;
		if( type == PROX_LOCAL && pInfo[p][player_bw] > 0) continue;
		if( type == PROX_SHOUT && pInfo[p][player_bw] > 0) continue;
		if( type == PROX_LOCAL_L && pInfo[p][player_bw] > 0) continue;
		if( type == PROX_QUIET && pInfo[p][player_bw] > 0) continue;
		
		new Float:distance = GetPlayerDistanceFromPoint(p, x, y, z);
		if( distance > ProxConfigDistance[type][0] ) continue;

		// Color formatting
		new color;
		if( distance <= ProxConfigDistance[type][0] ) color = ProxConfigColor[type][0];
		if( distance <= ProxConfigDistance[type][1] ) color = ProxConfigColor[type][1];
		if( distance <= ProxConfigDistance[type][2] ) color = ProxConfigColor[type][2];
		if( distance <= ProxConfigDistance[type][3] ) color = ProxConfigColor[type][3];
		if( distance <= ProxConfigDistance[type][4] ) color = ProxConfigColor[type][4];
		
		SendClientMessage(p, color, beautyText);
	}
	
	return 0;
}

stock rand(v_min, v_max)
{
	new rndd = -1;
	while (rndd < v_min) rndd = random(v_max+1);
	
	return rndd;
}

stock SendGuiInformation(playerid, caption[], text[])
{
	return ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, caption, text, "OK", "");
}

stock IntToBase(number, const base) {
    new str[32];
    if(1 < base < 37) {
        new
            m = 1,
            depth = 0;
        while (m <= number) {
            m *= base;
            depth++;
        }
        for ( ; (--depth) != -1; ) {
            str[depth] = (number % base);
            number = ((number - str[depth]) / base);
            if(str[depth] > 9) str[depth] += 'A'; else str[depth] += '0';
        }
    }
    return str;
}
#define IntToDual(%0) IntToBase(%0, 2)
#define IntToOctal(%0) IntToBase(%0, 8)
#define IntToHex(%0) IntToBase(%0, 16)


new WeaponVisualModel[47] = {
	-1, -1, 333, 334, 335, 336, 337, 338, 339, 341, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 346, 347, 348, 349, 350, 351, 352, 353,
	355, 356, 372, 357, 358, -1, -1, -1, -1, -1, -1, 365, -1, -1, -1, -1, -1
};

new WeaponVisualBone[47] = {
	-1, -1, 1, 7, 7, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, 1, -1, -1, -1, -1, -1
};

new Float:FWeaponVisualPos[47][9] = {
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.017000, -0.114999, -0.140000, 88.999992, 86.500015, -1.800061, 1.000000, 1.000000, 1.000000 },
	{ 0.059000, 0.070000, -0.049000, -22.500000, 90.099998, -43.800014, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.422000, -0.168000, 0.193000, -7.400008, -34.300003, 173.700027, 1.000000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ 0.159000, 0.146999, 0.164000, -13.199998, 150.000045, 25.100008, 1.000000, 1.059000, 1.000000 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ 0.159000, 0.146999, 0.164000, -13.199998, 150.000045, 25.100008, 1.000000, 1.059000, 1.000000 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ 0.159000, 0.146999, 0.164000, -13.199998, 150.000045, 25.100008, 1.000000, 1.059000, 1.000000 },
	{ 0.159000, 0.146999, 0.164000, -13.199998, 150.000045, 25.100008, 1.000000, 1.059000, 1.000000 },
	{ 0.159000, 0.146999, 0.164000, -13.199998, 150.000045, 25.100008, 1.000000, 1.059000, 1.000000 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }
};

new Float:WeaponVisualPos[47][9] = {
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.017000, -0.114999, -0.140000, 88.999992, 86.500015, -1.800061, 1.000000, 1.000000, 1.000000 },
	{ 0.059000, 0.070000, -0.049000, -22.500000, 90.099998, -43.800014, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.422000, -0.168000, 0.193000, -7.400008, -34.300003, 173.700027, 1.000000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }
};