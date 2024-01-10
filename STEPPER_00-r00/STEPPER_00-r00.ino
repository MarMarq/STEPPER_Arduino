// Controle de motor - CNC Shield

// Definição dos pinos 
#define stepPin 2 //Passo eixo X
int dirPin = 5; // Direção eixo X

void setup() {
 // Definindo ambos os pinos acima como saída
 pinMode(stepPin,OUTPUT); 
 pinMode(dirPin,OUTPUT);
 Serial.begin(9600);
 }

//---------------------------------------------
void loop() {
digitalWrite(dirPin,HIGH);
for (unsigned long i = 1600 ; i > 0 ; i--)
{
float T = 0.000035;
 digitalWrite(stepPin,HIGH); 
 delayMicroseconds(T*1000000); 
 digitalWrite(stepPin,LOW); 
 delayMicroseconds(T*1000000);
}

delay(2000);
}
