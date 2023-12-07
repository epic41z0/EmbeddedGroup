#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TimeLib.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int random(int min, int max) {
    return min + random(max - min + 1);
}

int numbViews(int payment) {
    return int(float(payment) / 1000 * 2); // 2 views per 1000kr
}

void displayMessage(const char *message, bool scroll = false, bool blink = false) {
    lcd.clear();

    if (scroll) {
        int messageLength = strlen(message);
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
            for (int j = 0; j < 16; ++j) {
                int charIndex = i + j;
                if (charIndex >= 0 && charIndex < messageLength) {
                    lcd.write(message[charIndex]);
                } else {
                    lcd.write(' ');
                }
            }

            delay(200);
        }
    } else {
        int x = (16 - strlen(message)) / 2;
        lcd.setCursor(x, 0);
        if (x < 0) {
            lcd.setCursor(0, 0);
        }
        lcd.print(message);

        if (blink) {
            for (int blinkCount = 0; blinkCount < 5; ++blinkCount) {
                delay(500);
                lcd.clear();
                delay(500);
                lcd.setCursor(x, 0);
                lcd.print(message);
            }
        }
    }
}

void displayRandomMessage(const char *messages[], const bool scroll[], const bool blink[], int payment) {
    int len = sizeof(messages) / sizeof(messages[0]);
    int randomIndex = random(0, len);
    displayMessage(messages[randomIndex], scroll[randomIndex], blink[randomIndex]);
    delay(20000);
}

void displayAlternateMessage(const char *message1, const char *message2, const bool scroll[], const bool blink[], int payment) {
    int currentMinute = minute();
    bool isEvenMinute = (currentMinute % 2 == 0);

    if (isEvenMinute) {
        displayMessage(message1, scroll[0], blink[0]);
    } else {
        displayMessage(message2, scroll[0], blink[0]);
    }
    delay(20000);
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

void setup() {
    lcd.begin(16, 2);
    lcd.print("Booting...");
    lcd.setCursor(0, 0);

    // Randomiserar företag vid start
    randomSeed(analogRead(0));
}

void loop() {
    const int displayDuration = 20000;

    const char *harryMessages[] = {"Kop bil hos Harry", "En god bilaffar(for Harry!)", "Hederlige Harrys Bilar"};
    const bool harryScroll[] = {true, false, false};
    const bool harryBlink[] = {false, false, true};
    const char *farmorMessages[] = {"Kop paj hos Farmor Anka", "Skynda innan Marten atit alla pajer"};
    const bool farmorScroll[] = {true, false};
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
                displayMessage(harryMessages[2], false, true);  // Display "Hederlige Harrys Bilar" with scrolling and blinking
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
