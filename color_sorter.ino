#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// Define color sensor pins
#define S0 4
#define S1 2
#define S2 13
#define S3 7
#define SENSOR_OUT 8

// Define positions for servomotors
#define SENSOR_POS 50
#define TOP_MIN 10
#define TOP_MAX 100

// Define RGB LED pins
#define RED_PIN 3
#define GREEN_PIN 5
#define BLUE_PIN 6

// Define the position of each colored container
#define RED_POS 0
#define GREEN_POS 35
#define YELLOW_POS 70
#define BLUE_POS 105
#define BROWN_POS 140

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buzzerPin = A0;  // Pin A0 connected to the buzzer

// Set the initial position for the top servomotor
int pos = 0;

// Calibration Values
// Get these from Calibration Sketch

int redMin = 32; // Red minimum value
int redMax = 198; // Red maximum value
int greenMin = 42; // Green minimum value
int greenMax = 239; // Green maximum value
int blueMin = 33; // Blue minimum value
int blueMax = 209; // Blue maximum value

// Variables for color pulse width measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

// Variables for the number of sorted candies (for each color)
int orangeNum = 0;
int pinkNum = 0;

// Top and bottom servomotors
Servo top;
Servo bottom;

void setup()
{
  Serial.begin(9600);

  // Initialize the servomotors
  top.attach(10);
  bottom.attach(9);

  // Set S0 - S3 as outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // Set RGB LED pins as outputs
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Set Sensor output as input
  pinMode(SENSOR_OUT, INPUT);

  // Set Frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Initialize the LCD
  lcd.init();
  // Print a "HELLO" message to the LCD
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Hi, there! :)");
  lcd.setCursor(0, 1);
  lcd.print("I'm Sweet Sort");

  pinMode(buzzerPin, OUTPUT);  // Set the buzzer pin as an output
}

void loop()
{
  // Variables for final color values

  int redValue = 0;
  int greenValue = 0;
  int blueValue = 0;

  int redValuePW;
  int greenValuePW;
  int blueValuePW;

  String line1 = "";
  String line2 = "";

  // Rotate the servomotor to remove the candies from the tube
  for (pos = 10; pos <= 100; pos += 1)
  {
    // Tell servo to go to position in variable 'pos'
    top.write(pos);
    // Wait 15 ms for servo to reach the position
    delay(15);
  }
  // Wait for the candy to fall out of the tube
  delay(500);

  // Rotate the servomotor to the color sensor
  for (pos = TOP_MAX; pos >= TOP_MIN; pos -= 1)
  {
    top.write(pos);
    if (pos == SENSOR_POS)
    {
      // The candy is in front of color sensor
      // Delay to stabilize sensor
      delay(1000);

      // Read Red value
      redPW = getRedPW();
      Serial.print("Red PW: ");
      Serial.print(redPW);
      Serial.print(" ");
      redValuePW = map(redPW, redMin,redMax,255,0);
      // Delay to stabilize sensor
      delay(1000);

      // Read Green value
      greenPW = getGreenPW();
      Serial.print("Green PW: ");
      Serial.print(greenPW);
      Serial.print(" ");
      greenValuePW = map(greenPW, greenMin,greenMax,255,0);
      // Delay to stabilize sensor
      delay(1000);

      // Read Blue value
      bluePW = getBluePW();
      Serial.print("Blue PW: ");
      Serial.print(bluePW);
      Serial.println();
      blueValuePW = map(bluePW, blueMin,blueMax,255,0);
      // Delay to stabilize sensor
      delay(1000);

      // Parameters for ORANGE color
      if ((redPW >= 65 && redPW <= 80) &&
          (greenPW >= 64 && greenPW <= 85) &&
          (bluePW >= 60 && bluePW <= 85))
      {
        // Set values for RGB LED
        redValue = 255;
        greenValue = 30;
        blueValue = 10;

        playTone(1000, 1000);  // Play a tone with a frequency of 1000Hz for 1 second

        // Set position for bottom servomotors
        bottom.write(180);

        // Increment the number of yellow candies
        orangeNum = orangeNum + 1;
      }
      else {
        if ((redPW >= 50 && redPW <= 100) &&
          (greenPW >= 50 && greenPW <= 120) &&
          (bluePW >= 50  && bluePW <= 130))
        {
          // Set values for RGB LED
          redValue = 255;
          greenValue = 10;
          blueValue = 150;

          playTone(1000, 1000);  // Play a tone with a frequency of 1000Hz for 1 second

          // Set position for bottom servomotors
          bottom.write(30);

          // Increment the number of yellow candies
          pinkNum = pinkNum + 1;
        }
      }

      // Compute the message for LCD
      line1 += "ORANGE:";
      line1 += orangeNum;  
      line2 += " PINK:"; 
      line2 += pinkNum;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(line1);
      lcd.setCursor(0, 1);
      lcd.print(line2);
      
      analogWrite(RED_PIN, redValue);
      analogWrite(GREEN_PIN, greenValue);
      analogWrite(BLUE_PIN, blueValue);
    }

    // wait 15 ms for servo to reach the position
    delay(15);
  }
  delay(200);
}

// Function to read Red Pulse Widths
int getRedPW()
{
  // Set sensor to read Red only
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  // Define integer to represent Pulse Width
  int PW;

  // Read the output Pulse Width
  PW = pulseIn(SENSOR_OUT, LOW);

  // Return the value
  return PW;
}

// Function to read green pulse widths
int getGreenPW()
{
  // Set sensor to read green only
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);

  // Define integer to represent pulse width
  int PW;

  // Read the output pulse width
  PW = pulseIn(SENSOR_OUT, LOW);

  // Return the value
  return PW;
}

// Function to read blue pulse widths
int getBluePW() {

  // Set sensor to read blue only
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);

  // Define integer to represent pulse width
  int PW;

  // Read the output pulse width
  PW = pulseIn(SENSOR_OUT, LOW);

  // Return the value
  return PW;
}

void playTone(unsigned int frequency, unsigned long duration) {
  unsigned long period = 1000000 / frequency;  // Calculate the period of the tone
  unsigned long cycles = frequency * duration / 1000;  // Calculate the number of cycles
  for (unsigned long i = 0; i < cycles; i++) {
    digitalWrite(buzzerPin, HIGH);  // Turn on the buzzer
    delayMicroseconds(period / 2);  // Half period on
    digitalWrite(buzzerPin, LOW);  // Turn off the buzzer
    delayMicroseconds(period / 2);  // Half period off
  }
  delay(1000);  // Pause for 1 second after playing the tone
}