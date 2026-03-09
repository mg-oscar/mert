//
// Rutinas para prueba de Impresora
//
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
//
//
/******************************************************************************/
/* Regresa de la prueba                                                       */
/******************************************************************************/
void BtnPrinter (GtkWidget *widget, gpointer *data)
{
	printf("IMPRIME\n");
	testEncabezado();
}

/******************************************************************************/
/* Regresa de la prueba                                                       */
/******************************************************************************/
void BtnRetPrinter (GtkWidget *widget, gpointer *data)
{
	winMenuTest = TEST_RETURN;
	gtk_widget_destroy(frmPrinter);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winPrinter(void)
{	GtkWidget *fxPrinter, *btnRtn, *btnPrt;;

	printf("IMPRESORA\n");

	// FRAME
	frmPrinter = gtk_frame_new("Prueba de Impresora");

	if(mert_famisa)
		gtk_fixed_put (GTK_FIXED(fxMantto), frmPrinter, 50,  100);
	else
		gtk_fixed_put (GTK_FIXED(fxMantto), frmPrinter, 50,  0);

	gtk_setcolorbg(frmPrinter, colorPrt);
	gtk_setfont(frmPrinter, fontPrt, 700,15);

	fxPrinter = gtk_fixed_new();
	gtk_setsize(fxPrinter, 700, 800);
	gtk_container_add(GTK_CONTAINER (frmPrinter), fxPrinter);


	// Boton de Imprimir
	btnPrt = CreateButton (fxPrinter, (char *)"IMPRIMIR", 400, 400);
	gtk_setfont(btnPrt, fontPrt,700, 40);
	g_signal_connect ( btnPrt, "clicked",  G_CALLBACK (BtnPrinter), (gpointer *)"1");	

	// Boton de Return
	btnRtn = CreateButton (fxPrinter, (char *)"REGRESAR", 400, 600);
	gtk_setfont(btnRtn, fontPrt,700, 40);
	g_signal_connect ( btnRtn, "clicked",  G_CALLBACK (BtnRetPrinter), (gpointer *)"1");	

	gtk_widget_show_all (winmantto);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void AccionesImpresora(void)
{
	
}
////////////////////////////////////////////////////////////////////////////////
