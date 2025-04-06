//Napravio Fran Klasić 3.F

#include <SPI.h>
#include <nRF24L01.h> //Library za NRF24L01
#include <RF24.h>

//Pinovi za NRF24L01
int CE = 8;
int CNS = 7;
//Pinovi za joystick modul
int SW = 6;
int VRx = A0;
int VRy = A1;

RF24 radio(CE, CNS); //Inicijaliziramo NRF24L01

const byte address[6] = "00001"; //Postavimo adresu NRF24L01

void setup() {
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println("NRF24L01 failed to initialize");
  }
  radio.setAutoAck(true); //Omogućuje automatsko slanje potvrde kada se paket uspješno primi od Arduino MEGA
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);  
  //radio.setDataRate(RF24_250KBPS); 
  radio.stopListening(); //Tu trebamo slati pakete, pa ne trebamo slušati za pakete

  //Inicijaliziramo pinove
  pinMode(VRy,INPUT_PULLUP);
  pinMode(VRx,INPUT_PULLUP);
  pinMode(SW,INPUT_PULLUP);
}

void loop() {
  int xPos = analogRead(VRx); //Vrijednost x osi joysticka
  int yPos = analogRead(VRy); //Vrijednost y osi joysticka
  int pressed = digitalRead(SW); //Čitanje statusa tipke (je li pritisnuta)

  int nSpeed = map(xPos,1023,0,256,-256); //Pretvaramo x poziciju u brzinu (-256 do 256)
  //nSpeed bi trbao kontrolirati ENA i ENB pinove za L298N Motor Driver na Arduinu MEGA, na kraju sam postavio ENA i ENB na 128 kako bi bolje radilo
  //Sad nSpeed se koristi samo kao dio uvjeta, ako auto treba ići naprijed, unatrag ili zaustaviti se
  int nRotation = map(yPos,1023,0,0,180); //Pretvaramo y poziciju u kut rotacije (0 do 180)

  if(nSpeed <=30 && nSpeed >= -30){ //Ako je brzina otprilike blizu 0, postavi se na 0 kako bi bilo stabilno
    nSpeed = 0;
  }

  if(nRotation >= 80 && nRotation <= 100){ //Ako je rotacija otprilike 90, postavi se na 90 kako bi bilo stabilno
    nRotation = 90;
  }

  const char* ifHonk = (pressed == 0) ? "Honk" : "NoHonk"; //Provjera ako je stisnuto tipkalo za trubu
  
  //Kreiramo paket s podacima koji će biti poslan (brzina, rotacija, status zujalice)
  char command[32];
  snprintf(command, sizeof(command), "%d,%d,%s", nSpeed, nRotation, ifHonk); //Formatiramo podatke u string

  bool success = radio.write(&command, sizeof(command)); //Pokušavamo poslat paket

  if (success) { //Ako je slanje uspješno, ispisujemo paket na serijski monitor
    Serial.print("Message Sent: ");
    Serial.print(command);
    Serial.println();
  } else { //Ako slanje nije uspjelo, ispisujemo grešku
    Serial.println("Failed to Send!");
  }
}
