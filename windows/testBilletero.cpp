/******************************************************************************/
/*               PROCEDIMIENTOS DE PRUEBA DEL BILLETERO                       */
/******************************************************************************/
//
#include "winmain.h"
//
/******************************************************************************/
/* RadioEvent                                                                 */
/******************************************************************************/
void ButtonEventBill (GtkWidget *widget, gpointer *data)
{
	stModIO->AccionBilletero = atoi((char *)data);
}

/******************************************************************************/
/* Regresa de la prueba del billetero                                         */
/******************************************************************************/
void BtnRegresar (GtkWidget *widget, gpointer *data)
{
	//
	bills.fg_master_inhibit = 0;
	while(stModIO->cmdBill !=0 );
	bills.SendResetDevice();
	while(stModIO->cmdBill !=0 );
//	sleep(10);
	
	testBill=0;
	// deshabilita Billetero
	stModIO->AccionBilletero = 3;

	winMenuTest = TEST_RETURN;
	gtk_widget_destroy(frmBill);
	
	stModIO->cmdBill = CMD_BILL_DISABLE;
	while(stModIO->cmdBill !=0 );

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void InitDatosBill(GtkWidget *fx)
{	// ************* 
	GtkWidget* frmInit = gtk_frame_new("DATOS BILLETERO");
	gtk_fixed_put ( GTK_FIXED (fx), frmInit, 15, 580);
	gtk_setsize(frmInit, 350, 125);

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
		bills.cctalkID.cctalk_id,		// Equipment ID
		bills.cctalkID.cctalk_mid,		// Manufacturer ID
		bills.cctalkID.cctalk_pc,		// Product Code
		bills.cctalkID.cctalk_bc,		// Build Code
		bills.cctalkID.cctalk_sr,		// 
		bills.cctalkID.cctalk_sn,		// numero de serie
		bills.cctalkID.cctalk_pp);		// polling priority

	gtk_text_buffer_set_text ( bufInit, tmptxt, strlen(tmptxt));
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void billLed(int noled)
{	int tot;
	char label[20];
	
	struct key *p1=NULL;
	p1= &g_array_index(arrBillTotal, struct key, noled);

	tot = atoi (gtk_label_get_label( GTK_LABEL(p1->label) ) );
	tot++;
	snprintf(label, 10, "%d",tot);
	gtk_label_set_label( GTK_LABEL(p1->label), label);
	
	p1= &g_array_index(arrBillLeds, struct key, noled);

	p1->image = gtk_image_new_from_file (Led_Verde15);
	gtk_button_set_image (GTK_BUTTON (p1->button), p1->image);
	gtk_widget_show_all (winmantto);
	gtk_widget_hide(msgMain);

	unsigned int tm = milisegundos();
	while( (milisegundos() - tm) < 500)
		updWin();

	p1->image = gtk_image_new_from_file (Led_Gris15);
	gtk_button_set_image (GTK_BUTTON (p1->button), p1->image);

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void billLedEscrow(int noled, char img[])
{	int i, tot;
//	char label[20];

	tot = g_array_get_element_size  (arrBillLeds);
	
	struct key *p1=NULL;

	p1= &g_array_index(arrBillLeds, struct key, noled);
	p1->image = gtk_image_new_from_file (img);
	gtk_button_set_image (GTK_BUTTON (p1->button), p1->image);

	gtk_widget_show(p1->button);
	gtk_widget_show_all (winmantto);
	gtk_widget_hide(msgMain);

	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void AccionesBilletero(void)
{	static int lapso=milisegundos();
	int ne, ivalor, k;
	char cvalor[10];
	static int ipos=0;

	if( (milisegundos() - lapso) >= 300)
	{
		lapso = milisegundos();

		switch(stModIO->AccionBilletero)
		{
			case 0:
				
				if(mert_famisa)
				{
					// consulta billetero
					if( (bills.Posicion>0) && (bills.Path == BILLETE_EN_CAJA) )
					{
						ipos   = bills.Posicion;
						ivalor = bills.Value(ipos);

						snprintf(cvalor, 10, "%4.2f", double(ivalor/100));

						billLedEscrow(ipos, Led_Gris15);

						bills.Posicion=0;	// elimina evento
						break;
					}

					if( (bills.Posicion>0) && (bills.Path == BILLETE_EN_ESCROW) )
					{
						ipos   = bills.Posicion;
						ivalor = bills.Value(ipos);

						snprintf(cvalor, 10, "%4.2f", double(ivalor/100));
						gtk_button_set_label( GTK_BUTTON(lblUDBill), cvalor);
						billLedEscrow(ipos, Led_Rojo15);
						bills.Posicion=0;	// elimina evento
						break;
					}
				}
				else
				{
					// consulta billetero
					if( (bills.Posicion>0) && (bills.Path == BILLETE_EN_CAJA) )
					{
						ipos   = bills.Posicion;
						ivalor = bills.Value(ipos);

						snprintf(cvalor, 10, "%4.2f", double(ivalor/100));

						billLedEscrow(ipos, Led_Gris15);

						bills.Posicion=0;	// elimina evento
						break;
					}

					if( (bills.Posicion>0) && (bills.Path == BILLETE_EN_ESCROW) )
					{
						ipos   = bills.Posicion;
						ivalor = bills.Value(ipos);

						snprintf(cvalor, 10, "%4.2f", double(ivalor/100));
						gtk_button_set_label( GTK_BUTTON(lblUDBill), cvalor);
						billLedEscrow(ipos, Led_Rojo15);
						bills.Posicion=0;	// elimina evento
						break;
					}
				}
				break;

			case 1:		
					stModIO->AccionBilletero = 0;
					break;

			case 2:	// Habilita Billetero
					stModIO->cmdBill = CMD_BILL_ENABLE;	
					while(stModIO->cmdBill !=0 );
					stModIO->AccionBilletero = 0;
					break;

			case 3:
					stModIO->cmdBill = CMD_BILL_DISABLE;	
					while(stModIO->cmdBill !=0 );
					stModIO->AccionBilletero = 0;
					break;

			case 4:
					stModIO->cmdBill = CMD_BILL_RETURN;
					while(stModIO->cmdBill !=0 );
					billLedEscrow(ipos, Led_Gris15);
					ipos = 0;
					stModIO->AccionBilletero = 0;
					break;

			case 5:
					stModIO->cmdBill = CMD_BILL_ACCEPT;	
					while(stModIO->cmdBill !=0 );
					stModIO->AccionBilletero = 0;
										billLedEscrow(ipos, Led_Gris15);

					break;

			default:		
					stModIO->AccionBilletero = 0;
					break;
		}
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winBill_f(cctalk &bill)
{	GtkWidget	*radio, *frmRad;
	GtkWidget	*lblfactor, *lblUD;
	GtkWidget	*fxBill;		// billetero
	int i;
	char cvf[20];
	char scala[10];

	// FRAME
	frmBill = gtk_frame_new("BILLETERO");
	if(mert_famisa)
	{	gtk_fixed_put (GTK_FIXED(fxMantto), frmBill, xbill,  ybill);
		gtk_setsize(frmBill, 800, 750);
	}
	else
	{	gtk_fixed_put (GTK_FIXED(fxMantto), frmBill, 0,  0);
		gtk_setsize(frmBill, 1024, 750);
	}

	gtk_setcolorbg(frmBill,  (char *)"gainsboro");
	gtk_setfont(frmBill, "SANS", 700, 15);

	//
	fxBill = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmBill), fxBill);

	// Botones de accion
	frmRad = gtk_frame_new("ACCION");
	gtk_fixed_put ( GTK_FIXED (fxBill), frmRad, 10, 10);
	GtkWidget *fxB = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmRad), fxB);
	gtk_setsize(fxB, 402, 470);

	radio = CreateButton (fxB, (char *)"INICIALIZA",10,10);
	gtk_setfont(radio, "SANS", 700, 35);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventBill), (gpointer *)"1");	

	radio = CreateButton (fxB, (char *)"HABILITA",10,100);
	gtk_setfont(radio, "SANS", 700, 35);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventBill), (gpointer *)"2");

	radio = CreateButton (fxB, (char *)"DESHABILITA",10,200);
	gtk_setfont(radio, "SANS", 700, 35);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventBill), (gpointer *)"3");

	radio = CreateButton (fxB, (char *)"REGRESA BILLETE",10,300);
	gtk_setfont(radio, "SANS", 700, 35);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventBill), (gpointer *)"4");

	radio = CreateButton (fxB, (char *)"BILLETE A CAJA",10,400);
	gtk_setfont(radio, "SANS", 700, 35);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventBill), (gpointer *)"5");	

	//
	if(mert_famisa)
		arrBillCvt =   createArrayBtn( fxBill, den_bill, 0, TOTAL_BILLS, 1,  440, 10);
	else
	{
		arrBillCvt =   createArrayBtn( fxBill, den_bill, 0, TOTAL_BILLS, 1,  430, 10);
	}

	struct key *p1=NULL;
	for(i=0; i<bills.TotalDenominaciones; i++)
	{
		sprintf( cvf,"%s", bills.depositado.cvf[i]);
		p1= &g_array_index(arrBillCvt, struct key, i);
		gtk_label_set_label (GTK_LABEL(p1->label) , cvf);
	}

	if(mert_famisa)
		arrBillTotal = createArrayBtn( fxBill, tot_bill, 0, TOTAL_BILLS, 1, 570, 10);
	else
		arrBillTotal = createArrayBtn( fxBill, tot_bill, 0, TOTAL_BILLS, 1, 510, 10);

	for(i=0; i<TOTAL_BILLS; i++)
	{
		p1 = &g_array_index(arrBillTotal, struct key, i);
		gtk_setsize( p1->label, 40, 10);
		gtk_label_set_xalign (GTK_LABEL(p1->label), 1.0);
	}

	char txtC[TOTAL_DENOMINACION_MAX][10];
	memset(txtC, 0x00, sizeof(txtC));
	for(i=0; i<bills.TotalDenominaciones; i++)
	{
		snprintf(txtC[i],10," $%6.2f", (double)bills.depositado.valor[i]/100);
	}
	if(mert_famisa)
		arrBillLeds	= createArrayBtnImg( fxBill, txtC, Led_Gris15, 0, TOTAL_BILLS, 1, 670, 6);
	else
		arrBillLeds	= createArrayBtnImg( fxBill, txtC, Led_Gris15, 0, TOTAL_BILLS, 1, 580, 6);

	InitDatosBill(fxBill);

	// scala
	if(mert_famisa)
	{
		lblfactor = CreateLabel (fxBill, (char *)"Scala", (char *)"SANS", 700, 15, 455, 580);
		radio = CreateButton (fxBill, scala,520,574);
	}
	else
	{
		lblfactor = CreateLabel (fxBill, (char *)"Scala", (char *)"SANS", 700, 15, 740, 335);
		radio = CreateButton (fxBill, scala,800,330);
	}
	gtk_setfont(radio, "SANS", 700, 15);

	// Divisor
	snprintf(scala,10,"%d", bill.cctalk_divisor);
	if(mert_famisa)
	{
		lblfactor = CreateLabel (fxBill, (char *)"Divisor", (char *)"SANS", 700,15, 455, 504);
		radio = CreateButton (fxBill, scala,520,500);
	}
	else
	{
		lblfactor = CreateLabel (fxBill, (char *)"Divisor", (char *)"SANS", 700,15, 700, 450);
		radio = CreateButton (fxBill, scala,800,454);
	}
	gtk_setfont(radio, "SANS", 700, 15);

	// Ulino Deposito
	if(mert_famisa)
	{
		lblUD = CreateLabel (fxBill, (char *)"Ult.Dep.", (char *)"SANS", 700, 15, 600, 580);
		lblUDBill = CreateButton (fxBill, (char *)" 0.00",680,574);
	}
	else
	{
		lblUD = CreateLabel (fxBill, (char *)"Ult.Dep.", (char *)"SANS", 700, 15, 700, 580);
		lblUDBill = CreateButton (fxBill, (char *)" 0.00",800,574);
	}
	gtk_setfont(radio, "SANS", 700, 15);

	// REGRESAR
	if(mert_famisa)
		radio = CreateButton (fxBill, (char *)"REGRESAR",460,660);
	else
		radio = CreateButton (fxBill, (char *)"REGRESAR",700,660);	
	gtk_setfont(radio, "SANS", 700, 45);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (BtnRegresar), (gpointer *)"1");	


	gtk_widget_show_all (winmantto);
	gtk_widget_hide(msgMain);
	updWin();

	testBill = 1;
}

/******************************************************************************/
/* Crea arreglo de botones, lo agregar a un gripd en filas y columnas         */
/* array = arreglo donde se agregaran los botones                             */
/* fxW  widget donde se agregara el arreglo de botones                        */
/* irow = rom inicial para agregar los botones                                */
/* rows / columns  filas y columnas del arreglo                               */
/******************************************************************************/
GArray *createArrayBtnImg2(GtkWidget *fxW, GArray *array,char msg[][10], char *img, gint irow, gint rows, gint columns,
						gint xpos, gint ypos, gint ind)
{
	GtkWidget *myGrid, *image;
//	GArray *array;
	int i,j,t;

	t = rows * columns;	// total de elementos

	// arreglo de botones
	struct key k1;
//	array = g_array_new(FALSE, FALSE, sizeof(struct key));

	for(i=0; i<t; i++)
	{
		k1.image = gtk_image_new_from_file ( img );
		k1.id=i;
		k1.button = gtk_button_new_with_label( msg[i]);
		gtk_setfont(k1.button, "Ubuntu Mono", 700, 15);
		gtk_button_set_relief (GTK_BUTTON(k1.button), GTK_RELIEF_NONE);
		gtk_setsize(k1.button, 30, 10);
		gtk_button_set_always_show_image (GTK_BUTTON (k1.button), TRUE);
		gtk_button_set_image (GTK_BUTTON (k1.button), k1.image);
		gtk_setcolorbg(k1.button,  (char *)"gainsboro");
		g_array_append_val(array, k1);
	}

	//A hash table to look up array index values.
	struct key *p1=NULL;
	GHashTable *hash_table=g_hash_table_new(NULL, NULL);
	for(i=0; i<t; i++)
	{
		p1 = &g_array_index(array, struct key, i);
		g_hash_table_insert(hash_table, p1->button, &(p1->id));
	}

	myGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER (fxW), myGrid);
	gtk_fixed_move ( GTK_FIXED (fxW), myGrid, xpos, ypos);

	gtk_grid_set_row_spacing( GTK_GRID(myGrid), 1);

	for(i=16; i<t+16; i++)
	{
		p1 = &g_array_index(array, struct key, i);
		gtk_grid_attach(GTK_GRID(myGrid), p1->button, 0, i, 1, 1);
	}

	return array;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winBill_d(cctalk &bill)
{	GtkWidget	*radio, *frmRad;
	GtkWidget	*lblfactor, *lblUD;
	GtkWidget	*fxBill;		// billetero
	int i,td;
	char cvf[20];
	char scala[10];

	// FRAME
	frmBill = gtk_frame_new("BILLETERO");
	
	gtk_fixed_put (GTK_FIXED(fxMantto), frmBill, 0,  0);
	gtk_setsize(frmBill, 1024, 800);

	gtk_setcolorbg(frmBill,  (char *)"gainsboro");
	gtk_setfont(frmBill, "SANS", 700, 15);

	//
	fxBill = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmBill), fxBill);

	// Botones de accion
	frmRad = gtk_frame_new("ACCION");
	gtk_fixed_put ( GTK_FIXED (fxBill), frmRad, 10, 10);
	GtkWidget *fxB = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmRad), fxB);
	gtk_setsize(fxB, 402, 470);

	radio = CreateButton (fxB, (char *)"INICIALIZA",10,10);
	gtk_setfont(radio, "SANS", 700, 35);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventBill), (gpointer *)"1");	

	radio = CreateButton (fxB, (char *)"HABILITA",10,100);
	gtk_setfont(radio, "SANS", 700, 35);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventBill), (gpointer *)"2");

	radio = CreateButton (fxB, (char *)"DESHABILITA",10,200);
	gtk_setfont(radio, "SANS", 700, 35);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventBill), (gpointer *)"3");

	radio = CreateButton (fxB, (char *)"REGRESA BILLETE",10,300);
	gtk_setfont(radio, "SANS", 700, 35);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventBill), (gpointer *)"4");

	radio = CreateButton (fxB, (char *)"BILLETE A CAJA",10,400);
	gtk_setfont(radio, "SANS", 700, 35);
	gtk_setsize(radio, 382,60);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (ButtonEventBill), (gpointer *)"5");	

	//************************ BILLETERO CANALES *******************************
	td = bills.TotalDenominaciones;
	if(bills.TotalDenominaciones>=16)
		td = 16;

	// primeros 16 canales
	arrBillCvt =   createArrayBtn( fxBill, den_bill, 0, td, 1,  430, 10);

	struct key *p1=NULL;
	for(i=0; i<td; i++)
	{
		sprintf( cvf,"%d-%s", i, bills.depositado.cvf[i]);
		p1= &g_array_index(arrBillCvt, struct key, i);
		gtk_label_set_label (GTK_LABEL(p1->label) , cvf);
	}

	arrBillTotal = createArrayBtn( fxBill, tot_bill, 0, td, 1, 510, 10);

	for(i=0; i<td; i++)
	{
		p1 = &g_array_index(arrBillTotal, struct key, i);
		gtk_setsize( p1->label, 40, 10);
		gtk_label_set_xalign (GTK_LABEL(p1->label), 1.0);
	}

	char txtC[TOTAL_DENOMINACION_MAX][10];
	memset(txtC, 0x00, sizeof(txtC));
	for(i=0; i<td; i++)
	{
		snprintf(txtC[i],10," $%6.2f", (double)bills.depositado.valor[i]/100);
	}

	arrBillLeds	= createArrayBtnImg( fxBill, txtC, Led_Gris15, 0, td, 1, 570, 6);


	// segundos 16 canales
	
	if(bills.TotalDenominaciones>16)
	{
		td = bills.TotalDenominaciones -16;

		arrBillCvt2 =   createArrayBtn( fxBill, den_bill, 0, td, 1,  710, 10);

		struct key *p1=NULL;
		for(i=0; i<td; i++)
		{
			sprintf( cvf,"%d-%s", i+16, bills.depositado.cvf[i+16]);
			p1= &g_array_index(arrBillCvt2, struct key, i);
			gtk_label_set_label (GTK_LABEL(p1->label) , cvf);
		}

		arrBillTotal2 = createArrayBtn( fxBill, tot_bill, 0, td, 1, 795, 10);

		for(i=0; i<td; i++)
		{
			p1 = &g_array_index(arrBillTotal2, struct key, i);
			gtk_setsize( p1->label, 40, 10);
			gtk_label_set_xalign (GTK_LABEL(p1->label), 1.0);
		}

	char txtC[TOTAL_DENOMINACION_MAX][10];
	memset(txtC, 0x00, sizeof(txtC));
	for(i=0; i<td; i++)
	{
		snprintf(txtC[i],10," $%6.2f", (double)bills.depositado.valor[i+16]/100);
	}
	arrBillLeds	= createArrayBtnImg2( fxBill, arrBillLeds, txtC, Led_Gris15, 0, td, 1, 855, 6,16);

	}
	//**************************************************************************

	InitDatosBill(fxBill);

	// scala
	lblfactor = CreateLabel (fxBill, (char *)"Scala", (char *)"SANS", 700, 15, 450, 700);
	radio = CreateButton (fxBill, scala,530,690);

	gtk_setfont(radio, "SANS", 700, 15);

	// Divisor
	snprintf(scala,10,"%d", bill.cctalk_divisor);
	
	lblfactor = CreateLabel (fxBill, (char *)"Divisor", (char *)"SANS", 700,15, 450, 650);
	radio = CreateButton (fxBill, scala,530,640);
	gtk_setfont(radio, "SANS", 700, 15);

	// Ulino Deposito
	lblUD = CreateLabel (fxBill, (char *)"Ult.Dep.", (char *)"SANS", 700, 15, 700, 600);
	lblUDBill = CreateButton (fxBill, (char *)" 0.00",800,594);
	gtk_setfont(radio, "SANS", 700, 15);

	// REGRESAR
	radio = CreateButton (fxBill, (char *)"REGRESAR",700,660);	
	gtk_setfont(radio, "SANS", 700, 45);
	g_signal_connect ( radio, "clicked",  G_CALLBACK (BtnRegresar), (gpointer *)"1");	


	gtk_widget_show_all (winmantto);
	gtk_widget_hide(msgMain);
	updWin();

	testBill = 1;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winBill(cctalk &bill)
{
	if(mert_famisa)
		winBill_f(bill);		// famisa
	else
		winBill_d(bill);		// dte
}
////////////////////////////////////////////////////////////////////////////////