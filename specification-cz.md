# smart-fridge
Projekt se zabývá implementací systému "chytré lednice." Jedná se o jednoduché zařízení, které bude mít uživatel u lednice/v lednici. Toto zařízení pak bude uživateli umožňovat skenovat produkty, které bude do lednice vkládat a monitorovat aktuální teplotu v lednici. Informace o produktech pak budou uživateli k dispozici ve webové aplikaci.
Projekt se skládá ze dvou částí: 
Microcontrolleru, který se stará o sběr dat
Server, který data zpracovává a poskytuje je uživateli

## Zařízení pro sběr dat
O sběr dat se stará deska ESP-32 s připojeným teploměrem, malým displejem a kamerou. 
Hlavní úkol této desky je skenování čárových kódů, skenování data spotřeby, kontrola teploty, zobrazování jednoduchých informací/upozornění na displeji.

## Server
Jako server bude pracovat Raspberry Pi. Bude se starat hlavně o databázi produktů a uživatelský interface ve formě webové aplikace.

### Uživatelský interface
Uživatel si bude moci ve webové aplikaci zobrazit seznam produktů v lednici a jejich aktuální počet, uložené recepty z dostupných surovin, nákupní seznam
K zařízení pro sběr dat pak bude připojen menší displej, kde uživatel uvidí, co naskenoval.

## Komunikace mezi zařízeními
Zařízení budou připojena na společnou Wi-Fi síť, kde mezi sebou budou komunikovat na základě HTTP protokolu.

## Hardwaree
 - ESP-32-CAM
 - Raspberry Pi
 - Dallas DS18B20 (teploměr)
 - SPI/I2C displej 128x64

## Technologie
 - ESP-32 - C++ ESP-IDF framework
 - Raspberry Pi - Linux, C# web server
 - Knihovny: (pravděpodobně něco na práci s kamerou - OCR)

## Hlavní Funkce
 - Seznam potravin v lednici
 - Hlídání data spotřeby
 - Monitorování teploty
 - Vedení nákupního seznamu
 - Pravidelné upozornění (například prášky, které by měl uživatel brát každý den)
 - Recepty ze surovin v lednici

## Vývojové prostředí
Visual Studio Code + PlatformIO