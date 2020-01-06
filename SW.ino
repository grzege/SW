#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int MAX_LEVEL = 30+1;//n+1, n poziomów
int n = 6+1; //n+1, n - liczba poziomów w multiplayer
int sequence[MAX_LEVEL];//sekwencja wygenerowana dla gracza 1.
int sequence1[MAX_LEVEL];//sekwencja wygenerowana dla gracza 2.
int your_sequence[MAX_LEVEL]; //sekwencja wskazana przez gracza 1.
int your_sequence1[MAX_LEVEL]; //sekwencja wskazana przez gracza 2.
int level = 1; //poziom
int score = 0; //wynik gracza 1.
int score1 = 0; //wynik gracza 2.
bool turn = false; //0 - pierwszy gracz, 1 - drugi gracz
bool mode = false; //0 - jeden gracz, 1 - dwóch graczy

int game_speed = 1000; //szybkość gry w milisekundach
int tones[4] = {100,200,300,400};//częstotliwości dźwięków kolejno dla diody czerwonej, zielone, niebieskiej i zółtej
//diody - piny
#define LED_RED 2
#define LED_GREEN 3
#define LED_BLUE 4
#define LED_YELLOW 5
//przyciski - piny
#define BUTTON_RED 8
#define BUTTON_GREEN 9
#define BUTTON_BLUE 10
#define BUTTON_YELLOW 11
//buzzer - pin
#define BUZZER 13
//buzzer - częstotliwości dla dźwięków oraz długość dźwięku
#define RED_TONE 100
#define GREEN_TONE 200
#define BLUE_TONE 300
#define YELLOW_TONE 400
#define TONE_DURATION 250
//przeciski sterujące - piny
#define RIGHT 0
#define LEFT 6
#define MIDDLE 7
//komunikaty tekstowe do LCD
char str1[] = "Witaj graczu!";
char str2[] = "1-os, prawy;";
char str3[] = "2-os, lewy.";
char str4[] = "Wynik:";
char str5[] = "Przegrales :(";
char str6[] = "Zdobyles ";
char str7[] = " pkt.";
char str8[] = "Wynik 2-gr:";
char str9[] = "Wygral gracz 1!";
char str10[] = "Wygral gracz 2!";
char str11[] = "Remis!";
char str12[] = "Koniec gry";

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {

  lcd.begin(16,2);
  lcd.backlight(); // zalaczenie podwietlenia 
  lcd.setCursor(0,0);
  //ilekroć pojawia się pętla zawierająca lcd.print
  //jest to wypisywanie tekstu litera po literze
  for(int i=0;i<sizeof(str1)-1;i++){//witaj graczu!
    lcd.print(str1[i]);
    delay(150);
  }
  delay(2000);
  lcd.clear();
  for(int i=0;i<sizeof(str2)-1;i++){//1-os, prawy
    lcd.print(str2[i]);
    delay(150);
  }
  lcd.setCursor(0,1);
  for(int i=0;i<sizeof(str3)-1;i++){//2-os, lewy
    lcd.print(str3[i]);
    delay(150);
  }
  //przyciski - wejscie
  pinMode(BUTTON_RED, INPUT);
  pinMode(BUTTON_GREEN, INPUT);
  pinMode(BUTTON_BLUE, INPUT);
  pinMode(BUTTON_YELLOW, INPUT);
  //diody - wyjscie
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  //gasimy diody
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, LOW);

}

void loop()
{
  if (level == 1&&!turn) {
    generate_sequences();//generuje sekwencje dla graczy
  }
  if (digitalRead(RIGHT) == LOW){//wybrano tryb dla jednego gracza
    mode=false;
    turn=false;
    score=0;
    score1=0;
    game_speed = 1000;
  }
  if (digitalRead(LEFT) == LOW){//wybrano tryb dla dwóch gracza
    mode=true;
    turn=false;
    score=0;
    score1=0;
    game_speed = 1000;
  }
  if (digitalRead(RIGHT) == LOW || digitalRead(LEFT) == LOW || (level < MAX_LEVEL && level!=1) || (level < MAX_LEVEL && level==1 && turn)) //jeśli wciśnięto przycisk rozpoczęcia gry
  {
    if(level==1&&!turn){
      if(mode)//wybór liczby rund
      {
        while(1)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("L-1 S-OK P+1");
          lcd.setCursor(0,1);
          lcd.print("Liczba rund: ");
          lcd.print(n);
          lcd.setCursor(0,1);      
          if(digitalRead(RIGHT)==LOW)n++;
          if(digitalRead(LEFT)==LOW)n--;
          if(digitalRead(MIDDLE)==LOW)break;
          delay(100);
        }
        n++;
      }
      //odliczanie 1,2,3... START
      lcd.clear();
      digitalWrite(LED_RED, HIGH);
      delay(200);
      tone(BUZZER, BLUE_TONE, TONE_DURATION);
      lcd.print("1 ");
      digitalWrite(LED_RED, LOW);
      delay(800);
      digitalWrite(LED_GREEN, HIGH);
      delay(200);
      tone(BUZZER, BLUE_TONE, TONE_DURATION);
      lcd.print("2 ");
      digitalWrite(LED_GREEN, LOW);
      delay(800);
      digitalWrite(LED_BLUE, HIGH);
      delay(200);
      tone(BUZZER, BLUE_TONE, TONE_DURATION);
      lcd.print("3...");
      digitalWrite(LED_BLUE, LOW);
      delay(800);
      digitalWrite(LED_YELLOW, HIGH);
      delay(200);
      tone(BUZZER, 1000, TONE_DURATION);
      lcd.clear();
      lcd.print("START!");
      digitalWrite(LED_YELLOW, LOW);
      delay(800);
    }
    //wyświetlanie wyników
    lcd.clear();
    lcd.setCursor(0,0);
    for(int i=0;i<sizeof(str4)-1;i++){
      lcd.print(str4[i]);
      delay(100);
    }
    lcd.print(score);
    //2 graczy
    if(mode)
    {
      lcd.setCursor(0,1);
      for(int i=0;i<sizeof(str8)-1;i++){
        lcd.print(str8[i]);
        delay(100);
      }
      lcd.print(score1);
    }
    show_sequence();//pokazywanie sekwencji na diodach    
    get_sequence();//odbieranie sekwencji od gracza
    if(mode)turn=!turn;//zmiana tury gracza w przypadku gry dwuosobowej     
  }
  if(!mode&&level==MAX_LEVEL)//koniec gry, tryb jednoosobowy
  {
    lcd.clear();
    lcd.setCursor(0,0);
    for(int i=0;i<sizeof(str12)-1;i++){
      lcd.print(str12[i]);
      delay(150);
    }
    lcd.setCursor(0,1);
    for(int i=0;i<sizeof(str6)-1;i++){
      lcd.print(str6[i]);
      delay(150);
    }
    lcd.print(score);
    for(int i=0;i<sizeof(str7)-1;i++){
      lcd.print(str7[i]);
      delay(150);
    }
    delay(3000);
    level = 1;
    lcd.clear();
    lcd.setCursor(0,0);
    for(int i=0;i<sizeof(str2)-1;i++){
      lcd.print(str2[i]);
      delay(150);
    }
    lcd.setCursor(0,1);
    for(int i=0;i<sizeof(str3)-1;i++){
      lcd.print(str3[i]);
      delay(150);
    }
  }
  if(mode&&level==n)//koniec gry, tryb dwuosobowy
  {
    lcd.clear();
    lcd.setCursor(0,0);
    if(score>score1)
    {
      
      for(int i=0;i<sizeof(str9)-1;i++){
        lcd.print(str9[i]);
        delay(100);
      }
    }
    else if (score<score1)
    {
      for(int i=0;i<sizeof(str10)-1;i++){
        lcd.print(str10[i]);
        delay(100);
      }
    }
    else
    {
      for(int i=0;i<sizeof(str11)-1;i++){
        lcd.print(str11[i]);
        delay(100);
      }
    }
    delay(3000);
    level = 1;
    turn=false;
    lcd.clear();
    lcd.setCursor(0,0);
    for(int i=0;i<sizeof(str2)-1;i++){
      lcd.print(str2[i]);
      delay(150);
    }
    lcd.setCursor(0,1);
    for(int i=0;i<sizeof(str3)-1;i++){
      lcd.print(str3[i]);
      delay(150);
    }
  }
}
void generate_sequences()
{
  randomSeed(millis());
  for (int i = 0; i < MAX_LEVEL; i++)
  {
    sequence[i] = random(2,6);//2,3,4 lub 5 - gracz 1.
    sequence1[i] = random(2,6);//2,3,4 lub 5 - gracz 2.
  }
}
void show_sequence()
{
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, LOW);

  if(!turn){
    for (int i = 0; i < level; i++)//gracz 1.
    {
      digitalWrite(sequence[i], HIGH);
      tone(BUZZER, tones[sequence[i]-2], TONE_DURATION);
      delay(game_speed);
      digitalWrite(sequence[i], LOW);
      delay(200);
    }
  }
  else
  {
    for (int i = 0; i < level; i++)//gracz 2.
    {
      digitalWrite(sequence1[i], HIGH);
      tone(BUZZER, tones[sequence1[i]-2], TONE_DURATION);
      delay(game_speed);
      digitalWrite(sequence1[i], LOW);
      delay(200);
    }
  }
}

void get_sequence()
{
  int flag = 0; //this flag indicates if the sequence is correct
  
  for (int i = 0; i < level; i++)
  {
    flag = 0;//flaga do kontroli wciśnięcia przycisku
    while(flag == 0)
    {
      if (digitalRead(BUTTON_YELLOW) == LOW)
      {
        digitalWrite(LED_YELLOW, HIGH);
        tone(BUZZER,YELLOW_TONE,TONE_DURATION);
        your_sequence[i] = LED_YELLOW;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i]&&!turn)
        {
          wrong_sequence();
          return;
        }
        if (your_sequence[i] != sequence1[i]&&turn)
        {
          wrong_sequence();
          return;
        }
        digitalWrite(LED_YELLOW, LOW);
      }
    
      if (digitalRead(BUTTON_BLUE) == LOW)
      {
        digitalWrite(LED_BLUE, HIGH);
        tone(BUZZER,BLUE_TONE,TONE_DURATION);
        your_sequence[i] = LED_BLUE;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i]&&!turn)
        {
          wrong_sequence();
          return;
        }
        if (your_sequence[i] != sequence1[i]&&turn)
        {
          wrong_sequence();
          return;
        }
        digitalWrite(LED_BLUE, LOW);
      }
        
      if (digitalRead(BUTTON_GREEN) == LOW)
      {
        digitalWrite(LED_GREEN, HIGH);
        tone(BUZZER,GREEN_TONE,TONE_DURATION);
        your_sequence[i] = LED_GREEN;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i]&&!turn)
        {
          wrong_sequence();
          return;
        }
        if (your_sequence[i] != sequence1[i]&&turn)
        {
          wrong_sequence();
          return;
        }
        digitalWrite(LED_GREEN, LOW);
      }
      
      if (digitalRead(BUTTON_RED) == LOW)
      {
        digitalWrite(LED_RED, HIGH);
        tone(BUZZER,RED_TONE,TONE_DURATION);
        your_sequence[i] = LED_RED;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i]&&!turn)
        {
          wrong_sequence();
          return;
        }
        if (your_sequence[i] != sequence1[i]&&turn)
        {
          wrong_sequence();
          return;
        }
        digitalWrite(LED_RED, LOW);
      }  
    }
  }
  right_sequence();
}


void wrong_sequence()
{
  delay(250);
  int game_over_tune=100;
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_YELLOW, HIGH);
    if(i<2)tone(BUZZER, game_over_tune, TONE_DURATION);
    else tone(BUZZER, game_over_tune,TONE_DURATION*2);
    delay(250);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_YELLOW, LOW);
    delay(250);
    game_over_tune-=20;
  }
  if(!mode){
    lcd.clear();
    for(int i=0;i<sizeof(str5)-1;i++){
      lcd.print(str5[i]);
      delay(150);
    }
    lcd.setCursor(0,1);
    for(int i=0;i<sizeof(str6)-1;i++){
      lcd.print(str6[i]);
      delay(150);
    }
    lcd.print(score);
    for(int i=0;i<sizeof(str7)-1;i++){
      lcd.print(str7[i]);
      delay(150);
    }
  
    delay(4000);
    lcd.clear();
    lcd.setCursor(0,0);
    for(int i=0;i<sizeof(str2)-1;i++){
      lcd.print(str2[i]);
      delay(150);
    }
    lcd.setCursor(0,1);
    for(int i=0;i<sizeof(str3)-1;i++){
      lcd.print(str3[i]);
      delay(150);
    }
    level = 1;
  }else if(turn&&level < MAX_LEVEL)
  {
    level++;
    game_speed -= 75;
  }
}


void right_sequence()
{
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, LOW);
  delay(250);

  digitalWrite(LED_RED, HIGH);
  delay(100);
  tone(BUZZER, RED_TONE, TONE_DURATION);
  digitalWrite(LED_RED, LOW);
  delay(100);
  digitalWrite(LED_GREEN, HIGH);
  delay(100);
  tone(BUZZER, GREEN_TONE, TONE_DURATION);
  digitalWrite(LED_GREEN, LOW);
  delay(100);
  digitalWrite(LED_BLUE, HIGH);
  delay(100);
  tone(BUZZER, BLUE_TONE, TONE_DURATION);
  digitalWrite(LED_BLUE, LOW);
  delay(100);
  digitalWrite(LED_YELLOW, HIGH);
  delay(100);
  tone(BUZZER, YELLOW_TONE, TONE_DURATION);
  digitalWrite(LED_YELLOW, LOW);
  delay(250);
  
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_YELLOW, HIGH);
  tone(BUZZER, 300, TONE_DURATION);
  delay(200);
  tone(BUZZER, 400, TONE_DURATION);
  delay(300);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, LOW);
  delay(500);
  
  if(!turn)score++;
  else score1++;
  if ((level < MAX_LEVEL && mode && turn) || (level < MAX_LEVEL && !mode))
  {
    level++;
    game_speed -= 75;
  }
}
