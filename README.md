# DE2-Project
# Měření výšky vodní hladiny v nádrži

### Team members

* Czmelová Zuzana (responsible for xxx)
* Shelemba Pavlo (responsible for xxx)
* Točený Ivo (responsible for xxx)
* Tomešek Jiří (responsible for xxx)

Link to this file in your GitHub repository:

https://github.com/xtomes07/DE2-Project

### Table of contents

* [Project objectives](#objectives)
* [Hardware description](#hardware)
* [Libraries description](#libs)
* [Main application](#main)
* [Video](#video)
* [References](#references)

<a name="objectives"></a>

## Project objectives

Cílem projektu je pomocí arduina a programovacího jazyka C realizovat ovládání nádrže s kontrolou vodní hladiny. Pro snímání vodní hladiny byl vybrán ultrazvukový senzor HC-SR04, díky kterému je možné přesně zjistit výšku hladiny vody v nádrži na základě doby, za kterou ultrazvukový signál dorazí k vodní hladině a zpět k senzoru. Stav hladainy se následně bude zobrazovat na dipleji Hd44780. Jako příslušenství k nádrži byl zvolen ventil pro vypouštění vody samospádem a čerpadlo pro napouštění. K ventilu bude připojen servo motor, který ventil ovládá. Ventil bude pracovat automaticky na základě vodní hladiny, když hrozí přetečení nádrže, ventil se automaticky otevře a odpustí vodu. Lze ho také ovládat manuálně pomocí tlačítka. Čerpadlo bude ovládano pouze ručně přes tlačítko, které dáva signál arduinu a arduino na základě programu spíná relé, které spíná čerpadlo. Program zároveň bude vyhodnocovat stav hladiny a v případě,že by při čerpání čerpadla hrozilo přetečení nádrže, tak čerpadlo automaticky vypne. Informace o stavu hladiny vody v nádrži pomocí procent, On/Off čepadla a Open/Close ventilu budou zobrazovány na displaeji Hd44780. K arduinu budou také připojeny dvě pomocné signalizační led diody. Zelená dioda bude svítit, když je nádrž plná vody a blikat když bude čerpadlo čerpat vodu do nádrže. Červená led dioda bude svítit, když je nádrž prázdná a svítit, když odpoštěcí ventil otevřený.

<a name="hardware"></a>

## Hardware description

- Arduino UNO
- Ultrazvukový senzor HC-SR04
- LCD displej Hd44780
- Relé pro ovládání čerpadla
- Servo motor pro ovládání ventilu
- Čerpadlo (v SimulID ho představuje DC motor)
- Dva spínače pro ruční ovladání čerpadla a ventilu
- Zelená a červená LED dioda pro pomocnou signalizaci

K arduinu uno je přes piny D4 až D7, B1 a B0 připojený LCD displej Hd44780, na kterém se zobrazují aktuální informace o dění a stavu hladiny. Přes piny B6 a B7 jsou připojeny pomocné signalizační LED diody. Samotný senzor HC-SR04 je připojen přes piny D0 a D2. Na pinu D0 je připojen Trig a na D2 je připojen pin Echo. Spínač pro servo motor, který ovláda ventil, je na pinu C2 a spínač pro čerpadlo je na pinu C1. Ovladaní servo motoru je na pinu B2 a čerpadlo je ovladané přes relé, které je připojeno na pin C0.
### Zapojení obvodu:
![zapojení obvodu simulace](Images/SchemaZapojeni.PNG)

<a name="libs"></a>

## Libraries description

Pro ovladaní displeje byly pouzity knihovny LCD.C, LCD.H a LCD_DEFINITION.H. Pro vstupní a výstupní obvody knihovny GPIO.C a GPIO.H. Pro ovladání časovače byla použita hnikovna TIMER.H. Všechny tyto knihovny byly vytvořeny v hodinách DE2.

[GPIO.C](https://github.com/xtomes07/DE2-Project/blob/main/WaterTankController/WaterTankController/gpio.c)

[GPIO.H](https://github.com/xtomes07/DE2-Project/blob/main/WaterTankController/WaterTankController/gpio.h)

[LCD.C](https://github.com/xtomes07/DE2-Project/blob/main/WaterTankController/WaterTankController/lcd.c)

[LCD.H](https://github.com/xtomes07/DE2-Project/blob/main/WaterTankController/WaterTankController/lcd.h)

[LCD_DEFINITIONS.H](https://github.com/xtomes07/DE2-Project/blob/main/WaterTankController/WaterTankController/lcd_definitions.h)

[TIMER.H](https://github.com/xtomes07/DE2-Project/blob/main/WaterTankController/WaterTankController/timer.h)

<a name="main"></a>

## Main application

Write your text here.

<a name="video"></a>

## Video

![video](https://www.youtube.com/watch?v=sDZL-BgPtco)

<a name="references"></a>

## References

1. Write your text here.
