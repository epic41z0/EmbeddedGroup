#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TimeLib.h>


void setup();
void loop();
int random(int min, int max);
int numbViews(int payment);
void displayMessage(const char *message, bool scroll = false, bool blink = false);
void displayRandomMessage(const char *messages[], const bool scroll[], const bool blink[], int payment);
void displayAlternateMessage(const char *message1, const char *message2, const bool scroll[], const bool blink[], int payment);
int randomCustomerNumber(const int freqList[], int len, int previousCustomer = -1);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int random(int min, int max) {
    return min + random(max - min + 1);
}

int numbViews(int payment) {
    return int(float(payment) / 1000 * 2); // 2 views per 1000kr
}

void displayMessage(const char *message, bool scroll = false, bool blink = false) {
    lcd.clear();
    int messageLength = strlen(message);
    
    if (scroll && messageLength > 16) {
        for (int i = 0; i <= messageLength + 16; ++i) {
            int offset = i - 16;

            lcd.setCursor(0, 0);
            for (int j = 0; j < 16; ++j) {
                if (offset + j >= 0 && offset + j < messageLength) {
                    lcd.write(message[offset + j]);
                } else {
                    lcd.write(' ');
                }
            }

            lcd.setCursor(0, 1);
            delay(200);
          }
      } else {
        if (blink && strcmp(message, "Hederlige Harrys Bilar") == 0) {
            for (int blinkCount = 0; blinkCount < 5; ++blinkCount) {
                lcd.clear();
                delay(500);
                lcd.setCursor(0, 0);
                lcd.print("Hederlige Harrys"); // Första ordet
                lcd.setCursor(0, 1);
                lcd.print("Bilar"); 
                delay(500);
                lcd.clear();
                delay(500);
            }
            lcd.setCursor(0, 0);
            lcd.print("Hederlige Harrys"); // Första ordet
            lcd.setCursor(0, 1);
            lcd.print("Bilar"); 
        }else {
            lcd.setCursor(0, 0);
            lcd.print(message);

            if (messageLength > 16) {
                lcd.setCursor(0, 1);
                for (int i = 16; i < messageLength; ++i) {
                    lcd.write(message[i]);
                }
            }
        }
    }
} 


void displayRandomMessage(const char *messages[], const bool scroll[], const bool blink[], int payment) {
    int len = sizeof(messages) / sizeof(messages[0]);
    int randomIndex = random(0, len);
    displayMessage(messages[randomIndex], scroll[randomIndex], blink[randomIndex]);
    delay(2000);
}

void displayAlternateMessage(const char *message1, const char *message2, const bool scroll[], const bool blink[], int payment) {
    int currentMinute = minute();
    bool isEvenMinute = (currentMinute % 2 == 0);

    if (isEvenMinute) {
        displayMessage(message1, scroll[0], blink[0]);
    } else {
        displayMessage(message2, scroll[0], blink[0]);
    }
    delay(2000);
}

int randomCustomerNumber(const int freqList[], int len, int previousCustomer = -1) {
    int randomIndex = random(0, len - 1);
    if (previousCustomer == -1) {
        return freqList[randomIndex];
    } else {
        while (freqList[randomIndex] == previousCustomer) {
            randomIndex = random(0, len - 1);
        }
        return freqList[randomIndex];
    }
}

void setup(){
    lcd.begin(16, 2);
    lcd.print("Booting...");
    lcd.setCursor(0, 0);


    randomSeed(analogRead(0));
}

void loop() {
    const int displayDuration = 2000;

    const char *harryMessages[] = {"Kop bil hos Harry", "En god bilaffar(for Harry!)", "Hederlige Harrys Bilar"};
    const bool harryScroll[] = {true, false, false};
    const bool harryBlink[] = {false, false, true};
    const char *farmorMessages[] = {"Kop paj hos Farmor Anka", "Skynda innan Marten atit alla pajer"};
    const bool farmorScroll[] = {true, true};
    const bool farmorBlink[] = {false, false};
    const char *petterMessages[] = {"Lat Petter bygga at dig", "Bygga svart? Ring Petter"};
    const bool petterScroll[] = {true, false};
    const bool petterBlink[] = {false, false};
    const char *langbenMessages[] = {"Mysterier? Ring Langben", "Langben fixar biffen"};
    const bool langbenScroll[] = {false, false};
    const bool langbenBlink[] = {false, false};
    const char *iotMessages[] = {"Synas har? IOT:s Reklambyra"};
    const bool iotScroll[] = {true};
    const bool iotBlink[] = {false};

    int harryPayment = 5000;
    int farmorPayment = 3000;
    int petterPayment = 1500;
    int langbenPayment = 4000;
    int iotPayment = 1000;

    int totalPayment = harryPayment + farmorPayment + petterPayment + langbenPayment + iotPayment;
    int totLength = numbViews(totalPayment);

    int freqList[5] = {numbViews(harryPayment), numbViews(farmorPayment), numbViews(petterPayment), numbViews(langbenPayment), numbViews(iotPayment)};

    int customersFreqList[totLength];

    int lastPos = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = lastPos; j < lastPos + freqList[i]; j++) {
            customersFreqList[j] = i;
        };
        lastPos += freqList[i];
    }

    int previousCustomer = -1;
    int curreCustomer = randomCustomerNumber(customersFreqList, totLength, previousCustomer);

    for (int i = 0; i < 29; i++) {
        switch (curreCustomer) {
            case 0:
                displayMessage(harryMessages[2], harryScroll[2], harryBlink[2]);
                displayRandomMessage(harryMessages, harryScroll, harryBlink, harryPayment);
                break;
            case 1:
                displayRandomMessage(farmorMessages, farmorScroll, farmorBlink, farmorPayment);
                break;
            case 2:
                displayAlternateMessage(petterMessages[0], petterMessages[1], petterScroll, petterBlink, petterPayment);
                break;
            case 3:
                displayRandomMessage(langbenMessages, langbenScroll, langbenBlink, langbenPayment);
                break;
            case 4:
                displayRandomMessage(iotMessages, iotScroll, iotBlink, iotPayment);
                break;
        }

        previousCustomer = curreCustomer;
        curreCustomer = randomCustomerNumber(customersFreqList, totLength, previousCustomer);
    }

    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Restarting...");
}
