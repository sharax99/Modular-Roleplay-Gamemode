/*
-               Timestamp To Date converter	            -
-                    Made by Jochemd                    -
-          http://forum.sa-mp.com/member.php?u=580      -
*/


#include <a_samp>

new MonthTimes[12][3] = 
{
	{ 31, 31, 2678400 },
	{ 28, 29, 2505600 },
	{ 31, 31, 2678400 },
	{ 30, 30, 2592000 },
	{ 31, 31, 2678400 },
	{ 30, 30, 2592000 },
	{ 31, 31, 2678400 },
	{ 31, 31, 2678400 },
	{ 30, 30, 2592000 },
	{ 31, 31, 2678400 },
	{ 30, 30, 2592000 },
	{ 31, 31, 2678400 }
};

stock IsLeapYear(year)
{
	if(year % 4 == 0) return 1;
	else return 0;
}

stock TimestampToDate(Timestamp, &year, &month, &day, &iHour, &iMinute, &second, iHourGMT, iMinuteGMT = 0)
{
	new tmp = 2;
	year = 1970;
	month = 1;
	for(;;)
	{
		if(Timestamp >= 31536000)
		{
			year ++;
			Timestamp -= 31536000;
			tmp ++;
			if(tmp == 4)
			{
				if(Timestamp >= 31622400)
				{
					tmp = 0;
					year ++;
					Timestamp -= 31622400;
				}
				else break;
			}
		}
		else break;
	}		
	for(new i = 0; i < 12; i ++)
	{
		if(Timestamp >= MonthTimes[i][2])
		{
			month ++;
			Timestamp -= MonthTimes[i][2];
		}
		else break;
	}
	day = 1 + (Timestamp / 86400);
	Timestamp %= 86400;
	iHour = iHourGMT + (Timestamp / 3600);
	Timestamp %= 3600;
	iMinute = iMinuteGMT + (Timestamp / 60);
	second = (Timestamp % 60);
	if(iMinute > 59)
	{
		iMinute = 0;
		iHour ++;
		if(iHour > 23)
		{
			iHour -= 24;
			day ++;
			if(day > MonthTimes[month][IsLeapYear(year)])
			{
				day = 1;
				month ++;
				if(month > 12)
				{
					month = 1;
					year ++;
				}
			}
		}
	}
	return 1;
}