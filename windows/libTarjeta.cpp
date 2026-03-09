/******************************************************************************/
/*               PROCEDIMIENTOS DE PRUEBA DEL DISPENSADOR                       */
/******************************************************************************/
//
#include "winmain.h"
//
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void ButtonLock (GtkWidget *widget, gpointer *data)
{
	if( recarga.UNLOCK(LOCK_ON)!=0)	// bloquea la tarjeta
	{	// error al bloquear la tarjeta
		printf("Error al bloquear CRT\n");
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void ButtonUnLock (GtkWidget *widget, gpointer *data)
{
	if( recarga.UNLOCK(LOCK_OFF)!=0)	// bloquea la tarjeta
	{	// error al bloquear la tarjeta
		printf("Error al desbloquear CRT\n");
	}
}

/******************************************************************************/
/* Regresa de la prueba del dispensador                                       */
/******************************************************************************/
void BtnRegresarFCard (GtkWidget *widget, gpointer *data)
{
	gtk_widget_destroy(frmFCard);	
	winMenuTest = 0xFF;
	mert_operacion.no_menu = MENU_REGRESO;

	if( recarga.UNLOCK(LOCK_ON)!=0)	// bloquea la tarjeta
	{	// error al bloquear la tarjeta
		printf("Error al bloquear CRT\n");
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winLibera(void)
{	GtkWidget *btn;
	GtkWidget *lbl;
	GtkWidget *fxFCard;		// fix dispensador
	GtkWidget *fx, *frm;	// fix Status
	int i;
	char msg[100];

	// FRAME
	frmFCard = gtk_frame_new("LIBERA TARJETA");
	gtk_fixed_put (GTK_FIXED(fxMantto), frmFCard, 200,  200);
	gtk_setsize(frmFCard, 800, 480);
	gtk_setcolorbg(frmFCard,  (char *)"gainsboro");
	gtk_setfont(frmFCard, "SANS", 700,15);

	//
	fxFCard = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmFCard), fxFCard);

	//
//	lbl = CreateLabel (fxFCard, (char *)"Estatus de Comunicacion ", 
//												(char *)"SANS", 700, 30, 120, 10);
//	commFCard = CreateButtonImage(fxFCard, (char *)"", Led_Gris25, 600, 10);

	// Botones de accion
	btn = CreateButton (fxFCard, (char *)"LIBERAR\nTARJETA",50,80);
	gtk_setfont(btn, "SANS", 700, 35);
	gtk_setsize(btn, 260,50);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (ButtonUnLock), (gpointer *)"1");	

	// Botones de accion
	btn = CreateButton (fxFCard, (char *)"BLOQUEAR\nLECTOR",400,80);
	gtk_setfont(btn, "SANS", 700, 35);
	gtk_setsize(btn, 260,50);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (ButtonLock), (gpointer *)"2");	

	// Tarjeta en Lector
	frm = gtk_frame_new("TARJETA EN LECTOR");
	gtk_fixed_put (GTK_FIXED(fxFCard), frm, 250,  250);
	gtk_setsize(frm, 250, 60);
	gtk_setcolorbg(frm,  (char *)"gainsboro");
	gtk_setfont(frm, "SANS", 700,15);
	//
	fx = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frm), fx);

	lbl = CreateLabel (fx, (char *)"UID", (char *)"SANS", 700, 15, 5, 5);
	lblFCard = CreateButtonLabel (fx, (char *)"", 0.0, 55, 2);
	gtk_setsize(lblFCard, 180, 30);

	// REGRESO
	btn = CreateButton (fxFCard, (char *)"REGRESAR",450,360);
	gtk_setfont(btn, "SANS", 700, 45);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnRegresarFCard), (gpointer *)"1");	

	gtk_widget_show_all (winmantto);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void exeLibera(void)
{	unsigned char MUID[10];
	char muid[20];

	if( recarga.CardPresent() != 0)
	{
		gtk_button_set_label ( GTK_BUTTON(lblFCard), (char *)"NO HAY TARJETA" );
	}
	else
	{
		if( recarga.reader_crt285.Read_MFUID( MUID) !=0 )
		{
			gtk_button_set_label ( GTK_BUTTON(lblFCard), (char *)"" );
		}
		else
		{
			sprintf(muid,"%02X%02X%02X%02X", MUID[0],MUID[1],MUID[2],MUID[3]);
			gtk_button_set_label ( GTK_BUTTON(lblFCard), muid );						
		}
	}
}
////////////////////////////////////////////////////////////////////////////////