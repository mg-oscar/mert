/******************************************************************************/
/*               PROCEDIMIENTOS DE PRUEBA DEL MONEDERO                        */
/******************************************************************************/
//
/******************************************************************************/
/* Regresa de la prueba del monedero                                          */
/******************************************************************************/
void BotonRegresar (GtkWidget *widget, gpointer *data)
{
	testCoin = 0;
	stModIO->AccionMonedero = 3;	// deshabilita monedero

	winMenuTest = TEST_RETURN;
	gtk_widget_destroy(frmCoin);

	stModIO->cmdCoin = CMD_COIN_DISABLE;	
	while(stModIO->cmdCoin !=0 );
}

/******************************************************************************/
/* RadioEvent                                                                 */
/******************************************************************************/
void ButtonEventCoin (GtkWidget *widget, gpointer *data)
{
//	g_print ("radio event: %s\n", (char *)data);
	stModIO->AccionMonedero = atoi((char *)data);
}

/******************************************************************************/
/* Event                                                                      */
/******************************************************************************/
void ButtonEventErase (GtkWidget *widget, gpointer *data)
{	char cvalor[10];
	
	totaltest = 0;
	snprintf(cvalor, 10, "%4.2f", double(totaltest/100));
	gtk_button_set_label( GTK_BUTTON(lblTotalTest), cvalor);
}

//GTK_BUTTONS_OK_CANCEL
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int winDialogoOK( char *msg1, char *msg2)
{	int response;
	GtkDialogFlags flags = GTK_DIALOG_MODAL;	// | GTK_DIALOG_DESTROY_WITH_PARENT;
 
	GtkWidget* dialogo = gtk_message_dialog_new(
        NULL, flags, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
        "%s",msg1);

    gtk_message_dialog_format_secondary_text(
        GTK_MESSAGE_DIALOG(dialogo),
        "%s", msg2);

    response = gtk_dialog_run(GTK_DIALOG(dialogo));
	gtk_widget_destroy(dialogo);

	if(response == GTK_RESPONSE_OK)
		return 0x00;	// Operacion Confirmada

	return 0x01;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void ButtonCambioMonedero (GtkWidget *widget, gpointer *data)
{	int k, re_monedero=1;

	printf("CAMBIO DE MONEDERO\n");

	// Revisar que esten en Cero los contadores
	for(k=0;k<15; k++)
	{	if(detMonedas.Total[k] != 0)
		break;
	}
//	printf("k=%d\n", k);

	if(k!=15)
	{	// Indicar que tiene que realizar el corte
		winDialogoOK((char *)"REALIZAR CORTE CAJA DE MONEDAS", (char *)"");
		return;
	}

	// Indicar que tiene que realizar el corte
	winDialogoOK((char *)"PROCEDA A REALIZAR EL CAMBIO", (char *)"OK CUANDO TERMINE");

	// Inserta un Registro en Ceros
	memset(&detMonedas, 0, sizeof(detMonedas));
	if(insBDDetalle(dbMonedas, detMonedas) != 0)
	{
		printf("Error al Insertar Registro Monedas\n");
		return;	// Error en Base de Datos
	}
//	printf("OK al insertar Registro Monedas\n");
	lastMovCoins = rdLastCaja(dbMonedas, &detMonedas, 0);

	// detiene el barrido del Monedero
	stModIO->cmdCoin = CMD_COIN_WAIT;

	usleep(300000);
	if( (re_monedero = Monedero_Init()) == 0)
	{	// ya Inicializo el Monedero, revisar si la BD tiene los CVF grabados
//		revCVFMonedas();
		if( posiciona_destrabador()!=0)
		{
			printf("Error al destrabar el monedero\n");
		}
	}
	detMonedas.CajaID = idCajaMonedas;
	UpCajaIDM();

	stModIO->cmdCoin =  CMD_COIN_POLL;

	gtk_widget_destroy(frmCoin);
	winCoin();
	stModIO->AccionMonedero = 0;
	gtk_widget_hide(msgMain);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void Initdatos(GtkWidget *fx, int x, int y)
{	// ************* 
	GtkWidget* frmInit = gtk_frame_new("DATOS MONEDERO");
	gtk_fixed_put ( GTK_FIXED (fx), frmInit, x, y);
	gtk_setsize(frmInit, 315, 150);

	GtkWidget		*scrolled_window;
	GtkTextBuffer	*bufInit;
	GtkWidget		*txtInit;

	bufInit = gtk_text_buffer_new (NULL);

	char tmptxt[1024];
	txtInit = gtk_text_view_new_with_buffer(bufInit);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (txtInit), GTK_WRAP_WORD); 

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), 
									GTK_POLICY_AUTOMATIC,
									GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scrolled_window), txtInit);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 5);
	gtk_container_add (GTK_CONTAINER (frmInit), scrolled_window);

						// monospaced font
	gtk_setfont(txtInit, "DejaVu Sans Mono", 700, 10);

	sprintf(tmptxt, 
		"Equipment ID:      %s\n"
		"Manufacturer ID:   %s\n"
		"Product Code:      %s\n"
		"Build Code:        %s\n"
		"Software Revision: %s\n"
		"Numero de Serie:   %ld\n"
		"Polling Priority:  %ld\n", 
		coins.cctalkID.cctalk_id,		// Equipment ID
		coins.cctalkID.cctalk_mid,		// Manufacturer ID
		coins.cctalkID.cctalk_pc,		// Product Code
		coins.cctalkID.cctalk_bc,		// Build Code
		coins.cctalkID.cctalk_sr,		// 
		coins.cctalkID.cctalk_sn,		// numero de serie
		coins.cctalkID.cctalk_pp);		// polling priority

	gtk_text_buffer_set_text ( bufInit, tmptxt, strlen(tmptxt));
	gtk_widget_show_all (winmantto);
	gtk_widget_hide(msgMain);
	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winCoin(void)
{	GtkWidget *radio, *frmRad;
	GtkWidget *lblUD;
	GtkWidget *fxCoin;		// window Monedero
	int i;
	char txtf[10];

	// FRAME
	frmCoin = gtk_frame_new("MONEDERO");
	
	if(mert_famisa)
		gtk_fixed_put (GTK_FIXED(fxMantto), frmCoin, xcoin,  ycoin);
	else
		gtk_fixed_put (GTK_FIXED(fxMantto), frmCoin, 50, 0);
		
	gtk_setsize(frmCoin, 800, 750);
	gtk_setcolorbg(frmCoin,  (char *)"gainsboro");
	gtk_setfont(frmCoin, "SANS", 700,15);

	//
	fxCoin = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmCoin), fxCoin);

	// Botones de accion
	frmRad = gtk_frame_new("ACCION");
	gtk_fixed_put ( GTK_FIXED (fxCoin), frmRad, 8, 20);
	GtkWidget *fxB = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmRad), fxB);
	gtk_setsize(fxB, 400, 250);

	radio = CreateButton (fxB, (char *)"INICIALIZA",10,10);
	gtk_setfont(radio, "SANS", 700, 45);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventCoin), (gpointer *)"1");	

	radio = CreateButton (fxB, (char *)"HABILITA",10,100);
	gtk_setfont(radio, "SANS", 700, 45);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventCoin), (gpointer *)"2");

	radio = CreateButton (fxB, (char *)"DESHABILITA", 10, 190);
	gtk_setfont(radio, "SANS", 700, 45);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventCoin), (gpointer *)"3");

	// Totalizadores
	frmRad = gtk_frame_new("TOTALES");
	gtk_fixed_put ( GTK_FIXED (fxCoin), frmRad, 8, 320);
	fxB = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmRad), fxB);
	gtk_setsize(fxB, 400, 90);

//	lblUD = CreateLabel (fxB, (char *)"Ult.Dep.", (char *)"SANS", 700, 15, 5, 5);
//	lblUDCoin = CreateButton (fxB, (char *)"   0.00", 100,5);

	totaltest = 0;
	lblUD = CreateLabel (fxB, (char *)"Deposito", (char *)"SANS", 700, 15, 5, 20);
	lblTotalTest = CreateButton (fxB, (char *)"   0.00", 100,20);

	radio = CreateButton (fxB, (char *)"Borrar",200,10);
	gtk_setfont(radio, "SANS", 700, 35);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventErase), (gpointer *)"1");

	// cambio de monedero
	frmRad = gtk_frame_new("Cambio de Monedero");
	gtk_fixed_put ( GTK_FIXED (fxCoin), frmRad, 8, 450);
	fxB = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmRad), fxB);
	gtk_setsize(fxB, 400, 120);

	radio = CreateButton (fxB, (char *)" CAMBIAR\nMONEDERO",10,5);
	gtk_setfont(radio, "SANS", 700, 40);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonCambioMonedero), (gpointer *)"1");

/*	i = detMonedas.Total[15];
	if(i==0) i=1;
	sprintf(txtf,"%2d", i);

	lblUD = CreateLabel (fxB, (char *)"Factor", (char *)"SANS", 700, 15, 340, 5);
	entFactor  = CreateEntry(fxB, txtf, (char *)"Sans", 700,20, 340,30 );
	gtk_entry_set_max_length(GTK_ENTRY(entFactor), 3);
	gtk_setsize(entFactor, 20, 30);
	gtk_widget_show (entFactor);
*/

	//**************************************************************************
	arrCoinCvt =   createArrayBtn( fxCoin, den_coin, 0, coins.TotalDenominaciones, 1,  500, 20);
	struct key *p1=NULL;
	for(i=0; i<coins.TotalDenominaciones; i++)
	{
//		sprintf(txtf,"%s", coins.cbDesgloze.cvf[i]);
		sprintf(txtf,"%s", coins.depositado.cvf[i]);
		p1= &g_array_index(arrCoinCvt, struct key, i);
		gtk_label_set_label (GTK_LABEL(p1->label) ,  txtf); //coins.cbDesgloze.cvf[i]);
	}

	arrCoinTotal = createArrayBtn( fxCoin, tot_coin, 0, coins.TotalDenominaciones, 1, 590, 20);
	for(i=0; i<coins.TotalDenominaciones; i++)
	{
		p1 = &g_array_index(arrCoinTotal, struct key, i);
		gtk_setsize( p1->label, 40, 10);
		gtk_label_set_xalign (GTK_LABEL(p1->label), 1.0);
	}

	char txtC[32][10];
	memset(txtC, 0x00, sizeof(txtC));
	for(i=0; i<coins.TotalDenominaciones; i++)
		snprintf(txtC[i],10," $%6.2f", (double)coins.depositado.valor[i]/100);

	arrCoinLeds	= createArrayBtnImg( fxCoin, txtC, Led_Gris15, 0, coins.TotalDenominaciones, 1, 670, 15);

	Initdatos(fxCoin, 50, 600);

	lblUD = CreateLabel (fxCoin, (char *)"Ult.Moneda", (char *)"SANS", 700, 15, 510, 590);
	lblUDCoin = CreateButton (fxCoin, (char *)"0.00", 620, 580);

	// Regresar
	radio = CreateButton (fxCoin, (char *)"REGRESAR",450,650);
	gtk_setfont(radio, "SANS", 700, 45);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (BotonRegresar), (gpointer *)"1");	

	gtk_widget_show_all (winmantto);
	gtk_widget_hide(msgMain);

	testCoin=1;
	stModIO->AccionMonedero = 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void coinLed(int noled)
{	int i, tot;
	char label[20];

	struct key *p1=NULL;
	p1= &g_array_index(arrCoinTotal, struct key, noled);

	tot = atoi (gtk_label_get_label( GTK_LABEL(p1->label) ) );
	tot++;
	snprintf(label, 10, "%d",tot);
	gtk_label_set_label( GTK_LABEL(p1->label), label);

	p1= &g_array_index(arrCoinLeds, struct key, noled);

	p1->image = gtk_image_new_from_file (Led_Verde15);
	gtk_button_set_image (GTK_BUTTON (p1->button), p1->image);
	gtk_widget_show_all (winmantto);
	gtk_widget_hide(msgMain);
	

	updWinTm(500);

//	updWin();
//	usleep(500000);

	p1->image = gtk_image_new_from_file (Led_Gris15);
	gtk_button_set_image (GTK_BUTTON (p1->button), p1->image);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void AccionesMonedero(void)
{	static int lapso=milisegundos();
	int ipos, ivalor;
	char cvalor[10];
	double dvalor;

	
	if( (milisegundos() - lapso) >= 300)
	{	lapso = milisegundos();
//		printf("monedero=%d\n",stModIO->AccionMonedero);

		switch(stModIO->AccionMonedero)
		{
			case 0:

				if( (coins.Posicion>0) && (coins.Path>=MONEDA_EN_CAJA) )
				{
					ipos   = coins.Posicion;
					ivalor = coins.Value(ipos);

					printf("Moneda POS=%d valor=%d\n", ipos, ivalor);

					coins.Posicion=0;	// elimina evento

					dvalor = ivalor;
					snprintf(cvalor, 10, "%4.2f", double(dvalor/100));
					gtk_button_set_label( GTK_BUTTON(lblUDCoin), cvalor);

					dvalor = totaltest += ivalor;
					printf("totaltest = %d\n", totaltest);
					snprintf(cvalor, 10, "%4.2f", double(dvalor/100));
					gtk_button_set_label( GTK_BUTTON(lblTotalTest), cvalor);	
					coinLed(ipos);
				}
				break;

			case 1:		
					stModIO->AccionMonedero = 0;
					break;

			case 2:	// Habilita Monedero
					stModIO->cmdCoin = CMD_COIN_ENABLE;	
					while(stModIO->cmdCoin !=0 );
					stModIO->AccionMonedero = 0;
					break;

			case 3:
					stModIO->cmdCoin = CMD_COIN_DISABLE;	
					while(stModIO->cmdCoin !=0 );
					stModIO->AccionMonedero = 0;
					break;

			default:
					stModIO->AccionMonedero = 0;
					break;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////