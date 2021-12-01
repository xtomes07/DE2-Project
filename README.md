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
- HC-SR04
- Hd44780
- Relé pro ovládání čerpadla
- Servo motor
- Čerpadlo
- Dva přepínače
- Zelená a červená LED dioda



<a name="libs"></a>

## Libraries description

Write your text here.

<a name="main"></a>

## Main application

Write your text here.

<a name="video"></a>

## Video

Write your text here

<a name="references"></a>

## References

1. Write your text here.
