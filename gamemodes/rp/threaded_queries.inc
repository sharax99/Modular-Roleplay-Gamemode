public OnQueryError(errorid, error[], callback[], query[], connectionHandle)
{
	printf("[CAPTIVA]: ERROR: %s",query);
	switch(errorid)
	{
		case CR_SERVER_GONE_ERROR:
		{
			printf("Captiva - lost connection to MySQL, reconnecting...");
			mysql_reconnect(connectionHandle);
		}
		case ER_SYNTAX_ERROR:
		{
			printf("[CAPTIVA]: SYNTAX ERROR: %s",query);
		}
	}
	return 1;
}