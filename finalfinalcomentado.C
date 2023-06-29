#include <htc.h>				
#include "time.h"								//incluimos la libreria time.h para utilizar el comando rand()
#include "randomizar.h"							//incluimos la libreria randomizar.h		
__CONFIG(WRT_OFF & WDTE_OFF & PWRTE_OFF & FOSC_XT & LVP_OFF);		
#define _XTAL_FREQ 4000000						//configuramos el oscilador interno para 4MHz
unsigned char matriz[8];						//definimos las variables, siendo matriz el resultado final que simulamos en la placa
unsigned char matrizr[8];						//variables de matrices provisionales
unsigned char matrizp[8];				
unsigned char filar;							//variable para definir la fila
unsigned char provy;  							//provy es el contenido de la columna donde esta la cabeza
unsigned char x;								//Variable para definir el numero de fila
unsigned char y;								//Variable
unsigned char xn1;								//Variables para definir los valores anteriores de la serpiente
unsigned char yn1;
unsigned char xn2;
unsigned char yn2;
unsigned char xn3;
unsigned char yn3;
unsigned char xn4;
unsigned char yn4;
unsigned int vx;								//Variable que depende de la posicion del joystick cuando señala a la izquierda o derecha
unsigned int vy;								//Variable que depende de la posicion del joystick cuando señala arriba o abajo
unsigned char impar;							
unsigned char premio;							//Variable para cuando la cabeza de la serpiente coincida con la manzana
unsigned int tempo;								//Variable para ayudar a que el timer2 cuente hasta 15 segundos en total
unsigned char contador;							//Variable para definir lo larga que es la serpiente
unsigned char repetir;							
unsigned char direccion; 						//variable para la direccion tomada, 1 derecha, 2 arriba, 3 izquierda, 4 abajo. 
static void recordar(unsigned char, unsigned char); //funcion para recordar las posiciones anteriores de la serpiente.
static void conversion();						//funcion para llamar a la conversion
static void movimiento(unsigned int,unsigned int);	//funcion para el movimiento de la serpiente
static void contento();							//funcion para la cara contenta
static void triste();							//funcion cara triste

void interrupt isr(void){						//programa de interrupciones
	 
	if (CCP2IF==1){								//cuando salte la flag de interrupcion por el modo comparacion, ponemos a 0 el Timer1
	    TMR1=0;
	    CCP2IF=0;                 				//bajamos flag
	    conversion();							//llamamos a conversion
	        }
	else if(T0IF==1){							//si salta la flag por interrupcion del Timer0, ponemos el TMR0 a 251.
		TMR0=251;				
		RE0=~RE0;								//negamos el puerto RE0				
		if(RE0==1){								//si el puerto RE0 esta a 1, incrementamos filar
			filar++;
		}
		if(filar>8){							//si filar es mayor que 8, la igualamos a 1 y ponemos a 1 el pin RE1 para resetear la matriz. 
			filar=1;
			RE1=1;
		}
		RE1=0;									//Ponemos a 0 el RE1 para que la matriz no se resetee. 		
		 
		PORTD=matriz[filar-1];					//Ponemos en el puerto D matriz(filar-1) 
		T0IF=0;									//Bajamos la flag por interrupcion
		}
	else if(TMR2IF==1){							//Si salta la flag por interrupcion del Timer2, si tempo es menor que 300, lo incrementamos.
		if(tempo<300){
        tempo++;
		 
        TMR2IF=0;								//Bajamos la flag de interrupcion
        PR2=194;								//Como tras la interrupcion el PR2 es resetea, volvemos a ponerle el valor 194. 
		}
		else if (tempo>=300){					//Si tempo es igual o mayor que 300, apagamos el timer 2 ya que eso significa que han pasado 15 segundos desde que lo iniciamos
		TMR2ON=0;
		TMR2IF=0;								//Bajamos la flag por interrupcion
	 
		PR2=194;								//Volvemos a poner PR2 a 194
		triste();								//Llamamos a la funcion triste
    }
	}
}
static void conversion(){						//funcion de conversion	
	ADON=1;                						//inicializamos conversion poniendo a uno el bit ADON para encender el modulo de encendido del convertidor A/D
	__delay_us(40);								//delay de 40 us
	GO_DONE=1;					
	while(GO_DONE==1){							//mientras el GO_DONE esta a 1, la conversion estara en progreso. Una vez se ponga a 0, se terminara la conversion y saldra del bucle.
	}
	       
	if (CHS0==0){								//Si el canal de conversion es 0, igualamos vy a lo que esta almacenado en ADRESH*256+ADRESL 
	    vy=ADRESH*256+ADRESL    ;
	
	                        }
	if (CHS0==1){								//Si el canal de conversion es 1, igualamos vx a lo que esta almacenado en ADRESH*256+ADRESL
	    vx=ADRESH*256+ADRESL  ;
	                        }
	CHS0=~CHS0;             					//cambiamos el canal
	ADON=0    ;									//ponemos a 0 el bit de ADON para apagar el modulo de encendido del convertidor A/D. 
	movimiento(vx,vy);							//llamamos a la funcion de movimiento.
}
void main(void){								//Programa principal
	TRISA=0xFF;									//Configuramos el PORTA como entrada
	TRISD=0x00; 								//Configuramos el PORTD como salida
	TRISE0=0;									//Configuramos el bit RE0 como salida
	TRISE1=0;									//Configuramos el bit RE1 como salida
	TRISC=0xFF;									//Configuramos el PORTC como entrada
	RE0=1;     									//Ponemos a 1 RE0
	RE1=0;										//Ponemos a 0 RE1
	matriz[0]=0xFF; 							//Igualamos todas las columnas de matriz[x] a 0xFF para que no haya ninguna led encendida								
	matriz[1]=0xFF;    
	matriz[2]=0xFF;  
	matriz[3]=0xFF;  
	matriz[4]=0xFF;  
	matriz[5]=0xFF;  
	matriz[6]=0xFF;  
	matriz[7]=0xFF;
	matrizr[0]=0xFF; 							//Igualamos todas las columnas de matrizr[x] a 0xFF para que no haya ninguna led encendida
	matrizr[1]=0xFF;    
	matrizr[2]=0xFF;  
	matrizr[3]=0xFF;  
	matrizr[4]=0xFF;  
	matrizr[5]=0xFF;  
	matrizr[6]=0xFF;  
	matrizr[7]=0xFF;    
	matrizp[0]=0xFF; 							//Igualamos todas las columnas de matrizp[x] a 0xFF para que no haya ninguna led encendida
	matrizp[1]=0xFF;    
	matrizp[2]=0xFF;  
	matrizp[3]=0xFF;  
	matrizp[4]=0xFF;  
	matrizp[5]=0xFF;  
	matrizp[6]=0xFF;  
	matrizp[7]=0xFF;    
	TMR1ON=0; 									//Mantenemos apagado el Timer1
	ADCON1=0xB4;								//Ponemos el ADCON1 0xB4 para ajustar el resultado a la derecha y para poner los pines del puerto E como digitales mientras que ponemos RA3, RA1 y RA0 como analogicos
	PORTD=0x00;									//Ponemos a 0 el PORTD
	OPTION_REG=7;								//En el registro OPTION_REG seleccionamos un factor de division del TMR0 de 256. 
	TMR0=251;									//Ponemos TMR0 a 251
	GIE=1;										//Ponemos el bit GIE a 1 para habilitar el sistema de interrupciones del microcontrolador 	
	PEIE=1;										//Habilitacion de interrupciones de los perifericos
	T0IE=1;										//Habilitacion por interrupcion por desbordamiento del TMR0
	provy=0x00;									//El valor inicial de provy es 0			
	ADCS2=0;									//Con los bits ADCSx, seleccionamos Fosc/2 para la seleccion del reloj para la conversion A/D
	ADCS1=0;
	ADCS0=0;
	CHS2=0;										//Seleccionamos el canal de conversion 0 con los bits CHSx
	CHS1=0;
	CHS0=0;
	ADFM=1;										//Ajustamos el resultado obtenido a la derecha
	CCP2CON=10;									//Ponemos el CCP2CON para seleccionar el modo comparacion y poner el bit CCPIF es puesto a uno cuando el resultado de la comparacion es positivo
	T1CKPS1=1;									//Seleccion de un factor de division del pre-divisor del timer1 de 8 con los bits T1CKPSx
	T1CKPS0=1;
	TMR2ON=0;           						//Primero el Timer2 estar‡ apagado
    PR2=194;     								//valor inicial tmr2
    TMR2IE=1;									//Habilitamos la interrupcion por desbordamiento del Timer2.
    T2CON=0b01111111;							//Seleccionamos un post-divisor y pre-divisor de 16
	CCPR2=31250;								//Si se iguala el valor en el registro CCPR2 en el de TMR1, se genera una interrupcion
	provy=0b11111110;							//el valor inicial de provy es 0b01111111
	x=2; 										//valor incial de x
	y=0;										//valor inicial de y
	matriz[x]=matriz[x] & provy;  
	premio=2;									//valor inicial premio
	repetir=0;									//valor inical repetir
	tempo=0;									//valor inicial tempo

	while(1){
		if(RC4==0){								//Cuando presionemos el RC4, comenzara a contar el Timer1 y el timer 2
			TMR1ON=1;
            TMR2ON=1;
            PR2=194;
			}
	
	}
}

static void movimiento (unsigned int vx, unsigned int vy){
 
	if (vx>1020){								//Si vx>1020, significa que estamos yendo hacia la derecha
		if(direccion==3){						//Si previamente estabamos yendo hacia la izquierda, no podremos ir a la derecha.
			NOP();
		}
	else{				
		xn4=xn3;								//Si no estabamos yendo hacia la izquierda, guardamos las posiciones anteriores.
		yn4=yn3;
		xn3=xn2;
		yn3=yn2;
		xn2=xn1;
		yn2=yn1;
		xn1=x;
		yn1=y;
		if(x==7){								//Si x=7, igualamos matrizr[7] a 0xFF para poner apagar todas las leds de esa columna.
			matrizr[x]=0xFF; 				
			matrizr[0]=matrizp[x];				//Igualamos matrizr[0] a matrizp[7]
	 
					}
		else {									//De otra manera, matrizr[x] a 0xFF y matrizr[x+1]=matrizp[x]
			matrizr[x]=0xFF;  			
			matrizr[x+1]=matrizp[x];
					}
			
			 
		for(char i=0;i<8;i++){                  // igualamos la matrizp a la matriz ya modificada
			matrizp[i]=matrizr[i];
					}
		 x++;									//incrementamos x
		if(x>7){								//si x es mayor que siete, igualamos x a 0
			x =0; 
				}
	    if (contador==2){						//si contador=2, llamamos a la funcion recordar con unsigned char xn1 y yn1
	        recordar(xn1,yn1);
	    }
	    if (contador==3){						//si contador=3, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
			repetir=0;
	 		recordar(xn1,yn1);
			repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
			repetir=0;							//ponemos repetir=0
	 	   }
		if (contador==4){						//si contador=4, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
	 		repetir=0;								
			recordar(xn1,yn1);
	   		repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
			recordar(xn3,yn3);					//llamamos a la funcion recordar con unsigned char xn3 y yn3
			repetir=0;							//ponemos repetir=0
	    }
		if (contador==5){						//si contador=5, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
			repetir=0;									
	 		recordar(xn1,yn1);
			repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
	    	recordar(xn3,yn3);					//llamamos a la funcion recordar con unsigned char xn3 y yn3
			recordar(xn4,yn4);					//llamamos a la funcion recordar con unsigned char xn4 y yn4
			repetir=0;							//repetir=0
	    }
	direccion=1;								//igualamos direccion a 1
		}
		}
	if (vx<5){									//si vx<5, giramos el joystick a la izquierda. 
	if(direccion==1){							//si estabamos yendo previamente hacia la derecha, no podemos girar a la izquierda
			NOP();
			}
	else{
		xn4=xn3;								//recordamos posiciones anteriores
		yn4=yn3;
		xn3=xn2;
		yn3=yn2;
		xn2=xn1;
		yn2=yn1;
		xn1=x;
		yn1=y;
 
		if(x==0){								//Si x=0, matrizr[0]=0xFF para apagar todas las luces e igualamos matrizr[7] a matrizp[0]
			matrizr[x]=0xFF; 
			matrizr[7]=matrizp[x];					
				}
		else{									//Si x no es igual a 0, matrizr[x] se iguala a 0xFF y matrizr[x-1] se iguala a matrizp[x]
			matrizr[x]=0xFF;  
			matrizr[x-1]=matrizp[x];
			}
			for(char i=0;i<8;i++){				// igualamos la matrizp a la matriz ya modificada
				matrizp[i]=matrizr[i];
					}
			x --;								//Decrementamos x
			if(x ==0xFF){						//Si x=0xFF significa que hemos decrementado x=0, por que para evitar esto, igualamos x=7
				x =7;
			}
		if (contador==2){						//si contador=2, llamamos a la funcion recordar con unsigned char xn1 y yn1
	        recordar(xn1,yn1);
	    }
	    if (contador==3){						//si contador=3, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
			repetir=0;
	 		recordar(xn1,yn1);
			repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
			repetir=0;							//ponemos repetir=0
	 	   }
		if (contador==4){						//si contador=4, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
	 		repetir=0;								
			recordar(xn1,yn1);
	   		repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
			recordar(xn3,yn3);					//llamamos a la funcion recordar con unsigned char xn3 y yn3
			repetir=0;							//ponemos repetir=0
	    }
		if (contador==5){						//si contador=5, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
			repetir=0;									
	 		recordar(xn1,yn1);
			repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
	    	recordar(xn3,yn3);					//llamamos a la funcion recordar con unsigned char xn3 y yn3
			recordar(xn4,yn4);					//llamamos a la funcion recordar con unsigned char xn4 y yn4
			repetir=0;							//repetir=0
	    }
		 
	   	  direccion=3;							//igualamos direccion=3
		}		 
}
if (vy>1020){									//si vy>1020, significa que estamos yendo hacia arriba con el joystick
	if(direccion==4){							//si previamente estabamos yendo hacia abajo, no podemos ir hacia arriba
			NOP();
			}
	else{										//Guardamos las posiciones anteriores
		xn4=xn3;
		yn4=yn3;
		xn3=xn2;
		yn3=yn2;
		xn2=xn1;
		yn2=yn1;	
		xn1=x;
		yn1=y;
		matrizr[x]=0xFF;						//Igualamos la matrizr[x] a 0x00 para la ejecucion de la funcion recordar
		for(char i=0;i<8;i++){
			matrizp[i]=matrizr[i];              // igualamos la matrizp a la matriz ya modificada
			}	 
		if (provy>127){							//Cuando provy es mayor que 127
				provy=(provy+provy)+1;          //multiplicamos por dos y sumamos uno
			}
		else if (provy<=127){                   //Si es menor
				provy=(provy+provy);            //multiplicamos por dos
		}
		y++;									//Incrementamos y
		if(y >7){								//Si y es mayor que siete, igualamos y=0 ya que la matriz de leds solo tiene 8 columnas y 8 filas
			y=0; 
		}
	 
	   	if (contador==2){						//si contador=2, llamamos a la funcion recordar con unsigned char xn1 y yn1
	        recordar(xn1,yn1);
	    }
	    if (contador==3){						//si contador=3, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
			repetir=0;
	 		recordar(xn1,yn1);
			repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
			repetir=0;							//ponemos repetir=0
	 	   }
		if (contador==4){						//si contador=4, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
	 		repetir=0;								
			recordar(xn1,yn1);
	   		repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
			recordar(xn3,yn3);					//llamamos a la funcion recordar con unsigned char xn3 y yn3
			repetir=0;							//ponemos repetir=0
	    }
		if (contador==5){						//si contador=5, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
			repetir=0;									
	 		recordar(xn1,yn1);
			repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
	    	recordar(xn3,yn3);					//llamamos a la funcion recordar con unsigned char xn3 y yn3
			recordar(xn4,yn4);					//llamamos a la funcion recordar con unsigned char xn4 y yn4
			repetir=0;							//repetir=0
	    }
		direccion=2;							//Igualamos direccion a 2
	}
		 
}
	if (vy<5){									//Si vy es menor que 5, apuntamos con el joystick hacia abajo
		if(direccion==2){						//Si estabamos yendo hacia arriba previamente, no podremos ir hacia abajo
		NOP();
		}
		else  {									//Guardamos las posiciones anteriores
			xn4=xn3;
			yn4=yn3;
			xn3=xn2;
			yn3=yn2;
	   	  	xn2=xn1;
			yn2=yn1;
			xn1=x;
			yn1=y;
			matrizr[x]=0xFF;					
		for(char i=0;i<8;i++){                  // igualamos la matrizp a la matrizr
			matrizp[i]=matrizr[i];
				}
		impar=provy%2;                          //division para saber si es par o impar
		if (impar==1){                          //si es impar
				provy=(provy/2)+128;            //dividimos entre dos y sumamos 128
				}
		else if (impar==0){                     //si es par
				provy=provy/2;                  //solo dividimos entre dos
		}
		y--;									//decrementamos y
		if(y==0xFF){							//Si y toma el valor 0xFF, significa que hemos decrementado y=0. Como nuestra matriz de leds es 8x8, igualamos y a 7.
			y=7; 
		}
	 
	   	if (contador==2){						//si contador=2, llamamos a la funcion recordar con unsigned char xn1 y yn1
	        recordar(xn1,yn1);
	    }
	    if (contador==3){						//si contador=3, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
			repetir=0;
	 		recordar(xn1,yn1);
			repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
			repetir=0;							//ponemos repetir=0
	 	   }
		if (contador==4){						//si contador=4, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
	 		repetir=0;								
			recordar(xn1,yn1);
	   		repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
			recordar(xn3,yn3);					//llamamos a la funcion recordar con unsigned char xn3 y yn3
			repetir=0;							//ponemos repetir=0
	    }
		if (contador==5){						//si contador=5, ponemos repetir=0 y llamamos a la funcion recordar con unsigned char xn1 y yn1
			repetir=0;									
	 		recordar(xn1,yn1);
			repetir=1;							//ponemos repetir=1
			recordar(xn2,yn2);					//llamamos a la funcion recordar con unsigned char xn2 y yn2
	    	recordar(xn3,yn3);					//llamamos a la funcion recordar con unsigned char xn3 y yn3
			recordar(xn4,yn4);					//llamamos a la funcion recordar con unsigned char xn4 y yn4
			repetir=0;							//repetir=0
	    }
		 
		direccion=4;							//igualamos direccion a 4
			}	 
	}
	if (contador==1){							//si contador=1, simplemente igualamos la matrizp[x] a provy, que es el contenido de la columna donde se encuentra la cabeza de la serpiente
		matrizp[x]=provy;
	}
  
	for(char i=0;i<8;i++){                      //igualamos la matriz principal a la matriz previamente modicada
		matriz[i]=matrizp[i];
}

	matrizp[x]=provy;							//igualamos matrizp[x] y matrizr[x] a provy
	matrizr[x]=provy;                           //ya que, en caso de que haya habido movimiento vertical, ni matrizr ni matrizp han sido modificadas
	premio=~matriz[xmanzana] & ~manzanay;       //comprobamos si la serpiente se ha comido una manzana
	matriz[xmanzana]=matriz[xmanzana] & manzanay;//a–adimos la manzana a la matriz principal
 	if (premio!=0){								//Cuando premio es distinto de 0, significa que la cabeza de la serpiente y la manzana han coincidido, por lo que llamamos a randomizar, que provocara la aparicion de una nueva manzana
  		randomizar();				
  		premio=0;								//Igualamos premio a 0
  		contador++;								//Aumentamos el contador para hacer mas larga la serpiente
		if(contador==6){						//Cuando contador llega a seis, se gana el juego y llamamos a la funcion contento. 
			contento();
		}
  	}
}
static void recordar(unsigned char t, unsigned char q){             //funci—n para el cuerpo de la serpiente
	if(repetir==0){                                                 //en caso de que repetir estŽ a cero
		matrizp[t]=matrizr[t] & provy;                              //a–adimos la cabeza al vector matrip[t]
	}
    if (q==0){
        matrizp[t]=matrizp[t] & 0b11111110;                         //si q es cero, a–adimos al vector matrizp[t] un vector que enciende el led de la primera fila
    }
    if (q==1){
        matrizp[t]=matrizp[t] & 0b11111101;                         //si q es uno, a–adimos al vector matrizp[t] un vector que enciende el led de la segunda fila
    }
    if (q==2){
        matrizp[t]=matrizp[t] &  0b11111011;                         //si q es dos, a–adimos al vector matrizp[t] un vector que enciende el led de la tercera fila
    }
    if (q==3){
        matrizp[t]=matrizp[t] &  0b11110111;                         //si q es tres, a–adimos al vector matrizp[t] un vector que enciende el led de la cuarta fila
    }
    if (q==4){
        matrizp[t]=matrizp[t] & 0b11101111;                         //si q es cuatro, a–adimos al vector matrizp[t] un vector que enciende el led de la quinta fila
    }
    if (q==5){
        matrizp[t]=matrizp[t] &   0b11011111;                       //si q es cinco, a–adimos al vector matrizp[t] un vector que enciende el led de la sexta fila
    }
    if (q==6){
        matrizp[t]=matrizp[t] & 0b10111111;                         //si q es seis, a–adimos al vector matrizp[t] un vector que enciende el led de la septima fila
    }
    if (q==7){
        matrizp[t]=matrizp[t] &  0b01111111;                         //si q es siete, a–adimos al vector matrizp[t] un vector que enciende el led de la octava fila
    }

}
static void contento(){				//Dibujamos en la matriz de leds una cara contenta cuando ganamos el juego
    matriz[0]=0b11110111;			//Valores que hay que poner en las diferentes columnas para obtener la cara contenta
    matriz[1]=0b00001011;
    matriz[2]=0b00001101;
    matriz[3]=0b11111101;
    matriz[4]=0b11111101;
    matriz[5]=0b00001101;
    matriz[6]=0b00001011;
    matriz[7]=0b11110111;
	TMR1ON=0;						//Se para el convertidor
	TMR2ON=0;						//Se para de contar
}
static void triste(){     			//Dibujamos la cara triste en la matriz de leds cuando perdemos el juego
    matriz[0]=0b11111110;			//Valores que se ponen en las diferentes columnas para obtener cara triste
    matriz[1]=0b00001101;
    matriz[2]=0b00001011;
    matriz[3]=0b11111011;
    matriz[4]=0b11111011;
    matriz[5]=0b00001011;
    matriz[6]=0b00001101;
    matriz[7]=0b11111110;
    TMR1ON=0;              			//Se para el convertidor
    TMR2ON=0;             			//Se deja de contar
}
