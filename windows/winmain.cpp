/******************************************************************************/
/*                                                                            */
/*                  RUTINAS PARA EL MANEJO DE LA PANTALLA                     */
/*                                                                            */
/******************************************************************************/
//
//
/******************************************************************************/
/* set font  & size & bold                                                    */
/******************************************************************************/
static void gtk_setfont (GtkWidget *widget, const char font[], int weight, int size)
{	GtkCssProvider *provider;
	gchar css[100];

	provider	= gtk_css_provider_new ();

	sprintf(css,"#my_font { font-family: %s; font-style: normal; font-weight: %d; "
					"font-size: %dpx;}", font, weight, size);

	gtk_css_provider_load_from_data(provider, css,-1,NULL);	
	gtk_style_context_add_provider (
									gtk_widget_get_style_context (widget),
									GTK_STYLE_PROVIDER (provider),
									GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	gtk_widget_set_name(widget, "my_font");

	g_object_unref (provider);
}

/******************************************************************************/
/* set background                                                             */
/******************************************************************************/
void gtk_setcolorbg(GtkWidget *widget,  GdkRGBA colorbg)
{	gchar			*css;
	gchar			buff[100],strs[10];
	GtkCssProvider *provider;

	snprintf(buff, 100, "* { background-color:rgba(%f, %f, %f, %f);}", 
							colorbg.red, colorbg.green, colorbg.blue, colorbg.alpha );

	css = g_strdup_printf ("%s",buff);
 
	provider = gtk_css_provider_new(); 
	gtk_css_provider_load_from_data(provider, css,-1,NULL); 
	g_free (css);

	GtkStyleContext * context = gtk_widget_get_style_context(widget);   
	gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

	g_object_unref (provider);
}

/******************************************************************************/
/* set background                                                             */
/******************************************************************************/
void gtk_setcolorbg(GtkWidget *widget,  char name[] )
{	gchar			*css;
	gchar			buff[100];
	GtkCssProvider *provider;

	snprintf(buff, 100, "* {background-color: %s; }", name);

	css = g_strdup_printf ("%s",buff);
 
	provider = gtk_css_provider_new(); 
	gtk_css_provider_load_from_data(provider, css,-1,NULL); 
	g_free (css);

	GtkStyleContext * context = gtk_widget_get_style_context(widget);   
	gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

	g_object_unref (provider);
}

/******************************************************************************/
/* set background                                                             */
/******************************************************************************/
void gtk_setcolorbtnbgx(GtkWidget *widget,  char name[])
{	gchar			*css;
	gchar			buff[100];
	GtkCssProvider *provider;

	snprintf(buff, 100, "* {background-color: black; font-size: 30px;}");
//	printf("css=[%s]\n", buff);
	css = g_strdup_printf ("%s",buff);
 
	provider = gtk_css_provider_new(); 

	gtk_css_provider_load_from_data(provider, css,-1,NULL); 
	g_free (css);

	GtkStyleContext * context = gtk_widget_get_style_context(widget);   
	gtk_style_context_add_provider            (context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	g_object_unref (provider);
}

/******************************************************************************/
/* set color                                                                  */
/******************************************************************************/
void gtk_setcolorfg(GtkWidget *widget,  GdkRGBA colorfg)
{	gchar			*css;
	gchar			buff[100],strs[10];
	GtkCssProvider	*provider;

	snprintf(buff, 100, "* { color:rgba(%f, %f, %f, %f);}", 
							colorfg.red, colorfg.green, colorfg.blue, colorfg.alpha );
	css = g_strdup_printf ("%s",buff);
 
	provider = gtk_css_provider_new(); 
	gtk_css_provider_load_from_data(provider, css,-1,NULL); 
	g_free (css);

	GtkStyleContext * context = gtk_widget_get_style_context(widget);   
	gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	g_object_unref (provider);
}

/******************************************************************************/
/*  GTK_JUSTIFY_LEFT GTK_JUSTIFY_RIGHT GTK_JUSTIFY_CENTER  GTK_JUSTIFY_FILL   */
/******************************************************************************/
void gtk_setjustify(GtkLabel *label, GtkJustification  jtype)
{
	gtk_label_set_justify(label, jtype);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void gtk_setsize(GtkWidget* widget, gint width, gint height )
{
	gtk_widget_set_size_request(widget, width, height);	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void gtk_hide(GtkWidget* widget)
{
	gtk_widget_hide( widget );
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void gtk_show(GtkWidget* widget)
{
	gtk_widget_show( widget );
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void gtk_removebuttonpadding(GtkWidget *widget)
{	gchar			*css;
	gchar			buff[100];
	GtkCssProvider	*provider;

	snprintf(buff, 100, "* { padding: 0; margin: 0; min-width: 0; min-height: 0;}");
	css = g_strdup_printf ("%s",buff);
 
	provider = gtk_css_provider_new(); 
	gtk_css_provider_load_from_data(provider, css,-1,NULL); 
	g_free (css);

	GtkStyleContext * context = gtk_widget_get_style_context(widget);   
	gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	g_object_unref (provider);
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winMsg( GtkWidget *lbl, GdkRGBA colorbg, GdkRGBA colorfg, char msg[], 
												const char font[], int w, int size)
{
	gtk_label_set_text( GTK_LABEL(lbl), msg);
	gtk_setfont (lbl, "Sans", w, size);
	gtk_setcolorbg(lbl, colorbg);
	gtk_setcolorfg(lbl, colorfg);
	gtk_widget_show(lbl);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int winMensajeWarning(char *msg1, char *msg2)
{	int response;

	GtkDialogFlags flags = GTK_DIALOG_MODAL;	// | GTK_DIALOG_DESTROY_WITH_PARENT;
 
	GtkWidget* dialogo = gtk_message_dialog_new(
        GTK_WINDOW(winmantto), flags, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
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
int winMensajeOK(char *msg1, char *msg2)
{	int response;

	GtkDialogFlags flags = GTK_DIALOG_MODAL;	// | GTK_DIALOG_DESTROY_WITH_PARENT;
 
	GtkWidget* dialogo = gtk_message_dialog_new(
        GTK_WINDOW(winmantto), flags, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
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
/* CreateRadio: Crea un RadioBUtton y lo agrega a un box                      */
/* box     - packing box                                                      */
/* group   - group to add the box to.                                         */
/* szLabel - Label to put by the radio button.                                */
/******************************************************************************/
GtkWidget *CreateRadio (GtkWidget *box, GSList **group, char *szLabel)
{	GtkWidget *radio;

	/* --- Get the radio button --- */
	radio = gtk_radio_button_new_with_label (*group, szLabel);
	*group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));

    /* --- Pack the radio button into the vertical box (box).  --- */
	gtk_box_pack_start (GTK_BOX (box), radio, TRUE, FALSE, 10);

    /* --- Show the widget --- */
    gtk_widget_show (radio);
    return (radio);
}

/******************************************************************************/
/* CreateButon: Crea un BUtton y lo agrega a un fixed                         */
/******************************************************************************/
GtkWidget *CreateButton (GtkWidget *fx, char *label, gint x, gint y)
{	GtkWidget *button;

	button = gtk_button_new_with_label (label);
	gtk_container_add(GTK_CONTAINER (fx), button);
	gtk_fixed_move ( GTK_FIXED (fx), button, x, y);

    gtk_widget_show (button);
    return (button);
}

/******************************************************************************/
/* CreateButon: Crea un BUtton y lo agrega a un fixed                         */
/******************************************************************************/
GtkWidget *CreateButtonLabel (GtkWidget *fx, char *label, gfloat xalign, gint x, gint y)
{	GtkWidget *button, *lbl;

	lbl = gtk_label_new( label );
	gtk_label_set_xalign ( GTK_LABEL(lbl), xalign);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER (button), lbl);

	gtk_container_add(GTK_CONTAINER (fx), button);
	gtk_fixed_move ( GTK_FIXED (fx), button, x, y);

    gtk_widget_show (button);
    return (button);
}

/******************************************************************************/
/* CreateButon: Crea un BUtton con Texto                                      */
/******************************************************************************/
GtkWidget *CreateButtonLabel (char *label, gfloat xalign, gfloat yalign)
{	GtkWidget *button, *lbl;

	lbl = gtk_label_new( label );
	gtk_label_set_xalign ( GTK_LABEL(lbl), xalign);
	gtk_label_set_yalign ( GTK_LABEL(lbl), yalign);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER (button), lbl);

    gtk_widget_show (button);
    return (button);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
GtkWidget  *CreateButtonImage(GtkWidget *fx, char msg[], 
												char *img, gint xpos, gint ypos)
{
	GtkWidget *image, *btn;

	image = gtk_image_new_from_file ( img );
	btn = gtk_button_new_with_label( msg);
	gtk_setfont(btn, "Ubuntu Mono", 700, 15);
	gtk_button_set_relief (GTK_BUTTON(btn), GTK_RELIEF_NONE);
//	gtk_setsize(btn, 30, 10);
	gtk_button_set_always_show_image (GTK_BUTTON (btn), TRUE);
	gtk_button_set_image (GTK_BUTTON (btn), image);
	gtk_container_add(GTK_CONTAINER (fx), btn);
	gtk_fixed_move ( GTK_FIXED (fx), btn, xpos, ypos);


	gtk_setcolorbg(btn,  (char *)"gainsboro");

	return (btn);
}

/******************************************************************************/
/* CreateButon: Crea un BUtton y lo agrega a un fixed                         */
/******************************************************************************/
GtkWidget *CreateLabel (GtkWidget *fx, char *msg, char *font, int weight, int size,
																gint x, gint y)
{	GtkWidget *label;

	label = gtk_label_new (msg);
	gtk_container_add(GTK_CONTAINER (fx), label);
	gtk_fixed_move ( GTK_FIXED (fx), label, x, y);
	gtk_setfont(label, font, weight, size);

    gtk_widget_show (label);
    return (label);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
GtkWidget *CreateEntry (GtkWidget *fx, char *msg, char *font, int weight, int size,
																gint x, gint y)
{	GtkWidget *entry;

	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 15);
	gtk_entry_set_text (GTK_ENTRY(entry), msg);
	gtk_setsize(entry, 140, 50);
	gtk_setfont(entry, font, weight, size);
	gtk_container_add(GTK_CONTAINER (fx), entry);
	gtk_fixed_move ( GTK_FIXED (fx), entry, x, y);
	gtk_widget_show (entry);

	return (entry);
}

/******************************************************************************/
/* Crea arreglo de botones, lo agregar a un gripd en filas y columnas         */
/* array = arreglo donde se agregaran los botones                             */
/* fxW  widget donde se agregara el arreglo de botones                        */
/* irow = rom inicial para agregar los botones                                */
/* rows / columns  filas y columnas del arreglo                               */
/******************************************************************************/
GArray *createArrayBtn(GtkWidget *fxW, const gchar **msg, gint irow, gint rows, gint columns,
						gint xpos, gint ypos)
{	GtkWidget *myGrid;
	GArray *array;
	int i,j,t;

	t = rows * columns;	// total de elementos

	// arreglo de botones
	struct key k1; 

	array = g_array_new(FALSE, FALSE, sizeof(struct key));    
	for(i=0; i<t; i++)
	{
		k1.id=i;
		k1.label = gtk_label_new( msg[i] );
		gtk_setfont(k1.label, "Sans", 700, 10);
		k1.button = gtk_button_new();

		gtk_container_add(GTK_CONTAINER (k1.button), k1.label);
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

	for(i=0; i<t; i++)
	{
		p1= &g_array_index(array, struct key, i);
		gtk_grid_attach(GTK_GRID(myGrid), p1->button, 0, i, 1, 1);
	}
	return array;
}

/******************************************************************************/
/* Crea arreglo de botones, lo agregar a un gripd en filas y columnas         */
/* array = arreglo donde se agregaran los botones                             */
/* fxW  widget donde se agregara el arreglo de botones                        */
/* irow = rom inicial para agregar los botones                                */
/* rows / columns  filas y columnas del arreglo                               */
/******************************************************************************/
GArray *createArrayBtnImg(GtkWidget *fxW, char msg[][10], char *img, gint irow, gint rows, gint columns,
						gint xpos, gint ypos)
{
	GtkWidget *myGrid, *image;
	GArray *array;
	int i,j,t;

	t = rows * columns;	// total de elementos

	// arreglo de botones
	struct key k1;	
	array = g_array_new(FALSE, FALSE, sizeof(struct key));;

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

	for(i=0; i<t; i++)
	{
		p1= &g_array_index(array, struct key, i);
		gtk_grid_attach(GTK_GRID(myGrid), p1->button, 0, i, 1, 1);
	}

	return array;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void updWin(void)
{
	while (gtk_events_pending ()) 
			gtk_main_iteration_do (TRUE);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void updWinTm(unsigned int tm)
{	unsigned int t0= milisegundos();

	while( (milisegundos() - t0) < tm)
		if (gtk_events_pending ())
			gtk_main_iteration_do (TRUE);
}

/******************************************************************************/
/*                    Pone mensajes en la ventana principal                   */
/******************************************************************************/
void msgwinmain(char nmsg)
{
	switch(nmsg)
	{	case 1:
		
				winMsg(msgMain, rgb_lime_green, rgb_black, (char *)"CARGANDO\n\nCONFIGURACION", 
						"Sans", 700,65);
				break;
		case 2:
				break;
	}
	updWin();
}

/******************************************************************************/
/* muestra hora/fecha en pantalla                                             */
/******************************************************************************/
char gtk_seconds_main(void)
{	static char seg=0;
	time_t TimeNow;
	struct tm ticket_time;
	char dfecha[30];

	time(&TimeNow);  // time actual
	ticket_time = *localtime(&TimeNow);

	if( seg != ticket_time.tm_sec )
	{
		strftime(dfecha, sizeof(dfecha),"%d/%m/%Y %H:%M:%S", &ticket_time);
		gtk_button_set_label( GTK_BUTTON(dateMain), dfecha );	
		seg = ticket_time.tm_sec;
	}
	return ticket_time.tm_min;
}

/******************************************************************************/
#include "testIO.cpp"
#include "testMonedero.cpp"
#include "testBilletero.cpp"
#include "testDispensador.cpp"
#include "testPrinter.cpp"
#include "testPerifericos.cpp"

#include "retirocajas.cpp"
#include "dispensador.cpp"

#include "libTarjeta.cpp"
#include "valCaja.cpp"

#include "winVentaRecarga.cpp"
//
//
/******************************************************************************/
//
//
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void btn_menu (GtkWidget *button, gpointer *user_data)
{	gint index;

	index = GPOINTER_TO_INT(user_data);
//	printf("Index = %d->%s\n", index, opc_menu[ index]);

	win_menu = index;	
	fgMenu = 1;	// Menu secundario

	switch(index)
	{
		case MENU_REIMPRIME_CORTES:		// 0
			fgMenu = 0x00;	// Menu Principal

			break;

		case MENU_CLIENTES:				// 1
			fgMenu = 0x00;	// Menu Principal
			break;

		case MENU_RETIRO_CAJAS:			// 2
			// Retiro Cajas de Recaudacion
			printf("Retiro de Cajas\n");
			gtk_widget_destroy(GTK_WIDGET (fxMenu));
			winRetiroCajas();
			gtk_widget_show_all (winmantto);
			gtk_widget_hide(msgMain);
			break;

		case MENU_DISPENSADOR:			// 3
			// Dispensador
			printf("Dispensador\n");
			gtk_widget_destroy(GTK_WIDGET (fxMenu));
			winDispensador();
			gtk_widget_show_all (winmantto);
			gtk_widget_hide(msgMain);
			break;

		case MENU_ESTADO:				// 4
			fgMenu = 0x00;	// Menu Principal
			break;

		case MENU_PRUEBA:				// 5
			// Test Perifericos
			printf("test perifericos\n");
			gtk_widget_destroy(GTK_WIDGET (fxMenu));
			winTest();
			gtk_widget_show_all (winmantto);
			gtk_widget_hide(msgMain);
			break;

		case MENU_LIBERA_TARJETA:		// 6
			// Libera Tarjeta
			printf("Libera Tarjeta\n");
			gtk_widget_destroy(GTK_WIDGET (fxMenu));
			winLibera();
			gtk_widget_show_all (winmantto);
			gtk_widget_hide(msgMain);
			break;

		case MENU_VALIDA_CAJAS:				// 7
			printf("vALIDA CAJAS\n");
			gtk_widget_destroy(GTK_WIDGET (fxMenu));
			winValCaja();
			gtk_widget_show_all (winmantto);
			gtk_widget_hide(msgMain);
			break;

		case MENU_FUTURO3:				// 8
				fgMenu = 0x00;	// Menu Principal
			break;

		case MENU_FUTURO4:				// 9
				fgMenu = 0x00;	// Menu Principal
			break;
			
	}
}

#define TOTAL_PERFILES	30
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winMenuCrea(unsigned char perfil)
{	GtkWidget *myGrid;
	int i,j, nmenu;

	// busca perfil
	for(i=0; i<TOTAL_PERFILES; i++)
	{	if( perfil == MenuProfile.profile[i])
		{
//			printf("Perfil Encontrado=%d %d\n", i, MenuProfile.menu[i]);
			break;
		}
	}

	nmenu = MenuProfile.menu[i];

	if(nmenu==0)
	{
//		printf("MENU NO ASIGNADO\n");
		return;
	}

	//
	fxMenu = gtk_fixed_new();
	gtk_fixed_put (GTK_FIXED(fxMantto), fxMenu, xmn1,  ymn1);
		
	// arreglo de botones
	struct key k1;
	GArray *keyboard = g_array_new(FALSE, FALSE, sizeof(struct key));    
	for(i=0; i<MenuType[nmenu-1].tot_reg; i++)
	{
		k1.id=MenuType[nmenu-1].opcion[i] - 1;	// i
		k1.label = gtk_label_new( opc_menu[ MenuType[nmenu-1].opcion[i] - 1 ]) ;
		k1.button = gtk_button_new();
		gtk_container_add(GTK_CONTAINER (k1.button), k1.label);
		gtk_setsize(k1.button, 100, 80);
		gtk_setfont(k1.label, "Sans", 700, 30);
		g_array_append_val(keyboard, k1);
	}

	//A hash table to look up array index values.
	struct key *p1=NULL;
	GHashTable *hash_table=g_hash_table_new(NULL, NULL);
	for(i=0; i<MenuType[nmenu-1].tot_reg; i++)
	{
		p1 = &g_array_index(keyboard, struct key, i);
		g_hash_table_insert(hash_table, p1->button, &(p1->id));
	}
	myGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER (fxMenu), myGrid);

	for(i=0; i<MenuType[nmenu-1].tot_reg; i++)
	{	p1= &g_array_index(keyboard, struct key, i);
		gtk_grid_attach(GTK_GRID(myGrid), p1->button, 0, i, 1, 1);
		g_signal_connect(p1->button, "clicked", G_CALLBACK(btn_menu), GINT_TO_POINTER(p1->id));
	}
	gtk_widget_show_all (winmantto);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void exeMenu(void)		// ejecuta opciones Menu 1
{
	switch(win_menu)
	{
		case MENU_REIMPRIME_CORTES:		// 0
			break;

		case MENU_CLIENTES:				// 1
			break;

		case MENU_RETIRO_CAJAS:			// 2
			break;

		case MENU_DISPENSADOR:			// 3
			break;

		case MENU_ESTADO:				// 4
			break;

		case MENU_PRUEBA:				// 5
			// Test Perifericos
			exeTestPerifericos();
			break;

		case MENU_LIBERA_TARJETA:		// 6
			exeLibera();
			break;

		case MENU_VALIDA_CAJAS:			// 7
			exeValBox();
			break;

		case MENU_FUTURO3:				// 8
			break;

		case MENU_FUTURO4:				// 9
			break;

		case WIN_MENU1_EXIT:			// 99
			// Regresar
//			printf("regresar\n");
			mert_operacion.no_menu= MODO_TARJETA;
			gtk_widget_destroy(GTK_WIDGET (fxMenu));
			win_menu = 0x100;
//			stModIO->StatusMantto = 0x00;
			fgMenu = 0x00;	// Menu Principal
			break;
			
	}
}

/******************************************************************************/
/******************************************************************************/
/*   WINDOW PASSWORD                                                          */
/******************************************************************************/
//
//
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
//static 
void button_clicked(GtkWidget *button, gpointer *user_data)
{	gint index;
	char iptext[30],ll;

	index = GPOINTER_TO_INT(user_data);

	strcpy(iptext, gtk_entry_get_text(GTK_ENTRY(entPass)));
	ll = strlen(iptext);
	if( strcmp(letters[index],"B") == 0)
		iptext[ll-1] = '\0';		// borra un caracter
	else
		strcat(iptext, letters[index] );

	gtk_entry_set_text(GTK_ENTRY(entPass), iptext);
	gtk_editable_set_position(GTK_EDITABLE(entPass), -1);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
//static 
void btn_passwd (GtkWidget *button, gpointer *user_data)
{	char iptext[30], pass[10];

	strcpy(pass, (char *)user_data);
//	printf("PASS = %s\n", pass);

	strcpy(iptext, gtk_entry_get_text(GTK_ENTRY(entPass)));
	if( strcmp(pass, iptext) == 0)
	{
		//printf("PASSWORD OK=[%s]\n", iptext);
		mert_operacion.password = TRUE;
	}

	if( strcmp(pwd_exit, iptext) == 0)
	{	// cerrar bases de datos y archivos, apagar luces
		// deshabilitar billetero y monedero
	//	gtk_quit();
		exit(0);
	}
	if( strcmp(pwd_off, iptext) == 0)
	{	// cerrar bases de datos, archivos, apagar luces
		// deshabilitar billetero y monedero
		system("shutdown -P now");
		while(1)
		{
//			printf("espera a apagarse\n");
			sleep(1);
		}
		exit(0);
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winPasswd(char pwd[])
{	GtkWidget *frmKB, *myGrid, *lblEnter, *btn;
	int i,j;

	// Ventana para pedir el Password
	fxPass = gtk_fixed_new();
	gtk_fixed_put (GTK_FIXED(fxMantto), fxPass, 100,  100);

	entPass = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entPass), 15);
	gtk_entry_set_text (GTK_ENTRY(entPass),"");
	gtk_setsize(entPass, 110, 90);
	gtk_setfont(entPass, "Sans", 700, 50);
	gtk_entry_set_visibility (GTK_ENTRY (entPass), 0);

	lblEnter = gtk_label_new("ENTER");
	btn = gtk_button_new();
	gtk_container_add(GTK_CONTAINER (btn), lblEnter);
	gtk_setsize(btn, 120, 100);

	gtk_setfont(lblEnter, "Sans", 700, 30);
	g_signal_connect(btn, "clicked", G_CALLBACK(btn_passwd), pwd);

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
	gtk_container_add(GTK_CONTAINER (fxPass), myGrid);

	gtk_grid_attach(GTK_GRID(myGrid), entPass, 0, 0, 3, 1);
	for(i=0;i<4;i++)
	{	for(j=0;j<3;j++)
		{	p1= &g_array_index(keyboard, struct key, i*3+j);
			gtk_grid_attach(GTK_GRID(myGrid), p1->button, j, i+1, 1, 1);
			g_signal_connect(p1->button, "clicked", G_CALLBACK(button_clicked), 
													GINT_TO_POINTER(p1->id));
		}
	}
	gtk_grid_attach(GTK_GRID(myGrid), btn, 0, i+1, 3, 1);

	gtk_widget_show_all (winmantto);
}

/******************************************************************************/
/*                         Muestra el siguiente menu                          */
/******************************************************************************/
void gtk_nextmenu(unsigned char profile, char op)
{
	winMenuCrea(profile);
	gtk_widget_hide(msgMain);
}

/******************************************************************************/
/*  Ventana principal de mantenimiento                                        */
/******************************************************************************/
void gtk_winmantto(void)
{	char dat[30];

	winmantto = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_decorated(GTK_WINDOW(winmantto), FALSE);

	if(mert_famisa)
		gtk_window_set_default_size(GTK_WINDOW(winmantto),1280,1024);
	else
		gtk_window_set_default_size(GTK_WINDOW(winmantto),1024,600);

	g_signal_connect(winmantto, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_move (GTK_WINDOW(winmantto),0,0);
	gtk_setcolorbg(winmantto, rgb_white_smoke);

	// fondo
	fxMantto = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (winmantto), fxMantto);

	
	
	if(mert_famisa)
		gtk_setsize(fxMantto,1280,1024);
	else
		gtk_setsize(fxMantto,1024,600);

	gtk_window_fullscreen(GTK_WINDOW(winmantto) );

	msgSuperior = CreateButton (fxMantto, (char *)"METRORREY",2,0);

	datetime(dat);

	if(mert_famisa)
	{
		gtk_setsize(msgSuperior, 878, 25);
		gtk_setfont(msgSuperior, "SANS", 700, 20);

		verMain = CreateButton (fxMantto, (char *)VERSION,881,0);
		gtk_setsize(verMain, 100, 25);
		gtk_setfont(verMain, "SANS", 700, 20);

		dateMain = CreateButton (fxMantto, dat,985,0);
		gtk_setsize(dateMain, 291, 25);
		gtk_setfont(dateMain, "SANS", 700, 20);
	}
	else
	{
		gtk_setsize(msgSuperior, 640, 25);
		gtk_setfont(msgSuperior, "SANS", 500, 20);

		verMain = CreateButton (fxMantto, (char *)VERSION,642,0);
		gtk_setsize(verMain, 100, 25);
		gtk_setfont(verMain, "SANS", 700, 20);

		dateMain = CreateButton (fxMantto, dat,732,0);
		gtk_setsize(dateMain, 291, 25);
		gtk_setfont(dateMain, "SANS", 500, 20);
	}

	gtk_widget_set_can_focus ( msgSuperior, FALSE);
	gtk_widget_set_can_focus ( verMain, FALSE);
	gtk_widget_set_can_focus ( dateMain, FALSE);

	if(mert_famisa)
	{	
		msgInferior = CreateButton (fxMantto, (char *)"ESTATUS",0,990);
		gtk_setsize(msgInferior, 1280, 25);
	}
	else
	{		
		msgInferior = CreateButton (fxMantto, (char *)"ESTATUS",0,732);
		gtk_setsize(msgInferior, 1024, 25);
	}
	gtk_setfont(msgInferior, "SANS", 700, 20);
	
	if(mert_famisa)
		msgMain = CreateLabel (fxMantto, (char *)"", (char *)"Sans", 700, 45, 200, 300);
	else
		msgMain = CreateLabel (fxMantto, (char *)"", (char *)"Sans", 700, 45, 100, 200);

	gtk_setsize(msgMain, 800, 400);
	gtk_setjustify(GTK_LABEL(msgMain), GTK_JUSTIFY_CENTER);

	gtk_widget_set_can_focus ( msgInferior, FALSE);

	gtk_widget_show_all (winmantto);
}

/******************************************************************************/
/*  Genera las Diferentes ventanas de la app                                  */
/******************************************************************************/
void wininicial(int argc, char** argv)
{
	gtk_init (&argc, &argv);

	gtk_winmantto();	// crea ventana de mantenimiento
	msgwinmain(1);

	gtk_winprocess();		// crea la ventana principal

	winMsgInicializando();	// mensaje de inicializacion
	winButtonMainHide();
	gtk_hide(fxSaldo);

	// desaperece el cursor en la venta de proceso
	//gtkSetCursor(GDK_WATCH);

	updWin();
}
////////////////////////////////////////////////////////////////////////////////