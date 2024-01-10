   // Controle de motor - CNC Shield

// Definição dos pinos 
#define stepPin 2 //Passo eixo X
#define dirPin 5 // Direção eixo X

unsigned int tonoff = 375;
unsigned int Npassos = 800;
 
 void setup() {
 // Definindo ambos os pinos acima como saída
 pinMode(stepPin,OUTPUT); 
 pinMode(dirPin,OUTPUT);
 Serial.begin(9600);
 }
 void loop() {

for (int i = 1000; i > 50 ; i=i-50)
{
  
 // Habilita o motor para que se movimente em um sentido particular
 digitalWrite(dirPin,HIGH);
 Serial.print("Valor de i = ");
 Serial.println(i);

// Conta 1600 pulsos para que o motor gire
  for(int x = 0; x < Npassos; x++) {
 digitalWrite(stepPin,HIGH); 
 delayMicroseconds(i); 
 digitalWrite(stepPin,LOW); 
 delayMicroseconds(i); 
 }

 //Aguarda 1 segundo antes de executar a próxima instrução
 delay(1000);

 //Inverte o sentido de rotação
 digitalWrite(dirPin,LOW);

 // Conta 600 passos para que o motor gire
 for(int x = 0; x < Npassos; x++) {
 digitalWrite(stepPin,HIGH);
 delayMicroseconds(i);
 digitalWrite(stepPin,LOW);
 delayMicroseconds(i);
 }

//Aguarda 1 segundo e reinicia o loop
 delay(1000);

}
 
}
