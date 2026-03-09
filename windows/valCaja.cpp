/******************************************************************************/
/*               PROCEDIMIENTOS DE PRUEBA DEL DISPENSADOR                       */
/******************************************************************************/
//
#include "winmain.h"
//
/******************************************************************************/
/* Regresa de la prueba del dispensador                                       */
/******************************************************************************/
void BtnRetValBox (GtkWidget *widget, gpointer *data)
{	int re;

	re = 0;

	if( (fgCajaMonedas==0) || (fgCajaBilletes==0) )
		re = winMensajeWarning((char *)"Alguna Caja esta ALARMADA", (char *)"Necesita Validar Caja");

	if(re==0)
	{
		gtk_widget_destroy(frmBoxVal);	
		winMenuTest = 0xFF;
		mert_operacion.no_menu = MENU_REGRESO;
	}
}

/******************************************************************************/
/* Validar Caja                                                               */
/******************************************************************************/
void BtnValBox(GtkWidget *widget, gpointer *data)
{
	// poner mensaje de confirmacion

	if( fgCajaMonedas==0)
	{
		if(idCajaMonedas!=0)
		{
			fgCajaMonedas=1;
			creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_MONEDAS_VALIDADA, detMonedas.CajaID, idCajaMonedas);
		}
	}

	if( fgCajaBilletes==0)
	{
		if(idCajaBilletes!=0)
		{
			fgCajaBilletes=1;
			creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_BILLETES_VALIDADA, detMonedas.CajaID, idCajaMonedas);
		}
	}

	if( (fgCajaMonedas==1) && (fgCajaBilletes==1) )
		winMensajeOK((char *)"AMBAS CAJAS VALIDADAS", (char *)"Alarma cancelada");
	else
		winMensajeOK((char *)"ALGUNA CAJA CONTINUA ALARMADA", (char *)"Proceda a Validar");

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winValCaja(void)
{	GtkWidget *btn;
	GtkWidget *lbl;
	GtkWidget *fxVCaja;		// fix dispensador
	int i;
	char msg[100];

	// FRAME
	frmBoxVal = gtk_frame_new("VALIDA CAJAS");
	gtk_fixed_put (GTK_FIXED(fxMantto), frmBoxVal, 200,  200);
	gtk_setsize(frmBoxVal, 800, 480);
	gtk_setcolorbg(frmBoxVal,  (char *)"gainsboro");
	gtk_setfont(frmBoxVal, "SANS", 700,15);

	//
	fxVCaja = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (frmBoxVal), fxVCaja);

	lbl = CreateLabel (fxVCaja, (char *)"ID Caja de Monedas",  (char *)"SANS", 700, 20, 30, 75);
	lbl = CreateLabel (fxVCaja, (char *)"ID Caja de Billetes", (char *)"SANS", 700, 20, 30, 125);

	lbl = CreateLabel (fxVCaja, (char *)"Base Datos", (char *)"SANS", 700, 20, 300, 40);
	lbl = CreateLabel (fxVCaja, (char *)"ID Actual", (char *)"SANS",  700, 20, 460, 40);

	sprintf(msg, "%08x", detMonedas.CajaID);
	lbl = CreateButton (fxVCaja, msg, 310,70);

	sprintf(msg, "%08x", detBilletes.CajaID);
	lbl = CreateButton (fxVCaja, msg, 310,120);

	
	sprintf(msg, "%08x", idCajaMonedas);
	lblIdCoin = CreateButton (fxVCaja, msg, 460,70);

	sprintf(msg, "%08x", idCajaBilletes);
	lblIdBill = CreateButton (fxVCaja, msg, 460,120);

	// validar caja
	btn = CreateButton (fxVCaja, (char *)"VALIDAR\n CAJA",100,300);
	gtk_setfont(btn, "SANS", 700, 45);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnValBox), (gpointer *)"1");	


	// REGRESO
	btn = CreateButton (fxVCaja, (char *)"REGRESAR",450,300);
	gtk_setfont(btn, "SANS", 700, 45);
	g_signal_connect ( btn, "clicked",  G_CALLBACK (BtnRetValBox), (gpointer *)"1");	

	gtk_widget_show_all (winmantto);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void exeValBox(void)
{	static unsigned int tm = milisegundos();
	char uid[20];

	if( ( milisegundos() - tm ) > 1000)
	{
		tm = milisegundos();

		sprintf(uid, "%08x", idCajaMonedas);
		gtk_button_set_label ( GTK_BUTTON(lblIdCoin), uid );
	
		sprintf(uid, "%08x", idCajaBilletes);
		gtk_button_set_label ( GTK_BUTTON(lblIdBill), uid );
	}
}
////////////////////////////////////////////////////////////////////////////////