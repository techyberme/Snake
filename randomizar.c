#include <htc.h>            //añadimos librerías
#include "time.h"
#include "randomizar.h"
#define _XTAL_FREQ 4000000
void randomizar(){          //derfinición de la función
manzanay=0b11111110;        //valor inicial
xmanzana=rand()%8;          //valor de coordenada x entre 0 y 7
ymanzana=rand()%8;          //valor de coordenada y entre 0 y 7
if(ymanzana==0){               //si ymanzana es cero
manzanay=0b11111110;           //el vector manzanay tiene un cero en el primer bit
}
if(ymanzana==1){               //si ymanzana es uno
manzanay=0b11111110;           //el vector manzanay tiene un cero en el segundo bit
}
if(ymanzana==2){               //si ymanzana es dos
manzanay=0b11111101;           //el vector manzanay tiene un cero en el tercer bit
}
if(ymanzana==3){               //si ymanzana es tres
manzanay=0b11111011;           //el vector manzanay tiene un cero en el cuarto bit
}
if(ymanzana==4){               //si ymanzana es cuatro
manzanay=0b11110111;           //el vector manzanay tiene un cero en el quinto bit
}
if(ymanzana==5){               //si ymanzana es cinco
manzanay=0b11011111;           //el vector manzanay tiene un cero en el sexto bit
}
if(ymanzana==6){               //si ymanzana es seis
manzanay=0b10111111;           //el vector manzanay tiene un cero en el septimo bit
}
if(ymanzana==7){               //si ymanzana es siete
manzanay=0b01111111;           //el vector manzanay tiene un cero en el octavo bit
}
}
