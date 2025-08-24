#include <Arduino.h>

/**  * 
 * 
 * @file STEPPER_02_r01.ino 
 * 
 * @mainpage Controle de motor - CNC Shield
 * 
 *  
 * @section description Descrição
 * Controla um motor de passos utilizando um arquino Shield (eixo X) ou os pinos 2 (CLK) e 5 (DIR)
 * @version 02 - Revisão 01
 * @author Márcio Henrique Diniz Marques 
 * 
*/


/** * @brief Pino de Clock
 * Passo eixo X */
#define stepPin 2


/** * @brief Pino de Direção 
 * Direção eixo X */
int dirPin = 5;   


unsigned int Vf = 150; /**< @brief Velocidade Final em mm/s - Padrão = 150 mm/s*/
unsigned int ac = 1500; /**< @brief Aceleração em mm/s^2 (igual na Frenagem) - Padrão = 1500 mm/s^2 */ 
unsigned int Rmotor = 800; /**< @brief Resolução de Trabalho do motor de passos em Passos/Revolução*/
unsigned int NDentes = 20; /**< @brief Número de dentes tanto para Engrenagem + Cremalheira quanto para Polias Sincronizadas*/

/*! @brief Seleciona o Sentido de Rotação do Motor de Passos.
 *  As entradas possíveis são mostrados abaixo:
 *  - sentido = 0 --> Anti-horário
 *  - sentido = 1 --> Horário
 * @attention OBS.: Outros valores serão ignorados.
*/
int sentido = 0;

/*! @brief Seleciona o Tipo de Junta a ser utilizada.
 *  As entradas possíveis são mostrados abaixo:
 *  - tipojunta = 0 --> Fuso + Porca 
 *  - tipojunta = 1 --> Pinhão + Cremalheira (Não implementado!!)
 *  - tipojunta = 2 --> Polia + Correia Dentada
 * @attention OBS.: Outros valores serão ignorados.
*/
int tipoJunta = 0;

/// @brief Passo do Fuso em mm/revolução
float Passo = 10;

/// @brief Deslocamento Total em mm
float DELTA_S = 200;

/** @brief Move o motor do eixo com base em parâmetros de aceleração e velocidade.
 * @details Esta função calcula o movimento do motor, incluindo fases de aceleração,
 * velocidade constante e desaceleração, com base nos parâmetros de entrada.
 * @param [in] tipoJ Tipo de junta (0: Fuso+Porca, 1: Pinhão+Cremalheira, 2: Polia+Correia).
 * @param [in] sentido Direção da rotação (0: Anti-horário, 1: Horário).
 * @param [in] Vf Velocidade final do motor.
 * @param [in] ac Aceleração do motor.
 * @param [in] DELTA_S Deslocamento desejado.
 * @param [in] Rmotor Resolução do motor em passos/revolução.
 * @param [in] Passo Passo do fuso/cremalheira/polia.
 * @param [in] Z Número de dentes (para polia e pinhão). */
void move_Eixo(int tipoJ, int sentido, float Vf, float ac, float DELTA_S, unsigned int Rmotor, float Passo, unsigned int Z) 
{
  if (sentido == 0) 
        digitalWrite(dirPin,HIGH); 
        else
        digitalWrite(dirPin,LOW); 

unsigned int Npassos; /* Número de Passos durante a Aceleração e Frenagem */
unsigned int Npassos_cte; /* Número de Passos durante o trecho em velocidade constante */

/* Determina se haverá um trecho em velocidade constante
 * Valores Possíveis
 *  - V_cte = true --> Gráfico com 3 trechos (Aceleração + Movimento Retilíno Uniforma + Frenagem) 
 *  - V_cte = false --> Gráfico com 2 trechos (Aceleração + Frenagem)
*/
boolean V_cte = false;

//-------------------------------
float V_Calc = sqrt(ac * DELTA_S); /* Velocidade Calculada pela Equação de Torricelli */
float DELTA_ta; /* Delta_T  (em segundos) durante a aceleração e, consequentemente a freangem*/
float DELTA_Sa; /*  DElta_S (em milímetros) durante a aceleração e, consequentemente a freangem*/
float DELTA_S_cte;/* DElta_S (em milímetros) durante o trecho em velocidade constante*/


if (V_Calc > Vf)
{
  // Ultrapassa Vf --> Gráfico V/T com 3 trechos
  V_cte = true; 
  
  Serial.println("--> TRÊS trechos");
   DELTA_ta = (float)Vf / (float)ac;
   DELTA_Sa = (float)Vf * DELTA_ta / 2.0;
   DELTA_S_cte = DELTA_S - (2 * DELTA_Sa);
    }
  else
{
  // Ultrapassa Vf --> Gráfico V/T com 2 trechos
  V_cte = false; 
  Serial.println("--> DOIS trechos");
   DELTA_Sa = DELTA_S / 2;
   Vf = V_Calc;
    }
//-------------------------------
  
if (tipoJ == 0)
    {
    Npassos = DELTA_Sa * (float)Rmotor / Passo;
    Npassos_cte = (unsigned long)(DELTA_S_cte*Rmotor / Passo);
    Z = 1;
    }
  else
    {
      Npassos = DELTA_Sa * (float)Rmotor / (Passo*Z);
      Npassos_cte = (unsigned long)(DELTA_S_cte*Rmotor / (Passo*Z));
    }

float dV = (float)Vf / (float)Npassos;
float T;

for (unsigned long i = 0 ; i < Npassos  ; i++)
{
T = (Passo*Z) / (dV*i*Rmotor);
 digitalWrite(stepPin,HIGH); 
 delayMicroseconds(T*1000000); 
 digitalWrite(stepPin,LOW); 
 delayMicroseconds(T*1000000);
}

if (V_cte)
      for (unsigned long i = 0 ; i < Npassos_cte ; i++)
        {
        digitalWrite(stepPin,HIGH); 
        delayMicroseconds(T*1000000); 
        digitalWrite(stepPin,LOW); 
        delayMicroseconds(T*1000000);
        }
  

for (unsigned long i = Npassos ; i > 0 ; i--)
{
T = (Passo*Z) / (dV*i*Rmotor);
 digitalWrite(stepPin,HIGH); 
 delayMicroseconds(T*1000000); 
 digitalWrite(stepPin,LOW); 
 delayMicroseconds(T*1000000);
}


}

//------------------------------------------------------


/** @brief Função que lê uma string da Serial
 * Caracter por caraceter (tipo char), concatenando-os e retorna uma string (tipo String) */
String leStringSerial(){
  String conteudo = ""; /* Conteúdo da String a ser lida pela serial */
  char caractere; /* Cada caractere lido pel serial */
  
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
/*! @brief Função de configuração
 *  Definições
 * - Pino de CLK
 * - Pino de DIR
 * - Inicia a Seria em 9600 bps 
 */
void setup() {
 // Definindo ambos os pinos acima como saída
 pinMode(stepPin,OUTPUT); 
 pinMode(dirPin,OUTPUT);
 Serial.begin(9600);
 }

//---------------------------------------------
//---------------------------------------------
/** @brief Função Principal
 */ 
void loop() {

// ---- LEITURA (Porta Serial)--------------
 String recebido = leStringSerial(); 
 String check = recebido;
 String dado;


// --- Seta VELOCIDADE --------------------
if (recebido.startsWith("v"))
    {
      dado = recebido.substring(1);
      Vf = dado.toFloat();
      Serial.print("\n--------------\nConfiguração Atualizada: Veloc.final"); 
      Listar();
    }
 
// --- Seta ACELERACAO --------------------
if (recebido.startsWith("a"))
    {
      dado = recebido.substring(1);
      ac = dado.toFloat();
      Serial.print("\n--------------\nConfiguração Atualizada: Aceleração"); 
      Listar();
    }
 
// --- Seta DESLOCAMENTO ------------------
if (recebido.startsWith("d"))
    {
      dado = recebido.substring(1);
      DELTA_S = dado.toFloat();
      Serial.print("\n--------------\nConfiguração Atualizada: DELTA_S"); 
      Listar();
    }
 
// --- Seta Passo -------------------------
if (recebido.startsWith("p"))
    {
      dado = recebido.substring(1);
      Passo = dado.toFloat();
      Serial.print("\n--------------\nConfiguração Atualizada: Passo"); 
      Listar();
    }
 
// --- Seta Rmotor ------------------------
if (recebido.startsWith("m"))
    {
      dado = recebido.substring(1);
      Rmotor = dado.toFloat();
      Serial.print("\n--------------\nConfiguração Atualizada: Rmotor"); 
      Listar();
    }
  
// --- Seta Sentido Rotacao ---------------
if (recebido.startsWith("s"))
    {
      dado = recebido.substring(1);
      sentido = dado.toInt();
      Serial.print("\n--------------\nConfiguração Atualizada: Sentido Rotação"); 
      Listar();
    }
// --- Seta Tipo de Junta -----------------
if (recebido.startsWith("t"))
    {
      dado = recebido.substring(1);
      tipoJunta = dado.toInt();
      Serial.print("\n--------------\nConfiguração Atualizada: Tipo Junta"); 
      Listar();
    }
  
// --- Seta Núm. de Dentes ----------------
if (recebido.startsWith("z"))
    {
      dado = recebido.substring(1);
      NDentes = dado.toInt();
      Serial.print("\n--------------\nConfiguração Atualizada: Núm.Dentes"); 
      Listar();
    }
  
//------------------------------------------
// CONTROLES MOTOR -------------------------
check.toLowerCase();
    if (check == "run")
    {
      Serial.print("\n--------------\nExecutando"); 
      Listar();
    move_Eixo(tipoJunta, sentido, Vf, ac, DELTA_S , Rmotor, Passo, NDentes);
    }  

    if (check == "list")
      Listar();


delay(500);
 
}


/*! @brief Função utilizada para listagem das configurações 
 *
 * - "LISTAR:" 
 *
 *        SINTAXE: list 
 *
 *    Função: Apresenta um lista dos parâmetros para a execução do movimento, são eles:
 *
 *    **Vf** = Velocidade Final/Máxima	[mm/s] 
 *
 *    **Ac** = Aceleração			[mm/s²] 
 * 
 *    **DELTA_S** = O deslocamento total do movimento	[mm] 
 * 
 *    **Rmotor** = A resolução do motor de passos configurada [passos/rev] 
 * 
 *    **Sentido** = Horário/Anti-horário 
 * 
 *    **tipoJunta** = O Tipo de junta empregada, sendo os tipos; 
 * 
 *
 *    - 0: Fuso + Motor 
 *        +  Passo = Passo do Fuso [mm/rev]
 *    - 1: Pinhão + Cremalheira (Não Implementado!!) 
 *    - 2: Polia Sincronizada 
 *	      +  Z = Num. de Dente da polia sincronizada
 *	      +  Passo = Passo da correia [mm]
 *
 * - "EXECUTAR:" Executa o movimento configurado
 * 
 *        SINTAXE: run 
 *
 * - "ALTERAR:" 
 *
 *    - "ACELERACAO:" Altera o valor da Aceleração
 *
 *        SINTAXE: a[valor]
 *
 *        Exemplo: a250
 * 
 *    - "VELOCIDADE:" Altera o valor da Velocidade
 *
 *        SINTAXE: v[valor]
 * 
 *        Exemplo: v125
 * 
 *    - "DESLOCAMENTO:" Altera o valor do DELTA_S
 *
 *        SINTAXE: d[valor]
 * 
 *        Exemplo: d500
 * 
 *    - "SENTIDO:" Altera o sentido de rotação do motor
 *
 *        SINTAXE: s[valor]
 * 
 *        + 0 --> Sentido Anti-Horário
 * 
 *        + 1 --> Sentido Horário
 *
 *        Exemplos:
 *
 *          s0 
 * 
 *          s1
 *
 *    - "PASSO:" Altera o passo do fuso ou correia
 
 *        SINTAXE: p[valor]
 * 
 *        (fuso e correia em milimetros)
 * 
 *        Exemplo:
 *        p10
 *
 *    - "RMOTOR:" Altera a Resolução do Motor de Passo 
 *
 *        SINTAXE: m[valor]
 * 
 *        (em passo/rev)
 * 
 *        Exemplo: m1600
 *
 *    - "TIPOJUNTA:" Altera 0 Tipo de Junta ------
 *
 *        SINTAXE: t[valor]
 *
 *	      + 0: Fuso + Motor (Passo = Passo do Fuso [mm/rev])
 *
 *	      + 1: Pinhão + Cremalheira (Não Implementado!!) 
 *
 *        + 2: Polia Sincronizada 
 *
 *	        Z = Num. de Dente da polia sincronizada
 *		      Passo = Passo da correia [mm]
 *
 *        Exemplos:
 * 
 *        t0
 * 
 *        t1 (Não Implementado)
 * 
 *        t2
 *
 *    - "NUMDENTES:" Altera o Número de Dentes ------
 *
 *        SINTAXE: z[valor]
 * 
 *        Exemplo: z10 
 */
void Listar(void)
{

      Serial.print("\n--------------\nVf = "); Serial.println(Vf);
      Serial.print("Ac = "); Serial.println(ac);
      Serial.print("DELTA_S = "); Serial.println(DELTA_S);
      Serial.print("Rmotor = "); Serial.println(Rmotor);
      Serial.print("Sentido = "); Serial.println(sentido);
      
      switch (tipoJunta)
      {
        case 0:
        Serial.println("tipoJunta = Fuso + Motor");
      break;
        case 1:
        Serial.println("tipoJunta = Pinhão + Cremalheira (Não Implementado!!)"); 
      break;
        case 2:
        Serial.println("tipoJunta = Polia Sincronizada"); 
        Serial.print("Z = "); Serial.println(NDentes);
      break;
        default :
        tipoJunta = 0;
      }
      Serial.print("Passo = "); Serial.println(Passo);
    }
