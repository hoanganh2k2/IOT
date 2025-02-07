// C++ code
//
void setup()
{
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);
}

void case1()
{
  for(int i=10; i<14; i++)
  {
    digitalWrite(i,HIGH);
    delay(500);
  }
  for(int i=10; i<14; i++)
    digitalWrite(i,LOW);
  delay(500);
}

void case2()
{
  for(int i=10; i<14; i++)
  {
    digitalWrite(i,HIGH);
    delay(500);
    digitalWrite(i,LOW);
    delay(500);
  }
}

void loop()
{
  case1();
}