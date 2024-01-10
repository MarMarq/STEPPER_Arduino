   // Controle de motor - CNC Shield

// Definição dos pinos 
#define stepPin 2 //Passo eixo X
#define dirPin 5 // Direção eixo X


unsigned int Vf = 150;
unsigned int ac = 1500;
unsigned int Rmotor = 800;

float Passo = 10;

float DELTA_S = 200;
//-----------------------------------------------
void move_Eixo(float Vf, float ac, float DELTA_S, unsigned int Rmotor, float Passo) 
{

        Serial.print("\n\n\nVf = "); Serial.println(Vf);
      Serial.print("Ac = "); Serial.println(ac);
      Serial.print("DELTA_S = "); Serial.println(DELTA_S);
      Serial.print("Rmotor = "); Serial.println(Rmotor);
      Serial.print("Passo = "); Serial.println(Passo);

  
unsigned int Npassos;
boolean V_cte = false;

//-------------------------------
float V_Calc = sqrt(ac * DELTA_S);
float DELTA_ta;
float DELTA_Sa;
float DELTA_S_cte;

if (V_Calc > Vf)
{
  V_cte = true; // Ultrapassa Vf --> Gráfico V/T com 3 trechos
  Serial.println("--> 3 trechos");
   DELTA_ta = (float)Vf / (float)ac;
   DELTA_Sa = (float)Vf * DELTA_ta / 2.0;
   DELTA_S_cte = DELTA_S - (2 * DELTA_Sa);
    }
  else
{
  V_cte = false; // NÃO Ultrapassa Vf --> Gráfico V/T com 2 trechos
  Serial.println("--> DOIS trechos");
   DELTA_Sa = DELTA_S / 2;
   Vf = V_Calc;
    }
//-------------------------------
  

Npassos = DELTA_Sa * (float)Rmotor / Passo;

float dV = (float)Vf / (float)Npassos;
float T;

for (unsigned long i = 0 ; i < Npassos  ; i++)
{
T = Passo / (dV*i*Rmotor);
 digitalWrite(stepPin,HIGH); 
 delayMicroseconds(T*1000000); 
 digitalWrite(stepPin,LOW); 
 delayMicroseconds(T*1000000);
}

if (V_cte)
      for (unsigned long i = 0 ; i < (unsigned long)(DELTA_S_cte*Rmotor / Passo) ; i++)
        {
        digitalWrite(stepPin,HIGH); 
        delayMicroseconds(T*1000000); 
        digitalWrite(stepPin,LOW); 
        delayMicroseconds(T*1000000);
        }
  

for (unsigned long i = Npassos ; i > 0 ; i--)
{
T = Passo / ( (float)(dV*i*Rmotor) );
 digitalWrite(stepPin,HIGH); 
 delayMicroseconds(T*1000000); 
 digitalWrite(stepPin,LOW); 
 delayMicroseconds(T*1000000);
}


}

//------------------------------------------------------
//------------------------------------------------------
/**
 * Função que lê uma string da Serial
 * e retorna-a
 */
String leStringSerial(){
  String conteudo = "";
  char caractere;
  
  // Enquanto receber algo pela serial
  while(Serial.available() > 0) {
    // Lê byte da serial
    caractere = Serial.read();
    // Ignora caractere de quebra de linha
    if (caractere != '\n'){
      // Concatena valores
      conteudo.concat(caractere);
    }
    // Aguarda buffer serial ler próximo caractere
    delay(10);
  }
  return conteudo;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void setup() {
 // Definindo ambos os pinos acima como saída
 pinMode(stepPin,OUTPUT); 
 pinMode(dirPin,OUTPUT);
 Serial.begin(9600);
 }

//---------------------------------------------
//---------------------------------------------
 
void loop() {

// ---- LEITURA (Porta Serial)--------------
 String recebido = leStringSerial(); 
 String check = recebido;
 String dado;


// --- Seta VELOCIDADE --------------------
if (recebido.startsWith("v"))
    {
      dado = recebido.substring(1);
//      Serial.print("Vf = "); Serial.println(dado);
      Vf = dado.toFloat();
    }
 
// --- Seta ACELERACAO --------------------
if (recebido.startsWith("a"))
    {
      dado = recebido.substring(1);
//      Serial.print("Ac = "); Serial.println(dado);
      ac = dado.toFloat();
    }
 
// --- Seta DESLOCAMENTO ------------------
if (recebido.startsWith("d"))
    {
      dado = recebido.substring(1);
//      Serial.print("DELTA_S = "); Serial.println(dado);
      DELTA_S = dado.toFloat();
    }
 
// --- Seta Passo -------------------------
if (recebido.startsWith("p"))
    {
      dado = recebido.substring(1);
//      Serial.print("Passo = "); Serial.println(dado);
      Passo = dado.toFloat();
    }
 
// --- Seta Rmotor ------------------------
if (recebido.startsWith("m"))
    {
      dado = recebido.substring(1);
//      Serial.print("Rmotor = "); Serial.println(dado);
      Rmotor = dado.toFloat();
    }
  
//------------------------------------------
// CONTROLES MOTOR -------------------------
check.toLowerCase();
    if (check == "run")
    {
    move_Eixo( Vf, ac, DELTA_S , Rmotor, Passo);
    }  



delay(500);
 
}
