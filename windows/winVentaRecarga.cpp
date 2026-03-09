
/******************************************************************************/
/******************************************************************************/
/*                 ventanas para el proceso de venta/ recarga                 */
/******************************************************************************/
/******************************************************************************/
//
//
void rep_alarma( int nv);
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
char *sel_mensaje(int no_msg)
{
	if(mert_operacion.fgIdioma==0)
		return (char *)&mensajes_ep[no_msg].msg;
	if(mert_operacion.fgIdioma==1)
		return (char *)&mensajes_in[no_msg].msg; 
	return NULL;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
char *sel_font(int no_msg)
{
	if(mert_operacion.fgIdioma==0)
		return (char *)&mensajes_ep[no_msg].font; 
	if(mert_operacion.fgIdioma==1)
		return (char *)&mensajes_in[no_msg].font; 
	return NULL;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int sel_weight(int no_msg)
{
	if(mert_operacion.fgIdioma==0)
		return mensajes_ep[no_msg].weight; 
	if(mert_operacion.fgIdioma==1)
		return mensajes_in[no_msg].weight;

	return 700;	// BOLD
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int sel_size(int no_msg)
{
	if(mert_operacion.fgIdioma==0)
		return mensajes_ep[no_msg].size; 
	if(mert_operacion.fgIdioma==1)
		return mensajes_in[no_msg].size;

	return 20;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
char *sel_msgfont(int no_msg, char font[])
{
	if(mert_operacion.fgIdioma==0)
	{	strcpy(font, (char *)&mensajes_ep[no_msg].font);
		return (char *)&mensajes_ep[no_msg].msg; 
	}
	if(mert_operacion.fgIdioma==1)
	{	strcpy(font, (char *)&mensajes_in[no_msg].font);
		return (char *)&mensajes_in[no_msg].msg; 
	}
	return NULL;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winButtonMainHide(void)
{
	gtk_widget_hide( fxBtnSD );
	gtk_widget_hide( fxBtnSI );
	gtk_widget_hide( fxBtnII );
	gtk_widget_hide( fxBtnID );

	gtk_widget_hide( fxIniBtnSI);
	gtk_widget_hide( fxIniBtnIC);
	
	gtk_widget_hide(lblMsgBill);
	fgMsgBill = 0;	//hide
	
	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char winCambioIdioma(unsigned char idioma)
{	char msg[50];

	if(mert_operacion.fgIdioma == SPANISH)
		mert_operacion.fgIdioma = INGLES;
	else
		mert_operacion.fgIdioma = SPANISH;

	gtk_label_set_text( GTK_LABEL(lblIniBtnSI), sel_mensaje(MSG_IDIOMA));
	gtk_setfont(lblIniBtnSI, sel_font(MSG_IDIOMA), sel_weight(MSG_IDIOMA), sel_size(MSG_IDIOMA) );

	gtk_label_set_text( GTK_LABEL(lblIniBtnIC), sel_mensaje(MSG_INSERTA_MIA));
	gtk_setfont(lblIniBtnIC, sel_font(MSG_INSERTA_MIA), sel_weight(MSG_INSERTA_MIA), sel_size(MSG_INSERTA_MIA) );

	gtk_label_set_text( GTK_LABEL(lblBtnSD), sel_mensaje(MSG_VENTA)  );
	gtk_setfont(lblBtnSD, sel_font(MSG_VENTA), sel_weight(MSG_VENTA), sel_size(MSG_VENTA) );
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void on_btnExit_clicked(void) //GtkObject *object, gpointer user_data)
{
	printf("\n\n*********** I'll be back *************\n\n");
//	gtk_main_quit();

	exit(0);
}

/******************************************************************************/
/* muestra hora/fecha en pantalla                                             */
/******************************************************************************/
char gtk_seconds(void)
{	static char seg=0, min=-1;
	time_t TimeNow;
	struct tm ticket_time;
	char msg[100];

	time(&TimeNow);  // time actual
	ticket_time = *localtime(&TimeNow);

	if( seg != ticket_time.tm_sec )
	{
		strftime(msg, sizeof(msg),"%d/%m/%Y %H:%M:%S", &ticket_time);
		gtk_label_set_text( GTK_LABEL(lblDate), msg );	
		seg = ticket_time.tm_sec;
	}

	if( min != ticket_time.tm_min )
	{
		obtieneIP_MASK((char *)"enp2s0", mert_operacion.ip, mert_operacion.nm);
//		sprintf(msg, "[IP=%s][NM=%s][%s]", mert_operacion.ip, mert_operacion.nm, VERSION );
		sprintf(msg, "[IP=%s][%s]", mert_operacion.ip, VERSION );
		gtk_label_set_text( GTK_LABEL(lblDatos), msg );	
		min = ticket_time.tm_min;
	}

	return ticket_time.tm_min;
}


/******************************************************************************/
/* gtk visible?                                                               */
/******************************************************************************/
gboolean gtk_is_visible(GtkWidget *gtkwidget)
{

	return gtk_widget_get_visible(gtkwidget);
}

/******************************************************************************/
/* mensajes en pantalla principal                                             */
/******************************************************************************/
gboolean gtk_lbl_msg(GtkWidget *lbl, const gchar *msg, 
										const gchar *font, GtkJustification opc)
{
	gtk_setjustify(GTK_LABEL(lbl), opc);
	gtk_label_set_text(GTK_LABEL(lbl), msg);
	return TRUE;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winMensajes(GtkWidget *fx, GtkWidget *lbl, GdkRGBA colorbg, GdkRGBA colorfg, char msg[], 
		const char font[], int weight, int size)
{
	gtk_label_set_text( GTK_LABEL(lbl), msg);

	gtk_setfont(lbl, font, weight, size);
	gtk_setcolorbg(lbl, colorbg);
	gtk_setcolorfg(lbl, colorfg);
}

/******************************************************************************/
/*                       Mensaje de Inicializacion                            */
/******************************************************************************/
void winMsgInicializando(void)
{
	winMensajes(fxProcess, lblMensajes, rgb_khaki, rgb_black, sel_mensaje(MSG_INICIA), 
				sel_font(MSG_INICIA), sel_weight(MSG_INICIA), sel_size(MSG_INICIA) );
	gtk_widget_show(lblMensajes);

}

/******************************************************************************/
/*        Movimientos                                                         */
/******************************************************************************/
void winMsgMovimientos( char msg[], const char font[], int weight, int size)
{
	gtk_hide(fxSaldo);
	winButtonMainHide();
	NoPasoProceso = PASO_MOV_REGRESAR;
	win_show_botones(WIN_PROCESO);

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 100, 160);
	gtk_setsize(lblMensajes, 800, 200);

	winMensajes(fxProcess, lblMensajes, rgb_khaki, rgb_black, msg, font, weight, size );

	gtk_setjustify(GTK_LABEL(lblMensajes), GTK_JUSTIFY_LEFT);
	
	gtk_widget_show(lblMensajes);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winExitMovimientos(void)
{
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);
	gtk_setjustify(GTK_LABEL(lblMensajes), GTK_JUSTIFY_CENTER);
}

/******************************************************************************/
/*  Destrabar                                                         */
/******************************************************************************/
void winMsgDestrabar( char msg[], const char font[], int weight, int size)
{	unsigned int tm;

	gtk_hide(fxSaldo);
	winButtonMainHide();

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);

	gtk_setsize(lblMensajes, 800, 200);

	winMensajes(fxProcess, lblMensajes, rgb_khaki, rgb_black, msg, font, weight, size );
	gtk_widget_show(lblMensajes);

	tm = milisegundos();
	while( (milisegundos() - tm) < 500 )
		updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winExitDestrabar(void)
{
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);
	gtk_show(fxSaldo);
	win_show_botones(WIN_PROCESO);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winMsgLimite(void)
{	int lapso = milisegundos();

	winButtonMainHide();
	
	gtk_widget_hide(fxSaldo);
	
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);

	winMensajes(fxProcess, lblMensajes, rgb_red, rgb_black, sel_mensaje(MSG_LIMITE_EXC), 
				sel_font(MSG_LIMITE_EXC), sel_weight(MSG_LIMITE_EXC), sel_size(MSG_LIMITE_EXC) );
	gtk_widget_show(lblMensajes);

	while( (milisegundos() - lapso) <=  1500)
		updWin();

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);

	win_show_botones(WIN_PROCESO);
	gtk_widget_show(fxSaldo);

	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winMsgReset(void)
{	int lapso = milisegundos();

	winButtonMainHide();
	
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);

	winMensajes(fxProcess, lblMensajes, rgb_orange_red, rgb_black, sel_mensaje(MSG_BILL_RESET), 
				sel_font(MSG_BILL_RESET), sel_weight(MSG_BILL_RESET), sel_size(MSG_BILL_RESET) );
	gtk_widget_show(lblMensajes);

	while( (milisegundos() - lapso) <=  20000)
		updWin();

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);

	win_show_botones(WIN_INICIO);

	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winErrorMsg(int nomsg, int tiempo)
{	int lapso = milisegundos();

	winButtonMainHide();
	gtk_widget_hide(fxSaldo);
	
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);

	winMensajes(fxProcess, lblMensajes, rgb_red, rgb_black, sel_mensaje(nomsg), 
				sel_font(nomsg), sel_weight(nomsg), sel_size(nomsg) );
	gtk_widget_show(lblMensajes);

	while( (milisegundos() - lapso) <=  tiempo)
		updWin();

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);

	win_show_botones(WIN_PROCESO);
	gtk_widget_show(fxSaldo);

	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winValidaBill(int valor)
{	char msg[100];
	int lapso = milisegundos();

	sprintf(msg, "%s\n$%.2f", sel_mensaje(MSG_VALIDA), (double)valor/100);

	winButtonMainHide();

	gtk_widget_hide(fxSaldo);

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 300, 250);
	gtk_setsize(lblMensajes, 600, 200);

	winMensajes(fxProcess, lblMensajes, rgb_black, rgb_white, msg, 
				sel_font(MSG_VALIDA), sel_weight(MSG_VALIDA), sel_size(MSG_VALIDA) );
	gtk_widget_show(lblMensajes);

	while( (milisegundos() - lapso) <=  500)
		updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winValidaExit(void)
{

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);

	win_show_botones(WIN_PROCESO);
	gtk_widget_show(fxSaldo);

	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winErrorDispensador(void)
{	int lapso = milisegundos();

	winButtonMainHide();
	
	gtk_widget_hide(fxSaldo);
	
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);

	winMensajes(fxProcess, lblMensajes, rgb_red, rgb_black, sel_mensaje(MSG_ERROR_VENTA), 
				sel_font(MSG_ERROR_VENTA), sel_weight(MSG_ERROR_VENTA), sel_size(MSG_ERROR_VENTA) );
	gtk_widget_show(lblMensajes);

	while( (milisegundos() - lapso) <=  1500)
		updWin();

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);

	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winErrorVentaIncompleta(void)
{	int lapso = milisegundos();

	winButtonMainHide();
	
	gtk_widget_hide(fxSaldo);
	
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);

	winMensajes(fxProcess, lblMensajes, rgb_red, rgb_black, sel_mensaje(MSG_INCOMPLETA), 
				sel_font(MSG_INCOMPLETA), sel_weight(MSG_INCOMPLETA), sel_size(MSG_INCOMPLETA) );
	gtk_widget_show(lblMensajes);

	while( (milisegundos() - lapso) <=  1500)
		updWin();

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);

	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winMsgFaltante(int cost, int dep)
{	char msg[100];
	double falta;

	falta = dep * -1;
	falta = falta/100;

	sprintf(msg, "%s $%.2f", sel_mensaje(MSG_FALTANTE), falta);

	winButtonMainHide();	
	gtk_widget_hide(fxSaldo);

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 200);

	winMensajes(fxProcess, lblMensajes, rgb_red, rgb_black, msg, 
				sel_font(MSG_FALTANTE), sel_weight(MSG_FALTANTE), sel_size(MSG_FALTANTE) );

	gtk_widget_show(lblMensajes);

	win_show_botones(PASO_FALTANTE);

	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winMsgCasiLLeno(int nm)
{	int lapso = milisegundos();
	char msg[100];

	strcpy(msg, "\nCAJAS CASI LLENAS\n");
	if(nm & 0x01)
		strcat(msg, "[MONEDERO]\n");
	if(nm & 0x02)
		strcat(msg, "[BILLETERO]\n");

	winButtonMainHide();
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 250);
	gtk_setsize(lblMensajes, 800, 200);

	winMensajes(fxProcess, lblMensajes, rgb_khaki, rgb_black, msg, 
				sel_font(MSG_LIMITE_SEMI), sel_weight(MSG_LIMITE_SEMI), sel_size(MSG_LIMITE_SEMI) );
	gtk_widget_show(lblMensajes);

	updWin();

	rep_alarma(2);
	
	while( (milisegundos() - lapso) <=  2000)
		updWin();

	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);

	win_show_botones(WIN_PROCESO);

	updWin();

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winMsgLLeno(int nm)
{	char msg[100];

	msg[0] = 0x00;

//	if(nm == 0x08)
//		strcat(msg, "\n[ERROR LECTOR RECARGA]\n");
//	else
//	{
//	strcpy(msg, "\nCAJAS LLENAS\n");
	if(nm & 0x01)
		strcat(msg, "[CAJA MONEDAS LLENA]\n");
	if(nm & 0x02)
		strcat(msg, "[CAJA BILLETES LLENA]\n");
	if(nm & 0x04)
		strcat(msg, "[BANDEJA CAPTURA LLENA]\n");
	if(nm & 0x08)
		strcat(msg, "\n[ERROR LECTOR RECARGA]\n");
//	}

	winButtonMainHide();
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 250);
	gtk_setsize(lblMensajes, 800, 200);

	winMensajes(fxProcess, lblMensajes, rgb_khaki, rgb_black, msg, 
				sel_font(MSG_LIMITE_SEMI), sel_weight(MSG_LIMITE_SEMI), sel_size(MSG_LIMITE_SEMI) );
	gtk_widget_show(lblMensajes);
	
	updWin();

	rep_alarma(4);

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void exit_full(void)
{
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);

	win_show_botones(WIN_PROCESO);
	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winGifWait(void)
{
	GdkPixbufAnimation * animation;
	GtkWidget * image;

	animation = gdk_pixbuf_animation_new_from_file (GifWait, NULL);
	image = gtk_image_new();
	gtk_image_set_from_animation (GTK_IMAGE(image), animation);

	gifWait = gtk_button_new();
	gtk_button_set_always_show_image (GTK_BUTTON (gifWait), TRUE);
	gtk_button_set_image (GTK_BUTTON (gifWait), image);
	gtk_fixed_put(GTK_FIXED(fxProcess),gifWait, 370, 350);
    gtk_widget_show (gifWait);
	updWin();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void destroyGif(void)
{
	gtk_widget_destroy(GTK_WIDGET(gifWait));
}

/******************************************************************************/
/*               ESPERE UN MOMENTO                                            */
/******************************************************************************/
void winMsgWaitMomment(unsigned char nmsg)
{	GdkRGBA colorbg;

	colorbg = rgb_white;
	colorbg.alpha = 0.5;	// se cambio lo traslucido-> 0=transparente 1=opaco

	winMensajes(fxProcess, lblMensajes, colorbg, rgb_black, sel_mensaje(nmsg), 
			sel_font(nmsg), sel_weight(nmsg), sel_size(nmsg) );
	gtk_widget_show(lblMensajes);

	int i = milisegundos();
	while( (milisegundos() - i) < 200)
		updWin();
}

/******************************************************************************/
/*  Equipo en Mantenimiento                                                   */
/******************************************************************************/
void winMsgMantenimiento(void)
{
	winButtonMainHide();

	gtk_setsize(lblMensajes, 800, 400);
	winMensajes(fxProcess, lblMensajes, rgb_yellow, rgb_black, sel_mensaje(MSG_MANTTO), 
			sel_font(MSG_MANTTO), sel_weight(MSG_MANTTO), sel_size(MSG_MANTTO) );
	gtk_widget_show(lblMensajes);
}

/******************************************************************************/
/*  Equipo en Mantenimiento                                                   */
/******************************************************************************/
void winMsgBill(void)
{

	gtk_setsize(lblMensajes, 800, 400);
	winMensajes(fxProcess, lblMensajes, rgb_yellow, rgb_black, sel_mensaje(MSG_MANTTO), 
			sel_font(MSG_MANTTO), sel_weight(MSG_MANTTO), sel_size(MSG_MANTTO) );
	gtk_widget_show(lblMensajes);
}

/******************************************************************************/
/*  Equipo en Mantenimiento                                                   */
/******************************************************************************/
void winMsgCajas(void)
{
	winButtonMainHide();

	gtk_setsize(lblMensajes, 800, 400);
	winMensajes(fxProcess, lblMensajes, rgb_red, rgb_black, sel_mensaje(MSG_CAJAS), 
			sel_font(MSG_CAJAS), sel_weight(MSG_CAJAS), sel_size(MSG_CAJAS) );
	gtk_widget_show(lblMensajes);
}

/******************************************************************************/
/*  Equipo Fuera de Servicio                                                  */
/******************************************************************************/
void winMsgOut(void)
{
	winButtonMainHide();

	gtk_setsize(lblMensajes, 800, 400);
	winMensajes(fxProcess, lblMensajes, rgb_red, rgb_black, sel_mensaje(MSG_FUERA_SERVICIO), 
			sel_font(MSG_FUERA_SERVICIO), sel_weight(MSG_FUERA_SERVICIO), sel_size(MSG_FUERA_SERVICIO) );
	gtk_widget_show(lblMensajes);
}

/******************************************************************************/
/*               ESPERE UN MOMENTO                                            */
/******************************************************************************/
void winMsgRemoveMIA(void)
{	
	winButtonMainHide();
	gtk_hide(fxSaldo);
	winMensajes(fxProcess, lblMensajes, rgb_white, rgb_black, sel_mensaje(MSG_RETIRA_MIA), 
			sel_font(MSG_RETIRA_MIA), sel_weight(MSG_RETIRA_MIA), sel_size(MSG_RETIRA_MIA) );
	gtk_widget_show(lblMensajes);

	updWin();
}

/******************************************************************************/
/*                       Mensaje de Inicializacion                            */
/******************************************************************************/
void winMsgComunicacion(void)
{
	winMensajes(fxProcess, lblMensajes, rgb_khaki, rgb_black, sel_mensaje(MSG_PERIFERICOS), 
			sel_font(MSG_PERIFERICOS), sel_weight(MSG_PERIFERICOS), sel_size(MSG_PERIFERICOS) );
	gtk_widget_show(lblMensajes);
}



/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void gtkSetCursor(GdkCursorType cursorType) 
{
	GdkWindow* win = gtk_widget_get_parent_window(fxProcess);

	GdkDisplay *display = gtk_widget_get_display (fxProcess);
	GdkCursor  *cursor  = gdk_cursor_new_for_display (display, GDK_BLANK_CURSOR);
	gdk_window_set_cursor( win, cursor);
}

/******************************************************************************/
/*  secuencia de mensajes en botones segun el paso en que va                  */
/******************************************************************************/
void win_show_botones(char win)
{
	gtk_widget_hide( fxBtnII );
	gtk_widget_hide( fxBtnID );
	gtk_widget_hide( fxBtnSI );
	gtk_widget_hide( fxBtnSD );
	gtk_widget_hide( fxIniBtnSI );
	gtk_widget_hide( fxIniBtnIC );
	gtk_widget_hide(lblMsgBill);
	fgMsgBill = 0;	//hide

	if(win==WIN_INICIO)
	{
		gtk_widget_show( fxIniBtnSI);
		gtk_widget_show( fxIniBtnIC);

		// Si es EXPENDERORA MUESTRA BOTON SUPERIOR DERECHO
		if( mert_operacion.fgVENTA)
			gtk_widget_show( fxBtnSD );
		else
			gtk_widget_hide( fxBtnSD );

		if(fgShowCoin==1)
		{	winMsg(lblMsgBill, rgb_orange_red, rgb_black, (char *)"--> SOLO MONEDAS <--", "Sans", 700,50);
			fgMsgBill = 1;	//show
		}
		return;
	}

	gtk_widget_hide(fxBtnII);
	gtk_widget_hide(fxBtnID);
	gtk_widget_hide(fxBtnSI);
	gtk_widget_hide(fxBtnSD);

	switch(NoPasoProceso)
	{
		case PASO_INICIAL:	// VENTANA PRINCIPAL DE VENTA/RECARGA
				gtk_label_set_text( GTK_LABEL(lblBtnII),  sel_mensaje(MSG_IDIOMA) );
				gtk_setfont( lblBtnII, sel_font(MSG_IDIOMA), 
								sel_weight(MSG_IDIOMA), sel_size(MSG_IDIOMA) );
				gtk_widget_show(fxBtnII);

				if(fgShowCoin==1)
				{	gtk_widget_show( lblMsgBill );
					fgMsgBill = 1;	//show
				}
				break;

		case PASO_MOV_FIN:	// MOV y FINALIZAR   BOTONES IZQ -> INFERIOR y SUPERIOR
				gtk_label_set_text( GTK_LABEL(lblBtnID),  sel_mensaje(MSG_ACEPTAR) );
				gtk_setfont( lblBtnID, sel_font(MSG_ACEPTAR), 
								sel_weight(MSG_ACEPTAR), sel_size(MSG_ACEPTAR) );
				gtk_widget_show(fxBtnID);

				gtk_label_set_text( GTK_LABEL(lblBtnSI),  sel_mensaje(MSG_DESATORA) );
				gtk_setfont( lblBtnSI, sel_font(MSG_DESATORA), 
								sel_weight(MSG_DESATORA), sel_size(MSG_DESATORA) );
				gtk_widget_show(fxBtnSI);

				if( mert_operacion.proceso == PROC_RECARGA)
				{	// solo si estamos en RECARGA mostramos opcion de "MOVIMIENTOS"
					gtk_label_set_text( GTK_LABEL(lblBtnII),  sel_mensaje(MSG_MOVIMIENTOS) );
					gtk_setfont( lblBtnII, sel_font(MSG_MOVIMIENTOS), 
									sel_weight(MSG_MOVIMIENTOS), sel_size(MSG_MOVIMIENTOS) );
					gtk_widget_show(fxBtnII);
				}

				if(fgShowCoin==1)
				{	gtk_widget_show( lblMsgBill );
					fgMsgBill = 1;	//show
				}
			break;
				
		case PASO_MOV_REGRESAR:	// INFERIOR DERECHO
				gtk_label_set_text( GTK_LABEL(lblBtnID),  sel_mensaje(MSG_REGRESAR) );
				gtk_setfont( lblBtnID, sel_font(MSG_REGRESAR), 
								sel_weight(MSG_REGRESAR), sel_size(MSG_REGRESAR) );
				gtk_widget_show(fxBtnID);

				gtk_widget_hide( lblMsgBill );
				fgMsgBill = 0;	//hide
				break;

		case PASO_IMP_FIN:
				gtk_label_set_text( GTK_LABEL(lblBtnID),  sel_mensaje(MSG_ACEPTAR) );
				gtk_setfont( lblBtnID, sel_font(MSG_ACEPTAR), 
								sel_weight(MSG_ACEPTAR), sel_size(MSG_ACEPTAR) );
				gtk_widget_show(fxBtnID);

				gtk_label_set_text( GTK_LABEL(lblBtnSI),  sel_mensaje(MSG_DESATORA) );
				gtk_setfont( lblBtnSI, sel_font(MSG_DESATORA), 
								sel_weight(MSG_DESATORA), sel_size(MSG_DESATORA) );
				gtk_widget_show(fxBtnSI);

//				if(bills.fg_master_inhibit==0)
				if(fgShowCoin==1)
				{	winMsg(lblMsgBill, rgb_orange_red, rgb_black, (char *)"--> SOLO MONEDAS <--", "Sans", 700,50);
					fgMsgBill = 1;	//show
				}
				else
				{	gtk_widget_hide( lblMsgBill );
					fgMsgBill = 0;	//hide
				}
				break;

		case PASO_FALTANTE:
				printf("FALTA DINERO\n");
				gtk_label_set_text( GTK_LABEL(lblBtnII),  sel_mensaje(MSG_ACEPTAR) );
				gtk_setfont( lblBtnII, sel_font(MSG_ACEPTAR), 
								sel_weight(MSG_ACEPTAR), sel_size(MSG_ACEPTAR) );
				gtk_widget_show(fxBtnII);

				gtk_label_set_text( GTK_LABEL(lblBtnID),  sel_mensaje(MSG_REGRESAR) );
				gtk_setfont( lblBtnID, sel_font(MSG_REGRESAR), 
								sel_weight(MSG_REGRESAR), sel_size(MSG_REGRESAR) );
				gtk_widget_show(fxBtnID);

				if(fgShowCoin==1)
				{
					gtk_widget_hide( lblMsgBill );
					fgMsgBill = 0;	//hide
				}
				break;
			
	}
}

/******************************************************************************/
/* show winprocess & hide botones                                             */
/******************************************************************************/
void gtk_show_winprocess(void)
{
	gtk_widget_show_all(winprocess);
	gtk_hide(fxSaldo);
	gtk_hide(lblMensajes);
	NoPasoProceso = PASO_INICIAL;
	win_show_botones(WIN_PROCESO);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winSaldo_show(void)
{

	gtk_widget_show_all(winprocess);	// muestra la ventana de saldo

	if( mert_operacion.proceso == PROC_RECARGA)
	{	// Oculta Costo de Tarjeta
		gtk_hide(lblCosto);
		gtk_hide(entCostoTarjeta);
	}
	gtk_hide(lblMensajes);

	gtk_changeLabel(0);
	
	NoPasoProceso = PASO_MOV_FIN;

	win_show_botones(WIN_PROCESO);

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void winmain_show(void)
{
	gtk_widget_show_all(winprocess);
	gtk_hide(fxSaldo);
	gtk_hide(lblMensajes);
	
	NoPasoProceso = PASO_INICIAL;
	win_show_botones(WIN_INICIO);
}

/******************************************************************************/
/*                    Etiquetas en la ventana de saldo                        */
/******************************************************************************/
void gtk_changeLabel(char operacion)
{
	if(operacion == PROCESO_RECARGA)
	{
		gtk_label_set_text( GTK_LABEL(lblEncabezado), sel_mensaje(MSG_RECARGA));
		gtk_setfont( lblEncabezado, sel_font(MSG_RECARGA), 
									sel_weight(MSG_RECARGA),
									sel_size(MSG_RECARGA) );
	}
	else
	{
		gtk_label_set_text( GTK_LABEL(lblEncabezado), sel_mensaje(MSG_VENTA_RECARGA));
		gtk_setfont( lblEncabezado, sel_font(MSG_VENTA_RECARGA), 
									sel_weight(MSG_VENTA_RECARGA),
									sel_size(MSG_VENTA_RECARGA) );
	}

	gtk_label_set_text( GTK_LABEL(lblCosto), sel_mensaje(MSG_COSTO));
	gtk_setfont( lblCosto,	sel_font(MSG_COSTO), 
								sel_weight(MSG_COSTO),
								sel_size(MSG_COSTO) );

	gtk_label_set_text( GTK_LABEL(lblInicial), sel_mensaje(MSG_INICIAL));
	gtk_setfont( lblInicial,	sel_font(MSG_INICIAL), 
								sel_weight(MSG_INICIAL),
								sel_size(MSG_INICIAL) );
	
	gtk_label_set_text( GTK_LABEL(lblSuPago), sel_mensaje(MSG_SUPAGO));
	gtk_setfont( lblSuPago,	sel_font(MSG_SUPAGO), 
								sel_weight(MSG_SUPAGO),
								sel_size(MSG_SUPAGO) );

	gtk_label_set_text( GTK_LABEL(lblActual), sel_mensaje(MSG_TOTAL));
	gtk_setfont( lblActual,	sel_font(MSG_TOTAL), 
								sel_weight(MSG_TOTAL),
								sel_size(MSG_TOTAL) );

	gtk_label_set_text( GTK_LABEL(lblBono), sel_mensaje(MSG_BONO));
	gtk_setfont( lblBono,	sel_font(MSG_BONO), 
								sel_weight(MSG_BONO),
								sel_size(MSG_BONO) );
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
//TotalCantidadDepositado, TotalCantidadInicial, TotalBonosInicial
void win_saldo_inicial(int icosto, int idepositado, int icantidad, int ibono)
{	char buff[40];
 
	// Costo MIA, solo si esta en VENTA
	if( mert_operacion.proceso == PROC_VENTA)
	{
		sprintf(buff, "$ %7.02f", (float)icosto/100);
		gtk_entry_set_text(GTK_ENTRY(entCostoTarjeta), buff);
	}

	// saldo inicial MIA
	sprintf(buff, "$ %7.02f", (float)idepositado/100);
	gtk_entry_set_text(GTK_ENTRY(entSuPago), buff);

	// saldo inicial MIA
	sprintf(buff, "$ %7.02f", (float)icantidad/100);
	gtk_entry_set_text(GTK_ENTRY(entSaldoInicial), buff);

	// bonificacion
	sprintf(buff, "$ %7.02f", (float)ibono/100);
	gtk_entry_set_text(GTK_ENTRY(entBono), buff);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
//TotalCantidadFinal, TotalBonosFinal, TotalCantidadDepositada
void win_saldo_final(int icantidad, int ibono, int itotal)
{	char buff[40];

	sprintf(buff, "$ %7.02f", (float)itotal/100);
	gtk_entry_set_text(GTK_ENTRY(entSuPago), buff);

	sprintf(buff, "$ %7.02f", (float)icantidad/100);
	gtk_entry_set_text(GTK_ENTRY(entSaldoActual), buff);

	// bono
	sprintf(buff, "$ %7.02f", (float)ibono/100);
	gtk_entry_set_text(GTK_ENTRY(entBono), buff);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void fxwinsaldo(void)
{//	GtkWidget	*btnExit;
	
	const int offset = 80;

	// fondo  ************************
	fxSaldo = gtk_fixed_new();
	gtk_fixed_put(GTK_FIXED(fxProcess),fxSaldo, 10, 10);
	gtk_setsize(fxSaldo, 800, 500);
	gtk_setcolorbg(fxSaldo, rgb_green);

	// ******************** Etiquetas para el SALDO ****************************
	// ENCABEZADO
	lblEncabezado = gtk_label_new(sel_mensaje(MSG_RECARGA));
	gtk_container_add(GTK_CONTAINER (fxSaldo), lblEncabezado);
	gtk_fixed_move ( GTK_FIXED (fxSaldo), lblEncabezado, 430, 180);
	gtk_setsize(lblEncabezado, 300, 40);
	gtk_setfont (lblEncabezado,"Arial Rounded Mt", 700, 60);
	gtk_setcolorfg(lblEncabezado,  rgb_black);

	int ypos = 200;
	lblCosto = gtk_label_new(sel_mensaje(MSG_COSTO));
	gtk_container_add(GTK_CONTAINER (fxSaldo), lblCosto);
	gtk_fixed_move ( GTK_FIXED (fxSaldo), lblCosto, 400, ypos+offset);
	gtk_setsize(lblCosto, 50, 40);
	gtk_setfont (lblCosto,"Arial Rounded Mt", 700, 40);
	gtk_setcolorfg(lblCosto,  rgb_black);

	lblInicial = gtk_label_new(sel_mensaje(MSG_INICIAL));
	gtk_container_add(GTK_CONTAINER (fxSaldo), lblInicial);
	gtk_fixed_move ( GTK_FIXED (fxSaldo), lblInicial, 400, ypos+60+offset);
	gtk_setsize(lblInicial, 50, 40);
	gtk_setfont (lblInicial,"Arial Rounded Mt", 700, 40);
	gtk_setcolorfg(lblInicial,  rgb_black);

	lblSuPago = gtk_label_new(sel_mensaje(MSG_SUPAGO));
	gtk_container_add(GTK_CONTAINER (fxSaldo), lblSuPago);
	gtk_fixed_move ( GTK_FIXED (fxSaldo), lblSuPago, 400, ypos+120+offset);
	gtk_setsize(lblSuPago, 50, 40);
	gtk_setfont (lblSuPago,"Arial Rounded Mt", 700, 40);
	gtk_setcolorfg(lblSuPago,  rgb_black);

	lblActual = gtk_label_new(sel_mensaje(MSG_TOTAL));
	gtk_container_add(GTK_CONTAINER (fxSaldo), lblActual);
	gtk_fixed_move ( GTK_FIXED (fxSaldo), lblActual, 400, ypos+180+offset);
	gtk_setsize(lblActual, 50, 40);
	gtk_setfont (lblActual,"Arial Rounded Mt", 700, 40);
	gtk_setcolorfg(lblActual,  rgb_black);

	lblBono = gtk_label_new(sel_mensaje(MSG_BONO));
	gtk_container_add(GTK_CONTAINER (fxSaldo), lblBono);

	if(mert_famisa)
		gtk_fixed_move ( GTK_FIXED (fxSaldo), lblBono, 110, 310);
	else
		gtk_fixed_move ( GTK_FIXED (fxSaldo), lblBono, 110, 370);


	gtk_setsize(lblBono, 50, 30);
	gtk_setfont (lblBono,"Arial Rounded Mt", 700, 30);
	gtk_setcolorfg(lblBono,  rgb_black);


	int swidth = 190;
	int shight = 55;
	int sfont  = 35;	// size font
	int xpos = 780;

	ypos = 190;
	// Costo
	entCostoTarjeta = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER (fxSaldo), entCostoTarjeta);
	gtk_setsize(entCostoTarjeta, swidth, shight);
	gtk_entry_set_alignment(GTK_ENTRY(entCostoTarjeta), 1.0);
	gtk_setfont (entCostoTarjeta,"Ubuntu Mono", 700, sfont);
	gtk_fixed_move ( GTK_FIXED (fxSaldo), entCostoTarjeta , xpos, ypos+offset);
	gtk_entry_set_text(GTK_ENTRY(entCostoTarjeta), "$   1.99");

	// INICIAL
	entSaldoInicial = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER (fxSaldo), entSaldoInicial);
	gtk_setsize(entSaldoInicial, swidth, shight);
	gtk_entry_set_alignment(GTK_ENTRY(entSaldoInicial), 1.0);
	gtk_setfont (entSaldoInicial,"Ubuntu Mono", 700, sfont);
	gtk_fixed_move ( GTK_FIXED (fxSaldo), entSaldoInicial , xpos, ypos + 60+offset);
	gtk_entry_set_text(GTK_ENTRY(entSaldoInicial), "$   1.99");

	// DEPOSITO
	entSuPago = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER (fxSaldo), entSuPago);
	gtk_setsize(entSuPago, swidth, shight);
	gtk_entry_set_alignment(GTK_ENTRY(entSuPago), 1.0);
	gtk_setfont (entSuPago,"Ubuntu Mono", 700, sfont);
	gtk_fixed_move ( GTK_FIXED (fxSaldo), entSuPago , xpos, ypos + 120+offset);
	gtk_entry_set_text(GTK_ENTRY(entSuPago), "$ 666.88");
	
	// TOTAL
	entSaldoActual = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER (fxSaldo), entSaldoActual);
	gtk_setsize(entSaldoActual, swidth, shight);
	gtk_entry_set_alignment(GTK_ENTRY(entSaldoActual), 1.0);
	gtk_setfont (entSaldoActual,"Ubuntu Mono", 700, sfont);
	gtk_fixed_move ( GTK_FIXED (fxSaldo), entSaldoActual, xpos, ypos + 180+offset);
	gtk_entry_set_text(GTK_ENTRY(entSaldoActual), "$   5.55");

	entBono = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER (fxSaldo), entBono);
	gtk_setsize(entBono, swidth-50, shight-10);
	gtk_entry_set_alignment(GTK_ENTRY(entBono), 1.0);
	gtk_setfont (entBono,"Ubuntu Mono", 700, 25);
	if(mert_famisa)
		gtk_fixed_move ( GTK_FIXED (fxSaldo), entBono , 65, 340);
	else
		gtk_fixed_move ( GTK_FIXED (fxSaldo), entBono , 65, 400);
		
	gtk_entry_set_text(GTK_ENTRY(entBono), "$   1.23");

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void gtk_winprocess(void)
{	char fname[100];
	GtkWidget	*btnExit;
	GtkWidget	*gimgLogo;
	char path[60];

	sprintf(path, "%simagenes/",mert_operacion.path);
	printf("imagenes=%s\n", path);

	winprocess = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_decorated(GTK_WINDOW(winprocess), FALSE);
	gtk_window_set_default_size(GTK_WINDOW(winprocess),1024,768);
	g_signal_connect(winprocess, "destroy", G_CALLBACK(gtk_main_quit), NULL);	
	gtk_window_move (GTK_WINDOW(winprocess),1280,0);
	gtk_setcolorbg(winprocess, rgb_black);

	gtk_window_fullscreen(GTK_WINDOW(winprocess) );

	// fondo  ************************
	fxProcess = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER (winprocess), fxProcess);
	snprintf(fname,100,"%smetrorrey1024x768.jpeg",path);
	gimgLogo  = gtk_image_new_from_file(fname);
	gtk_container_add(GTK_CONTAINER (fxProcess), gimgLogo);

	// hora y fecha ******************
	lblDate = gtk_label_new("");	
	gtk_container_add(GTK_CONTAINER (fxProcess), lblDate);
	gtk_setsize(lblDate, 400, 20);
	gtk_setfont(lblDate,"Arial Rounded Mt", 700, 15);	//"Sans Bold 10"
	gtk_label_set_xalign (GTK_LABEL(lblDate), 1.0);
	gtk_setcolorfg(lblDate, rgb_black);
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblDate, 620, 0);

	// IP, NM y Version
	lblDatos = gtk_label_new("");	
	gtk_container_add(GTK_CONTAINER (fxProcess), lblDatos);
	gtk_setsize(lblDatos, 400, 20);
	gtk_setfont(lblDatos,"Arial Rounded Mt", 700, 15);
	gtk_setcolorfg(lblDatos, rgb_light_slate_gray);		//rgb_dark_slate_gray);	//rgb_black);

	gtk_label_set_xalign (GTK_LABEL(lblDatos), 0.0);
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblDatos, 0, 0);

	// mensajes en gral
	lblMensajes = gtk_label_new(sel_mensaje(MSG_INICIA));
	gtk_container_add(GTK_CONTAINER (fxProcess), lblMensajes);
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_setjustify(GTK_LABEL(lblMensajes), GTK_JUSTIFY_CENTER);
	gtk_setfont(lblMensajes, sel_font(MSG_INICIA), sel_weight(MSG_INICIA), sel_size(MSG_INICIA));


	// BOTONES DE OPERACION
	// Inferior Izquierdo
	fxBtnII = gtk_fixed_new();
	gtk_setsize(fxBtnII, 500, 180);
	gtk_container_add(GTK_CONTAINER (fxProcess), fxBtnII );

	if(mert_famisa)
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxBtnII, x_Izquierdo, y_Inferior );
	else
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxBtnII, x_Izquierdo, y_Inferior + 40 );

	GtkWidget	*btnI;
	snprintf(fname,100,"%samarilloclaro_izq500x180.png",path);
	btnI = gtk_image_new_from_file(fname);
	gtk_container_add(GTK_CONTAINER (fxBtnII), btnI);
	gtk_fixed_move ( GTK_FIXED (fxBtnII), btnI ,0, 0);

	lblBtnII = gtk_label_new( sel_mensaje(MSG_IDIOMA) );
	gtk_container_add(GTK_CONTAINER (fxBtnII), lblBtnII);
	gtk_fixed_move ( GTK_FIXED (fxBtnII), lblBtnII, 150, 10);
	gtk_setsize(lblBtnII, 330, 150);
	gtk_setjustify(GTK_LABEL(lblBtnII), GTK_JUSTIFY_CENTER);
	gtk_setfont(lblBtnII, sel_font(MSG_IDIOMA), sel_weight(MSG_IDIOMA), sel_size(MSG_IDIOMA) );
	gtk_setcolorfg(lblBtnII, rgb_black);

	// Inferior Derecho
	fxBtnID = gtk_fixed_new();
	gtk_setsize(fxBtnID, 500, 180);
	gtk_container_add(GTK_CONTAINER (fxProcess), fxBtnID );
	if(mert_famisa)
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxBtnID, x_Derecho, y_Inferior );
	else
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxBtnID, x_Derecho, y_Inferior + 40);
		
//	GtkWidget	*btnID;
	snprintf(fname,100,"%samarilloclaro_der500x180.png",path);
	btnI = gtk_image_new_from_file(fname);
	gtk_container_add(GTK_CONTAINER (fxBtnID), btnI);
	gtk_fixed_move ( GTK_FIXED (fxBtnID), btnI ,0, 0);

	lblBtnID = gtk_label_new( sel_mensaje(MSG_IDIOMA) );
	gtk_container_add(GTK_CONTAINER (fxBtnID), lblBtnID);
	gtk_fixed_move ( GTK_FIXED (fxBtnID), lblBtnID, 10, 10);
	gtk_setsize(lblBtnID, 330, 150);
	gtk_setjustify(GTK_LABEL(lblBtnID), GTK_JUSTIFY_CENTER);
	gtk_setfont(lblBtnID, sel_font(MSG_IDIOMA), sel_weight(MSG_IDIOMA), sel_size(MSG_IDIOMA) );
	gtk_setcolorfg(lblBtnID, rgb_black);
	
	// Superior Derecho
	fxBtnSD = gtk_fixed_new();
	gtk_setsize(fxBtnSD, 500, 180);
	gtk_container_add(GTK_CONTAINER (fxProcess), fxBtnSD );
	
	
	if(mert_famisa)
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxBtnSD, 724, 126 );
	else
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxBtnSD, 724, 260 );
		
	GtkWidget	*btnD;
	snprintf(fname,100,"%samarilloclaro_der300x100.png",path);
	btnD = gtk_image_new_from_file(fname);
	gtk_container_add(GTK_CONTAINER (fxBtnSD), btnD);
	gtk_fixed_move ( GTK_FIXED (fxBtnSD), btnD ,0, 0);

	lblBtnSD = gtk_label_new( sel_mensaje(MSG_VENTA) );
	gtk_container_add(GTK_CONTAINER (fxBtnSD), lblBtnSD);
	gtk_fixed_move ( GTK_FIXED (fxBtnSD), lblBtnSD, 5, 3);
	gtk_setsize(lblBtnSD, 230, 100);
	gtk_setjustify(GTK_LABEL(lblBtnSD), GTK_JUSTIFY_CENTER);
	gtk_setfont(lblBtnSD, sel_font(MSG_VENTA), sel_weight(MSG_VENTA), sel_size(MSG_VENTA));
	gtk_setcolorfg(lblBtnSD, rgb_black);

	// Superior Izquierdo Venta/Recarga
	fxBtnSI = gtk_fixed_new();
	gtk_setsize(fxBtnSI, 180, 60);
	gtk_container_add(GTK_CONTAINER (fxProcess), fxBtnSI );

	if(mert_famisa)
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxBtnSI, x_Izquierdo, y_Superior + 60);
	else
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxBtnSI, x_Izquierdo, y_Superior + 195);

	snprintf(fname,100,"%samarilloclaro_izq180x60.png",path);
	btnD = gtk_image_new_from_file(fname);
	gtk_container_add(GTK_CONTAINER (fxBtnSI), btnD);
	gtk_fixed_move ( GTK_FIXED (fxBtnSI), btnD ,0, 0);

	lblBtnSI = gtk_label_new( sel_mensaje(MSG_DESATORA) );
	gtk_container_add(GTK_CONTAINER (fxBtnSI), lblBtnSI);
	gtk_fixed_move ( GTK_FIXED (fxBtnSI), lblBtnSI, 20, 4);
	gtk_setsize(lblBtnSI, 180, 60);
	gtk_setjustify(GTK_LABEL(lblBtnSI), GTK_JUSTIFY_CENTER);
	gtk_setfont(lblBtnSI, sel_font(MSG_DESATORA), sel_weight(MSG_DESATORA), sel_size(MSG_DESATORA));
	gtk_setcolorfg(lblBtnSI, rgb_black);


	// Superior Izquierdo Menu Inicial
	fxIniBtnSI = gtk_fixed_new();
	gtk_setsize(fxIniBtnSI, 180, 60);
	gtk_container_add(GTK_CONTAINER (fxProcess), fxIniBtnSI );

	if(mert_famisa)
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxIniBtnSI, x_Izquierdo, y_Superior + 60);
	else
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxIniBtnSI, x_Izquierdo, y_Superior + 195);

	snprintf(fname,100,"%samarilloclaro_izq180x60.png",path);
	btnD = gtk_image_new_from_file(fname);
	gtk_container_add(GTK_CONTAINER (fxIniBtnSI), btnD);
	gtk_fixed_move ( GTK_FIXED (fxIniBtnSI), btnD ,0, 0);

	lblIniBtnSI = gtk_label_new( sel_mensaje(MSG_IDIOMA) );
	gtk_container_add(GTK_CONTAINER (fxIniBtnSI), lblIniBtnSI);
	gtk_fixed_move ( GTK_FIXED (fxIniBtnSI), lblIniBtnSI, 20, 4);
	gtk_setsize(lblIniBtnSI, 180, 60);
	gtk_setjustify(GTK_LABEL(lblIniBtnSI), GTK_JUSTIFY_CENTER);
	gtk_setfont(lblIniBtnSI,	sel_font(MSG_IDIOMA),	
								sel_weight(MSG_IDIOMA), 
								sel_size(MSG_IDIOMA));
	gtk_setcolorfg(lblIniBtnSI, rgb_black);

	// Inferior Centro  Menu Inicial
	fxIniBtnIC = gtk_fixed_new();
	gtk_setsize(fxIniBtnIC, 550, 180);
	gtk_container_add(GTK_CONTAINER (fxProcess), fxIniBtnIC );

	if(mert_famisa)
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxIniBtnIC, x_centro, y_centro);
	else
		gtk_fixed_move ( GTK_FIXED (fxProcess), fxIniBtnIC, x_centro-200, y_centro);

	snprintf(fname,100,"%srecarga_550x180.png",path);
	btnD = gtk_image_new_from_file(fname);
	gtk_container_add(GTK_CONTAINER (fxIniBtnIC), btnD);
	gtk_fixed_move ( GTK_FIXED (fxIniBtnIC), btnD ,0, 0);

	lblIniBtnIC = gtk_label_new( sel_mensaje(MSG_INSERTA_MIA) );
	gtk_container_add(GTK_CONTAINER (fxIniBtnIC), lblIniBtnIC);
	gtk_fixed_move ( GTK_FIXED (fxIniBtnIC), lblIniBtnIC, 50, 10);
	gtk_setsize(lblIniBtnIC, 550, 180);
	gtk_setjustify(GTK_LABEL(lblIniBtnIC), GTK_JUSTIFY_CENTER);
	gtk_setfont(lblIniBtnIC,	sel_font(MSG_INSERTA_MIA), 
								sel_weight(MSG_INSERTA_MIA), 
								sel_size(MSG_INSERTA_MIA));
	gtk_setcolorfg(lblIniBtnIC, rgb_black);

	////////////////////////////////////////////////////////////////////////////
	lblMsgBill = CreateLabel (fxProcess, (char *)"", (char *)"Sans", 100, 45, 300, 40);

	gtk_setsize(lblMsgBill, 400, 100);
	gtk_setjustify(GTK_LABEL(lblMsgBill), GTK_JUSTIFY_CENTER);

	////////////////////////////////////////////////////////////////////////////

	// boton de exit *************************
	btnExit = gtk_button_new();
	gtk_fixed_put(GTK_FIXED(fxProcess),btnExit, 0, 0);
	gtk_setcolorbg(btnExit, rgb_trans);
	gtk_setsize(btnExit, 20, 20);
	gtk_button_set_relief(GTK_BUTTON(btnExit), GTK_RELIEF_NONE);
	g_signal_connect(btnExit, "clicked", G_CALLBACK(on_btnExit_clicked), NULL);

	fxwinsaldo();

	gtk_widget_show_all(winprocess);	// muestra la ventana de saldo
	winButtonMainHide();
	gtk_hide(fxSaldo);

}
////////////////////////////////////////////////////////////////////////////////
