
// variables para prueba del billetero
GtkWidget	*frmBill;		// frame Billetero
GtkWidget	*lblUDBill;		// ultimo billete depositado

GArray *arrBillCvt;
GArray *arrBillTotal;
GArray *arrBillLeds;

GArray *arrBillCvt2;
GArray *arrBillTotal2;
GArray *arrBillLeds2;

int testBill=0;

//************************** Prueba de Perifericos *****************************
// Posicion de la ventana
const int xbill = 400;
const int ybill = 100;



#define TOTAL_BILLS			32		// total de billetes
const  gchar *den_bill[] = 
{
	"Billete 1",
	"Billete 2",
	"Billete 3",
	"Billete 4",
	"Billete 5",
	"Billete 6",
	"Billete 7",
	"Billete 8",
	"Billete 9",
	"Billete 10",
	"Billete 11",
	"Billete 12",
	"Billete 13",
	"Billete 14",
	"Billete 15",
	"Billete 16"
	"Billete 17"
	"Billete 18"
	"Billete 19"
	"Billete 20"
	"Billete 21"
	"Billete 22"
	"Billete 23"
	"Billete 24"
	"Billete 25"
	"Billete 26"
	"Billete 27"
	"Billete 28"
	"Billete 29"
	"Billete 30"
	"Billete 31"
	"Billete 32"
};

const  gchar *tot_bill[] = 
{ "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
  "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",};
