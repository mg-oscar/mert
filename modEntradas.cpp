//
//
/******************************************************************************/
/* Hilo para interrogar al modulo de E/S                                      */
/******************************************************************************/
void *mod_EntSal(void *inp)
{	unsigned int tmReintenta = milisegundos();
	int k;

	struct st_io *mod;
	mod = (struct st_io*)inp;

	printf("inicia\n");
	printf("Status Comm: %d\n", (int)mod->op);

	mod->status_comm = 0x01;

	while(1)
	{
		switch(mod->op)
		{	case 0:		// Espera a tener la configuracion del puerto
				break;

			case 1:		// OPEN
				if( (milisegundos() - tmReintenta) >= 1000 )
				{	// intenta abrir el puerto serial
					if( moduloES.OpenComm(mod->CommName, mod->Baud) != 0)
					{
						printf("Open Comm Error Mod. E/S: %s\n", mod->CommName);
						mod->status_comm = 0x01;	// error open comm
					}
					else
					{
						printf("OK Open Comm Mod. E/S: %s\n", mod->CommName);
						mod->status_comm = 0x00;	// OK open comm
						mod->op = 2; 
						moduloES.dirmodulo = DirModuModbus;
						moduloES.wrAddress();
					}
					tmReintenta = milisegundos();
				}
				break;

			case 2:		// Abre todas las salidas
				if( (milisegundos() - tmReintenta) >= TiempoBarridoModuloIO)
				{	tmReintenta = milisegundos();
					moduloES.relay_all(moduloES.dirmodulo,0x00);
					mod->op = 3;						
				}
				break;

			case 3:		// Barrido
				if( (milisegundos() - tmReintenta) >= TiempoBarridoModuloIO)
				{
					tmReintenta = milisegundos();
					if(mod->NoControl != 0)
					{
						if(mod->OnOFF==0)
							moduloES.relay(mod->NoControl-1, RELAY_OPEN);
						else if(mod->OnOFF==1)
							moduloES.relay(mod->NoControl-1, RELAY_CLOSE);
						else
							moduloES.pulse(mod->NoControl-1, RELAY_OPEN, mod->tiempo);
						mod->NoControl = 0;
					}
					else
					{
						if( moduloES.ReadEntradas() != 0)
						{
							printf("Error input\n");
							mod->status_comm = 0x01;	// error en barrido
						}
						else
						{
							mod->status_comm = 0x00;	// OK

							for(k=0; k<4; k++)
							{
								if( moduloES.Entradas[k]== 0x81)
								{
									if(moduloES.Botones[k] == 0)
									{	moduloES.Botones[k] = 1;
										moduloES.tmBotones[k] = milisegundos();
										moduloES.fgBotones[k] = 0x01;
									}
								}
								else
								{
									if(moduloES.fgBotones[k] == 0x00)
										moduloES.Botones[k] = 0;
								}

								if(moduloES.fgBotones[k] == 0x01)
								{
									if( (milisegundos() - moduloES.tmBotones[k]) > 500)
										moduloES.fgBotones[k] = 0x00;
								}
							}
						}
					}
				}
				break;

			case 4:
				moduloES.CloseComm();
				mod->op = 0;
				break;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////


