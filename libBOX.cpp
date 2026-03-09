
/******************************************************************************/
/*  Mueve Contadores a WriteBlocks                                            */
/******************************************************************************/
void CLASS_BOX::BoxToBlocks( unsigned int *addr, BYTE wrblock[64][16])
{	int i, k;
	unsigned char *p;
	int crc;

	p = (unsigned char *)addr;

	for(i=0; i<48; i++)
		wrblock[4][i] = *p++;

	for(i=0; i<48; i++)
		wrblock[8][i] = *p++;

	crc = 0xFFFF;
	p = (unsigned char *)addr;
	for(i=0; i< (sizeof(COIN_BOX)-4); i++)
		crc = crcModBus( *p++, crc);
	printf("crc=%04X\n",crc);

	p = (unsigned char *)&crc;
	for(i=0; i<4; i++)
		wrblock[12][i] = *p++;

	printf("WriteBlocks\n");
	for(i = 0; i < 64; i++)
	{	printf("B%02d=",i);
		for(int k = 0; k < 16; k++)
			printf("%02X",wrblock[i][k]);
		printf("\n");
	}
}

/******************************************************************************/
/* Mueve ReadBlocks a Contadores                                              */
/******************************************************************************/
void CLASS_BOX::BlocksToBox( unsigned int *addr, BYTE rdblock[64][16])
{	int i, k;
	unsigned char *p;
	int crc;

	crc = 0xFFFF;
	for(i=0; i<48; i++)
		crc = crcModBus( rdblock[4][i], crc);
	for(i=0; i<48; i++)
		crc = crcModBus( rdblock[8][i], crc);

	if(crc != *(int *)&rdblock[12])
		return;
	
//	printf("CRC OK\n");
	
	
	p = (unsigned char *)addr;

	for(i=0; i<48; i++)
		*p++ = rdblock[4][i];

	for(i=0; i<48; i++)
		*p++ = rdblock[8][i];

	for(i=0; i<4; i++)
		*p++ = rdblock[12][i];
	
}


/******************************************************************************/
/* c_crc_MODBUS: Calcula el crc                                               */
/* Polinomio:  x16+x15+x2+1                                                   */
/******************************************************************************/
int CLASS_BOX::crcModBus(unsigned char dato, int crc)
{
 	return (crc>>8) ^ t_crc_MODBUS[(crc ^ dato) & 0x00ff];
}

/******************************************************************************/