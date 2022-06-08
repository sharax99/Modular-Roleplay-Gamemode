// Color management

stock Convert_HexToRGB(color, &r, &g, &b)
{
	r = (color >> 24) & 0xFF;
    g = (color >> 16) & 0xFF;
    b = (color >> 8) & 0xFF;
}

stock Convert_RGBToHex(r, g, b)
{
	return (((r & 0xFF) << 16) | ((g & 0xFF) << 8) | ((b & 0xFF) << 0));
}

stock Convert_HexToDecimal(color)
{	
	return color >>> 8;
}

stock RGBAToHex(r, g, b, a) //By Betamaster
{
    return (r<<24 | g<<16 | b<<8 | a);
}

stock hexstring(string[])
{
	new
	ret,
	val,
	i;
	if (string[0] == '0' && string[1] | 0x20 == 'x') i = 2;
	while (string[i])
	{
		ret <<= 4;
		val = string[i++] - '0';
		if (val > 0x09) val -= 0x07;
		if (val > 0x0F) val -= 0x20;
		if (val < 0x01) continue;
		if (val < 0x10) ret += val;
	}
	return ret;
}

stock HexToRGBA(colour, &r, &g, &b, &a) //By Betamaster
{
    r = (colour >> 24) & 0xFF;
    g = (colour >> 16) & 0xFF;
    b = (colour >> 8) & 0xFF;
    a = colour & 0xFF;
}

stock MakeColorDarker(color, percent)
{
	new 
		r, 
		g, 
		b, 
		a,
		Float:percentage = (100 - percent) + 0.1;
		
	HexToRGBA(color, r, g, b, a);
	
	r = floatround(r * percentage / 100);
	g = floatround(g * percentage / 100);
	b = floatround(b * percentage / 100);
	
	return RGBAToHex(r, g, b, a);
}

stock MakeColorLighter(color, percent)
{
	new 
		r, 
		g, 
		b, 
		Float:percentage = (100 - percent) + 0.1;
	
	Convert_HexToRGB(color, r, g, b);
	
	r = floatround(r * percentage / 100) + floatround(255 - percentage / 100 * 255);
	g = floatround(g * percentage / 100) + floatround(255 - percentage / 100 * 255);
	b = floatround(b * percentage / 100) + floatround(255 - percentage / 100 * 255);
	
	return Convert_RGBToHex(r, g, b);
}