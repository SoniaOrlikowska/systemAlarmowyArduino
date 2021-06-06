
// potrzebujemy dwie biblioteki do obsługi wyświetlacza LCD oraz keypadu
#include <LiquidCrystal.h>
#include <Keypad.h>

// zdefiniowanie pinów przyłączeniowych dla buzzera i czujnika ultradźwiękowego
#define buzzer 8
#define trigPin 9
#define echoPin 10

//zdefiniowanie wykorzystywanych zmiennych
long duration;
int distance, initialDistance, currentDistance, i;
boolean showMenu = true;
String password = "1234";
String tempPassword;
boolean activated = false;
boolean isActivated;
boolean activateAlarm = false;
boolean alarmActivated = false;
boolean enteredPassword;
boolean passChangeMode = false;
boolean passChanged = false;

//ustawienia keypadu
const byte ROWS = 4; //4 rzędy klawiatury
const byte COLS = 4; //4 kolumny klawiatury
char keypressed;

//zdefiniowanie macierzy symboli klawiatury
char keyMap[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {14, 15, 16, 17}; //Przypisanie numerów pinów do każdego rzędu pinpadu
byte colPins[COLS] = {18, 19, 20, 21}; //Przypisanie numerów pinów do każdej kolumny pinpadu

Keypad myKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, ROWS, COLS);  //obiekt Keypad

LiquidCrystal lcd(1, 2, 4, 5, 6, 7); // LC obiekt. Parametry: (rs, enable, d4, d5, d6, d7)

void setup() {
  lcd.begin(16, 2);
  pinMode(buzzer, OUTPUT); // Ustawia buzzer jako output
  pinMode(trigPin, OUTPUT); // Ustawia trigPin jako output
  pinMode(echoPin, INPUT); // Ustawia echoPin jako input
}

void loop() {
  if (activateAlarm) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm bedzie");
    lcd.setCursor(0, 1);
    lcd.print("aktywowany za ");

    int countdown = 5; // 5 sekund do aktywacji alarmu
    while (countdown != 0) {
      lcd.setCursor(15, 1);
      lcd.print(countdown);
      countdown--;
      tone(buzzer, 1300, 100);
      delay(1000);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    initialDistance = getDistance();
    lcd.print("Alarm Aktywny!!");
    activateAlarm = false;
    alarmActivated = true;
  }

  //Aktywowanie alarmu dźwiękowego gdy przedmiot zostanie zabrany
  if (alarmActivated == true) {
    currentDistance = getDistance()  ;
    if ( currentDistance  > 2  && currentDistance < 200) {
      tone(buzzer, 1000);
      enterPassword();
    }
  }

  //Pierwsza akcja na wyświetlaczu
  if (!alarmActivated) {
    if (showMenu) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("A Aktywuj Alarm");
      lcd.setCursor(0, 1);
      lcd.print("B Zmien Haslo");
      showMenu = false;
    }
    keypressed = myKeypad.getKey();
    if (keypressed == 'A') {      //Po kliknięciu 'A' Alarm jest aktywowany
      //tone(buzzer, 1000, 200);
      activateAlarm = true;
    }
    else if (keypressed == 'B') {
      lcd.clear();
      int i = 1;
      tone(buzzer, 2000, 100);
      tempPassword = "";
      lcd.setCursor(0, 0);
      lcd.print("Obecne Haslo");
      lcd.setCursor(0, 1);
      lcd.print(">");
      passChangeMode = true;
      passChanged = true;
      while (passChanged) {
        keypressed = myKeypad.getKey();
        if (keypressed != NO_KEY) {
          if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
              keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
              keypressed == '8' || keypressed == '9' ) {
            tempPassword += keypressed;
            lcd.setCursor(i, 1);
            lcd.print("*");
            i++;
            tone(buzzer, 2000, 100);
          }
        }
        if (i > 5 || keypressed == '#') {
          tempPassword = "";
          i = 1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Obecne Haslo");
          lcd.setCursor(0, 1);
          lcd.print(">");
        }
        if ( keypressed == '*') {
          i = 1;
          tone(buzzer, 2000, 100);
          if (password == tempPassword) {
            tempPassword = "";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Ustaw nowe haslo");
            lcd.setCursor(0, 1);
            lcd.print(">");
            while (passChangeMode) {
              keypressed = myKeypad.getKey();
              if (keypressed != NO_KEY) {
                if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
                    keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
                    keypressed == '8' || keypressed == '9' ) {
                  tempPassword += keypressed;
                  lcd.setCursor(i, 1);
                  lcd.print("*");
                  i++;
                  tone(buzzer, 2000, 100);
                }
              }
              if (i > 5 || keypressed == '#') {
                tempPassword = "";
                i = 1;
                tone(buzzer, 2000, 100);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Obecne Haslo:");
                lcd.setCursor(0, 1);
                lcd.print(">");
              }
              if ( keypressed == '*') {
                i = 1;
                tone(buzzer, 2000, 100);
                password = tempPassword;
                passChangeMode = false;
                passChanged = false;
                showMenu = true;
              }
            }
          }
        }
      }
    }
  }
}

void enterPassword() {
  int k = 5;
  tempPassword = "";
  activated = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" *** ALARM *** ");
  lcd.setCursor(0, 1);
  lcd.print("Pass>");
  while (activated) {
    keypressed = myKeypad.getKey();
    if (keypressed != NO_KEY) {
      if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
          keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
          keypressed == '8' || keypressed == '9' ) {
        tempPassword += keypressed;
        lcd.setCursor(k, 1);
        lcd.print("*");
        k++;
      }
    }
    if (k > 9 || keypressed == '#') {
      tempPassword = "";
      k = 5;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" *** ALARM *** ");
      lcd.setCursor(0, 1);
      lcd.print("Pass>");
    }
    if ( keypressed == '*') {
      if ( tempPassword == password ) {
        activated = false;
        alarmActivated = false;
        noTone(buzzer); //wyłączenie sygnału alarmu po wprowadzeniu poprawnego hasła
        showMenu = true;
      }
      else if (tempPassword != password) {
        lcd.setCursor(0, 1);
        lcd.print("Zle haslo!!");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" !!! ALARM !!! ");
        lcd.setCursor(0, 1);
        lcd.print("Pass>");
      }
    }
  }
}
// Funkcja do odczytu odległości z czujnika ultradźwięków
long getDistance() {

  //zerowanie trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  //trigPin wprowadzony w stan HIGH na 10 micro sekund
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Odczyt z input
  duration = pulseIn(echoPin, HIGH);

  //odległość podana w cm
  distance = duration / 58;

  return distance;

}
