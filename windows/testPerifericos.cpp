/******************************************************************************/

#include "winmain.h"

/******************************************************************************/
/*               Ventanas para la prueba de los perifericos                   */
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
//static 
void btn_test (GtkWidget *button, gpointer *user_data)
{	gint index;

	index = GPOINTER_TO_INT(user_data);
	winMenuTest = index;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winTest(void)
{	GtkWidget *myGrid, *btnsalir;
	int i,j;

	// Ventana para pedir el Password
	fxTest = gtk_fixed_new();
	
	if(mert_famisa)
		gtk_fixed_put (GTK_FIXED(fxMantto), fxTest, xtest,  ytest);
	else
		gtk_fixed_put (GTK_FIXED(fxMantto), fxTest, xtest,  ytest-180);
		
	// arreglo de botones
	struct key k1;
	GArray *keyboard = g_array_new(FALSE, FALSE, sizeof(struct key));    

	for(i=0;i<TOTAL_TEST;i++)
	{
		k1.id=i;
		k1.button = gtk_button_new_with_label(opc_test[i]);
		gtk_setsize(k1.button, 80, 60);
		gtk_setfont(k1.button, "SANS", 700, 25);
		gtk_setcolorbg(k1.button,  (char *)"silver");
		g_array_append_val(keyboard, k1);
	}

	//A hash table to look up array index values.
	struct key *p1=NULL;
	GHashTable *hash_table=g_hash_table_new(NULL, NULL);
	for(i=0;i<TOTAL_TEST;i++)
	{
		p1 = &g_array_index(keyboard, struct key, i);
		g_hash_table_insert(hash_table, p1->button, &(p1->id));
	}
	myGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER (fxTest), myGrid);

	for(i=0;i < TOTAL_TEST; i++)
	{	p1= &g_array_index(keyboard, struct key, i);
		gtk_grid_attach(GTK_GRID(myGrid), p1->button, 0, i, 1, 1);
		g_signal_connect(p1->button, "clicked", G_CALLBACK(btn_test), GINT_TO_POINTER(p1->id));
	}
	gtk_widget_show(fxTest);
	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void exeTestPerifericos(void)
{	unsigned char Entradas[TOTAL_ENTRADAS];
	static char fgfirst = TRUE;

	switch(winMenuTest)
	{	case 0:	// Modulo de Entrada Salidas
				if(fgfirst == TRUE)
				{	gtk_widget_set_sensitive (fxTest, FALSE);
					windModES();		
					gtk_widget_hide(msgMain);
					gtk_update_input( moduloES.Entradas);
					memcpy( Entradas, moduloES.Entradas, TOTAL_ENTRADAS);
					fgfirst = FALSE;
				}
				else
				{
					if( memcmp( Entradas, moduloES.Entradas, TOTAL_ENTRADAS) != 0 )
					{
						gtk_update_input( moduloES.Entradas);
						memcpy( Entradas, moduloES.Entradas, TOTAL_ENTRADAS);
					}
				}
				break;

		case 1:	// BILLETERO
				if(fgfirst == TRUE)
				{	printf("PRUEBA DEL BILLETERO\n");
					gtk_widget_set_sensitive (fxTest, FALSE);
					winBill( bills );		
					gtk_widget_hide(msgMain);
					fgfirst = FALSE;
				}
				else
				{	
					// Inicia Barrido del Billetero
					AccionesBilletero( );
				}
				break;
		case 2:	//MONEDERO
				if(fgfirst == TRUE)
				{	printf("PRUEBA DEL MONEDERO\n");
					gtk_widget_set_sensitive (fxTest, FALSE);
					winCoin();		
					gtk_widget_hide(msgMain);
					fgfirst = FALSE;
				}
				else
				{	
					// Inicia Barrido del Monedero
					AccionesMonedero( );
				}
				break;

		case 3:	// DISPENSADOR
				if(fgfirst == TRUE)
				{	printf("PRUEBA DEL DISPENSADOR\n");
					gtk_widget_set_sensitive (fxTest, FALSE);
					winTestDispensador();
					gtk_widget_hide(msgMain);
					fgfirst = FALSE;
				}
				else
				{
					// Inicia Barrido del Monedero
					AccionesDispensador( );
				}
				break;

		case 4:	// IMPRESORA
				if(fgfirst == TRUE)
				{	printf("PRUEBA DE LA IMPRESORA\n");
					gtk_widget_set_sensitive (fxTest, FALSE);
					winPrinter();		
					gtk_widget_hide(msgMain);
					fgfirst = FALSE;
				}
				else
				{
					// Inicia Barrido del Monedero
					AccionesImpresora();
				}
				break;

		case 5:
				break;

		case 6:
				break;

		case 7:
				break;

		case 8:
			// SALIR
			fgfirst = TRUE;
			winMenuTest = 0xFF;
			gtk_widget_destroy(fxTest);
			mert_operacion.no_menu = MENU_REGRESO;
			break;


		case TEST_RETURN:
			// Regresa de alguna prueba
			gtk_widget_set_sensitive (fxTest, TRUE);
			winMenuTest = 0xFF;
			fgfirst = TRUE;
			break;		
	}
}
////////////////////////////////////////////////////////////////////////////////


