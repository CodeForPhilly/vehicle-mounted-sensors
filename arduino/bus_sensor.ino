#include "CayenneLPP.h"
#include "DHT.h"
#include"Arduino.h"
#include"AirQuality.h"

#define DHTPIN A3     // A3 is closes to the usb port of Marvin
// define the type of sensor used (there are others)
#define DHTTYPE DHT11   // DHT 11 

AirQuality airqualitysensor;


DHT dht(DHTPIN, DHTTYPE);

// set max size
int     MAX_SIZE = 52;
CayenneLPP lpp(MAX_SIZE);

// Port to assign the type of lora data (any port can be used between 1 and 223)
int     set_port  = 1;

// Some standard ports that depend on the layout of the Marvin
long    defaultBaudRate = 57600;
int     reset_port = 5;
int     RN2483_power_port = 6;
int     led_port = 13;
int     dustPin = 4;
int     current_quality =-1;

unsigned long duration;
unsigned long starttime;
unsigned long loopstarttime;
unsigned long sampletime_ms = 2000;
unsigned long looptime_ms = 3000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

//*** Set parameters here BEGIN ---->
String  set_nwkskey = "db52ba628d2349901429853ad9934194"; // Put your 32 hex char here
String  set_appskey = "d17c13fe63607d83c4e36fe97d6c9768"; // Put your 32 hex char here
String  set_devaddr = "04001E28"; // Put your 8 hex char here
//*** <---- END Set parameters here

// Some global items
String reader = "";

// counter
int   count = 1;

void setup() {
  InitializeSerials(defaultBaudRate);
  initializeRN2483(RN2483_power_port, reset_port);
  Serial.println("STARTUP..." );
  airqualitysensor.init(14);
  pinMode(led_port, OUTPUT); // Initialize LED port
  pinMode(dustPin,INPUT);
  starttime = millis();  
  loopstarttime = millis();
  dht.begin();
  blinky();
}

void loop() {
  
  duration = pulseIn(dustPin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;

  if ((millis()-starttime) >= sampletime_ms)//if the sampel time = = 30s
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=&gt;100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    lowpulseoccupancy = 0;
    starttime = millis();
  }

  if ((millis()-loopstarttime) >= looptime_ms)//if the sampel time = = 30s
  {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int current_quality = airqualitysensor.curr_vol();
    int temp = (int)t;
    int hum = (int)h;  
    int tempdec = t * 100;
    int humdec = h * 100;
    
  //if (buttonState == 1 || button2State == 1) {
    char payload[MAX_SIZE] = "";

    lpp.reset();

    // Stub out payloads if you want.
    
    lpp.addDigitalInput(1, 4);
    lpp.addTemperature(1, temp);
    lpp.addRelativeHumidity(1, hum);
    lpp.addAnalogOutput(1, concentration);
    lpp.addGPS(1, 39.952315, -75.164053, 2);
    //lpp.addAnalogInput(1, current_quality); //packets greater than 11 bytes fail

    uint8_t buff = *lpp.getBuffer();

    Serial.print("Buffer size:" );
    Serial.println(lpp.getSize());

    for (int i = 0; i < lpp.getSize(); i++) 
      {
      char tmp[16];

      sprintf(tmp, "%.2X",(lpp.getBuffer())[i]);
      strcat(payload, tmp);
      }
    if (isnan(t) || isnan(h)) 
      {
        Serial.println("Failed to read from DHT");
      } 
    else 
      {
       
        Serial.print("Humidity: "); 
        Serial.print(h);
        Serial.println(" %\t");
        Serial.print("Temperature: "); 
        Serial.print(t);
        Serial.println(" *C");
        Serial.print("Concentration: ");
        Serial.print(concentration);
        Serial.println(" pcs/0.01cf");
        Serial.print("Quality: ");
        Serial.println(current_quality);
        Serial.print("Buffer content:" );
        Serial.println(payload);
      }
    
    if(count % 10 == 0)
      {
      send_LoRa_data(set_port, payload);
      }
    Serial.println(count);
    Serial.println("");
    
    count = count + 1;

  //} // end if

} // end loop()
}

ISR(TIMER1_OVF_vect)
{
  if(airqualitysensor.counter==61)//set 2 seconds as a detected duty
  {

      airqualitysensor.last_vol=airqualitysensor.first_vol;
      airqualitysensor.first_vol=analogRead(A4);
      airqualitysensor.counter=0;
      airqualitysensor.timer_index=1;
      PORTB=PORTB^0x20;
  }
  else
  {
    airqualitysensor.counter++;
  }
}

void InitializeSerials(long baudrate) {
  Serial.begin(baudrate);
  Serial1.begin(baudrate);
  delay(500);
  print_to_console("Serial ports initialised");
}


void initializeRN2483(int pwr_port, int rst_port) {
  //Enable power to the RN2483
  pinMode(pwr_port, OUTPUT);
  digitalWrite(pwr_port, HIGH);
  print_to_console("RN2483 Powered up");
  delay(500);

  //Disable reset pin
  pinMode(rst_port, OUTPUT);
  digitalWrite(rst_port, HIGH);

  //Configure LoRa module
  send_LoRa_Command("sys reset");
  read_data_from_LoRa_Mod();

  send_LoRa_Command("mac set nwkskey " + set_nwkskey);
  read_data_from_LoRa_Mod();

  send_LoRa_Command("mac set appskey " + set_appskey);
  read_data_from_LoRa_Mod();

  send_LoRa_Command("mac set devaddr " + set_devaddr);
  read_data_from_LoRa_Mod();

  //For this commands some extra delay is needed.
  send_LoRa_Command("mac set adr on");
  //send_LoRa_Command("mac set dr 0"); //uncomment this line to fix the RN2483 on SF12 (dr=DataRate)
  delay(500);
  read_data_from_LoRa_Mod();

  send_LoRa_Command("mac save");
  delay(500);
  read_data_from_LoRa_Mod();

  send_LoRa_Command("mac join abp");
  delay(500);
  read_data_from_LoRa_Mod();

  send_LoRa_Command("radio set crc off");
  delay(500);
  read_data_from_LoRa_Mod();
}

void print_to_console(String message) {
  Serial.println(message);
}

void read_data_from_LoRa_Mod() {
  if (Serial1.available()) {
    String inByte = Serial1.readString();
    Serial.println(inByte);
  }

}

void send_LoRa_Command(String cmd) {
  print_to_console("Now sending: " + cmd);
  Serial1.println(cmd);
  delay(500);
}

void send_LoRa_data(int tx_port, String rawdata) {
  // send_LoRa_Command("mac tx uncnf " + String(tx_port) + String(" ") + rawdata);
  send_LoRa_Command("mac tx cnf " + String(tx_port) + String(" ") + rawdata);
}

void blinky()
{
  digitalWrite(led_port, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                     // wait for a second
  digitalWrite(led_port, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                     // wait for a second
  digitalWrite(led_port, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                     // wait for a second
  digitalWrite(led_port, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                     // wait for a second

}


