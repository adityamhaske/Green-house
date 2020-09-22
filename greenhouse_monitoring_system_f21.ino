// This arduino in general is responsible for air quality assessment in GREENHOUSE MONITORING
#include <LiquidCrystal.h>  // This is to properly utilize LCD Display

// These are used to connect to LCD screen
int regSel = 7; // This will select the register for LCD signalling
int enable = 6; // This will enable the LCD in general
int sig1 = 5; // Signal 1
int sig2 = 4; // Signal 2
int sig3 = 3; // Signal 3
int sig4 = 2; // Signal 4

// These are used to run our heating and cooling elements
int motor1 = 8; // Control clockiwise heater
int motor2 = 9; // Control anticlockwise heater
int motor3 = 10; // Control clockwise cooler
int motor4 = 11; // Control anticlockwise cooler

// This will control the artificial light
int light = 12; // led pin

// These are actual sensors used to detect
char temp = A0; // temperature sensor
char lumos = A1; // photoresistor

// Static declarations for our own use
float temperature; // will hold temp in celcius
float intensity; // will hold light intensity in lu
int flag = 0; // will determine welcome message

// Initialize a object of LCD
LiquidCrystal lcd(regSel, enable, sig1, sig2, sig3, sig4);

// Function declarations
float readTemperature(char); // determines temperature
float readLuminousity(char); // determines lighting
void authenticate(); // whether to allow functions

void setup() {
    // Pins  to which we will write data
    pinMode(motor1, OUTPUT);
    pinMode(motor2, OUTPUT);
    pinMode(motor3, OUTPUT);
    pinMode(motor4, OUTPUT);
    pinMode(light, OUTPUT);

    // Pins from which we will read stuff
    pinMode(temp, INPUT);
    pinMode(lumos, INPUT);

    // Serial monitor to debug stuff
    Serial.begin(1156200); // baud rate of 1156200 for comm
    // LCD to actuall log stuff;
  	Serial.flush();
    lcd.begin(16, 2); // 16*2 grid of lcd
}

void loop() {
  	lcd.clear();
	temperature = readTemperature(temp); // reads temp
    lcd.setCursor(3, 0); // decides lcd position
    lcd.print("Temp: "); // prints supplied string
    lcd.setCursor(9, 0); 
    lcd.print(temperature);
    if (temperature < 20) { // For a tropical greenhouse below 20 is cold
    	lcd.setCursor(4, 1);
    	lcd.print("TOO COLD"); 
        digitalWrite(8, HIGH); // enables heater
        digitalWrite(10, LOW); // disables cooler
    } else if (temperature > 27) { // Above 27 is hot for a temperate greenhouse
        lcd.setCursor(4, 1); 
    	lcd.print("TOO HOT");
        digitalWrite(8, LOW); // disables heater
        digitalWrite(10, HIGH); // enables cooler
    } else {
      	lcd.setCursor(4, 1); 
    	lcd.print("PERFECT");
        digitalWrite(8, LOW); // disables both
        digitalWrite(10, LOW);
    }
    digitalWrite(9, LOW); // prevents anticlockwise motion
    digitalWrite(11, LOW);
    delay(150);
    lcd.clear();
    intensity = readLuminousity(lumos); // reads light
    lcd.setCursor(1, 0);
   	lcd.print("Intensity: "); 
    lcd.setCursor(12, 0);
   	lcd.print(intensity); 
    if (intensity < 100) { // If it is too dark outside
    	lcd.setCursor(4, 1);
    	lcd.print("TOO DIMM");
        digitalWrite(light, HIGH); // enable lighting
    } else  if (intensity > 180) { // It is morning now
        lcd.setCursor(4, 1);
    	lcd.print("TOO BRIGHT");
    } else {
      	lcd.setCursor(4, 1);
    	lcd.print("PERFECT");
    }
    delay(150);
    digitalWrite(light, LOW); // Light is kept at blinking state to signify lighting
}

// This function reads temperature and converts to celcius
float readTemperature(char pin) {
    float volts = analogRead(pin); // this value is in terms of milivolts 8BCD enncoding
    float value = map(((volts - 20) * 3.04), 0, 1023, -40, 125); // this we get proper range of -40C to 125C
    return value;
}

// This function reads light and converts to luminousity
float readLuminousity(char pin) {
    float lum = analogRead(pin); //  this value is likely b/w 0 - 1023
    float value = map(lum, 0, 700, 0, 255); // this will be more readble for light intensity
    return value;
}

/**************
* This function colud be used to unlock device post auth
* however serial comm is disabled
***************/
// This function will toggle flag 
void authenticate() {
  lcd.setCursor(1,0);
  if (char(Serial.read()) == 'w')
  lcd.print('y');
  else
  lcd.print('n');
  
}
