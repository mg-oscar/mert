//
//
//
#include "dispensador.h"
//
void winDispensador(void);

/******************************************************************************/
/* Regresa de Retiro de Cajas                                                 */
/******************************************************************************/
void BtnRegDisp (GtkWidget *widget, gpointer *data)
{
	gtk_widget_destroy(frmDispensador);
	mert_operacion.no_menu = MENU_REGRESO;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int ConfirmaOperacion( int nstk, char *msg1, char *msg2)
{	int response;
	GtkDialogFlags flags = GTK_DIALOG_MODAL;	// | GTK_DIALOG_DESTROY_WITH_PARENT;
 
	GtkWidget* dialogo = gtk_message_dialog_new(
        NULL, flags, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
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
/* Grabar Stacker Activo                                                      */
/******************************************************************************/
void BtnSaveStk (GtkWidget *widget, gpointer *data)
{	char msg[100];
	gint index;

	if( Config.Stacker == index)
	{
		// mismo dato, no graba
		return;	
	}

	index = gtk_combo_box_get_active( GTK_COMBO_BOX(cmbDisp) );
//	printf("STACKER%d\n", index);

	sprintf(msg, "Stacker Activo =  Stacker%d", index);

	if( ConfirmaOperacion(index, (char *)"CONFIRMAR STACKER ACTIVO" ,msg) == 0)
	{		
		Config.Stacker = index;
//		printf("Stacker=%d\n", Config.Stacker);
		UpdateConfig();
	}
}


/******************************************************************************/
/* Grabar Vacia Bandeja de Captura                                            */
/******************************************************************************/
void BtnVaciar (GtkWidget *widget, gpointer *data)
{	char msg[100];

	gint index;

	if( Config.Captura == 0)
	{
		// no hay tarjetas en bandeja
		return;	
	}

	sprintf(msg, "Retirar %d Tarjetas", Config.Captura);

	if( ConfirmaOperacion(index, (char *)"CONFIRMAR VACIAR BANDEJA" ,msg) == 0)
	{		

		ImprimeCaptura();

		Config.Captura = 0;
		Config.NoMov++;
		UpdateConfig();

		xmlCaptura( );

		gtk_widget_destroy(frmDispensador);
		winDispensador();
	}
}

/******************************************************************************/
/* Accion: Agrega Tarjetas				                                      */
/******************************************************************************/
void BtnStkAgrega (GtkWidget *widget, gpointer *data)
{	int opc, cant;
	char msg[100];

	opc = GPOINTER_TO_INT(data);

	cant = atoi( gtk_entry_get_text(GTK_ENTRY( entDisp[opc*2] ) ));
	if(cant<=0)
		return;

	sprintf(msg, "Agregar %d Tarjetas al Stacker%d", cant, opc+1);

	if( ConfirmaOperacion(opc, (char *)"CONFIRMAR AGREGAR TARJETAS" ,msg) == 0)
	{
		detStackers[opc].TipoOperacion  = 1;		// Carga Tarjetas
		detStackers[opc].TotalOperacion = cant;
		detStackers[opc].TotalTarjetas += cant;
		detStackers[opc].NoEmpleado     = data_user[NOUSER_1].noempleado;
		strcpy(detStackers[opc].UID_MIA, "");

		UpdateStacker(opc);
		xmlStacker(opc, detStackers[opc] );
		ImprimeStacker(opc, detStackers[opc]);

		gtk_widget_destroy(frmDispensador);
		winDispensador();
	}
}

/******************************************************************************/
/* Accion: Retira Tarjetas                                                    */
/******************************************************************************/
void BtnStkRetira (GtkWidget *widget, gpointer *data)
{	int opc, cant;
	char msg[100];

	opc = GPOINTER_TO_INT(data);

	cant = atoi( gtk_entry_get_text(GTK_ENTRY( entDisp[opc*2+1] ) ));
	if(cant<=0)
		return;

	sprintf(msg, "Retirar %d Tarjetas del Stacker%d", cant, opc+1);

	if( ConfirmaOperacion(opc, (char *)"CONFIRMAR RETIRAR TARJETAS" ,msg) == 0)
	{
		detStackers[opc].TipoOperacion  = 2;		// Retira Tarjetas
		detStackers[opc].TotalOperacion = cant;
		detStackers[opc].TotalTarjetas -= cant;
		detStackers[opc].NoEmpleado     = data_user[NOUSER_1].noempleado;
		strcpy(detStackers[opc].UID_MIA, "");

		UpdateStacker(opc);
		xmlStacker(opc, detStackers[opc] );
		ImprimeStacker(opc, detStackers[opc]);

		gtk_widget_destroy(frmDispensador);
		winDispensador();
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void ent_add(GtkWidget *entry, gpointer *user_data)
{
	dispEntry= GPOINTER_TO_INT(user_data);
//	printf("dispEntry=%d\n", dispEntry);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
//static 
void btn_clickDisp(GtkWidget *button, gpointer *user_data)
{	gint index;
	char iptext[30],ll;
	GtkWidget *entAux;

	entAux = entDisp[dispEntry];

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
void winKB_Disp( GtkWidget *fx, gint xpos, gint ypos)
{	GtkWidget *fxDatos, *myGrid;
	int i,j;

	void (*fun_ptr)(GtkWidget *, gpointer *) = &btn_clickDisp;

	// Ventana para pedir el Password
	fxDatos = gtk_fixed_new();
	gtk_fixed_put (GTK_FIXED(fx), fxDatos, xpos,  ypos);

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
			g_signal_connect(p1->button, "clicked", G_CALLBACK(fun_ptr), 
													GINT_TO_POINTER(p1->id));
		}
	}
	gtk_widget_show_all (winmantto);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winNivelMagazine(GtkWidget *fx, GtkWidget *frm, int nstk)
{	GtkWidget *fxNStk;
	GtkWidget *lbl, *btn;
	char porc[30];
	double v;
	int v1;

	//****** Nivel del Magazine   *frmNM, *fxNM, *evbNV;
	frm = gtk_frame_new(NULL);
	gtk_fixed_put (GTK_FIXED(fx), frm, 10,  10);
	gtk_frame_set_shadow_type (GTK_FRAME (frm), GTK_SHADOW_ETCHED_OUT);

//	printf("Maximo= %d \n", MertLmt.capacity_dispenser);
	if(nstk==MAGAZINE1)	
		v1 = detStackers[MAGAZINE1].TotalTarjetas;
	else
		v1 = detStackers[MAGAZINE2].TotalTarjetas;

	v = ((double)v1 * 100)/MertLmt.capacity_dispenser;

	snprintf(porc, 30,"%4.1f%%\n%d/%d", v, v1, MertLmt.capacity_dispenser );

	int y;
	fxNStk = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frm), fxNStk);
	gtk_widget_set_size_request(fxNStk, d_ancho, d_altura);

	if(v1> MertLmt.capacity_dispenser)
		v1 = MertLmt.capacity_dispenser;	
	y = d_altura - ( (v1*d_altura) / MertLmt.capacity_dispenser);

	btn = gtk_event_box_new ();
	gtk_container_add(GTK_CONTAINER (fxNStk), btn);
	gtk_fixed_move ( GTK_FIXED (fxNStk), btn, 0, y);
	gtk_widget_set_size_request(btn, d_ancho, d_altura-y);
	gtk_widget_set_can_focus ( btn, FALSE);
	gtk_setcolorbg( btn, rgb_lime);

	lbl = CreateLabel ( fxNStk, porc, (char *)"Sans", 700, 25, 0, d_altura/2-12);	
	gtk_setsize(lbl, d_ancho, 30);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_CENTER);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winFrameDisp(GtkWidget *fxD, int nstk, int xpos, int ypos)
{	GtkWidget *frmStk, *fxStk, *frmNStk, *fxNStk;
	GtkWidget *lbl, *btn;
	double v;
	int v1;
	char porc[30];

	//*********************** Frame Stcaker
	if(nstk==MAGAZINE1)
		frmStk = gtk_frame_new("MAGAZINE 1");

	gtk_fixed_put (GTK_FIXED(fxD), frmStk, xpos,  ypos);

	gtk_setcolorbg(frmStk,  (char *)"gainsboro");
	gtk_setfont(frmStk, "SANS", 700, 15);

	fxStk = gtk_fixed_new();
	gtk_setsize(fxStk, 300, 530);
	gtk_container_add(GTK_CONTAINER (frmStk), fxStk);

	/**************************************************************************/
	//****** Nivel del MAGAZINE   *frmNM, *fxNM, *evbNV;
	frmNStk = gtk_frame_new(NULL);
	gtk_fixed_put (GTK_FIXED(fxStk), frmNStk, 10,  10);
	gtk_frame_set_shadow_type (GTK_FRAME (frmNStk), GTK_SHADOW_ETCHED_OUT);

	if(nstk==MAGAZINE1)
		v1 = detStackers[MAGAZINE1].TotalTarjetas;

	v = ((double)v1 * 100)/MertLmt.capacity_dispenser;

	snprintf(porc, 30,"%4.1f%%\n%d/%d", v, v1, MertLmt.capacity_dispenser );

	int y;
	fxNStk = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmNStk), fxNStk);
	gtk_widget_set_size_request(fxNStk, d_ancho, d_altura);

	if(v1> MertLmt.capacity_dispenser)
		v1 = MertLmt.capacity_dispenser;	
	y = d_altura - ( (v1*d_altura) / MertLmt.capacity_dispenser);

	btn = gtk_event_box_new ();
	gtk_container_add(GTK_CONTAINER (fxNStk), btn);
	gtk_fixed_move ( GTK_FIXED (fxNStk), btn, 0, y);
	gtk_widget_set_size_request(btn, d_ancho, d_altura-y);
	gtk_widget_set_can_focus ( btn, FALSE);
	gtk_setcolorbg( btn, rgb_lime);

	lbl = CreateLabel ( fxNStk, porc, (char *)"Sans", 700, 25, 0, d_altura/2-12);	
	gtk_setsize(lbl, d_ancho, 30);
	gtk_setjustify(GTK_LABEL(lbl), GTK_JUSTIFY_CENTER);
	//**************************************************************************

	//	
	btn = CreateButton (fxStk, (char *)" AGREGA\nTARJETAS", 10, 350);
	gtk_setfont(btn, "Sans", 700, 23);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnStkAgrega), GINT_TO_POINTER(nstk) );	

	btn = CreateButton (fxStk, (char *)" RETIRA\nTARJETAS", 10, 440);
	gtk_setfont(btn, "Sans", 700, 23);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnStkRetira), GINT_TO_POINTER(nstk) );	

	// Agregar tarjetas
	entDisp[2*nstk]  = CreateEntry(fxStk, (char *)"", (char *)"Sans", 700, 30, 190,355 );
	gtk_entry_set_width_chars (GTK_ENTRY(entDisp[2*nstk]), 4);
	gtk_entry_set_max_length(GTK_ENTRY(entDisp[2*nstk]), 3);
	gtk_entry_set_alignment ( GTK_ENTRY(entDisp[2*nstk]), 1);
	g_signal_connect(G_OBJECT(entDisp[2*nstk]), "grab-focus", G_CALLBACK(ent_add), 
										GINT_TO_POINTER(2*nstk));

	// Retirar tarjetas
	entDisp[2*nstk+1]  = CreateEntry(fxStk, (char *)"", (char *)"Sans", 700, 30, 190,435 );
	gtk_entry_set_width_chars (GTK_ENTRY(entDisp[2*nstk+1]), 4);
	gtk_entry_set_max_length(GTK_ENTRY(entDisp[2*nstk+1]), 3);
	gtk_entry_set_alignment ( GTK_ENTRY(entDisp[2*nstk+1]), 1);
	g_signal_connect(G_OBJECT(entDisp[2*nstk+1]), "grab-focus", G_CALLBACK(ent_add), 
										GINT_TO_POINTER(2*nstk+1));
	
	printf("11111\n");
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winDispensador_f(void)
{	GtkWidget *fxDisp, *btn, *lbl;
	GtkWidget *frmAct, *fxAct;
	GtkWidget *frmBin, *fxBin;
	char card[10];

	//****************** FRAME DISPENSADOR *************************************
	frmDispensador = gtk_frame_new("DISPENSADOR");
	
	gtk_fixed_put (GTK_FIXED(fxMantto), frmDispensador, 100,  90);

		
	gtk_setcolorbg(frmDispensador,  (char *)"gainsboro");
	gtk_setfont(frmDispensador, "SANS", 700, 15);

	fxDisp = gtk_fixed_new();
	gtk_setsize(fxDisp, 1100, 800);
	gtk_container_add(GTK_CONTAINER (frmDispensador), fxDisp);

	winFrameDisp(fxDisp, MAGAZINE1,  20, 20);
	//(fxDisp, MAGAZINE2, 410, 20);
	
	winKB_Disp( fxDisp, 770, 30);
	gtk_entry_grab_focus_without_selecting ( GTK_ENTRY(entDisp[0]) );
	dispEntry=0;


	// **************  Stacker Activo *******************************************
	frmAct = gtk_frame_new("MAGAZINE ACTIVO");

	gtk_fixed_put (GTK_FIXED(fxDisp), frmAct, 20,  620);
	gtk_setcolorbg(frmAct,  (char *)"gainsboro");
	gtk_setfont(frmAct, "SANS", 700, 15);

	fxAct = gtk_fixed_new();
	gtk_setsize(fxAct, 350, 60);
	gtk_container_add(GTK_CONTAINER (frmAct), fxAct);

	//cmbDisp = gtk_combo_box_text_new();
	//gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cmbDisp), NULL, "Ninguno  ");
	//gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cmbDisp), NULL, "Magazine 1");
	//gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cmbDisp), NULL, "Magazine 2");
	//gtk_combo_box_set_active(GTK_COMBO_BOX(cmbDisp), Config.Stacker);
	//gtk_container_add(GTK_CONTAINER (fxAct), cmbDisp);
	//gtk_fixed_move ( GTK_FIXED (fxAct), cmbDisp, 10, 10);

	// Grabar Stacker Activo
	//btn = CreateButton (fxAct, (char *)"Seleccionar", 180, 10);
	//gtk_setfont(btn, "SANS", 700, 20);
	//g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnSaveStk), (gpointer *)"1");	
	//**************************************************************************

	// **************  Vaciar Bandeja de Captura *******************************
	frmBin = gtk_frame_new("Bandeja de Captura");

	gtk_fixed_put (GTK_FIXED(fxDisp), frmBin, 420,  620);
	gtk_setcolorbg(frmBin,  (char *)"gainsboro");
	gtk_setfont(frmBin, "SANS", 700, 15);

	fxBin = gtk_fixed_new();
	gtk_setsize(fxBin, 300, 60);
	gtk_container_add(GTK_CONTAINER (frmBin), fxBin);

	sprintf(card, "%d", Config.Captura);
	btn = CreateButtonLabel (fxBin, card, 1.0, 10,10);
	gtk_setsize(btn, 120, 20);
	gtk_setfont(btn, (char *)"Sans", 700, 15);
	gtk_widget_set_can_focus ( btn, FALSE);

	btn = CreateButton (fxBin, (char *)"Vaciar", 180, 10);
	gtk_setfont(btn, "SANS", 700, 20);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnVaciar), (gpointer *)"1");	
	// *************************************************************************
	
	//**************** Boton de Regreso ****************************************
	btn = CreateButton (fxDisp, (char *)"REGRESAR", 770, 720);
		
	gtk_setfont(btn, "SANS", 700, 40);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnRegDisp), (gpointer *)"1");	

	gtk_widget_show_all (winmantto);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winDispensador_d(void)
{	GtkWidget *fxDisp, *btn, *lbl;
	GtkWidget *frmAct, *fxAct;
	GtkWidget *frmBin, *fxBin;
	char card[10];

	//****************** FRAME DISPENSADOR *************************************
	frmDispensador = gtk_frame_new("DISPENSADOR");
	
	gtk_fixed_put (GTK_FIXED(fxMantto), frmDispensador, 0, 0);
	
	gtk_setcolorbg(frmDispensador,  (char *)"gainsboro");
	gtk_setfont(frmDispensador, "SANS", 700, 15);

	fxDisp = gtk_fixed_new();
	gtk_setsize(fxDisp, 1100, 800);
	gtk_container_add(GTK_CONTAINER (frmDispensador), fxDisp);

	winFrameDisp(fxDisp, MAGAZINE1,  20, 20);
	//winFrameDisp(fxDisp, MAGAZINE2, 370, 20);
	winKB_Disp( fxDisp, 720, 30);

	gtk_entry_grab_focus_without_selecting ( GTK_ENTRY(entDisp[0]) );
	dispEntry=0;

	// **************  Stacker Activo *******************************************
	frmAct = gtk_frame_new("MAGAZINE ACTIVO");

	gtk_fixed_put (GTK_FIXED(fxDisp), frmAct, 20,  620);
	gtk_setcolorbg(frmAct,  (char *)"gainsboro");
	gtk_setfont(frmAct, "SANS", 700, 15);

	fxAct = gtk_fixed_new();
	gtk_setsize(fxAct, 350, 60);
	gtk_container_add(GTK_CONTAINER (frmAct), fxAct);

	cmbDisp = gtk_combo_box_text_new();
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cmbDisp), NULL, "Ninguno  ");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cmbDisp), NULL, "Magazine 1");
	//gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cmbDisp), NULL, "Magazine 2");
	//gtk_combo_box_set_active(GTK_COMBO_BOX(cmbDisp), Config.Stacker);
	gtk_container_add(GTK_CONTAINER (fxAct), cmbDisp);
	gtk_fixed_move ( GTK_FIXED (fxAct), cmbDisp, 10, 10);

	// Grabar Stacker Activo
	btn = CreateButton (fxAct, (char *)"Seleccionar", 180, 10);
	gtk_setfont(btn, "SANS", 700, 20);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnSaveStk), (gpointer *)"1");	
	//**************************************************************************

	// **************  Vaciar Bandeja de Captura *******************************
	frmBin = gtk_frame_new("Bandeja de Captura");

	gtk_fixed_put (GTK_FIXED(fxDisp), frmBin, 420,  620);
	gtk_setcolorbg(frmBin,  (char *)"gainsboro");
	gtk_setfont(frmBin, "SANS", 700, 15);

	fxBin = gtk_fixed_new();
	gtk_setsize(fxBin, 300, 60);
	gtk_container_add(GTK_CONTAINER (frmBin), fxBin);

	sprintf(card, "%d", Config.Captura);
	btn = CreateButtonLabel (fxBin, card, 1.0, 10,10);
	gtk_setsize(btn, 120, 20);
	gtk_setfont(btn, (char *)"Sans", 700, 15);
	gtk_widget_set_can_focus ( btn, FALSE);

	btn = CreateButton (fxBin, (char *)"Vaciar", 180, 10);
	gtk_setfont(btn, "SANS", 700, 20);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnVaciar), (gpointer *)"1");	
	// *************************************************************************
	
	//**************** Boton de Regreso ****************************************
	btn = CreateButton (fxDisp, (char *)"REGRESAR", 740, 680);
		
	gtk_setfont(btn, "SANS", 700, 40);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnRegDisp), (gpointer *)"1");	

	gtk_widget_show_all (winmantto);
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winDispensador(void)
{	
	if(mert_famisa)
		winDispensador_f();
	else
		winDispensador_d();
}
////////////////////////////////////////////////////////////////////////////////