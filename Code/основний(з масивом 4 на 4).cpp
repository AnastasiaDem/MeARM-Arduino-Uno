#include <Servo.h> // Підключення бібліотеки для керування сервоприводами
#include <ArduinoJson.h> // Підключення бібліотеки для роботи з JSON

Servo servo_x;    // create servo object to control a servo
Servo servo_y;    // create servo object to control a servo
Servo servo_z;    // create servo object to control a servo
Servo servo_grip; // create servo object to control a servo

int step = 0; // Лічильник кроків для зміни позицій сервоприводів
const int stepscount = 4; // Кількість кроків у послідовності
int x_pos;  // Змінна для зберігання позиції сервопривода по осі X
int y_pos;  // Змінна для зберігання позиції сервопривода по осі Y
int z_pos;  // Змінна для зберігання позиції сервопривода по осі Z
int grip_pos;  // Змінна для зберігання позиції сервопривода захвату

unsigned long previousMillisStep = 0; // Час останнього виконання функції getSetpoint
unsigned long previousMillisRead = 0; // Час останнього виконання функції ReadPosition
unsigned long currentMillis; // Поточний час
unsigned long previousMillisCheckJson = 0; // Час останнього виконання функції checkForNewJson

const long intervalStep = 3000;  // Інтервал між викликами getSetpoint (в мілісекундах)
const long intervalRead = 3000;  // Інтервал між викликами ReadPosition (в мілісекундах)
const long intervalCheckJson = 3000;  // Інтервал між викликами checkForNewJson (в мілісекундах)

int arrayStep[4][4] = {
    {111, 122, 155, 77},  // Початкові позиції для кожного кроку
    {140, 120, 150, 70},  // Початкові позиції для кожного кроку
    {123, 134, 156, 66},  // Початкові позиції для кожного кроку
    {90, 110, 133, 50},  // Початкові позиції для кожного кроку
};

void setup()
{
  Serial.begin(9600);  // Ініціалізація серійного зв'язку зі швидкістю 9600 бод
  servo_x.attach(11);  // Приєднання сервопривода осі X до 11 піна
  servo_y.attach(10);  // Приєднання сервопривода осі Y до 10 пін
  servo_z.attach(9);  // Приєднання сервопривода осі Z до 9 піна
  servo_grip.attach(5);  // Приєднання сервопривода захвату до 5 піна

  x_pos = arrayStep[0][0];  // Ініціалізація позиції X з першого кроку
  y_pos = arrayStep[0][1];  // Ініціалізація позиції Y з першого кроку
  z_pos = arrayStep[0][2];  // Ініціалізація позиції Z з першого кроку
  grip_pos = arrayStep[0][3];  // Ініціалізація позиції захвату з першого кроку

  servo_x.write(x_pos);  // Переміщення сервопривода осі X в початкову позицію
  servo_y.write(y_pos);  // Переміщення сервопривода осі Y в початкову позицію
  servo_z.write(z_pos);  // Переміщення сервопривода осі Z в початкову позицію
  servo_grip.write(grip_pos);  // Переміщення сервопривода захвату в початкову позицію

  getSetpoint(); // Виклик функції для ініціалізації позицій сервоприводів
}

void loop()
{
  currentMillis = millis(); // Отримання поточного часу

  if (currentMillis - previousMillisStep >= intervalStep)  // Перевірка, чи пройшов інтервал для getSetpoint
  {
    previousMillisStep = currentMillis;  // Оновлення часу останнього виклику getSetpoint
    getSetpoint();  // Виклик функції для зміни позицій сервоприводів
  }

  if (currentMillis - previousMillisRead >= intervalRead)  // Перевірка, чи пройшов інтервал для ReadPosition
  {
    previousMillisRead = currentMillis;  // Оновлення часу останнього виклику ReadPosition
    ReadPosition();  // Виклик функції для зчитування поточних позицій сервоприводів
  }

  if (currentMillis - previousMillisCheckJson >= intervalCheckJson)  // Перевірка, чи пройшов інтервал для checkForNewJson
  {
    previousMillisCheckJson = currentMillis;  // Оновлення часу останнього виклику checkForNewJson
    checkForNewJson();  // Виклик функції для перевірки нових даних JSON
  }

  servo_x.write(x_pos);  // Переміщення сервопривода осі X в початкову позицію
  servo_y.write(y_pos);  // Переміщення сервопривода осі Y в початкову позицію
  servo_z.write(z_pos);  // Переміщення сервопривода осі Z в початкову позицію
  servo_grip.write(grip_pos);  // Переміщення сервопривода захвату в початкову позицію
}

void getSetpoint()
{
  x_pos = arrayStep[step][0];  // Зміна позиції X на основі поточного кроку
  y_pos = arrayStep[step][1];  // Зміна позиції Y на основі поточного кроку
  z_pos = arrayStep[step][2];  // Зміна позиції Z на основі поточного кроку
  grip_pos = arrayStep[step][3];  // Зміна позиції захвату на основі поточного кроку

  step++;  // Збільшення лічильника кроків
  if (step >= stepscount)  // Перевірка, чи досягнуто кінця послідовності кроків
  {
    step = 0;  // Скидання лічильника кроків, якщо досягнуто кінця
  }
}


void ReadPosition()
{
  int x = servo_x.read();  // Зчитування поточної позиції сервопривода осі X
  int y = servo_y.read();  // Зчитування поточної позиції сервопривода осі Y
  int z = servo_z.read();  // Зчитування поточної позиції сервопривода осі Z
  int grip = servo_grip.read();  // Зчитування поточної позиції сервопривода захвату

  Serial.print(x);  // Виведення позиції X в серійний порт
  Serial.print(" ");  // Виведення пробілу
  Serial.print(y);  // Виведення позиції Y в серійний порт
  Serial.print(" ");  // Виведення пробілу
  Serial.print(z);  // Виведення позиції Z в серійний порт
  Serial.print(" ");  // Виведення пробілу
  Serial.println(grip);  // Виведення позиції захвату в серійний порт та перехід на новий рядок
}

void checkForNewJson()
{
  StaticJsonDocument<200> doc;  // Оголошення документа JSON з фіксованим розміром 200 байт

  if (Serial.available() > 0)  // Перевірка, чи є доступні дані в серійному порті
  {
    String input = Serial.readString();  // Зчитування даних з серійного порту в рядок
    input.trim(); // Видалення будь-яких початкових або кінцевих пробілів
    Serial.print("Received input: ");  // Виведення отриманих даних в серійний порт
    Serial.println(input);  // Виведення отриманих даних в серійний порт з переходом на новий рядок

    if (input.charAt(input.length() - 1) != ']')  // Перевірка, чи рядок закінчується символом ']', якщо ні, то додати його
    {
      input += "]";  // Додавання символу ']' до кінця рядка
    }

    DeserializationError error = deserializeJson(doc, input);  // Спроба десеріалізації JSON з рядка

    if (error)  // Перевірка на помилку десеріалізації
    {
      Serial.print(F("deserializeJson() failed: "));  // Виведення повідомлення про помилку
      Serial.println(error.c_str());  // Виведення опису помилки
      switch (error.code())  // Перевірка коду помилки
      {
      case DeserializationError::InvalidInput:  // Якщо помилка через некоректний вхідний рядок
        Serial.print(F("Invalid input!"));  // Виведення повідомлення про некоректний вхідний рядок
        break;
      case DeserializationError::NoMemory:  // Якщо помилка через недостатню пам'ять
        Serial.print(F("Not enough memory"));  // Виведення повідомлення про недостатню пам'ять
        break;
      }
      return;  // Вихід з функції у разі помилки
    }

    for (int i = 0; i < 4; i++)  // Цикл для оновлення значень масиву arrayStep з отриманого JSON
    {
      JsonArray innerArray = doc[i];  // Отримання внутрішнього масиву з JSON
      if (innerArray.size() != 4)  // Перевірка, чи внутрішній масив має 4 елементи
      {
        Serial.println("Invalid JSON format");  // Виведення повідомлення про некоректний формат JSON
        return;  // Вихід з функції у разі некоректного формату
      }
      for (int j = 0; j < 4; j++)  // Цикл для оновлення значень кожного елемента в масиві arrayStep
      {
        arrayStep[i][j] = innerArray[j];  // Присвоєння значення з JSON до масиву arrayStep
      }
    }
  }
}