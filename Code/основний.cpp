#include <Servo.h>       // Підключення бібліотеки для керування сервоприводами
#include <ArduinoJson.h> // Підключення бібліотеки для роботи з JSON

Servo servo_x;    // Оголошення об'єкту для керування сервоприводом по осі X
Servo servo_y;    // Оголошення об'єкту для керування сервоприводом по осі Y
Servo servo_z;    // Оголошення об'єкту для керування сервоприводом по осі Z
Servo servo_grip; // Оголошення об'єкту для керування сервоприводом захвату

int step = 0;             // Лічильник кроків для зміни позицій сервоприводів
const int stepscount = 3; // Кількість кроків у послідовності
int x_pos;                // Змінна для зберігання позиції сервопривода по осі X
int y_pos;                // Змінна для зберігання позиції сервопривода по осі Y
int z_pos;                // Змінна для зберігання позиції сервопривода по осі Z
int grip_pos;             // Змінна для зберігання позиції сервопривода захвату
String input;             // Змінна для зберігання вхідного JSON рядка

unsigned long previousMillisStep = 0;      // Час останнього виконання функції getSetpoint
unsigned long previousMillisRead = 0;      // Час останнього виконання функції ReadPosition
unsigned long currentMillis;               // Поточний час
unsigned long previousMillisCheckJson = 0; // Час останнього виконання функції checkForNewJson

const long intervalStep = 3000;      // Інтервал між викликами getSetpoint (в мілісекундах)
const long intervalRead = 3000;      // Інтервал між викликами ReadPosition (в мілісекундах)
const long intervalCheckJson = 3000; // Інтервал між викликами checkForNewJson (в мілісекундах)

int arrayStep[10][4] = {
    // Масив для зберігання позицій сервоприводів для кожного кроку
    {111, 122, 155, 77},
    {140, 120, 150, 70},
    {123, 134, 156, 66}, // кінцевий step, всі інші для виділення пам'яті
    {123, 134, 156, 66},
    {123, 134, 156, 66},
    {123, 134, 156, 66},
    {123, 134, 156, 66},
    {123, 134, 156, 66},
    {123, 134, 156, 66},
    {123, 134, 156, 66},
};

void setup()
{
  Serial.begin(9600);   // Ініціалізація серійного зв'язку зі швидкістю 9600 бод
  servo_x.attach(11);   // Приєднання сервопривода осі X до 11 піна
  servo_y.attach(10);   // Приєднання сервопривода осі Y до 10 пін
  servo_z.attach(9);    // Приєднання сервопривода осі Z до 9 піна
  servo_grip.attach(5); // Приєднання сервопривода захвату до 5 піна

  x_pos = arrayStep[0][0];    // Ініціалізація позиції X з першого кроку
  y_pos = arrayStep[0][1];    // Ініціалізація позиції Y з першого кроку
  z_pos = arrayStep[0][2];    // Ініціалізація позиції Z з першого кроку
  grip_pos = arrayStep[0][3]; // Ініціалізація позиції захвату з першого кроку

  servo_x.write(x_pos);       // Переміщення сервопривода осі X в початкову позицію
  servo_y.write(y_pos);       // Переміщення сервопривода осі Y в початкову позицію
  servo_z.write(z_pos);       // Переміщення сервопривода осі Z в початкову позицію
  servo_grip.write(grip_pos); // Переміщення сервопривода захвату в початкову позицію

  getSetpoint(); // Виклик функції для ініціалізації позицій сервоприводів
}

void loop()
{
  currentMillis = millis(); // Отримання поточного часу

  if (currentMillis - previousMillisStep >= intervalStep) // Перевірка, чи пройшов інтервал для getSetpoint
  {
    previousMillisStep = currentMillis; // Оновлення часу останнього виклику getSetpoint
    getSetpoint();                      // Виклик функції для зміни позицій сервоприводів
  }

  if (currentMillis - previousMillisRead >= intervalRead) // Перевірка, чи пройшов інтервал для ReadPosition
  {
    previousMillisRead = currentMillis; // Оновлення часу останнього виклику ReadPosition
    ReadPosition();                     // Виклик функції для зчитування поточних позицій сервоприводів
  }

  checkForNewJson(); // Виклик функції для перевірки нових даних JSON

  servo_x.write(x_pos);       // Переміщення сервопривода осі X в початкову позицію
  servo_y.write(y_pos);       // Переміщення сервопривода осі Y в початкову позицію
  servo_z.write(z_pos);       // Переміщення сервопривода осі Z в початкову позицію
  servo_grip.write(grip_pos); // Переміщення сервопривода захвату в початкову позицію
}

void getSetpoint()
{
  x_pos = arrayStep[step][0];    // Зміна позиції X на основі поточного кроку
  y_pos = arrayStep[step][1];    // Зміна позиції Y на основі поточного кроку
  z_pos = arrayStep[step][2];    // Зміна позиції Z на основі поточного кроку
  grip_pos = arrayStep[step][3]; // Зміна позиції захвату на основі поточного кроку

  step++;                 // Збільшення лічильника кроків
  if (step >= stepscount) // Перевірка, чи досягнуто кінця послідовності кроків
  {
    step = 0; // Скидання лічильника кроків, якщо досягнуто кінця
  }
}

void ReadPosition()
{
  int x = servo_x.read();       // Зчитування поточної позиції сервопривода осі X
  int y = servo_y.read();       // Зчитування поточної позиції сервопривода осі Y
  int z = servo_z.read();       // Зчитування поточної позиції сервопривода осі Z
  int grip = servo_grip.read(); // Зчитування поточної позиції сервопривода захвату

  Serial.print(x);      // Виведення позиції X в серійний порт
  Serial.print(" ");    // Виведення пробілу
  Serial.print(y);      // Виведення позиції Y в серійний порт
  Serial.print(" ");    // Виведення пробілу
  Serial.print(z);      // Виведення позиції Z в серійний порт
  Serial.print(" ");    // Виведення пробілу
  Serial.println(grip); // Виведення позиції захвату в серійний порт та перехід на новий рядок
}

void checkForNewJson()
{
  StaticJsonDocument<200> doc; // Оголошення документа JSON з фіксованим розміром 200 байт
  while (Serial.available() > 0)
  {                               // Перевірка наявності даних у серійному порту
    input += Serial.readString(); // Зчитування даних з серійного порту та додавання їх до рядка input
  }
  if (input.length() > 30) // Перевірка довжини вхідного рядка для уникнення обробки неповних даних
  {
    Serial.print("JSON array: ");                // Виведення повідомлення про отримання JSON
    Serial.println(input);                       // Виведення отриманого JSON
    input.trim();                                // Видалення пробілів з початку та кінця рядка
    if (input.charAt(input.length() - 1) != ']') // Перевірка, чи останній символ є ']', щоб переконатися, що JSON завершений
    {
      input += "]"; // Додавання ']', якщо його немає
    }

    DeserializationError error = deserializeJson(doc, input); // Спроба десеріалізації JSON з рядка

    if (error) // Перевірка на помилку десеріалізації
    {
      Serial.print(F("deserializeJson() failed: ")); // Виведення повідомлення про помилку
      Serial.println(error.c_str());                 // Виведення опису помилки
      switch (error.code())                          // Перевірка коду помилки
      {
      case DeserializationError::InvalidInput: // Якщо помилка через некоректний вхідний рядок
        Serial.print(F("Invalid input!"));     // Виведення повідомлення про некоректний вхідний рядок
        break;
      case DeserializationError::NoMemory:    // Якщо помилка через недостатню пам'ять
        Serial.print(F("Not enough memory")); // Виведення повідомлення про недостатню пам'ять
        break;
      }
      return; // Вихід з функції у разі помилки
    }

    JsonArray jsonArray = doc.as<JsonArray>(); // Перетворення десеріалізованого документа в JSON масив
    stepscount = jsonArray.size();             // Оновлення кількості кроків на основі розміру JSON масиву
    for (int i = 0; i < jsonArray.size(); i++) // Цикл для обробки кожного кроку з JSON масиву
    {
      JsonArray innerArray = jsonArray[i]; // Отримання внутрішнього масиву для поточного кроку
      if (innerArray.size() != 4)          // Перевірка розміру внутрішнього масиву
      {
        Serial.println("Invalid JSON format"); // Виведення повідомлення про некоректний формат JSON
        return;                                // Вихід з функції у разі некоректного формату JSON
      }
      for (int j = 0; j < 4; j++) // Цикл для збереження значень з внутрішнього масиву до arrayStep
      {
        arrayStep[i][j] = innerArray[j]; // Збереження значення в arrayStep
      }
    }
    input = ""; // Очищення рядка input після обробки JSON
  }
}