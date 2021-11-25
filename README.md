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

Cílem projektu je pomocí arduina a programovacího jazyka C realizovat ovládání nádrže s kontrolou vodní hladiny. Pro snímání vodní hladiny byl vybrán ultrazvukový senzo HC-SR04, díky kterému je možné přesně zjistit kolik vody je v nádrži na základě doby, za kterou ultrazvukový signál dorazí k vodní hladině a zpět k senzoru. V našem případě je velikost nádrže limitovaná senzorem, protože senzor pracuje se vzdáleností 2cm až 400cm. Jako příslušenství k nádrži byl zvolen ventil pro vypouštění a čerpadlo pro napouštění. K ventilu je připojen servo motor, který ventil ovládá. Ventil pracuje automaticky na základě vodní hladiny, když hrozí přetečení nádrže, ventil se automaticky otevře a odpustí vodu. Lze ho také ovládat manuálně pomocí tlačítka. Čerpadlo je ovládano pouze ručně přes tlačítko, které dáva signál arduinu a arduino na základě programu spíná relé, které spíná čerpadlo. Program zaroveň vyhodnocuje stav hladiny a v případě,že by hrozilo přetečení nádrže, tak čerpadlo automaticky vypne. Informace o stavu hladiny vody v nádrži, On/Off čepadla a Open/Close ventilu jsou zobrazovány na displaeji Hd44780.

<a name="hardware"></a>

## Hardware description

Write your text here.

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
