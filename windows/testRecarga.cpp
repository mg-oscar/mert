/******************************************************************************/
/*               PROCEDIMIENTOS DE PRUEBA DEL DISPENSADOR                       */
/******************************************************************************/
//
#include "winmain.h"
//
/******************************************************************************/
/* RadioEvent                                                                 */
/******************************************************************************/
void ButtonEventDisp (GtkWidget *widget, gpointer *data)
{
//	g_print ("radio event: %s\n", (char *)data);

	stModIO->AccionDispensador = atoi((char *)data);

}

/******************************************************************************/
/* Regresa de la prueba del dispensador                                       */
/******************************************************************************/
void BtnRegresarDisp (GtkWidget *widget, gpointer *data)
{
//	printf("Regresar DISPENSADOR\n");

	winMenuTest = TEST_RETURN;

	gtk_widget_destroy(frmTDisp);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void ledDisp(GtkWidget *btn, char pled[], char msg[])
{	GtkWidget *image;

	image = gtk_image_new_from_file (pled);
	gtk_button_set_label ( GTK_BUTTON(btn), msg );	
	gtk_button_set_image (GTK_BUTTON (btn), image);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void StatusStacker(int nstk, char op)
{	GtkWidget *image;
	char msg[50];

	switch(op)
	{
		case 0:
				ledDisp(stStacker[nstk], Led_Gris25, (char *)"-----------");
				break;
		case 1:
				ledDisp(stStacker[nstk], Led_Verde25, (char *)"CON TARJETAS");
				break;
		case 2:
				ledDisp(stStacker[nstk], Led_Amarillo25, (char *)"POCAS TARJETAS");
				break;
		case 3:
				ledDisp(stStacker[nstk], Led_Rojo25, (char *)"SIN TARJETAS");
				
				break;
	}

	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void upStatusStacker(int re)
{				
	if(re == KYT_ERROR)
	{
		// led rojo
		ledDisp(commTDisp, Led_Rojo25, (char *)"");
		StatusStacker(0x00, 0x00);
		StatusStacker(0x01, 0x00);
	}
	else
	{
		// led verde
		if( re == KYT_OK)
		{	
			ledDisp(commTDisp, Led_Verde25, (char *)"");
			StatusStacker(0x00, venta.reader_cim6903.Stacker[0]);
			StatusStacker(0x01, venta.reader_cim6903.Stacker[1]);

			// actualizar estatus de stackers
//			printf("Stacker=%02x-%02X\n",	venta.reader_cim6903.Stacker[0], 
//											venta.reader_cim6903.Stacker[1]);
		}
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winTestDispensador(void)
{	GtkWidget *btn;
	GtkWidget *lbl;
	GtkWidget *fxDisp;		// fix dispensador
	GtkWidget *fx, *frm;	// fix Status
	
	int i;
	char msg[100];

	printf("DISPENSADOR");

	// FRAME
	frmTDisp = gtk_frame_new("PRUEBA DISPENSADOR");
	gtk_fixed_put (GTK_FIXED(fxMantto), frmTDisp, xdisp,  ydisp);
	gtk_setsize(frmTDisp, 800, 750);
	gtk_setcolorbg(frmTDisp,  (char *)"gainsboro");
	gtk_setfont(frmTDisp, "SANS", 700,15);

	//
	fxDisp = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmTDisp), fxDisp);

	//
	lbl = CreateLabel (fxDisp, (char *)"Estatus de Comunicacion ", (char *)"SANS", 700, 30, 120, 10);

	commTDisp = CreateButtonImage(fxDisp, (char *)"", Led_Gris25, 600, 10);

	// Botones de accion
	btn = CreateButton (fxDisp, (char *)"  TOMA\n TARJETA\nSTACKER 1",50,80);
	gtk_setfont(btn, "SANS", 700, 35);
	gtk_setsize(btn, 260,50);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (ButtonEventDisp), (gpointer *)"1");	

	// Botones de accion
	btn = CreateButton (fxDisp, (char *)"  TOMA\n TARJETA\nSTACKER 2",50,240);
	gtk_setfont(btn, "SANS", 700, 35);
	gtk_setsize(btn, 260,50);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (ButtonEventDisp), (gpointer *)"2");	

	// Botones de accion
	btn = CreateButton (fxDisp, (char *)"CAPTURA\n\nTARJETA",380,80);
	gtk_setfont(btn, "SANS", 700, 35);
	gtk_setsize(btn, 260,50);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (ButtonEventDisp), (gpointer *)"3");	

	btn = CreateButton (fxDisp, (char *)"EXPULSA\n\nTARJETA",380,240);
	gtk_setfont(btn, "SANS", 700, 35);
	gtk_setsize(btn, 260,50);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (ButtonEventDisp), (gpointer *)"4");	

	// FRAME
	for(i=0; i<2; i++)
	{	sprintf(msg,"STATUS STACKER %d", i+1);
		frm = gtk_frame_new(msg);
		gtk_fixed_put (GTK_FIXED(fxDisp), frm, (50 + 330*i),  420);
		gtk_setsize(frm, 250, 50);
		gtk_setcolorbg(frm,  (char *)"gainsboro");
		gtk_setfont(frm, "SANS", 700,15);
		//
		fx = gtk_fixed_new();
		gtk_container_add(GTK_CONTAINER (frm), fx);

		stStacker[i] = CreateButtonImage(fx, (char *)"-----------", Led_Gris25, 10, 3);
	}

	// Tarjeta en Lector
	frm = gtk_frame_new("TARJETA EN LECTOR");
	gtk_fixed_put (GTK_FIXED(fxDisp), frm, 50,  520);
	gtk_setsize(frm, 250, 60);
	gtk_setcolorbg(frm,  (char *)"gainsboro");
	gtk_setfont(frm, "SANS", 700,15);
	//
	fx = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frm), fx);

	lbl = CreateLabel (fx, (char *)"UID", (char *)"SANS", 700, 15, 5, 5);
	lblTDisp = CreateButtonLabel (fx, (char *)"", 0.0, 55, 2);
	gtk_setsize(lblTDisp, 180, 30);
	
	// REGRESO
	btn = CreateButton (fxDisp, (char *)"REGRESAR",450,650);
	gtk_setfont(btn, "SANS", 700, 45);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnRegresarDisp), (gpointer *)"1");	

	gtk_widget_show_all (winmantto);
}
	
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void AccionesDispensador(void)
{	static int lapso=milisegundos();
	static char op = 0;
	int re;
	unsigned char MUID[10];
	char muid[10];

	if( MertLmt.type != MERT_EXPENDEDORA)
		return;

	if( (milisegundos() - lapso) >= 300)
	{	lapso = milisegundos();
		switch(stModIO->AccionDispensador)
		{
			case 0:
				if(op<3)
				{
//					printf("pregunta 0 \n");
					re = venta.reader_cim6903.ReadStatusStacker();
					upStatusStacker(re);
					op++;
				}
				else
				{	op = 0;
					if( venta.reader_cim6903.CardPresent(MUID) != 0)
					{
						gtk_button_set_label ( GTK_BUTTON(lblTDisp), (char *)"" );
					}
					else
					{	sprintf(muid,"%02X%02X%02X%02X", MUID[0],MUID[1],MUID[2],MUID[3]);
						gtk_button_set_label ( GTK_BUTTON(lblTDisp), muid );						
					}
				}
				break;

			case 1:	//	
//				printf("pregunta 1 \n");
				venta.GetCard(STACKER1);				
				stModIO->AccionDispensador = 0;
				break;

			case 2:	//
//				printf("pregunta 2 \n");
				venta.GetCard(STACKER2);
				stModIO->AccionDispensador = 0;
				break;

			case 3:
//				printf("pregunta 3 \n");
				venta.reader_cim6903.Captura();
				stModIO->AccionDispensador = 0;
				break;

			case 4:
//				printf("pregunta 4 \n");
				venta.reader_cim6903.EjectDrop();
				stModIO->AccionDispensador = 0;
				break;

			default:
				stModIO->AccionDispensador = 0;
				break;

		}
	}
}
////////////////////////////////////////////////////////////////////////////////