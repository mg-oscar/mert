//
//
//
#include "retirocajas.h"
//
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void xmlCorte(int tipo, DET_BILL_COIN detalle)
{	FILE *fb;
	char dxml[2048] = {};
	char ln[70], fname[100];
	int i;

	strcpy (dxml, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n<archivo tipo=\"corte\">\n");

	sprintf (ln, "  <c1>%d</c1>\n", MertLmt.type);
	strcat(dxml, ln);
	sprintf (ln, "  <c2>%d</c2>\n", MertID.lineaTDE);
	strcat(dxml, ln);
	sprintf (ln, "  <c3>%d</c3>\n", MertID.estacionTDE);
	strcat(dxml, ln);
	sprintf (ln, "  <c4>%d</c4>\n", MertID.lineaTV);
	strcat(dxml, ln);
	sprintf (ln, "  <c5>%d</c5>\n", MertID.estacionTV);
	strcat(dxml, ln);
	sprintf (ln, "  <c6>%d</c6>\n", MertID.NumDisp);
	strcat(dxml, ln);
	sprintf (ln, "  <c7>%d</c7>\n", tipo);
	strcat(dxml, ln);
	sprintf (ln, "  <c8>%u</c8>\n", detalle.CajaID);
	strcat(dxml, ln);
	sprintf (ln, "  <c9>%d</c9>\n", detalle.NoMov);
	strcat(dxml, ln);
	sprintf (ln, "  <c10>%d</c10>\n", detalle.Usuario1);
	strcat(dxml, ln);
	sprintf (ln, "  <c100>%d</c100>\n", detalle.Usuario2);
	strcat(dxml, ln);
	sprintf (ln, "  <c11>%d</c11>\n", detalle.Ficha);
	strcat(dxml, ln);
	sprintf (ln, "  <c12>%d</c12>\n", detalle.Sello);
	strcat(dxml, ln);
	sprintf (ln, "  <c13>%s</c13>\n", detalle.FechaCreacion);
	strcat(dxml, ln);
	sprintf (ln, "  <c14>%s</c14>\n", detalle.FechaCorte);
	strcat(dxml, ln);
	sprintf (ln, "  <c15>%.2f</c15>\n", (double)detalle.CantidadTotal/100);
	strcat(dxml, ln);
	strcat (dxml, "  <detalle-list>\n");
	int tot= 0;
	for(i=0;i<TOTAL_DENOM_BM; i++)
	{
		if(detalle.Valor[i] != 0)
		{
			sprintf (ln, "    <detalle valor=\"%.2f\" cantidad=\"%04d\"/>\n",
							(double)detalle.Valor[i]/100, detalle.Total[i] );
			strcat(dxml, ln);
			tot += detalle.Total[i];
		}
	}
	strcat (dxml, "  </detalle-list>\n");
	sprintf (ln, "  <c16>%d</c16>\n", tot);
	strcat(dxml, ln);

	sprintf (ln, "  <c17>%08X</c17>\n", (unsigned int)data_user[NOUSER_1].nouid );
	strcat(dxml, ln);
	sprintf (ln, "  <c18>%s</c18>\n", data_user[NOUSER_1].name);
	strcat(dxml, ln);

	sprintf (ln, "  <c19>%08X</c19>\n", (unsigned int)data_user[NOUSER_2].nouid );
	strcat(dxml, ln);
	sprintf (ln, "  <c20>%s</c20>\n", data_user[NOUSER_2].name);
	strcat(dxml, ln);

	strcat (dxml, "</archivo>\n");

	printf("[bytes=%d]\n", (int)strlen(dxml));
	printf("[%s]\n", dxml);

	if(tipo==0)
		snprintf(fname,100,"%stransacciones/m",mert_operacion.path);
	else
		snprintf(fname,100,"%stransacciones/b",mert_operacion.path);

	snprintf(ln,70, "%02d%02d%03d%04d%08d.cor", MertLmt.type, MertID.lineaTDE,
												MertID.estacionTDE, MertID.NumDisp,
												detalle.NoMov);
	strcat(fname,ln);
	if( (fb=fopen(fname,"w")) != NULL)
	{	
		fprintf ( fb,"%s", dxml);
		fclose(fb);
	}
	else
	{	// error al crear archivo de transaccion
		sprintf(dxml,"Error al crear archivo de transaccion [%s]", fname);
		printf("%s\n",dxml);
	}
	return ;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void fillDatellaImp(DET_BILL_COIN detalle)
{	int i,j,tot;

	datImprimir.linea		= mert_identidad.LINEA;
	datImprimir.estacion	= mert_identidad.ESTACION;
	datImprimir.equipo		= mert_identidad.NUMDISP;

	// ******************* datos a capturar ************************************
	datImprimir.usuario1		= detalle.Usuario1;
	datImprimir.usuario2		= detalle.Usuario2;
	datImprimir.ficha		= detalle.Ficha;
	datImprimir.sello		= detalle.Sello;
	////////////////////////////////////////////////////////////////////////////		
	datImprimir.alcancia	= detalle.CajaID;

	datImprimir.folio		= detalle.NoMov;
	strcpy(datImprimir.fecha, detalle.FechaCreacion);
	strcpy(datImprimir.nlinea, Lineas.name[mert_identidad.LINEA-1]);

	strcpy(datImprimir.nestacion, 
			Lineas.estaciones[mert_identidad.LINEA-1].name[mert_identidad.ESTACION-1]);

	strcpy(datImprimir.nequipo, MertID.code);

	memset(&datImprimir.valor,   0, sizeof(datImprimir.valor));
	memset(&datImprimir.cantidad,0, sizeof(datImprimir.cantidad));

	j = tot= 0;
	for(i=0;i<TOTAL_DENOM_BM; i++)
	{
		if(detalle.Valor[i] != 0)
		{	datImprimir.valor[j]	= detalle.Valor[i];
			datImprimir.cantidad[j] = detalle.Total[i];
			tot += detalle.Total[i];
			j++;
		}
	}

	datImprimir.total = tot;
	datImprimir.monto  = detalle.CantidadTotal;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int waitBox( char *msg)
{	int response;
	GtkDialogFlags flags = GTK_DIALOG_MODAL;
 
	GtkWidget* dialogo = gtk_message_dialog_new(
        NULL, flags, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
        "CONFIRMAR CAJA NUEVA!");

    gtk_message_dialog_format_secondary_text(
        GTK_MESSAGE_DIALOG(dialogo), "%s", msg);

    response = gtk_dialog_run(GTK_DIALOG(dialogo));
	if(response == GTK_RESPONSE_OK)
	{	printf("CAJA=%d\n", corte_datos.caja);
		gtk_widget_destroy(dialogo);
		return 0x00;
	}
	gtk_widget_destroy(dialogo);
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int changeBox( char *msg)
{	int response;
	GtkDialogFlags flags = GTK_DIALOG_MODAL;
 
	GtkWidget* dialogo = gtk_message_dialog_new(
        NULL, flags, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
        "DESEAS CAMBIAR DE CAJA!");

    gtk_message_dialog_format_secondary_text(
        GTK_MESSAGE_DIALOG(dialogo), "%s", msg);

    response = gtk_dialog_run(GTK_DIALOG(dialogo));
	if(response == GTK_RESPONSE_OK)
	{	//printf("CAJA=%d\n", corte_datos.caja);
		gtk_widget_destroy(dialogo);

		// libera caja
		stModIO->OnOFF     = R_PULSE;
		stModIO->tiempo    = 75;	// 7.5 segundos
		stModIO->NoControl = SOLENOIDE_CAJA_MONEDERO;
		while(stModIO->NoControl !=0 );

		puerta.Buzzer(6);

		return 0x00;
	}
	gtk_widget_destroy(dialogo);
	return 0x01;
}

/******************************************************************************/
/* Regresa de Retiro de Cajas                                                 */
/******************************************************************************/
void BtnRegRC (GtkWidget *widget, gpointer *data)
{
	xmlCorteParcial();
	
	corte_datos.caja = 0;
	gtk_widget_destroy(frmRC);
	mert_operacion.no_menu = MENU_REGRESO;
	tbCajas = TM_CAJAS;		// restablece tiempo de barrido
}

/******************************************************************************/
/* Desbloquea Caja de Monedas                                                 */
/******************************************************************************/
void BtnBoxM (GtkWidget *widget, gpointer *data)
{	int opc;

	opc = GPOINTER_TO_INT(data);

	if(opc==1)
	{	// Retirar Caja de Monedas
		corte_datos.caja = 1;
		winCorteCaja((char *)"Retira Caja de Monedas");
		gtk_widget_hide (frmRC);
	}
	else if(opc==2)
	{	// Retirar Caja de Billetes
		corte_datos.caja = 2;
		winCorteCaja((char *)"Retira Caja de Billetes");
		gtk_widget_hide (frmRC);		
	}
}

/******************************************************************************/
/* Calcula la cantidad y el peso de las monedas depositadas en la caja        */
/******************************************************************************/
void CalculaPesoTotal(void)
{	int i;

	total_peso = total_monedas = 0;
	for(i=0;i<TOTAL_DENOM_BM; i++)
	{	if(detMonedas.Valor[i] != 0)
		{	total_monedas += detMonedas.Total[i];
			total_peso += pmoneda(detMonedas.Valor[i], detMonedas.Total[i]);
		}
	}
}

/******************************************************************************/
/* Calcula la cantidad de billetes en el stacker                              */
/******************************************************************************/
void CalculaBilletesTotales(void)
{	int i;

	total_billetes = 0;
	for(i=0;i<TOTAL_DENOM_BM; i++)
	{	if(detBilletes.Valor[i] != 0)
			total_billetes += detBilletes.Total[i];
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winDetalleCorte(GtkWidget *fx, DET_BILL_COIN detalle)
{	int k,j;
	char tmp[30];

	GtkWidget* frmInit = gtk_frame_new("Detalle");
	gtk_fixed_put ( GTK_FIXED (fx), frmInit, ancho +40 , 10);
	gtk_setsize(frmInit, ancho, altura);

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
	gtk_setfont(txtInit, "DejaVu Sans Mono", 700, 11);

	sprintf(tmptxt, 
		"NoMov:         %d\n"
		"Caja ID:       %u\n"
		"Apertura:      %s\n"
		"               %s\n\n",		
		detalle.NoMov,
		detalle.CajaID,
		substr(detalle.FechaCreacion,0,10),
		substr(detalle.FechaCreacion,11,10));

	j = 0;
	for(k=0;k<TOTAL_DENOM_BM; k++)
	{	if(detalle.Valor[k] != 0)
		{	j += detalle.Total[k];
			sprintf(tmp,"      %6.2f    %d\n", double(detalle.Valor[k])/100, detalle.Total[k]);
			strcat(tmptxt, tmp);
		}
	}
	
	sprintf(tmp,"\n      Total    %6d\n\n", j);
	strcat(tmptxt, tmp);

	sprintf(tmp,"\n\n  Monto Total  $ %6.2f\n\n", double(detalle.CantidadTotal)/100);
	strcat(tmptxt, tmp);

	gtk_text_buffer_set_text ( bufInit, tmptxt, strlen(tmptxt));
	gtk_text_view_set_editable( GTK_TEXT_VIEW (txtInit), FALSE);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winFrameCoin(GtkWidget *fxRC)
{	GtkWidget *frmBoxM, *fxBoxM, *frmNM, *fxNM;
	GtkWidget *lbl, *btn;
	double v;
	char porc[30];

	//*********************** Frame Monedas
	frmBoxM = gtk_frame_new("CAJA DE MONEDAS");
	gtk_fixed_put (GTK_FIXED(fxRC), frmBoxM, 20,  20);
	gtk_setcolorbg(frmBoxM,  (char *)"gainsboro");
	gtk_setfont(frmBoxM, "SANS", 700, 15);

	fxBoxM = gtk_fixed_new();
	gtk_setsize(fxBoxM, 450, 680);
	gtk_container_add(GTK_CONTAINER (frmBoxM), fxBoxM);

	//****** Nivel de la Caja   *frmNM, *fxNM, *evbNV;
	frmNM = gtk_frame_new(NULL);
	gtk_fixed_put (GTK_FIXED(fxBoxM), frmNM, 10,  10);
	gtk_frame_set_shadow_type (GTK_FRAME (frmNM), GTK_SHADOW_ETCHED_OUT);


	winDetalleCorte(fxBoxM, detMonedas);
	
	CalculaPesoTotal();

	v = total_monedas;
	v = (v * 100)/MertLmt.coin_capacity_stored;
	
	snprintf(porc, 30,"%4.1f%%\n%d/%d", v, total_monedas, MertLmt.coin_capacity_stored );

	int y;
	fxNM = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmNM), fxNM);
	gtk_widget_set_size_request(fxNM, ancho, altura);

	if(total_monedas> MertLmt.coin_capacity_stored)
		total_monedas = MertLmt.coin_capacity_stored;
	
	y = altura - ( (total_monedas*altura) / MertLmt.coin_capacity_stored);
	
	btn = gtk_event_box_new ();
	gtk_container_add(GTK_CONTAINER (fxNM), btn);
	gtk_fixed_move ( GTK_FIXED (fxNM), btn, 0, y);
	gtk_widget_set_size_request(btn, ancho, altura-y);
	gtk_widget_set_can_focus ( btn, FALSE);
	gtk_setcolorbg( btn, rgb_lime);

	lbl = CreateLabel ( fxNM, porc, (char *)"Sans", 700, 25, 0, altura/2-12);	
	gtk_setsize(lbl, ancho, 30);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_CENTER);

	//	
	btn = CreateButton (fxBoxM, (char *)"REALIZAR\n  CORTE", 150, 440);
	gtk_setfont(btn, "Sans", 700, 30);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnBoxM), GINT_TO_POINTER(1) );	


	//******************* Detalle de Caja
	lbl = CreateLabel ( fxBoxM, (char *)"CANTIDAD", (char *)"Sans", 700, 15, 70, 550);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);
	
	lbl = CreateLabel ( fxBoxM, (char *)"PESO", (char *)"Sans", 700, 15, 70, 580);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);
	
	lbl = CreateLabel ( fxBoxM, (char *)"NUMERO DE CORTE", (char *)"Sans", 700, 15, 70, 610);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);

	lbl = CreateLabel ( fxBoxM, (char *)"ID DE CAJA", (char *)"Sans", 700, 15, 70, 640);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);

	v = (detMonedas.CantidadTotal);
	v= v/100;
	sprintf(porc, "$  %6.2f",v);
	
	btn = CreateButtonLabel (fxBoxM, porc, 1.0, 260, 540);
	gtk_setsize(btn, 120, 20);
	gtk_setfont(btn, (char *)"Sans", 700, 15);
	gtk_widget_set_can_focus ( btn, FALSE);
	

	v = (total_peso); //  /100/1000;
	v= v/100000;
	sprintf(porc, "%6.3f Kg",v);
	btn = CreateButtonLabel (fxBoxM, porc, 1.0, 260, 570);
	gtk_setsize(btn, 120, 20);
	gtk_setfont(btn, (char *)"Sans", 700, 15);
	gtk_widget_set_can_focus ( btn, FALSE);

	sprintf(porc, "%06d", detMonedas.NoMov);
	btn = CreateButtonLabel (fxBoxM, porc, 1.0, 260, 600);
	gtk_setsize(btn, 120, 20);
	gtk_setfont(btn, (char *)"Sans", 700, 15);
	gtk_widget_set_can_focus ( btn, FALSE);

	sprintf(porc, "%u", detMonedas.CajaID);
	printf("ID CAJA MONEDAS=%u\n", detMonedas.CajaID);
	btn = CreateButtonLabel (fxBoxM, porc, 1.0, 260, 630);
	gtk_setsize(btn, 120, 20);
	gtk_setfont(btn, (char *)"Sans", 700, 15);
	gtk_widget_set_can_focus ( btn, FALSE);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winFrameBill(GtkWidget *fxRC)
{	GtkWidget *frmBoxB, *fxBoxB, *frmNB, *fxNB;
	GtkWidget *lbl, *btn;
	double v;
	char porc[30];
	int y;

	frmBoxB = gtk_frame_new("CAJA DE BILLETES");
	gtk_fixed_put (GTK_FIXED(fxRC), frmBoxB, 520,  20);
	gtk_setcolorbg(frmBoxB,  (char *)"gainsboro");
	gtk_setfont(frmBoxB, "SANS", 700, 15);

	fxBoxB = gtk_fixed_new();
	gtk_setsize(fxBoxB, 450, 650);
	gtk_container_add(GTK_CONTAINER (frmBoxB), fxBoxB);
	
	//****** Nivel de la Caja   *frmNM, *fxNM, *evbNV;
	frmNB = gtk_frame_new(NULL);
	gtk_fixed_put (GTK_FIXED(fxBoxB), frmNB, 10,  10);
	gtk_frame_set_shadow_type (GTK_FRAME (frmNB), GTK_SHADOW_ETCHED_OUT);

	winDetalleCorte(fxBoxB, detBilletes);

	CalculaBilletesTotales();

//	printf("Maximo= %d \n", MertLmt.bill_capacity_stored);	
	v = total_billetes;
	v = (v * 100)/MertLmt.bill_capacity_stored;
	
	snprintf(porc, 30,"%4.1f%%\n%d/%d", v, total_billetes, MertLmt.bill_capacity_stored );

	fxNB = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmNB), fxNB);
	gtk_widget_set_size_request(fxNB, ancho, altura);

	if(total_billetes> MertLmt.bill_capacity_stored)
		total_billetes = MertLmt.bill_capacity_stored;
	y = altura - ( (total_billetes*altura) / MertLmt.bill_capacity_stored);

	btn = gtk_event_box_new ();
	gtk_container_add(GTK_CONTAINER (fxNB), btn);
	gtk_fixed_move ( GTK_FIXED (fxNB), btn, 0, y);
	gtk_widget_set_size_request(btn, ancho, altura-y);
	gtk_widget_set_can_focus ( btn, FALSE);
	gtk_setcolorbg( btn, rgb_lime);

	lbl = CreateLabel ( fxNB, porc, (char *)"Sans", 700, 25, 0, altura/2-12);	
	gtk_setsize(lbl, ancho, 30);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_CENTER);

	btn = CreateButton (fxBoxB, (char *)"REALIZAR\n  CORTE", 150, 440);
	gtk_setfont(btn, "Sans", 700, 30);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnBoxM), GINT_TO_POINTER(2));	

	//******************* Detalle de Caja
	lbl = CreateLabel ( fxBoxB, (char *)"CANTIDAD", (char *)"Sans", 700, 15, 70, 550);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);

	lbl = CreateLabel ( fxBoxB, (char *)"NUMERO DE CORTE", (char *)"Sans", 700, 15, 70, 580);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);

	lbl = CreateLabel ( fxBoxB, (char *)"ID DE CAJA", (char *)"Sans", 700, 15, 70, 610);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);
	

	v = (detBilletes.CantidadTotal);
	v= v/100;
	sprintf(porc, "$  %6.2f",v);

	btn = CreateButtonLabel (fxBoxB, porc, 1.0, 260, 540);
	gtk_setsize(btn, 120, 20);
	gtk_setfont(btn, (char *)"Sans", 700, 15);
	gtk_widget_set_can_focus ( btn, FALSE);

	sprintf(porc, "%06d", detBilletes.NoMov);
	btn = CreateButtonLabel (fxBoxB, porc, 1.0, 260, 570);
	gtk_setsize(btn, 120, 20);
	gtk_setfont(btn, (char *)"Sans", 700, 15);
	gtk_widget_set_can_focus ( btn, FALSE);

	sprintf(porc, "%u", detBilletes.CajaID);
	btn = CreateButtonLabel (fxBoxB, porc, 1.0, 260, 600);
	gtk_setsize(btn, 120, 20);
	gtk_setfont(btn, (char *)"Sans", 700, 15);
	gtk_widget_set_can_focus ( btn, FALSE);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winRetiroCajas(void)
{	GtkWidget *fxRC, *btn;

	printf("RETIRO DE CAJAS DE RECAUDACION I/O\n");

	RetCajaMonedas = 0;
	RetCajaBilletes = 0;

	//****************** FRAME RETIRO DE CAJAS
	frmRC = gtk_frame_new("RETIRO DE CAJAS DE RECAUDACION");
	
	if(mert_famisa)
		gtk_fixed_put (GTK_FIXED(fxMantto), frmRC, 140,  70);
	else
		gtk_fixed_put (GTK_FIXED(fxMantto), frmRC, 0,  0);
		
	
	
	gtk_setcolorbg(frmRC,  (char *)"gainsboro");
	gtk_setfont(frmRC, "SANS", 700, 15);

	fxRC = gtk_fixed_new();
	gtk_setsize(fxRC, 1024, 800);
	gtk_container_add(GTK_CONTAINER (frmRC), fxRC);
	
	winFrameCoin(fxRC);

	winFrameBill(fxRC);

	corte_datos.caja = 0;

	//**************** Boton de Regreso
	btn = CreateButton (fxRC, (char *)"REGRESAR", 720, 700);
	gtk_setfont(btn, "SANS", 700, 40);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnRegRC), (gpointer *)"1");	

	gtk_widget_show_all (winmantto);
	gtk_widget_hide(msgMain);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
//static 
void btn_clicked(GtkWidget *button, gpointer *user_data)
{	gint index;
	char iptext[30],ll;
	GtkWidget *entAux;

	if(selEntry==1)
		entAux = entUser;
	else if(selEntry==2)
		entAux = entFicha;
	else  if(selEntry==3)
		entAux = entSello;
	else
		return;

	index = GPOINTER_TO_INT(user_data);
	printf("Index = %d\n", index);

	strcpy(iptext, gtk_entry_get_text(GTK_ENTRY(entAux)));
	ll = strlen(iptext);
	if( strcmp(letters[index],"B") == 0)
		iptext[ll-1] = '\0';		// borra un caracter
	else
		strcat(iptext, letters[index] );

	gtk_entry_set_text(GTK_ENTRY(entAux), iptext);
	gtk_editable_set_position(GTK_EDITABLE(entAux), -1);
	
	gtk_entry_grab_focus_without_selecting ( GTK_ENTRY(entAux) );
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void ent_clicked(GtkWidget *entry, gpointer *user_data)
{	
	selEntry= GPOINTER_TO_INT(user_data);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void ConfirmaCorte( char *msg)
{	int response;
	GtkDialogFlags flags = GTK_DIALOG_MODAL;	// | GTK_DIALOG_DESTROY_WITH_PARENT;
 
	GtkWidget* dialogo = gtk_message_dialog_new(
        NULL, flags, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
        "CONFIRMAR CORTE DE CAJA!");

    gtk_message_dialog_format_secondary_text(
        GTK_MESSAGE_DIALOG(dialogo),
        "%s", msg);

    response = gtk_dialog_run(GTK_DIALOG(dialogo));
	if(response == GTK_RESPONSE_OK)
	{	printf("CAJA=%d\n", corte_datos.caja);
		if(corte_datos.caja==1)
		{
			// actualizar Base de Datos de Monedas
			detMonedas.Usuario1 = atoi( gtk_entry_get_text(GTK_ENTRY(entUser))  );
			detMonedas.Usuario2 = atoi( gtk_entry_get_text(GTK_ENTRY(entUser))  );
			detMonedas.Ficha   = atoi( gtk_entry_get_text(GTK_ENTRY(entFicha))  );
			detMonedas.Sello   = atoi( gtk_entry_get_text(GTK_ENTRY(entSello))  );
			detMonedas.Status  = 2; // Cierra Caja
			strcpy(detMonedas.FechaCorte, datet(0));

			// imprime corte
			fillDatellaImp(detMonedas);

			impCaja(CAJA_MONEDAS, datImprimir);

			// libera caja
			stModIO->OnOFF     = R_PULSE;
			stModIO->tiempo    = 75;	// 7.5 segundos
			stModIO->NoControl = SOLENOIDE_CAJA_MONEDERO;	
			while(stModIO->NoControl !=0 );

			puerta.Buzzer(6);

			RetCajaMonedas  = 1;
		}
		else if(corte_datos.caja==2)
		{
			// actualizar Base de Datos de Monedas
			detBilletes.Usuario1 = atoi( gtk_entry_get_text(GTK_ENTRY(entUser))  );
			detBilletes.Usuario2 = atoi( gtk_entry_get_text(GTK_ENTRY(entUser))  );
			detBilletes.Ficha   = atoi( gtk_entry_get_text(GTK_ENTRY(entFicha))  );
			detBilletes.Sello   = atoi( gtk_entry_get_text(GTK_ENTRY(entSello))  );
			detBilletes.Status  = 2; // Cierra Caja
			strcpy(detBilletes.FechaCorte, datet(0));

			// imprime corte
			fillDatellaImp(detBilletes);

			impCaja(CAJA_BILLETES, datImprimir);

			puerta.Buzzer(6);

			RetCajaBilletes = 1;;
		}
	}
	gtk_widget_destroy(dialogo);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void BotonCorte(GtkWidget *entry, gpointer *user_data)
{	int opc, re;
	char msg[100];

	opc = GPOINTER_TO_INT(user_data);

	if( opc==1)
	{
		printf("REGRESAR=%d\n", opc);
		gtk_widget_destroy(frmCorte);
		fgCorte = 0;
		gtk_widget_show (frmRC);
		gtk_widget_set_sensitive (frmRC, TRUE);

		if( RetCajaMonedas==1)
		{	printf("Actualiza Caja Monedas\n");
			// crea xml
			xmlCorte(0, detMonedas);
			// write last registro
			printf("UPDATE BD\n");
			re = wrLastCorte(dbMonedas, detMonedas);

			// Espera caja nueva
			re = opc = 1;
			tbCajas = TM_CAJAS_2;
			idCajaMonedas=0;
			while(re)
			{
				sprintf(msg, "Caja de Monedas=[%u]", idCajaMonedas);
				if(waitBox( msg )== 0)
				{
					if(idCajaMonedas!= 0)
						re = 0;
					else
						opc = 0;
				}
				else
				{
					changeBox((char *)"Envia Pulso para liberar la Caja");
				}		
			}
			// Crea el registro del corte nuevo
			updateCorteMonedas();
			RetCajaMonedas = corte_datos.caja = 0;
			
			if(idCajaMonedas!=0)
				fgCajaMonedas=1;			
		}
		if(RetCajaBilletes==1)
		{	// genera xml
			xmlCorte(1, detBilletes);
			// write ultimo corte
			re = wrLastCorte(dbBilletes, detBilletes);

			// espera caja nueva
			re = opc = 1;
			tbCajas = TM_CAJAS_2;
			idCajaBilletes = 0;
			while(re)
			{
				sprintf(msg, "Caja de Billetes=[%u]", idCajaBilletes);
				if(waitBox( msg )== 0)
				{
					if(idCajaBilletes!= 0)
						re = 0;
					else
						opc = 0;
				}
			}
			// Crea el registro del corte nuevo		
			updateCorteBilletes();
			RetCajaBilletes = corte_datos.caja = 0;
			if(idCajaBilletes!=0)
				fgCajaBilletes=1;
		}
	}
	else if( opc==2)
	{
		printf("CONFIRMA=%d\n", opc);
		// validar que los entry's tengan informacion

		if( strlen( gtk_entry_get_text(GTK_ENTRY(entUser)) ) ==0)
		{	gtk_entry_grab_focus_without_selecting ( GTK_ENTRY(entUser) );
			return;
		}
		if( strlen( gtk_entry_get_text(GTK_ENTRY(entFicha)) ) ==0)
		{	gtk_entry_grab_focus_without_selecting ( GTK_ENTRY(entUser) );
			return;
		}
		if( strlen( gtk_entry_get_text(GTK_ENTRY(entSello)) ) ==0)
		{	gtk_entry_grab_focus_without_selecting ( GTK_ENTRY(entUser) );
			return;
		}

		corte_datos.usuario = atoi( gtk_entry_get_text(GTK_ENTRY(entUser))  );
		corte_datos.ficha   = atoi( gtk_entry_get_text(GTK_ENTRY(entFicha))  );
		corte_datos.sello   = atoi( gtk_entry_get_text(GTK_ENTRY(entSello))  );

		if(	corte_datos.caja == 1)
			ConfirmaCorte( (char *)"MONEDAS");
		else if( corte_datos.caja == 2)
			ConfirmaCorte( (char *)"BILLETES");
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winCorteKB( GtkWidget *fx)
{	GtkWidget *fxDatos, *myGrid, *lblEnter;
	int i,j;

	// Ventana para pedir el Password
	fxDatos = gtk_fixed_new();
	gtk_fixed_put (GTK_FIXED(fx), fxDatos, 450,  10);

	// arreglo de botones
	struct key k1;
	GArray *keyboard = g_array_new(FALSE, FALSE, sizeof(struct key));    
	for(i=0;i<TOTAL_TECLADO;i++)
	{	k1.id=i;
		k1.label = gtk_label_new( letters[i] );
		k1.button = gtk_button_new();
		gtk_container_add(GTK_CONTAINER (k1.button), k1.label);
		gtk_setsize(k1.button, 100, 80);
		gtk_setfont(k1.label, "Sans", 700, 30);
		g_array_append_val(keyboard, k1);
	}

	//A hash table to look up array index values.
	struct key *p1=NULL;
	GHashTable *hash_table=g_hash_table_new(NULL, NULL);
	for(i=0;i<TOTAL_TECLADO;i++)
	{
		p1 = &g_array_index(keyboard, struct key, i);
		g_hash_table_insert(hash_table, p1->button, &(p1->id));
	}
	myGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER (fxDatos), myGrid);

	for(i=0;i<4;i++)
	{	for(j=0;j<3;j++)
		{	p1= &g_array_index(keyboard, struct key, i*3+j);
			gtk_grid_attach(GTK_GRID(myGrid), p1->button, j, i+1, 1, 1);
			g_signal_connect(p1->button, "clicked", G_CALLBACK(btn_clicked), GINT_TO_POINTER(p1->id));
		}
	}
	gtk_widget_show_all (winmantto);
	gtk_widget_hide(msgMain);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void ledBox(char status)
{	char path[100];	
	GtkWidget *imgLed;

	if(status==0)
		sprintf(path, "%simagenes/led_rojo25.png",mert_operacion.path);
	else
		sprintf(path, "%simagenes/led_verde25.png",mert_operacion.path);

	imgLed = gtk_image_new_from_file (path);
	gtk_button_set_image (GTK_BUTTON (btnLed), imgLed);	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winCorteCaja(char *caja)
{	GtkWidget *fxCorte, *lbl;
	GtkWidget *btnCorte;
	char porc[20];

	//****************** FRAME RETIRO DE CAJAS
	frmCorte = gtk_frame_new(caja);
	
	if(mert_famisa)
		gtk_fixed_put (GTK_FIXED(fxMantto), frmCorte, 100,  100);
	else
		gtk_fixed_put (GTK_FIXED(fxMantto), frmCorte, 0,  0);
		

	gtk_setcolorbg(frmCorte,  (char *)"gainsboro");
	gtk_setfont(frmCorte, "SANS", 700, 15);

	fxCorte = gtk_fixed_new();
	gtk_setsize(fxCorte, 800, 600);
	gtk_container_add(GTK_CONTAINER (frmCorte), fxCorte);
	
	//** USUARIO
	lbl = CreateLabel ( fxCorte, (char *)"USUARIO", (char *)"Sans", 700, 35, 10, 60);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);

	//** FICHA
	lbl = CreateLabel ( fxCorte, (char *)"FICHA", (char *)"Sans", 700, 35, 10, 120);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);

	//** SELLO
	lbl = CreateLabel ( fxCorte, (char *)"SELLO", (char *)"Sans", 700, 35, 10, 180);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);
	
	lbl = CreateLabel ( fxCorte, (char *)"ID de Caja", (char *)"Sans", 700, 30, 10, 280);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_LEFT);

	// entry 1
	sprintf(porc,"%d", data_user[NOUSER_1].noempleado);
	entUser  = CreateEntry(fxCorte, porc, (char *)"Sans", 700,30, 200,50 );
//	gtk_editable_set_editable(GTK_EDITABLE(entUser), FALSE);		// para que no puedan editar
//	entUser  = CreateEntry(fxCorte, (char *)"", (char *)"Sans", 700,30, 200,50 );
	g_signal_connect(G_OBJECT(entUser), "grab-focus", G_CALLBACK(ent_clicked), GINT_TO_POINTER(1));


	entFicha = CreateEntry(fxCorte, (char *)"", (char *)"Sans", 700,30, 200,110 );
	g_signal_connect(G_OBJECT(entFicha), "grab-focus", G_CALLBACK(ent_clicked), GINT_TO_POINTER(2));

	entSello = CreateEntry(fxCorte, (char *)"", (char *)"Sans", 700,30, 200,170 );
	g_signal_connect(G_OBJECT(entSello), "grab-focus", G_CALLBACK(ent_clicked), GINT_TO_POINTER(3));

	if(corte_datos.caja == 1)
		sprintf(porc, "%u", detMonedas.CajaID);
	else
		sprintf(porc, "%u", detBilletes.CajaID);

	btnCaja = CreateButtonLabel (fxCorte, porc, 1.0, 250, 280);
	gtk_setsize(btnCaja, 120, 20);
	gtk_setfont(btnCaja, (char *)"Sans", 700, 15);
	gtk_widget_set_can_focus ( btnCaja, FALSE);

	char path_gris[100];
	sprintf(path_gris, "%simagenes/led_rojo25.png",mert_operacion.path);

	winCorteKB(fxCorte);

	// Boton Regresar
	btnCorte = CreateButton (fxCorte, (char *)"REGRESAR",50,420);
	gtk_setfont(btnCorte, "SANS", 700, 45);
	g_signal_connect ( btnCorte, "clicked",  G_CALLBACK (BotonCorte), GINT_TO_POINTER(1));	

	// Boton Confirmar
	btnCorte = CreateButton (fxCorte, (char *)"CONFIRMA\n  CORTE",420,400);
	gtk_setfont(btnCorte, "SANS", 700, 45);
	g_signal_connect ( btnCorte, "clicked",  G_CALLBACK (BotonCorte), GINT_TO_POINTER(2));

	gtk_widget_show_all (winmantto);
	gtk_widget_hide(msgMain);


	gtk_entry_grab_focus_without_selecting ( GTK_ENTRY(entFicha) );
	selEntry = 2;
	
//	gtk_entry_grab_focus_without_selecting ( GTK_ENTRY(entUser) );
//	selEntry = 1;

	fgCorte = 1;
}
////////////////////////////////////////////////////////////////////////////////