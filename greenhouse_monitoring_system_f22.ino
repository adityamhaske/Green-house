// This arduino is mainly responsible for handling water levels and ensure authentication
#include <Servo.h>  // This will allow us to use servo motor for pumping
#include <Keypad.h>// This will allow keypad based auth into device

// These are general keypad configuration
byte rows = 4; 
byte cols = 4; // 4*4 keypad setup
char keys[4][4] = { // These represent each key in keypad as 2D array
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte rowPins[4] = {9, 8, 7, 6}; // represents pins for map to row
byte colPins[4] = {5, 4, 3, 2}; // represents pins for map to col

// This represents an actual instance of keypad
Keypad kp = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols); // requires byte interface

// This represents an instance of servo motor controllor
Servo svr;

// These represent various pins to be used by arduino
char gas = A0; // used to read gas sensor value
int echo = 10; // used to read distance sensor reading
int trig = 11; // used to send pulse 
int serv = 12; // controls servo angle
int buzz = 13; // sends buzziness

// Static declarations
String password= "0000"; // contains the master key
String pressed=""; // contains the key entered

float distance; // stores distance reading from ultrasonic sensor
float gaser; // stores gassed value from gas sensor

int flag = 0; // trigger for authentication

// Function declarations
float measureDistance(int, int); // measures distance
float measureSmoke(int); // measures smoke
void checkKeyPad(); // makes auth mandatory
void setup() {
    Serial.begin(1156200); // serial monitor for communication  pinMode(trig, OUTPUT); // sends trigger signal
  	Serial.flush();  
  
  	pinMode(trig, OUTPUT); // sends pulse
  	pinMode(echo, INPUT); // reads echo signal
    pinMode(gas, INPUT); // reads gas value
    pinMode(buzz, OUTPUT); // sets piezo value

    svr.attach(serv); // Attaches instance of servo controller to port controlling motor
}

void loop()
{
    distance = 0.01723 * measureDistance(trig, echo); // 340 * 100 * 0.00001 / 2
  	Serial.print("Distance: ");
  	Serial.print(distance);
  	Serial.println();
    if (distance > 200) { // we only need to pump water if the distance is greater than 200
    	for (int i = 0; i <= 180; i++) {
        	svr.write(i);
            delay(5); //delay of 10 ms to reach the position
        }
      delay(50);
        for (int i = 180; i >= 0; i--) {
            svr.write(i); // pump must return to its original position
            delay(5);
        }
    } else {
        svr.write(0); // so that the pump returns to its original position
    }

    gaser = measureSmoke(gas); // read gas reading
  	Serial.print("Gas: ");
  	Serial.print(gaser);
  	Serial.println();
    if (gaser > 100) {
        for (int i = 200; i <= 600; i += 50) {
            tone(buzz, i, 10); // will allow toned buzzing
            delay(5);
        }
    } else {
        digitalWrite(buzz, 0); // stop buzz
    }
  	svr.write(0);
  	delay(20);
}

// Finds out distance travelled in cms
float measureDistance(int triggerPin, int echoPin) {
    digitalWrite(triggerPin, LOW);
  	delayMicroseconds(2);
  	// Sets the trigger pin to HIGH state for 10 microseconds
  	digitalWrite(triggerPin, HIGH);
  	delayMicroseconds(10);
  	digitalWrite(triggerPin, LOW);
  	// Reads the echo pin, and returns the sound wave travel time in microseconds
  	return pulseIn(echoPin, HIGH);
}

// Finds out the quantity of smoke in air
float measureSmoke(int pin) {
    return analogRead(pin);
}

/*************
* This function served as authentication, however is
* non-functional due to no serial communication
**************/

// Listens for key press event
void checkKeyPad() {
    char key = kp.getKey();
    if (int(key) != 0) {
        if (key == 'C') { // This will allow us to clear our input
     	    pressed = "";
      	    delay(100); // To clear any cache
        } else if (key == '*') { // This will allow us to submit pwd for checking
          if (pressed == password) {
                flag = 1; // will exit check loop
            }
          Serial.write(char('w'));
        } else {
     	    pressed.concat(key);  // store entered stuff
            Serial.write(char(key));
        }
    }
}

