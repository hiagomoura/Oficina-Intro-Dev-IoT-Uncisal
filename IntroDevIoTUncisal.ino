/*Oficina Reboot TSI Introdução a programação de dispositivos IoT
Por Hiago Moura
*/

#define pinReleLampada 7 // Pino no qual o módulo relé está conectado a lampada.
#define pinReleMotor 6   // Pino no qual o módulo relé está conectado ao motor.
#define pinLDR A0        // Pino do LDR
#define pinTMP36 A3		 // Pino do TMP36
#define pinPIR 13
#define nivelLuz 500     // Nivel de luz de referencia
#define nivelTemperatura 35 // Nivel de temperatura de referencia

String comando;      // Variável para ler os dados recebidos pela serial
bool escuro = false; // Variável que recebe resultado da comparação da luz atual no ambiente captada pelo sensor em relação ao valor de referencia.

bool modoAutomatico = true; // Habilita e desabilita o modo automático, ou seja, o modo em que o projeto é comandado por sensores.

/* Sobre o modo automático

O modo automático (comandado por sensores, só será desativado ao solicitar via comando específico (DMA - Desabilitar modo automático),
Ou ao ser utilizados quaisquer comandos VÁLIDOS na Serial,caso contrário, todos os sensores continuarão a funcionar.
*/

bool comandoValido; // Flag indicadora de comando válido(true) e inválido(false).

void setup()
{
  Serial.begin(9600);                // Inicializa a comunicação serial em 9600bps
  pinMode(pinReleLampada, OUTPUT);   // Seta o pino do relé da lampada como saída.
  pinMode(pinReleMotor, OUTPUT);     // Seta o pino do relé do Motor como saída.
  pinMode(pinPIR,INPUT);			 // Seta o pino do sensor de presença PIR como entrada.
  digitalWrite(pinReleLampada, LOW); // Seta o pino do relé da lampada com nível lógico baixo.
  digitalWrite(pinReleMotor, LOW);   // Seta o pino do relé do motor com nível lógico baixo.
}

void loop()
{

  if (modoAutomatico)
  {
    // Se o modo automático estiver habilitado (true), todos os sensores devem ser habilitados, e portanto serem considerados na programação.

    // Habilitando todos os sensores:
    
    bool presencaDetectada = digitalRead(pinPIR);

    // Habilitando Sensor de Luz (LDR)

    int valorLDR = analogRead(A0);
    escuro = valorLDR <= nivelLuz;

    Serial.print("Nivel de luz LDR: ");
    Serial.println(valorLDR);

    if (escuro && presencaDetectada)
    {
      digitalWrite(pinReleLampada, HIGH); // Aciona o pino do relé disparando o mesmo. (NF ---> NA), ou seja, ligando a lampada.
    }
    else
    {
      digitalWrite(pinReleLampada, LOW); // Desaciona o pino do relé (NA ---> NF), ou seja, desligando a lampada.
    }
    
    // Habilitando Sensor de Temperatura
    
    float valorTMP36 = analogRead(A3);
    
    float tensaoTMP36 = 5 * valorTMP36/1023;//Realizando conversão do valor digital de entrada da porta A3 (0 - 1023) vinda do ADC do Arduino, para um valor analógico de tensão em V.
    
    Serial.print("Valor de tensao TMP36: ");
    Serial.print(tensaoTMP36);
    Serial.println(" V");
    
    float temperatura = (tensaoTMP36 - 0.5) * 100;
    Serial.print("Nivel de temperatura TMP36: ");
    Serial.print(temperatura);
    Serial.println(" Celsius");
    
    bool calor = temperatura >= nivelTemperatura;
    
    if(calor && presencaDetectada){
      digitalWrite(pinReleMotor, HIGH); // Ligo o motor.
    }
    else{
      digitalWrite(pinReleMotor, LOW);
    }
    
  }

  if (Serial.available())
  {
    // Esse if com essa condição verifica se tem algum dado na Serial.
    comando = Serial.readStringUntil('\n'); // Faz a leitura dos dados do buffer da serial até que seja pressionado enter.

    // Checa se são comandos de habilitação dos sensores, senão testa os outros comandos.

    if (comando == "DMA")
    {
      // DMA - Desabilitar Modo Automático - Todos os sensores desabilitados.
      modoAutomatico = false;
      
      //Desabilitando pino do relé da lampada.
      digitalWrite(pinReleLampada, LOW); // Desaciona o pino do relé (NA ---> NF), ou seja, desligando a lampada.
      
      //Desabilitando pino do relé do motor.
      digitalWrite(pinReleMotor, LOW);
      
      Serial.println("Modo automatico foi desabilitado, todos os sensores foram desabilitados (OFF).");
      
    }
    else if (comando == "HMA")
    {
      // HMA - Habilitar Modo Automático - Todos os sensores habilitados.
      if (modoAutomatico)
      {
        Serial.println("O modo automático ja esta habilitado atualmente.");  
      }
      else
      {
        modoAutomatico = true;
        Serial.println("Modo automatico foi habilitado, todos os sensores foram habilitados (ON).");
      }
    }
    else
    {
      // Pra qualquer um dos comandos VÁLIDOS: Modo automatico desativado.
      comandoValido = true;

      if (comando.equalsIgnoreCase("LL"))
      {
        digitalWrite(pinReleLampada, HIGH); // Aciona o pino do relé disparando o mesmo. (NF ---> NA), ou seja, ligando a lampada.
      }
      else if (comando.equalsIgnoreCase("DL"))
      {
        digitalWrite(pinReleLampada, LOW);
      }
      else if (comando.equalsIgnoreCase("LM"))
      {
        digitalWrite(pinReleMotor, HIGH);
      }
      else if (comando.equalsIgnoreCase("DM"))
      {
        digitalWrite(pinReleMotor, LOW);
      }
      else if (comando.equalsIgnoreCase("DT"))
      {
        digitalWrite(pinReleLampada, LOW);
        digitalWrite(pinReleMotor, LOW);
      }
      else if(comando.equalsIgnoreCase("LT"))
      {
        digitalWrite(pinReleLampada, HIGH);
        digitalWrite(pinReleMotor, HIGH);
      }
      else
      {
        comandoValido = false;
        Serial.println("Comando inexistente!");
      }

      if (comandoValido)
      {
        modoAutomatico = false;
      }
    }
  }
}