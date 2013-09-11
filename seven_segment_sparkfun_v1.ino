#define DISPLAY_BRIGHTNESS  3000

#define DIGIT_ON  HIGH
#define DIGIT_OFF LOW

#define SEGMENT_ON  HIGH
#define SEGMENT_OFF LOW

//Intervalo para fazer sampling do adc
long virtualmillis = 0;   // millis virtual
long resetmillis = 0;     // offset 
long interval = 50;    // intervalo de 50ms

//Intervalo para calcular o ntc
long virtualmillis_2 = 0;   // millis virtual
long resetmillis_2 = 0;     // offset 
long interval_2 = 2000;    // intervalo


float Vin=5.0;     // [V]        
float Raux=10000;  // [ohm]      Resistor auxiliar
float R0=2500;    // [ohm]      valor nominal do RTC a 25ºC

float T0=298.15;   // [K] (25ºC)

float Vout=0.0;    // [V]        Tensão de saída do divisor
float Rout=0.0;    // [ohm]      Valor de resistência do NTC

//Ver Datasheet ou usar um multimetro e medir a resistencia do NTC nas duas temperaturas
float T1=273;      // [K]        Temperatura de calibração 1
float T2=373;      // [K]        Temperatura de calibração 2
float RT1=35563;   // [ohms]     Resistência medida na temperatura de calibração 1
float RT2=549;    // [ohms]     Resistência medida na temperatura de calibração 2

float beta=0.0;    // [K]        parâmetro Beta
float Rinf=0.0;    // [ohm]      parâmetro Rinf
float TempK=0.0;   // [K]        Temperatura de saída em Kelvin
float TempC=0.0;   // [ºC]       Temperatura de saída em °C

int digit1 = 0; //PWM Display pin 1
int digit2 = 0; //PWM Display pin 2
int digit3 = 4; //PWM Display pin 6
int digit4 = 3; //PWM Display pin 8

int segA = 13; //Display pin 14
int segB = 12; //Display pin 16
int segC = 17; //Display pin 13
int segD = 18; //Display pin 3
int segE = 11; //Display pin 5
int segF = 15; //Display pin 11
int segG = 16; //Display pin 15
int degP =  2;

const int numReadings = 15;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


void setup() {   

//parâmetros
  beta=(log(RT1/RT2))/((1/T1)-(1/T2));
  Rinf=R0*exp(-beta/T0);
  
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);
  
  pinMode(degP, OUTPUT);

  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);
  
 for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;          
  
}

void loop() {
  
  virtualmillis = millis() - resetmillis;   // o millis virtual é igual ao millis real menos o offset
  if (virtualmillis > interval){            // quando o millis for maior que o intervalo, ou seja maior
    resetmillis = millis();                 // faça isso, no caso o offset agora é igual ao millis real 
    
  // subtract the last reading:
  total= total - readings[index];         
  // read from the sensor:  
  readings[index] = analogRead(5); 
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;
  
  if (index >= numReadings){              
  // ...wrap around to the beginning: 
  index = 0;       
   }
  // calculate the average:
  average = total / numReadings;
  }
  
  virtualmillis_2 = millis() - resetmillis_2;   // o millis virtual é igual ao millis real menos o offset
  if (virtualmillis_2 > interval_2){            // quando o millis for maior que o intervalo, ou seja maior
  resetmillis_2 = millis();                 // faça isso, no caso o offset agora é igual ao millis real 
  
  //Calculo da resistência do NTC
  Vout=Vin*((float)(average)/1024.0);
  Rout=(Raux*Vout/(Vin-Vout));

  //Calculo da temperatura
  TempK=(beta/log(Rout/Rinf));
  TempC=TempK-273.15;
  }
  
  displayNumber(TempC);
  printdegree();
 
}

void printdegree(){
    
    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
    //Set segments
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    //Turn on deg digit
    digitalWrite(degP, DIGIT_ON);
    delayMicroseconds(DISPLAY_BRIGHTNESS);
    
    //Turn off all segments
    lightNumber(10); 
    //Turn off deg digit
    digitalWrite(degP, DIGIT_OFF);
    
}

void displayNumber(int toDisplay) {


  long beginTime = millis();

  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      break;
    case 2:
      digitalWrite(digit2, DIGIT_ON);
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
      break;
    case 4:
      digitalWrite(digit4, DIGIT_ON);
      break;
    }

    //Turn on the right segments for this digit
    lightNumber(toDisplay % 10);
    toDisplay /= 10;

    delayMicroseconds(DISPLAY_BRIGHTNESS); 
    //Display digit for fraction of a second (1us to 5000us, 500 is pretty good)

    //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
  }

  while( (millis() - beginTime) < 10) ; 
  //Wait for 20ms to pass before we paint the display again
}


//Given a number, turns on those segments
//If number == 10, then turn off number
void lightNumber(int numberToDisplay) {



  switch (numberToDisplay){

  case 0:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 1:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 2:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 3:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 4:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 5:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 6:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 7:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 8:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 9:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 10:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;
  }
}
