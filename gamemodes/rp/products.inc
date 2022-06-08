stock LoadProducts()
{
	Code_ExTimer_Begin(LoadProducts);

	new day= GetDayName();
    if(day == 7)
    {
    	printf("[Society] Weekly limits have been reseted.");
    	mysql_query(mySQLconnection, "UPDATE ipb_products SET product_limit_used = 0");
    }

	new rows, fields, p_id = 0;

	mysql_query(mySQLconnection, "SELECT * FROM `ipb_products`");
	cache_get_data(rows, fields);

	for(new row = 0; row != rows; row++)
	{
		p_id++;

		Product[p_id][product_id] = cache_get_row_int(row, 0);
		cache_get_row(row, 1, Product[p_id][product_name], mySQLconnection, 40);
		Product[p_id][product_type] = cache_get_row_int(row, 2);
		Product[p_id][product_owner] = cache_get_row_int(row, 3);
		Product[p_id][product_price] = cache_get_row_int(row, 4);
		Product[p_id][product_value1] = cache_get_row_int(row, 5);
		Product[p_id][product_value2] = cache_get_row_int(row, 6);
		Product[p_id][product_model] = cache_get_row_int(row, 7);
		Product[p_id][product_extra] = cache_get_row_int(row, 8);
		Product[p_id][product_group] = cache_get_row_int(row, 9);
		Product[p_id][product_limit] = cache_get_row_int(row, 10);
		Product[p_id][product_limit_used] = cache_get_row_int(row, 11);
		Product[p_id][product_player] = cache_get_row_int(row, 12);

		Iter_Add(Products, p_id);
	}

	printf("Loaded products [count: %d] [time %d ms]", p_id, Code_ExTimer_End(LoadProducts));

	return 1;
}

stock LoadProduct(prod_uid)
{
	new p_id = Iter_Free(Products), rows, fields;

	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_products` WHERE product_uid = %d LIMIT 1", prod_uid));
	cache_get_data(rows, fields);

	Product[p_id][product_id] = cache_get_row_int(0, 0);
	cache_get_row(0, 1, Product[p_id][product_name], mySQLconnection, 40);
	Product[p_id][product_type] = cache_get_row_int(0, 2);
	Product[p_id][product_owner] = cache_get_row_int(0, 3);
	Product[p_id][product_price] = cache_get_row_int(0, 4);
	Product[p_id][product_value1] = cache_get_row_int(0, 5);
	Product[p_id][product_value2] = cache_get_row_int(0, 6);
	Product[p_id][product_model] = cache_get_row_int(0, 7);
	Product[p_id][product_extra] = cache_get_row_int(0, 8);
	Product[p_id][product_group] = cache_get_row_int(0, 9);
	Product[p_id][product_limit] = cache_get_row_int(0, 10);
	Product[p_id][product_limit_used] = cache_get_row_int(0, 11);
	Product[p_id][product_player] = cache_get_row_int(0, 12);

	Iter_Add(Products, p_id);

	return p_id;
}

stock ReloadProduct(prod_uid)
{
	new prod_id = GetProductByUid(prod_uid);
	if(prod_id == -1) return 1;

	Product[prod_id][product_id] = 0;
	Product[prod_id][product_type] = 0;
	Product[prod_id][product_owner] = 0;
	Product[prod_id][product_price] = 0; 
	Product[prod_id][product_value1] = 0; 
	Product[prod_id][product_value2] = 0;   
	Product[prod_id][product_model] = 0;  
	Product[prod_id][product_extra] = 0;
	Product[prod_id][product_group] = 0;
	Product[prod_id][product_player] = 0;

	Iter_Remove(Products, prod_id);

	new p_id = Iter_Free(Products);
	new rows, fields;

	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_products` WHERE product_uid = %d LIMIT 1", prod_uid));
	cache_get_data(rows, fields);

	Product[p_id][product_id] = cache_get_row_int(0, 0);
	cache_get_row(0, 1, Product[p_id][product_name], mySQLconnection, 40);
	Product[p_id][product_type] = cache_get_row_int(0, 2);
	Product[p_id][product_owner] = cache_get_row_int(0, 3);
	Product[p_id][product_price] = cache_get_row_int(0, 4);
	Product[p_id][product_value1] = cache_get_row_int(0, 5);
	Product[p_id][product_value2] = cache_get_row_int(0, 6);
	Product[p_id][product_model] = cache_get_row_int(0, 7);
	Product[p_id][product_extra] = cache_get_row_int(0, 8);
	Product[p_id][product_group] = cache_get_row_int(0, 9);
	Product[p_id][product_limit] = cache_get_row_int(0, 10);
	Product[p_id][product_limit_used] = cache_get_row_int(0, 11);
	Product[p_id][product_player] = cache_get_row_int(0, 12);

	Iter_Add(Products, p_id);

	return p_id;
}

stock GetProductByUid(prodUid)
{
	foreach (new p : Products)
	{
		if( Product[p][product_id] == prodUid ) return p;
	}
	
	return -1;
}

stock RemoveProduct(prod_uid)
{
	new prod_id = GetProductByUid(prod_uid);
	if(prod_id == -1) return 1;

	Product[prod_id][product_id] = 0;
	Product[prod_id][product_type] = 0;
	Product[prod_id][product_owner] = 0;
	Product[prod_id][product_price] = 0; 
	Product[prod_id][product_value1] = 0; 
	Product[prod_id][product_value2] = 0;   
	Product[prod_id][product_model] = 0;  
	Product[prod_id][product_extra] = 0;
	Product[prod_id][product_group] = 0;
	Product[prod_id][product_player] = 0;

	Iter_Remove(Products, prod_id);
	
	return 1;
}