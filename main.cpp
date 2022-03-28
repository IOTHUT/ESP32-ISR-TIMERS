#include <Arduino.h>

hw_timer_t * timer0 = NULL; //definicao timer0
hw_timer_t * timer1 = NULL; //definicao timer1
portMUX_TYPE timerMux0 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;



#define led_pin     2                   //led pin 


#define tempo_t0    500                 //estouro do timer0 em ms
#define tempo_t1    1000                //estouro do timer1 em ms 
#define tempo_delay 5000                //delay para loop em ms



volatile boolean led1Aux = false;       //variavel auxiliar para led



//---------- prototipo de funcao ----------
void setupTimers();


//---------- Interrupt Service Routine ----------
void IRAM_ATTR onTimer0(){ 
  portENTER_CRITICAL_ISR(&timerMux0);
  
  led1Aux = !led1Aux;               // inverte estado da variavel
  digitalWrite(led_pin, led1Aux);   // inverte estado do led

  Serial.println("interrupt --> timer 0"); //debug

  portEXIT_CRITICAL_ISR(&timerMux0);
}

void IRAM_ATTR onTimer1(){ 
  portENTER_CRITICAL_ISR(&timerMux1);

  Serial.println("interrupt --> timer 1"); //debug apenas

  portEXIT_CRITICAL_ISR(&timerMux1);
}//end interrupt timer1




void setup(){

  Serial.begin(115200);     //inicia serial
  pinMode(led_pin, OUTPUT); //definicao de hardware

  setupTimers();            //deve ser chamado apos a definicao de hardware 

}

void loop(){

  Serial.println("main loop --> delay " + String(tempo_delay)  + " ms"); //debug apenas
  delay(tempo_delay);   
    
}




//configuracao dos timers utilizados
void setupTimers(){

//------------ configuracao timer0 ------------
  Serial.println("timer 0 --> start");              //debug

  timer0 = timerBegin(0, 80, true);                 // atribui configuracoes do timer a "variavel" timer0
                                                    // valor de 80 se deve aos 80 MHz do APB_CLK e entrega uma base de 1us
                                                    // este valor de 80 é o nosso fator de divisao ou nosso prescaler 

  timerAttachInterrupt(timer0, &onTimer0, true);    // configura interrupcao do timer0 com servico de rotina (onTimer0)

  timerAlarmWrite(timer0, (tempo_t0*1000), true);   // configura periodicidade para execucao da ISR (interrupt service routine)
                                                    // mantenham como true o ultimo parametro para que funcione de forma "automatica"
                                                    // no futuro iremos detalhar cada uma dessas funcionalidades
//------------ configuracao timer0 ------------



//------------ configuracao timer1 ------------
//mesma logica aplicada no exemplo anterior (timer0)

// 12.5ns --> 1/APB_CLK = 1/80 MHz

  Serial.println("timer 1 --> start");              //debug
  timer1 = timerBegin(1, 80, true);                 // timer 1, 12.5ns * 80 = 1000ns = 1us, true = incremento
  timerAttachInterrupt(timer1, &onTimer1, true);    // timer, funcao para rotina de interrupcao, edge = true 
  timerAlarmWrite(timer1, (tempo_t1*1000), true);   // (tempo_t1*1000) * 1 us = tempo_t1 ms, autoreload true

//------------ configuracao timer1 ------------




  // por ultimo, habilita o timerAlarm (interrupcao entra em "acao" apos esta chamada)
  timerAlarmEnable(timer0); // habilita timer0
  timerAlarmEnable(timer1); // habilita timer1

}
