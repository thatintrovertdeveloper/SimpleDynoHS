/*
  Sketch for use with SimpleDyno
  Developed on Arduino Uno Platform
  DamoRC - 2013-2014

  ALWAYS use the Sketch distributed with each new version of SimpleDyno

  Transmits:
    1 x Session timestamp
    1 x Interrupt timestamp and 1 x time interval since last interrupt for INT0 / Pin2 / RPM1
    1 x Interrupt timestamp and 1 x time interval since last interrupt for INT1 / Pin3 / RPM2
    6 x Analog Inputs (A0 and A1 are Voltage and Current, A2 and A3 are Temperature, A4 and A5 are open)
  Values are comma delimeted
  Baud rates selected in SD must match coded values in this Sketch.
*/

const int NumPortsToRead = 6;
int AnalogResult[NumPortsToRead];
String AllResult = "";

volatile unsigned long TimeStamp = 0;
volatile unsigned long time1 = 0;
volatile unsigned long time2 = 0;
volatile unsigned long PrevEdgeTimeStamp1 = 0;
volatile unsigned long PrevEdgeTimeStamp2 = 0;
volatile unsigned long NewestEdgeTimeStamp1 = 0;
volatile unsigned long NewestEdgeTimeStamp2 = 0;

volatile unsigned long highdetect1 = 0;
volatile unsigned long lowdetect1 = 0;
volatile bool state1 = false;

volatile unsigned long highdetect2 = 0;
volatile unsigned long lowdetect2 = 0;
volatile bool state2 = false;

int timer_preload = 65533;   // preload timer 65536-16MHz/256/20000Hz achieves around 64us period

void setup() {

  pinMode(2, INPUT);    // RPM1
  pinMode(3, INPUT);    // RPM2

  // Initialize serial communication
  // Ensure that Baud rate specified here matches that selected in SimpleDyno
  // Availailable Baud rates are:
  // 9600, 14400, 19200, 28800, 38400, 57600, 115200
  Serial.begin(38400);

  noInterrupts();           // disable all interrupts

  // initialize timer1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = timer_preload;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 &= ~(1 << TOIE1);   // disable timer overflow interrupt

  // initialize timer2
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = timer_preload;   // preload timer
  TCCR2B |= (1 << CS12);    // 256 prescaler
  TIMSK2 &= ~(1 << TOIE2);   // disable timer overflow interrupt

  interrupts();             // enable all interrupts

  orderT1interrupt();

}

void orderT1interrupt()
{
  TCNT1 = timer_preload;   // preload timer
  TIFR1 = 0x00;          //Timer1 INT Flag Reg: Clear Timer Overflow Flag
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
}

ISR(TIMER1_OVF_vect)        // interrupt service routine
{
  TIMSK1 &= ~(1 << TOIE1);   // disable timer overflow interrupt

  if (digitalRead(2) == HIGH)
  {
    highdetect1++;
    lowdetect1 = 0;
  }
  else
  {
    lowdetect1++;
    highdetect1 = 0;
  }

  if (highdetect1 > 2)
  {
    highdetect1 = 3;
    if (!state1)
    {
      state1 = true;
      NewestEdgeTimeStamp1 = micros();
      time1 = NewestEdgeTimeStamp1 - PrevEdgeTimeStamp1;
      if (time1 > 6000000) time1 = 6000000;
      PrevEdgeTimeStamp1 = NewestEdgeTimeStamp1;
    }
  }
  if (lowdetect1 > 2)
  {
    lowdetect1 = 3;
    if (state1)
    {
      state1 = false;
    }
  }


  if (digitalRead(3) == HIGH)
  {
    highdetect2++;
    lowdetect2 = 0;
  }
  else
  {
    lowdetect2++;
    highdetect2 = 0;
  }

  if (highdetect2 > 2)
  {
    highdetect2 = 3;
    if (!state2)
    {
      state2 = true;
      NewestEdgeTimeStamp2 = micros();
      time2 = NewestEdgeTimeStamp2 - PrevEdgeTimeStamp2;
      if (time2 > 6000000) time2 = 6000000;
      PrevEdgeTimeStamp2 = NewestEdgeTimeStamp2;
    }
  }
  if (lowdetect2 > 2)
  {
    lowdetect2 = 3;
    if (state2)
    {
      state2 = false;
    }
  }


  orderT1interrupt();

}

ISR(TIMER2_OVF_vect)        // interrupt service routine
{
  TIMSK2 &= ~(1 << TOIE2);   // disable timer overflow interrupt
}

void loop() {

  noInterrupts();
  unsigned long local_NewestEdgeTimeStamp1 = NewestEdgeTimeStamp1;
  unsigned long local_time1 = time1;
  unsigned long local_NewestEdgeTimeStamp2 = NewestEdgeTimeStamp2;
  unsigned long local_time2 = time2;
  interrupts();

  AllResult = "";
  AllResult += micros();
  AllResult += ",";
  AllResult += local_NewestEdgeTimeStamp1;
  AllResult += ",";
  AllResult += local_time1;
  AllResult += ",";
  AllResult += local_NewestEdgeTimeStamp2;
  AllResult += ",";
  AllResult += local_time2;

  for (int Looper = 0; Looper < NumPortsToRead; Looper++) {
    AnalogResult[Looper] = analogRead(Looper);
    AllResult += ",";
    AllResult += AnalogResult[Looper];
  }

  Serial.println (AllResult);
  Serial.flush();
  delay(1);
}


