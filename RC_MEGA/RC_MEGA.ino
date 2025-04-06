//Napravio Fran Klasić 3.F

#include <SPI.h>
#include <nRF24L01.h> //Library za NRF24L01
#include <RF24.h>

#include<Servo.h> //Library za servo motor

#include <string.h> //Library za raspakiranje naredbe od Arduino UNO-a

//Pin za servo motor
int ServoPin = 11;
//Pin za zujalicu
int Piezo = 10;
//Pinovi za L298N Motor Driver
int IN1 = 2;
int IN2 = 3;
int IN3 = 4;
int IN4 = 5;
int ENA = 8; //Max 128
int ENB = 9; //Max 128
//Pinovi za NRF24L01
int CNS = 12;
int CE = 13;

Servo myServo; //Inicijaliziramo servo
RF24 radio (CE, CNS); //Inicijaliziramo NRF24L01

const byte address[6] = "00001"; //Postavimo adresu NRF24L01

void setup() {
  Serial.begin(9600);

  myServo.attach(ServoPin); //Inicijaliziramo servo sa njegovim pinom

  if (!radio.begin()) {
    Serial.println("NRF24L01 failed to initialize");
  }

  radio.setAutoAck(true); //Šalje potvrdu da je dobijen paket od Arduino UNO
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);  
  //radio.setDataRate(RF24_250KBPS); 
  radio.startListening(); //Krenemo slušati za pakete

  //Inicijaliziramo pinove
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  //Postavimo konstantnu vrijednost za brzinu motora
  //U L298N smo postavili 9V bateriju, 128 na PWM pin će dati otprilike 4.5V svakom motoru
  analogWrite(ENA, 128);  //Motor A brzina (50% duty cycle)
  analogWrite(ENB, 128);  //Motor B brzina (50% duty cycle)
}

char receivedData[32]; //Varijabla u koju spremamo paket od Arduino UNO
unsigned long lastReceivedTime = 0;
const unsigned long timeout = 3000;  //3 seconds timeout

void loop() {
  if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData));
    Serial.print("Received: ");
    Serial.println(receivedData);
    lastReceivedTime = millis();
  }

  char* token = strtok(receivedData, ","); //Podijeli dobijeni paket u manje tokene
  int speed = atoi(token); //Pretvori string u integer

  token = strtok(NULL, ","); //Podijeli dobijeni paket u manje tokene
  int rotation = atoi(token); //Pretvori string u integer
  myServo.write(rotation); //Postavimo rotaciju servo motora
  delay(50); //Kako bi se servo motor mogao okrenuti, dodao sam delay

  token = strtok(NULL, ","); //Podijeli dobijeni paket u manje tokene
  char honkStatus[10]; 
  strcpy(honkStatus, token); //Kopiramo vrijednost tokena u honkStatus

  if(speed == 0){
    stopMotor(); //Ako je brzina 0 onda ne želimo da se motor pokreće pa zovemo funkciju stopMotor()
  }
  if(speed > 0){
    forwardMotor(); //Ako je brzina veća 0 onda želimo da se motor pokreće unaprijed pa zovemo funkciju forwardMotor()
  }
  if(speed < 0){
    backwardMotor(); //Ako je brzina manja 0 onda želimo da se motor pokreće unatrag pa zovemo funkciju backwardMotor()
  }
  //Funkcije za odrediti smjer motora su definirane kasnije

  if (strcmp(honkStatus, "Honk") == 0) {
    tone(Piezo,200,100); //Ako je honkStatus jednak stringu "Honk" onad želimo da se zujalica aktivira
  }
  if (millis() - lastReceivedTime > timeout) {
    Serial.println("⚠️ No message received for 3 seconds!"); //Ako nismo dobili poruku za timeout milisekundi (3000) pošalje poruku na Serial monitor
  } 
  honkStatus[0] = '\0'; //Ispraznimo honkStatus
  receivedData[0] = '\0'; //Ispraznimo recievedData
}

void forwardMotor(){ //Postavi L298N tako da se oba dva motora kreću prema naprijed
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void backwardMotor(){ //Postavi L298N tako da se oba dva motora kreću unatrag
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void stopMotor(){ //Postavi L298N tako da se oba dva motora prestanu kretati
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
