/******************************************
  Website: www.elegoo.com

  Time:2017.12.12

 ******************************************/
int Led = 13;    //define the led's port (Arduino's Internal Led)
int buttonpin = 2; //define the port of light blocking module
int val; //define digital variable val
int count; // Decoder count
int flag; // This flag tells me if the photoresistor is blocked or it's free
int jackpot; // This calculates if the rpm are the correct with 10% error
int first_read; // This tells me if we already made the first photoresistor calculation for the rpm
int rotation_count; // This tells me how many rotation have been registered
int local_max; // This is the local max
int local_min; // This is the local min
int pwm; // This is the pwm signal that is sent to the pin
long start_time; // this is where I set the start time
long end_time; // This is where I set the end time
long time_difference; // This is where we I calculate the time difference
long rot_per_min; // This are the calculated rpm
long desired_rot_per_min; // this is the input of how many rpm do we want

#define ENABLE 5 // This is set to the pin that enables the chip that controls the motor
#define DIRA 3 // This pin sets the direction of the motor
#define DIRB 4 // This pin sets the direction of the motor
#define DECODER_COUNT 1 // This variable is set to the amount of dents that the decoder has

int latch = 7; //74HC595  pin 9 STCP
int clock = 8; //74HC595  pin 10 SHCP
int data = 6; //74HC595  pin 8 DS

unsigned char table[] =
{ 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c
  , 0x39, 0x5e, 0x79, 0x71, 0x00
};

int i;

void  setup()
{

  desired_rot_per_min = 80;
  local_max = 255;
  local_min = 80;
  jackpot = 0;
  first_read = 1;

  pinMode(latch, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);

  start_time = millis(); //obtengo el tiempo del microcontrolador
  end_time = 0; 
  time_difference = 0;
  rot_per_min = 0;

  //---set pin direction
  pinMode(ENABLE, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);

  pinMode(Led, OUTPUT); //define digital variable val
  pinMode(buttonpin, INPUT); //define light blocking module as a output port
  count = 0;
  flag = 0;
  rotation_count = 0;

  Serial.begin(9600);
  Serial.print("Hello");

  analogWrite(ENABLE, 80);
  digitalWrite(DIRA, HIGH); //one way
  digitalWrite(DIRB, LOW);

  randomSeed(analogRead(0));

  delay(700);
}

void Display(unsigned char num)
{

  digitalWrite(latch, LOW);
  shiftOut(data, clock, MSBFIRST, table[num]);
  digitalWrite(latch, HIGH);

}

void  loop()
{

  if ((jackpot == 0) && (first_read == 1)) { // si aún no se ha llegado a los rpm conseguidos y no se ha realizado una lectura

    first_read = 0;
    
    pwm = random(local_min, local_max);

    if (( pwm - local_min) > (local_max - pwm)) {
      local_max = pwm;
    } else {
      local_min = pwm;
    }
  }


  analogWrite(ENABLE, pwm); //half speed
  digitalWrite(DIRA, HIGH); //one way
  digitalWrite(DIRB, LOW);

  val = digitalRead(buttonpin); //read the value of the digital interface 3 assigned to val

  if (val == HIGH)         //when the light blocking sensor have signal, LED blink
  {
    if (flag == 0) {
      flag = 1;
    }
    digitalWrite(Led, LOW);

    Serial.print("\n local_min: ");
    Serial.print(local_min);
    Serial.print(" local_max: ");
    Serial.print(local_max);
    Serial.print(" rotation count: ");
    Serial.print(rotation_count);
    Serial.print(" rotation per minute: ");
    Serial.print(rot_per_min);
    Serial.print(" pwm: ");
    Serial.print(pwm);
    Serial.print(" time_difference: ");
    Serial.print(time_difference);
    Serial.print("\n");
    Serial.print("\n");
    Serial.print(" Jackpot: ");
    Serial.print(jackpot);
    Serial.print(" First Read: ");
    Serial.print(first_read);

    //delay(300);

    //Display(rotation_count);
  }
  else
  {
    if (flag == 1) {

      count = count + 1;

      if (count == DECODER_COUNT) {
        rotation_count = rotation_count + 1;
        count = 0;
      }

      flag = 0;
    }
    digitalWrite(Led, HIGH);
    Serial.print("\n local_min: ");
    Serial.print(local_min);
    Serial.print(" local_max: ");
    Serial.print(local_max);
    Serial.print(" rotation count: ");
    Serial.print(rotation_count);
    Serial.print(" rotation per minute: ");
    Serial.print(rot_per_min);
    Serial.print(" pwm: ");
    Serial.print(pwm);
    Serial.print(" time_difference: ");
    Serial.print(time_difference);
    Serial.print("\n");
    Serial.print("\n");
    Serial.print(jackpot);
    Serial.print(" ");
    Serial.print(first_read);

    //Display(rotation_count);
  }

  end_time = millis();

  time_difference = end_time - start_time;

  if (time_difference >= 10000) {
    rot_per_min = rotation_count * 6;

    first_read = 1;
    
    if((abs(desired_rot_per_min - rot_per_min) <= 5) || (abs(rot_per_min - desired_rot_per_min) <= 5)){
      jackpot = 1;
    } else if(desired_rot_per_min > rot_per_min){
      local_max = random(local_max,255);
      jackpot = 0;
    } else {
      local_min = random(80, local_min);
      jackpot = 0;
    }
    
    rotation_count = 0;
    restart_time();
  }
}

void restart_time()
{
  start_time = millis();
}
