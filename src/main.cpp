#include <Arduino.h>

// 定义段引脚 (a-g)，使用安全的 GPIO
const int segPins[] = {13, 14, 15, 18, 19, 21, 22}; // a, b, c, d, e, f, g
const int digitPins[] = {23, 16, 17, 25};           // D1, D2, D3, D4
const int dpPin = 26;                               // 时钟点引脚
const int lightPins[] = {2, 4, 5};                  // light1 (绿), light2 (红), light3 (闹钟)
const int buttonPins[] = {27, 32, 33};              // button1, button2, button3

// 定义数字显示的段表 (0-9)，使用位图
const unsigned char numberPatterns[10] = {
    0b1111110, // 0: a, b, c, d, e, f
    0b0110000, // 1: b, c
    0b1101101, // 2: a, b, d, e, g
    0b1111001, // 3: a, b, c, d, g
    0b0110011, // 4: b, c, f, g
    0b1011011, // 5: a, c, d, f, g
    0b1011111, // 6: a, c, d, e, f, g
    0b1110000, // 7: a, b, c
    0b1111111, // 8: a, b, c, d, e, f, g
    0b1111011  // 9: a, b, c, f, g
};

// 按钮相关全局变量
const int numButtons = 3;                                          // 按钮数量
int buttonStates[numButtons] = {0, 0, 0};                          // 当前按钮状态数组（0 未按下，1 按下）
static int lastButtonStates[numButtons] = {1, 1, 1};               // 上次按钮状态数组
static unsigned long lastButtonPressTimes[numButtons] = {0, 0, 0}; // 上次按下时间戳数组
static bool buttonProcessed[numButtons] = {false, false, false};   // 按钮是否已处理
const int debounceDelay = 50;                                      // 去抖延时（毫秒）

// 状态机枚举
enum State {
  DISPLAY_TIME,       // 显示当前时间
  SET_ALARM_MINUTE,   // 设置闹钟分钟
  SET_ALARM_SECOND,   // 设置闹钟秒钟
  ALARM_TRIGGERED     // 闹钟触发
};

// 全局变量
static State currentState = DISPLAY_TIME; // 当前状态
static int currentMinutes = 14;           // 当前分钟
static int currentSeconds = 23;           // 当前秒钟
static int alarmMinutes = 8;              // 闹钟分钟
static int alarmSeconds = 0;              // 闹钟秒钟
static unsigned long lastTimeUpdate = 0;  // 上次时间更新时间戳
static unsigned long lastScan = 0;        // 上次扫描时间戳
static int scanPosition = 0;              // 当前扫描位置
static unsigned long lastBlink = 0;       // 上次闪烁时间戳
static bool blinkState = false;           // 闪烁状态（true 显示，false 隐藏）
const int blinkInterval = 500;            // 闪烁间隔（毫秒）
const int longPressDelay = 1000;          // 长按延时（1000ms）

// 函数声明
void setup(void);
void clear(void);
void displayNumber(int num);
void displayDigit(int digit, int position);
void displayColon(void);
void updateLights(void);
void loop(void);

// 初始化函数
void setup(void)
{
  for (int i = 0; i < 7; i++)
    pinMode(segPins[i], OUTPUT);

  for (int i = 0; i < 4; i++)
    pinMode(digitPins[i], OUTPUT);

  pinMode(dpPin, OUTPUT);
  digitalWrite(dpPin, LOW); // 默认关闭时钟点

  for (int i = 0; i < 3; i++)
  {
    pinMode(lightPins[i], OUTPUT);
    digitalWrite(lightPins[i], LOW); // 默认关闭 light
  }

  Serial.begin(115200);
  Serial.println("Serial Test OK");
}

// 清除段引脚状态
void clear(void)
{
  for (int i = 0; i < 7; i++)
    digitalWrite(segPins[i], LOW); // 共阴极数码管，段引脚低电平熄灭
}

// 显示单个数字
void displayNumber(int num)
{
  clear();
  unsigned char pattern = numberPatterns[num];
  for (int i = 0; i < 7; i++)
    digitalWrite(segPins[i], (pattern >> (6 - i)) & 0x01); // 共阴极数码管，段引脚高电平点亮
}

// 显示指定位置的数字
void displayDigit(int digit, int position)
{
  // 关闭所有位选引脚
  for (int i = 0; i < 4; i++)
    digitalWrite(digitPins[i], HIGH);

  digitalWrite(dpPin, LOW); // dpPin
  // 显示当前位的数字
  displayNumber(digit);
  // 使能当前位
  digitalWrite(digitPins[position], LOW);
}

// 显示时钟点
void displayColon(void)
{
  // 关闭所有位选引脚
  for (int i = 0; i < 4; i++)
    digitalWrite(digitPins[i], HIGH);

  clear();
  // 点亮时钟点
  digitalWrite(digitPins[1], LOW); // D2
  digitalWrite(dpPin, HIGH);       // dpPin
}

// 更新灯状态
void updateLights(void)
{
  switch (currentState)
  {
    case DISPLAY_TIME:
      digitalWrite(lightPins[0], HIGH); // 绿灯亮
      digitalWrite(lightPins[1], LOW);  // 红灯灭
      digitalWrite(lightPins[2], (currentMinutes == alarmMinutes) ? HIGH : LOW); // light3 根据分钟匹配
      break;
    case SET_ALARM_MINUTE:
    case SET_ALARM_SECOND:
      digitalWrite(lightPins[0], LOW);  // 绿灯灭
      digitalWrite(lightPins[1], HIGH); // 红灯亮
      digitalWrite(lightPins[2], LOW);  // light3 灭
      break;
    case ALARM_TRIGGERED:
      digitalWrite(lightPins[0], LOW);  // 绿灯灭
      digitalWrite(lightPins[1], HIGH); // 红灯常亮（不再闪烁）
      digitalWrite(lightPins[2], HIGH); // light3 亮
      break;
  }
}

// 主循环
void loop(void)
{
  unsigned long currentTime = millis();

  // 检测按钮状态
  for (int i = 0; i < numButtons; i++)
  {
    int buttonVoltage = analogRead(buttonPins[i]);
    int currentButtonState = (buttonVoltage < 500) ? 0 : 1;

    if (currentButtonState != lastButtonStates[i])
    {
      if (currentTime - lastButtonPressTimes[i] > debounceDelay)
      {
        if (currentButtonState == 0) // 按下
        {
          buttonStates[i] = 1;
          buttonProcessed[i] = false; // 重置处理标志
          Serial.print("Button ");
          Serial.print(i + 1);
          Serial.println(" pressed!");
        }
        else // 松开
        {
          buttonStates[i] = 0;
          Serial.print("Button ");
          Serial.print(i + 1);
          Serial.println(" released!");
        }
        lastButtonPressTimes[i] = currentTime;
      }
      lastButtonStates[i] = currentButtonState;
    }
  }

  // 处理长按检测
  bool button1Held = (buttonStates[0] == 1 && (currentTime - lastButtonPressTimes[0] > longPressDelay));
  static unsigned long button23HoldStart = 0;
  static bool button23Processed = false;
  bool button23Held = false;

  if (buttonStates[1] == 1 && buttonStates[2] == 1)
  {
    if (button23HoldStart == 0)
      button23HoldStart = currentTime; // 记录同时按下的起始时间
    if (currentTime - button23HoldStart > longPressDelay && !button23Processed)
      button23Held = true; // 长按超过 1s
  }
  else
  {
    button23HoldStart = 0; // 重置起始时间
    button23Processed = false; // 重置处理标志
  }

  // 处理三个按钮同时长按检测
  static unsigned long button123HoldStart = 0;
  static bool button123Processed = false;
  bool button123Held = false;

  if (buttonStates[0] == 1 && buttonStates[1] == 1 && buttonStates[2] == 1)
  {
    if (button123HoldStart == 0)
      button123HoldStart = currentTime; // 记录三个按钮同时按下的起始时间
    if (currentTime - button123HoldStart > longPressDelay && !button123Processed)
      button123Held = true; // 长按超过 1s
  }
  else
  {
    button123HoldStart = 0; // 重置起始时间
    button123Processed = false; // 重置处理标志
  }

  // 状态机逻辑
  switch (currentState)
  {
    case DISPLAY_TIME:
      if (buttonStates[0] == 1 && !buttonProcessed[0] && !button1Held) // Button 1 短按
      {
        currentState = SET_ALARM_MINUTE;
        buttonProcessed[0] = true;
      }
      if (currentMinutes == alarmMinutes)
      {
        currentState = ALARM_TRIGGERED;
      }
      break;

    case SET_ALARM_MINUTE:
      if (button1Held) // Button 1 长按
      {
        currentState = DISPLAY_TIME;
      }
      else if (buttonStates[1] == 1 && !buttonProcessed[1]) // Button 2 短按
      {
        alarmMinutes++;
        if (alarmMinutes >= 60) alarmMinutes = 0;
        buttonProcessed[1] = true;
      }
      else if (buttonStates[2] == 1 && !buttonProcessed[2]) // Button 3 短按
      {
        alarmMinutes--;
        if (alarmMinutes < 0) alarmMinutes = 59;
        buttonProcessed[2] = true;
      }
      else if (button23Held) // Button 2 & 3 长按 1s
      {
        currentState = SET_ALARM_SECOND;
        button23Processed = true; // 标记已处理
      }
      break;

    case SET_ALARM_SECOND:
      if (button1Held) // Button 1 长按
      {
        currentState = DISPLAY_TIME;
      }
      else if (buttonStates[1] == 1 && !buttonProcessed[1]) // Button 2 短按
      {
        alarmSeconds++;
        if (alarmSeconds >= 60) alarmSeconds = 0;
        buttonProcessed[1] = true;
      }
      else if (buttonStates[2] == 1 && !buttonProcessed[2]) // Button 3 短按
      {
        alarmSeconds--;
        if (alarmSeconds < 0) alarmSeconds = 59;
        buttonProcessed[2] = true;
      }
      else if (button23Held) // Button 2 & 3 长按 1s
      {
        currentState = SET_ALARM_MINUTE;
        button23Processed = true; // 标记已处理
      }
      break;

    case ALARM_TRIGGERED:
      if (buttonStates[1] == 1 && !buttonProcessed[1]) // Button 2 短按
      {
        currentState = DISPLAY_TIME;
        buttonProcessed[1] = true;
      }
      else if (button123Held) // Button 1, 2, 3 同时长按 1s
      {
        currentState = DISPLAY_TIME;
        button123Processed = true; // 标记已处理
      }
      break;
  }

  // 每秒更新当前时间
  if (currentTime - lastTimeUpdate >= 1000) // 1000ms = 1s
  {
    currentSeconds++;
    if (currentSeconds >= 60)
    {
      currentSeconds = 0;
      currentMinutes++;
      if (currentMinutes >= 60)
        currentMinutes = 0;
    }
    lastTimeUpdate = currentTime;
  }

  // 控制闪烁
  if (currentTime - lastBlink >= blinkInterval)
  {
    blinkState = !blinkState;
    lastBlink = currentTime;
  }

  // 多路复用显示
  if (currentTime - lastScan >= 3)
  {
    int minuteTens, minuteUnits, secondTens, secondUnits;

    if (currentState == DISPLAY_TIME || currentState == ALARM_TRIGGERED)
    {
      minuteTens = currentMinutes / 10;
      minuteUnits = currentMinutes % 10;
      secondTens = currentSeconds / 10;
      secondUnits = currentSeconds % 10;
    }
    else // SET_ALARM_MINUTE 或 SET_ALARM_SECOND
    {
      minuteTens = alarmMinutes / 10;
      minuteUnits = alarmMinutes % 10;
      secondTens = alarmSeconds / 10;
      secondUnits = alarmSeconds % 10;
    }

    switch (scanPosition)
    {
      case 0: // D1: 分钟十位
        if (currentState == SET_ALARM_MINUTE && !blinkState) clear();
        else displayDigit(minuteTens, 0);
        break;
      case 1: // D2: 分钟个位
        if (currentState == SET_ALARM_MINUTE && !blinkState) clear();
        else displayDigit(minuteUnits, 1);
        break;
      case 2: // D3: 秒钟十位
        if (currentState == SET_ALARM_SECOND && !blinkState) clear();
        else displayDigit(secondTens, 2);
        break;
      case 3: // D4: 秒钟个位
        if (currentState == SET_ALARM_SECOND && !blinkState) clear();
        else displayDigit(secondUnits, 3);
        break;
      case 4: // 时钟点
        if (currentState == ALARM_TRIGGERED && !blinkState) clear();
        else displayColon();
        break;
    }

    scanPosition++;
    if (scanPosition >= 5) scanPosition = 0;
    lastScan = currentTime;
  }

  // 更新灯状态
  updateLights();
}