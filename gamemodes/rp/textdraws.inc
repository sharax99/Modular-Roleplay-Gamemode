stock CheckTextDrawString(const string[])
{
	for(new znak = 0; znak < strlen(string); znak++)
	{
		if(string[znak] == '~')
		{
			if((string[znak+1] == 'r' || string[znak+1] == 'g' || string[znak+1] == 'b' || string[znak+1] == 'w' || string[znak+1] == 'l' || string[znak+1] == 'p' || string[znak+1] == 'n') && string[znak+2] == '~')
			{
				if(string[znak+2] == '~') znak += 2;
			}
		else return 0;
		}
	}
	return 1;
}

stock CreatePlayerTextdraws(playerid)
{	
	CreateGroupPlayerTextdraws(playerid);
	GroupDutyTag = CreatePlayerTextDraw(playerid, 8.000000, 428.000000, "GOV");
	PlayerTextDrawBackgroundColor(playerid, GroupDutyTag, 255);
	PlayerTextDrawFont(playerid, GroupDutyTag, 1);
	PlayerTextDrawLetterSize(playerid, GroupDutyTag, 0.230000, 0.899999);
	PlayerTextDrawColor(playerid, GroupDutyTag, -1);
	PlayerTextDrawSetOutline(playerid, GroupDutyTag, 1);
	PlayerTextDrawSetProportional(playerid, GroupDutyTag, 1);

	ZoneName = CreatePlayerTextDraw(playerid, 508.000000, 428.000000, "Idlewood");
	PlayerTextDrawBackgroundColor(playerid, ZoneName, 255);
	PlayerTextDrawFont(playerid, ZoneName, 1);
	PlayerTextDrawLetterSize(playerid, ZoneName, 0.250000, 0.899999);
	PlayerTextDrawColor(playerid, ZoneName, -1);
	PlayerTextDrawSetOutline(playerid, ZoneName, 1);
	PlayerTextDrawSetProportional(playerid, ZoneName, 1);
	
	OfferTD[0] = CreatePlayerTextDraw(playerid, 170.000000, 140.000000, "~n~ ~y~UID: ~w~456~n~~n~ ~y~Model: ~w~ 411");
	PlayerTextDrawBackgroundColor(playerid, OfferTD[0], 255);
	PlayerTextDrawFont(playerid, OfferTD[0], 1);
	PlayerTextDrawLetterSize(playerid, OfferTD[0], 0.230000, 0.799998);
	PlayerTextDrawColor(playerid, OfferTD[0], -1);
	PlayerTextDrawSetOutline(playerid, OfferTD[0], 1);
	PlayerTextDrawSetProportional(playerid, OfferTD[0], 1);
	PlayerTextDrawUseBox(playerid, OfferTD[0], 1);
	PlayerTextDrawBoxColor(playerid, OfferTD[0], 51);
	PlayerTextDrawTextSize(playerid, OfferTD[0], 410.000000, 0.000000);

	OfferTD[1] = CreatePlayerTextDraw(playerid, 369.000000, 148.000000, "~n~~n~~n~~n~~n~~n~~n~~n~");
	PlayerTextDrawAlignment(playerid, OfferTD[1], 2);
	PlayerTextDrawBackgroundColor(playerid, OfferTD[1], 255);
	PlayerTextDrawFont(playerid, OfferTD[1], 1);
	PlayerTextDrawLetterSize(playerid, OfferTD[1], 0.230000, 0.799998);
	PlayerTextDrawColor(playerid, OfferTD[1], -1);
	PlayerTextDrawSetOutline(playerid, OfferTD[1], 1);
	PlayerTextDrawSetProportional(playerid, OfferTD[1], 1);
	PlayerTextDrawUseBox(playerid, OfferTD[1], 1);
	PlayerTextDrawBoxColor(playerid, OfferTD[1], 51);
	PlayerTextDrawTextSize(playerid, OfferTD[1], 410.000000, 60.000000);

	OfferTD[2] = CreatePlayerTextDraw(playerid, 190.000000, 250.000000, "~y~~h~ Oferta od Vincent Dabrasco ~>~ vCard~n~~n~~b~~h~ Nazwa:~w~ bleh~n~~g~ Koszt:~w~ $0~n~~n~~n~~n~");
	PlayerTextDrawBackgroundColor(playerid, OfferTD[2], 255);
	PlayerTextDrawFont(playerid, OfferTD[2], 1);
	PlayerTextDrawLetterSize(playerid, OfferTD[2], 0.250000, 0.799998);
	PlayerTextDrawColor(playerid, OfferTD[2], -1);
	PlayerTextDrawSetOutline(playerid, OfferTD[2], 1);
	PlayerTextDrawSetProportional(playerid, OfferTD[2], 1);
	PlayerTextDrawUseBox(playerid, OfferTD[2], 1);
	PlayerTextDrawBoxColor(playerid, OfferTD[2], 51);
	PlayerTextDrawTextSize(playerid, OfferTD[2], 440.000000, 0.000000);

	OfferTD[3] = CreatePlayerTextDraw(playerid, 219.000000, 291.000000, "Szczegoly");
	PlayerTextDrawAlignment(playerid, OfferTD[3], 2);
	PlayerTextDrawBackgroundColor(playerid, OfferTD[3], 255);
	PlayerTextDrawFont(playerid, OfferTD[3], 1);
	PlayerTextDrawLetterSize(playerid, OfferTD[3], 0.230000, 0.799998);
	PlayerTextDrawColor(playerid, OfferTD[3], 0xFFDF00FF);
	PlayerTextDrawSetOutline(playerid, OfferTD[3], 1);
	PlayerTextDrawSetProportional(playerid, OfferTD[3], 1);
	PlayerTextDrawUseBox(playerid, OfferTD[3], 1);
	PlayerTextDrawBoxColor(playerid, OfferTD[3], 51);
	PlayerTextDrawTextSize(playerid, OfferTD[3], 10.000000, 36.000000);
	PlayerTextDrawSetSelectable(playerid, OfferTD[3], 1);

	OfferTD[4] = CreatePlayerTextDraw(playerid, 345.000000, 291.000000, "Accept");
	PlayerTextDrawAlignment(playerid, OfferTD[4], 2);
	PlayerTextDrawBackgroundColor(playerid, OfferTD[4], 255);
	PlayerTextDrawFont(playerid, OfferTD[4], 1);
	PlayerTextDrawLetterSize(playerid, OfferTD[4], 0.230000, 0.799998);
	PlayerTextDrawColor(playerid, OfferTD[4], 0x009300FF);
	PlayerTextDrawSetOutline(playerid, OfferTD[4], 1);
	PlayerTextDrawSetProportional(playerid, OfferTD[4], 1);
	PlayerTextDrawUseBox(playerid, OfferTD[4], 1);
	PlayerTextDrawBoxColor(playerid, OfferTD[4], 51);
	PlayerTextDrawTextSize(playerid, OfferTD[4], 10.000000, 36.000000);
	PlayerTextDrawSetSelectable(playerid, OfferTD[4], 1);

	OfferTD[5] = CreatePlayerTextDraw(playerid, 403.000000, 291.000000, "Refuse");
	PlayerTextDrawAlignment(playerid, OfferTD[5], 2);
	PlayerTextDrawBackgroundColor(playerid, OfferTD[5], 255);
	PlayerTextDrawFont(playerid, OfferTD[5], 1);
	PlayerTextDrawLetterSize(playerid, OfferTD[5], 0.230000, 0.799998);
	PlayerTextDrawColor(playerid, OfferTD[5], 0xC60000FF);
	PlayerTextDrawSetOutline(playerid, OfferTD[5], 1);
	PlayerTextDrawSetProportional(playerid, OfferTD[5], 1);
	PlayerTextDrawUseBox(playerid, OfferTD[5], 1);
	PlayerTextDrawBoxColor(playerid, OfferTD[5], 51);
	PlayerTextDrawTextSize(playerid, OfferTD[5], 10.000000, 36.000000);
	PlayerTextDrawSetSelectable(playerid, OfferTD[5], 1);
}

stock CreateTextdraws()
{	
	CreateGroupStaticTextdraws();

	Textdraw2 = TextDrawCreate(501.000000, 9.000000, "~b~~h~~h~s~w~ociety role~b~~h~~h~play ~w~1.6");
	TextDrawBackgroundColor(Textdraw2, 255);
	TextDrawFont(Textdraw2, 3);
	TextDrawLetterSize(Textdraw2, 0.330000, 1.100000);
	TextDrawColor(Textdraw2, -1);
	TextDrawSetOutline(Textdraw2, 1);
	TextDrawSetProportional(Textdraw2, 1);
	TextDrawSetSelectable(Textdraw2, 0);
	
	vehicleInfo = TextDrawCreate(438.000000, 366.000000, "Press ~y~LCTRL~w~ to start engine.~n~~y~LMB ~w~to turn on lights.");
	TextDrawBackgroundColor(vehicleInfo, 255);
	TextDrawFont(vehicleInfo, 1);
	TextDrawLetterSize(vehicleInfo, 0.220000, 0.899999);
	TextDrawColor(vehicleInfo, -1);
	TextDrawSetOutline(vehicleInfo, 0);
	TextDrawSetProportional(vehicleInfo, 1);
	TextDrawSetShadow(vehicleInfo, 1);
	TextDrawUseBox(vehicleInfo, 1);
	TextDrawBoxColor(vehicleInfo, 128);
	TextDrawTextSize(vehicleInfo, 611.000000, 0.000000);

	TextDrawSanNews = TextDrawCreate(1.000000, 438.000000, "~r~~h~~h~Weazel ~>~ ~w~There is no broadcast at this moment.");
	TextDrawBackgroundColor(TextDrawSanNews, 255);
	TextDrawFont(TextDrawSanNews, 1);
	TextDrawLetterSize(TextDrawSanNews, 0.250000, 0.899999);
	TextDrawColor(TextDrawSanNews, -1);
	TextDrawSetOutline(TextDrawSanNews, 1);
	TextDrawSetProportional(TextDrawSanNews, 1);
	TextDrawUseBox(TextDrawSanNews, 1);
	TextDrawBoxColor(TextDrawSanNews, 68);
	TextDrawTextSize(TextDrawSanNews, 640.000000, 600.000000);

	TextPenaltySprite = TextDrawCreate(11.800003, 279.993652, "LD_BUM:bum2");
	TextDrawLetterSize(TextPenaltySprite, 0.000000, 0.000000);
	TextDrawTextSize(TextPenaltySprite, 26.000000, -31.000000);
	TextDrawAlignment(TextPenaltySprite, 1);
	TextDrawColor(TextPenaltySprite, -1);
	TextDrawSetShadow(TextPenaltySprite, 0);
	TextDrawSetOutline(TextPenaltySprite, 0);
	TextDrawBackgroundColor(TextPenaltySprite, 255);
	TextDrawFont(TextPenaltySprite, 4);
	TextDrawSetProportional(TextPenaltySprite, 0);
	TextDrawSetShadow(TextPenaltySprite, 0);
	
	TextPenaltyType = TextDrawCreate(91.965774, 241.884826, "Admin_Jail");
	TextDrawLetterSize(TextPenaltyType, 0.156654, 0.820514);
	TextDrawTextSize(TextPenaltyType, -0.019997, 158.000000);
	TextDrawAlignment(TextPenaltyType, 2);
	TextDrawColor(TextPenaltyType, -1061109505);
	TextDrawUseBox(TextPenaltyType, 1);
	TextDrawBoxColor(TextPenaltyType, 250);
	TextDrawSetShadow(TextPenaltyType, 1);
	TextDrawSetOutline(TextPenaltyType, 0);
	TextDrawBackgroundColor(TextPenaltyType, 255);
	TextDrawFont(TextPenaltyType, 1);
	TextDrawSetProportional(TextPenaltyType, 1);
	TextDrawSetShadow(TextPenaltyType, 1);

	TextPenalty = TextDrawCreate(12.999958, 252.300277, "____________~w~Nadawca: ~b~Admin~n~____________~w~Gracz: ~b~Admin~n~____________~w~Powod: ~r~Deathmatch ~y~(7 lat)");
	TextDrawLetterSize(TextPenalty, 0.150323, 0.969847);
	TextDrawTextSize(TextPenalty, 171.000000, -0.019997);
	TextDrawAlignment(TextPenalty, 1);
	TextDrawColor(TextPenalty, -1061109505);
	TextDrawUseBox(TextPenalty, 1);
	TextDrawBoxColor(TextPenalty, 128);
	TextDrawSetShadow(TextPenalty, 1);
	TextDrawSetOutline(TextPenalty, 0);
	TextDrawBackgroundColor(TextPenalty, 255);
	TextDrawFont(TextPenalty, 1);
	TextDrawSetProportional(TextPenalty, 1);
	TextDrawSetShadow(TextPenalty, 1);

	for(new pp;pp<MAX_PLAYERS;pp++)
	{
		DoorInfo[pp] = TextDrawCreate(559.500854, 324.799987, "Nazwa_~y~(UID:0)~n~~w~Osob_wewnatrz:_~y~5b~n~~w~Aby_wejsc do srodka,_nacisnij_jednoczesnie_~n~~g~[LALT+SPACJA]");
		TextDrawLetterSize(DoorInfo[pp], 0.146657, 0.907625);
		TextDrawTextSize(DoorInfo[pp], -0.019997, 141.000000);
		TextDrawAlignment(DoorInfo[pp], 2);
		TextDrawColor(DoorInfo[pp], -1061109505);
		TextDrawUseBox(DoorInfo[pp], 1);
		TextDrawBoxColor(DoorInfo[pp], 128);
		TextDrawSetShadow(DoorInfo[pp], 1);
		TextDrawSetOutline(DoorInfo[pp], 0);
		TextDrawBackgroundColor(DoorInfo[pp], 255);
		TextDrawFont(DoorInfo[pp], 1);
		TextDrawSetProportional(DoorInfo[pp], 1);
		TextDrawSetShadow(DoorInfo[pp], 1);

		DoorInfo2[pp] = TextDrawCreate(446.499786, 318.799987, "");
		TextDrawLetterSize(DoorInfo2[pp], 0.000000, 0.000000);
		TextDrawTextSize(DoorInfo2[pp], 61.000000, 44.000000);
		TextDrawAlignment(DoorInfo2[pp], 1);
		TextDrawColor(DoorInfo2[pp], -45);
		TextDrawSetShadow(DoorInfo2[pp], 0);
		TextDrawSetOutline(DoorInfo2[pp], 0);
		TextDrawBackgroundColor(DoorInfo2[pp], 0);
		TextDrawFont(DoorInfo2[pp], 5);
		TextDrawSetProportional(DoorInfo2[pp], 0);
		TextDrawSetShadow(DoorInfo2[pp], 0);
		TextDrawSetPreviewModel(DoorInfo2[pp], 19860);
		TextDrawSetPreviewRot(DoorInfo2[pp], 0.000000, 0.000000, 218.000000, 1.000000);

		Tutorial[pp] = TextDrawCreate(16.000000, 150.000000, "~n~");
		TextDrawAlignment(Tutorial[pp], 1);
		TextDrawBackgroundColor(Tutorial[pp], 150);
		TextDrawFont(Tutorial[pp], 1);
		TextDrawLetterSize(Tutorial[pp], 0.210000, 1.000000);
		TextDrawColor(Tutorial[pp], -1);
		TextDrawSetOutline(Tutorial[pp], 1);
		TextDrawSetProportional(Tutorial[pp], 1);
		TextDrawSetShadow(Tutorial[pp], 1);
		TextDrawUseBox(Tutorial[pp], 1);
		TextDrawBoxColor(Tutorial[pp], 100);
		TextDrawTextSize(Tutorial[pp], 171.000000, 0);

		DashBoardPane[pp] = TextDrawCreate(202.000000, 340.000000, "~n~");
		TextDrawAlignment(DashBoardPane[pp], 2);
		TextDrawBackgroundColor(DashBoardPane[pp], 255);
		TextDrawFont(DashBoardPane[pp], 1);
		TextDrawLetterSize(DashBoardPane[pp], 0.500000, 14.000000);
		TextDrawColor(DashBoardPane[pp], -1);
		TextDrawSetOutline(DashBoardPane[pp], 0);
		TextDrawSetProportional(DashBoardPane[pp], 1);
		TextDrawSetShadow(DashBoardPane[pp], 1);
		TextDrawUseBox(DashBoardPane[pp], 1);
		TextDrawBoxColor(DashBoardPane[pp], 0x9E9EAD20);
		TextDrawTextSize(DashBoardPane[pp], 730.000000, -950.000000);

		DashBoardPane2[pp] = TextDrawCreate(152.000000, 349.000000, "Sluzba: ~b~grupa                    ~y~Ganton Gym (UID: 249)~n~~n~~w~Rozpoczeto: ~g~14:24               ~w~Tytul: ~r~Lider");
		TextDrawBackgroundColor(DashBoardPane2[pp], 255);
		TextDrawFont(DashBoardPane2[pp], 1);
		TextDrawLetterSize(DashBoardPane2[pp], 0.290000, 1.000000);
		TextDrawColor(DashBoardPane2[pp], -1);
		TextDrawSetOutline(DashBoardPane2[pp], 1);
		TextDrawSetProportional(DashBoardPane2[pp], 1);

		Dashboard[pp] = TextDrawCreate(320.000000, 300.000000, "");
		TextDrawAlignment(Dashboard[pp], 2);
		TextDrawBackgroundColor(Dashboard[pp], 255);
		TextDrawFont(Dashboard[pp], 1);
		TextDrawLetterSize(Dashboard[pp], 0.290000, 0.899999);
		TextDrawColor(Dashboard[pp], -1);
		TextDrawSetOutline(Dashboard[pp], 1);
		TextDrawSetProportional(Dashboard[pp], 1);

		AreaFlags[pp] = TextDrawCreate(500.000000, 109.000000, "~b~~h~~>~~w~zbyt narkotykow ~n~~>~kolejna flaga");
		TextDrawBackgroundColor(AreaFlags[pp], 255);
		TextDrawFont(AreaFlags[pp], 3);
		TextDrawLetterSize(AreaFlags[pp], 0.310000, 1.000000);
		TextDrawColor(AreaFlags[pp], -1);
		TextDrawSetOutline(AreaFlags[pp], 1);
		TextDrawSetProportional(AreaFlags[pp], 1);
		TextDrawSetSelectable(AreaFlags[pp], 0);

		AreaText[pp] = TextDrawCreate(541.000000, 98.000000, "~b~~h~ Area:~w~ 121");
		TextDrawBackgroundColor(AreaText[pp], 255);
		TextDrawFont(AreaText[pp], 3);
		TextDrawLetterSize(AreaText[pp], 0.310000, 1.000000);
		TextDrawColor(AreaText[pp], -1);
		TextDrawSetOutline(AreaText[pp], 1);
		TextDrawSetProportional(AreaText[pp], 1);
		TextDrawSetSelectable(AreaText[pp], 0);
	}
}

stock CreateGroupPlayerTextdraws(playerid)
{
	new Float:baseY = 183.0;
	for(new i=0;i<5;i++)
	{
		GroupsListRow[i] = CreatePlayerTextDraw(playerid, 104.000000, baseY+(i*14), "1       Pig Pen (669)");
		PlayerTextDrawBackgroundColor(playerid, GroupsListRow[i], 255);
		PlayerTextDrawFont(playerid, GroupsListRow[i], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListRow[i], 0.290000, 1.000000);
		PlayerTextDrawColor(playerid, GroupsListRow[i], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListRow[i], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListRow[i], 1);
		PlayerTextDrawUseBox(playerid, GroupsListRow[i], 1);
		PlayerTextDrawBoxColor(playerid, GroupsListRow[i], 112);
		PlayerTextDrawTextSize(playerid, GroupsListRow[i], 537.000000, 0.000000);
	}
	
	baseY = 184.0;
	for(new i=0;i<5;i++)
	{
		GroupsListStaticButtons[i][0] = CreatePlayerTextDraw(playerid, 344.000000, baseY+(playerid, i*14), "Info");
		PlayerTextDrawAlignment(playerid, GroupsListStaticButtons[i][0], 2);
		PlayerTextDrawBackgroundColor(playerid, GroupsListStaticButtons[i][0], 255);
		PlayerTextDrawFont(playerid, GroupsListStaticButtons[i][0], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListStaticButtons[i][0], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, GroupsListStaticButtons[i][0], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListStaticButtons[i][0], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListStaticButtons[i][0], 1);
		PlayerTextDrawUseBox(playerid, GroupsListStaticButtons[i][0], 1);
		PlayerTextDrawBoxColor(playerid, GroupsListStaticButtons[i][0], 112);
		PlayerTextDrawTextSize(playerid, GroupsListStaticButtons[i][0], 12.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, GroupsListStaticButtons[i][0], 1);

		GroupsListStaticButtons[i][1] = CreatePlayerTextDraw(playerid, 387.000000, baseY+(playerid, i*14), "Vehicles");
		PlayerTextDrawAlignment(playerid, GroupsListStaticButtons[i][1], 2);
		PlayerTextDrawBackgroundColor(playerid, GroupsListStaticButtons[i][1], 255);
		PlayerTextDrawFont(playerid, GroupsListStaticButtons[i][1], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListStaticButtons[i][1], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, GroupsListStaticButtons[i][1], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListStaticButtons[i][1], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListStaticButtons[i][1], 1);
		PlayerTextDrawUseBox(playerid, GroupsListStaticButtons[i][1], 1);
		PlayerTextDrawBoxColor(playerid, GroupsListStaticButtons[i][1], 112);
		PlayerTextDrawTextSize(playerid, GroupsListStaticButtons[i][1], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, GroupsListStaticButtons[i][1], 1);

		GroupsListStaticButtons[i][2] = CreatePlayerTextDraw(playerid, 430.000000, baseY+(playerid, i*14), "Duty");
		PlayerTextDrawAlignment(playerid, GroupsListStaticButtons[i][2], 2);
		PlayerTextDrawBackgroundColor(playerid, GroupsListStaticButtons[i][2], 255);
		PlayerTextDrawFont(playerid, GroupsListStaticButtons[i][2], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListStaticButtons[i][2], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, GroupsListStaticButtons[i][2], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListStaticButtons[i][2], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListStaticButtons[i][2], 1);
		PlayerTextDrawUseBox(playerid, GroupsListStaticButtons[i][2], 1);
		PlayerTextDrawBoxColor(playerid, GroupsListStaticButtons[i][2], 112);
		PlayerTextDrawTextSize(playerid, GroupsListStaticButtons[i][2], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, GroupsListStaticButtons[i][2], 1);

		GroupsListStaticButtons[i][3] = CreatePlayerTextDraw(playerid, 473.000000, baseY+(playerid, i*14), "Storage");
		PlayerTextDrawAlignment(playerid, GroupsListStaticButtons[i][3], 2);
		PlayerTextDrawBackgroundColor(playerid, GroupsListStaticButtons[i][3], 255);
		PlayerTextDrawFont(playerid, GroupsListStaticButtons[i][3], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListStaticButtons[i][3], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, GroupsListStaticButtons[i][3], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListStaticButtons[i][3], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListStaticButtons[i][3], 1);
		PlayerTextDrawUseBox(playerid, GroupsListStaticButtons[i][3], 1);
		PlayerTextDrawBoxColor(playerid, GroupsListStaticButtons[i][3], 112);
		PlayerTextDrawTextSize(playerid, GroupsListStaticButtons[i][3], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, GroupsListStaticButtons[i][3], 1);

		GroupsListStaticButtons[i][4] = CreatePlayerTextDraw(playerid, 516.000000, baseY+(playerid, i*14), "Online");
		PlayerTextDrawAlignment(playerid, GroupsListStaticButtons[i][4], 2);
		PlayerTextDrawBackgroundColor(playerid, GroupsListStaticButtons[i][4], 255);
		PlayerTextDrawFont(playerid, GroupsListStaticButtons[i][4], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListStaticButtons[i][4], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, GroupsListStaticButtons[i][4], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListStaticButtons[i][4], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListStaticButtons[i][4], 1);
		PlayerTextDrawUseBox(playerid, GroupsListStaticButtons[i][4], 1);
		PlayerTextDrawBoxColor(playerid, GroupsListStaticButtons[i][4], 112);
		PlayerTextDrawTextSize(playerid, GroupsListStaticButtons[i][4], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, GroupsListStaticButtons[i][4], 1);
	}
}

stock CreateGroupStaticTextdraws()
{
	GroupsListStaticHeader = TextDrawCreate(104.000000, 172.000000, "~b~~h~~h~SLOT           GROUP NAME                                          OPTIONS");
	TextDrawBackgroundColor(GroupsListStaticHeader, 255);
	TextDrawFont(GroupsListStaticHeader, 3);
	TextDrawLetterSize(GroupsListStaticHeader, 0.280000, 0.799999);
	TextDrawColor(GroupsListStaticHeader, -1);
	TextDrawSetOutline(GroupsListStaticHeader, 1);
	TextDrawSetProportional(GroupsListStaticHeader, 1);
}