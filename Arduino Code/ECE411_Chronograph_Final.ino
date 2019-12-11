/**************************************************************************************
   Practicum project - Ballistic Chronograph
   ECE 411
   Fall 2019
   Team 5: Sean Schorzman, Jaeyoon Lee, Jianyu Hao, Kelly Makinster
                                                                                     
   Description: Displays the speed in fps of an object that passes through a
                set of IR detector sensors. IR sensors are arranged in a PVC
                pipe 8 inches apart with an IR light directily accross from
                each sensor. Upon passing sensor 1, a timer begins and waits
                until either sensor 2 detects an object or a timeout occurs.
                An additional push button input is used to reset IR detector
                light levels in order to adjust detection sensitivity.
                                                                                     
 **************************************************************************************/

#include <LiquidCrystal.h> //include the library code

//LCD pins
LiquidCrystal lcd(4, 6, 10, 9, 8, 7);

//Variables
int sensor1_read = A0;      //IR Detector 1 light level reading
int sensor2_read = A1;      //IR Detector 2 light level reading
unsigned long start_sensor; //Used to clock the time after sensor 1 is tripped
unsigned long stop_sensor;  //Used to clock the time after sensor 2 is tripped
float calculated;           //Used in the ballistic velocity calculation
long sensor_delay_counter;  //Used to display the current sensor light levels
int sensor1_threshhold;     //threshold to trip sensor 1
int sensor2_threshhold;     //threshold to trip sensor 2
int period = 3000;          //delay for 3 seconds
unsigned long time_now = 0; //used to store current time

//Functions
void lcd_sensor_values();   //displays sensor readings on line 2
void calculate_velocity(unsigned long start_sensor, unsigned long stop_sensor);

//this finction is used to set up the inputs and outputs used on the chronograph
void setup()
{
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(2, INPUT);
}

//this loop takes care of all sensor reads, timers and button presses.
void loop()
{
  lcd.clear();
  lcd.print("Ready");
  
  //****************************************
  //recalibrate sensors
  sensor1_read = analogRead(A0);
  sensor2_read = analogRead(A1);
  sensor1_threshhold = sensor1_read / 2.0;
  sensor2_threshhold = sensor2_read / 2.0;
  //****************************************
  
  lcd_sensor_values();                  //displays current sensor values
  sensor_delay_counter = 0;
  
  //wait for sensor 1 to be tripped or button press
  while ( (analogRead(A0) > sensor1_threshhold) && digitalRead(2) == LOW )
  {
    ++sensor_delay_counter;
    if (sensor_delay_counter == 5000)   //short delay before displaying sensor values
    {
      lcd_sensor_values();              //displays current sensor values
    }
  }

  if (digitalRead(2) == LOW)            //button wasn't pressed
  {
    time_now = millis();                //get current time for timeout delay
    lcd.clear();
    lcd.print("Started...");
    start_sensor = micros();  //get start time for when sensor 1 was tripped

    //remain in loop until sensor 2 is tripped, button is pressed or timeout has occured
    while ( ( analogRead(A1) > sensor2_threshhold ) && (millis() < time_now + period) && digitalRead(2) == LOW )
    {
      //empty loop
    }

    //if sensor 2 was tripped
    if (analogRead(A1) < sensor2_threshhold)
    {
      stop_sensor = micros();
      calculate_velocity(start_sensor, stop_sensor);
    }
    //if button was pressed
    else if (digitalRead(2) == HIGH)
    {
      lcd.clear();
      lcd.print("Reset...");
      delay(2500);
    }

    //timeout occured
    else
    {
      lcd.clear();
      lcd.print("Error");
      delay(1000);
    }

  }

  //button was pressed
  else
  {
    lcd.clear();
    lcd.print("Reset...");
    delay(1000);
  }
}


//this function is used to print out the current detector sensor light level on line 2
void lcd_sensor_values()
{
  lcd.clear();
  lcd.print("Ready");
  lcd.setCursor(0, 1);
  lcd.print("S1:");
  lcd.setCursor(3, 1);
  lcd.print(analogRead(A0));  // Print a message to the LCD.
  lcd.setCursor(8, 1);
  lcd.print("S2:");
  lcd.setCursor(11, 1);
  lcd.print(analogRead(A1));  // Print a message to the LCD.
  sensor_delay_counter = 0;
}

void calculate_velocity(unsigned long start_sensor, unsigned long stop_sensor)
{
  lcd.clear();
  calculated = (5.000 / 12.000) / ((stop_sensor - start_sensor) / 1000000.000);
  lcd.print(calculated);
  lcd.setCursor(5, 0);
  lcd.print("FPS");
  delay(2500);
}
