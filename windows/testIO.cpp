/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

//
void windModES(void);
void winCoin(void);

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
//static
void btn_modES (GtkWidget *button, gpointer *user_data)
{	gint index;

	index = GPOINTER_TO_INT(user_data);
	
	if(index== (MOTOR_DESATORAR-1) )
		return;

	while(stModIO->NoControl !=0 );
	stModIO->OnOFF     = relay_accion;
	stModIO->tiempo    = 5;
	stModIO->NoControl = index+1;

	printf(" OUTPUT = %d\n", index);
}

/******************************************************************************/
/* RadioEvent                                                                 */
/******************************************************************************/
void RadioEvent (GtkWidget *widget, gpointer *data)
{
	g_print ("radio event: %s\n", (char *)data);

	if( strcmp((char *)data,"1") == 0)
		relay_accion=0;
	else if( strcmp((char *)data,"2") == 0)
		relay_accion = 1;
	else
		relay_accion = 2;
}

/******************************************************************************/
/*  Ventana de Controles                                                      */
/******************************************************************************/
void winOutput( GtkWidget *fx)
{	GtkWidget *myGrid, *frmMES, *vbox, *radio;
	GSList *group = NULL;
	int i,j;

	// FRAME
	frmMES = gtk_frame_new("SALIDAS");
	gtk_fixed_put (GTK_FIXED(fx), frmMES, 10,10);
	gtk_setsize(frmMES, 300, 740);
	gtk_setcolorbg(frmMES, colorIO);
	gtk_setfont(frmMES, fontIO, 700, 15);

	// Ventana para cmd de relay
	fxModOutput = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmMES), fxModOutput);

	// Operacion del Relay Open/Close/Pulso
	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	radio = CreateRadio (vbox, &group, (char *)"OPEN");
	g_signal_connect ( radio, "clicked",  G_CALLBACK (RadioEvent), (gpointer *)"1");
	radio = CreateRadio (vbox, &group, (char *)"CLOSE");
	g_signal_connect ( radio, "clicked",  G_CALLBACK (RadioEvent), (gpointer *)"2");
	radio = CreateRadio (vbox, &group, (char *)"PULSO");
	g_signal_connect ( radio, "clicked",  G_CALLBACK (RadioEvent), (gpointer *)"3");

	gtk_container_add (GTK_CONTAINER (fxModOutput), vbox);
	gtk_fixed_move ( GTK_FIXED (fxModOutput), vbox, 10, 300);

	// arreglo de botones
	struct key k1;
	GArray *keyboard = g_array_new(FALSE, FALSE, sizeof(struct key));    
	for(i=0;i<TOTAL_OUTPUT;i++)
	{
		k1.id=i;
//		k1.button = gtk_button_new_with_label(mod_output[i]);
		k1.button  = CreateButtonLabel ( (char *)mod_output[i], 0.5, 0.5);

		gtk_setsize(k1.button, 80, 80);
		gtk_setfont(k1.button, fontIO, 700, 30);
		gtk_setcolorbg(k1.button,  colorIO);
		g_array_append_val(keyboard, k1);
	}

	//A hash table to look up array index values.
	struct key *p1=NULL;
	GHashTable *hash_table=g_hash_table_new(NULL, NULL);
	for(i=0; i<TOTAL_OUTPUT; i++)
	{
		p1 = &g_array_index(keyboard, struct key, i);
		g_hash_table_insert(hash_table, p1->button, &(p1->id));
	}
	myGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER (fxModOutput), myGrid);
	gtk_grid_set_row_spacing( GTK_GRID(myGrid), 5);
	gtk_grid_set_column_spacing( GTK_GRID(myGrid), 5);

	for(i=0; i<TOTAL_OUTPUT; i++)
	{
		p1= &g_array_index(keyboard, struct key, i);
		gtk_grid_attach(GTK_GRID(myGrid), p1->button, 0, i, 1, 1);
		g_signal_connect(p1->button, "clicked", G_CALLBACK(btn_modES), GINT_TO_POINTER(p1->id));
	}
	gtk_fixed_move ( GTK_FIXED (fxModOutput), myGrid, 100, 20);
}

/******************************************************************************/
/*  Ventana de Entradas                                                       */
/******************************************************************************/
void winInput(GtkWidget *fx)
{	GtkWidget *myGrid, *frmMES;
	int i,j;

	char path_gris[100];
	sprintf(path_gris, "%simagenes/led_gris25.png",mert_operacion.path);

	// FRAME
	frmMES = gtk_frame_new("ENTRADAS");
	gtk_fixed_put (GTK_FIXED(fx), frmMES, 370,  10);
	gtk_setsize(frmMES, 200, 400);
	gtk_setcolorbg(frmMES,  colorIO);
	gtk_setfont(frmMES, fontIO, 700, 15);

	//
	fxModOutput = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmMES), fxModOutput);

	// arreglo de botones
	struct key k1;
	arrInput = g_array_new(FALSE, FALSE, sizeof(struct key));;
	for(i=0;i<TOTAL_OUTPUT;i++)
	{
		k1.image = gtk_image_new_from_file (path_gris);
		k1.id=i;
		k1.button = gtk_button_new_with_label(mod_input[i]);
		gtk_setfont(k1.button, fontIO, 700 ,15);

		gtk_button_set_relief (GTK_BUTTON(k1.button), GTK_RELIEF_NONE);
		gtk_button_set_always_show_image (GTK_BUTTON (k1.button), TRUE);
		gtk_button_set_image (GTK_BUTTON (k1.button), k1.image);
		gtk_setcolorbg(k1.button,  colorIO);
		g_array_append_val(arrInput, k1);
	}

	//A hash table to look up array index values.
	struct key *p1=NULL;
	GHashTable *hash_table=g_hash_table_new(NULL, NULL);
	for(i=0; i<TOTAL_INPUT; i++)
	{
		p1 = &g_array_index(arrInput, struct key, i);
		g_hash_table_insert(hash_table, p1->button, &(p1->id));
	}
	myGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER (fxModOutput), myGrid);
	gtk_grid_set_row_spacing( GTK_GRID(myGrid), 5);

	for(i=0; i<TOTAL_INPUT; i++)
	{
		p1= &g_array_index(arrInput, struct key, i);
		gtk_grid_attach(GTK_GRID(myGrid), p1->button, 0, i, 1, 1);
	}

	gtk_fixed_move ( GTK_FIXED (fxModOutput), myGrid, 20, 20);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void gtk_update_input(unsigned char input[])
{	int i;
	char path_gris[100];
	char path_verde[100];

	sprintf(path_gris, "%simagenes/led_gris25.png",mert_operacion.path);
	sprintf(path_verde, "%simagenes/led_verde25.png",mert_operacion.path);

	struct key *p1=NULL;

	for(i=0; i<TOTAL_ENTRADAS; i++)
	{
		p1= &g_array_index(arrInput, struct key, i);
		if(input[i] == BOTON_OPRIMIDO_SI)
		{
			p1->image = gtk_image_new_from_file (path_verde);
			gtk_button_set_image (GTK_BUTTON (p1->button), p1->image);
		}
		else
		{
			p1->image = gtk_image_new_from_file (path_gris);
			gtk_button_set_image (GTK_BUTTON (p1->button), p1->image);
		}
	}
}

/******************************************************************************/
/* Regresa de la prueba                                                       */
/******************************************************************************/
void BtnRegresa (GtkWidget *widget, gpointer *data)
{
	winMenuTest = TEST_RETURN;
	gtk_widget_destroy(frmModIO);
}

/******************************************************************************/
/* Prueba Destrabador                                                         */
/******************************************************************************/
void BtnDestrabador (GtkWidget *widget, gpointer *data)
{
	posiciona_destrabador();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void windModES(void)
{	GtkWidget *fxModIO, *btnRtn;

	printf("MODULO I/O\n");

	// FRAME
	frmModIO = gtk_frame_new("Modulo de Entradas/Salidas");
	if(mert_famisa)
		gtk_fixed_put (GTK_FIXED(fxMantto), frmModIO, xout,  yout);
	else
		gtk_fixed_put (GTK_FIXED(fxMantto), frmModIO, 50, 0);
		
		
	gtk_setcolorbg(frmModIO, colorIO);
	gtk_setfont(frmModIO, fontIO, 700,15);

	fxModIO = gtk_fixed_new();
	gtk_setsize(fxModIO, 700, 800);
	gtk_container_add(GTK_CONTAINER (frmModIO), fxModIO);

	winOutput(fxModIO);
	winInput(fxModIO);

	// Boton Prueba Destrabador
	btnRtn = CreateButton (fxModIO, (char *)"  PRUEBA\nDESTRABADOR", 350, 450);
	gtk_setfont(btnRtn, fontIO,700, 40);
	g_signal_connect ( btnRtn, "clicked",  G_CALLBACK (BtnDestrabador), (gpointer *)"1");	

	// Boton de Return
	btnRtn = CreateButton (fxModIO, (char *)"REGRESAR", 400, 660);
	gtk_setfont(btnRtn, fontIO,700, 40);
	g_signal_connect ( btnRtn, "clicked",  G_CALLBACK (BtnRegresa), (gpointer *)"1");	

	gtk_widget_show_all (winmantto);
}
////////////////////////////////////////////////////////////////////////////////
