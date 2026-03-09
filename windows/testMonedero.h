

// variables para prueba del monedero
GtkWidget	*frmCoin;		// window Monedero
GtkWidget	*lblUDCoin;		// ultima moneda depositada
GtkWidget	*lblTotalTest;	// Total depositado en la prueba
GtkWidget	*entFactor;	// Total depositado en la prueba

GArray *arrCoinCvt;
GArray *arrCoinTotal;
GArray *arrCoinLeds;

int totaltest;
int testCoin=0;


//************************** Prueba de Perifericos *****************************
// Posicion de la ventana
const int xcoin = 400;
const int ycoin = 100;



#define TOTAL_COINS			16		// total de monedas
const  gchar *den_coin[] = 
{
	"Moneda 1",
	"Moneda 2",
	"Moneda 3",
	"Moneda 4",
	"Moneda 5",
	"Moneda 6",
	"Moneda 7",
	"Moneda 8",
	"Moneda 9",
	"Moneda 10",
	"Moneda 11",
	"Moneda 12",
	"Moneda 13",
	"Moneda 14",
	"Moneda 15",
	"Moneda 16"
};

const  gchar *tot_coin[] = 
{ "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };


int Monedero_Init(void);
void revCVFMonedas(void);