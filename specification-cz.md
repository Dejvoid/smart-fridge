# smart-fridge
Projekt se zabývá implementací systému "chytré lednice." Jedná se o jednoduché zařízení, které bude mít uživatel u lednice/v kuchyni. Toto zařízení pak bude uživateli umožňovat skenovat produkty (čárový kód a datum spotřeby), které bude do lednice vkládat, a monitorovat aktuální teplotu v lednici. Informace o produktech pak budou uživateli k dispozici ve webové aplikaci.
Projekt se skládá ze dvou částí: 
- Microcontrolleru, který se stará o sběr dat a zobrazení aktuálních notifikací.
- Server, který data zpracovává. Poskytuje seznam produktů, upozorňuje uživatele na blížící se datum spotřeby, udržuje databázi produktů a nákupní seznam.

## Zařízení pro sběr dat
O sběr dat se stará deska ESP-32 s připojeným teploměrem, malým displejem a kamerou. 
Hlavní úkol této desky je skenování čárových kódů, skenování data spotřeby, kontrola teploty, zobrazování jednoduchých informací/upozornění na displeji.

## Server
Jako server bude pracovat Raspberry Pi. Bude se starat hlavně o databázi produktů a uživatelský interface ve formě webové aplikace.
Hlavní funkce: 
- Zobrazení seznamu produktů v lednici
- Posílání notifikací pro zařízení a zpracování dat ze zařízení
- Zobrazení přehledu o blížících se datech trvanlivosti
- Údržba nákupního seznamu
- Přístup k databázi

## Uživatelský interface
Uživatel si bude moci ve webové aplikaci zobrazit seznam produktů v lednici a jejich aktuální počet, uložené recepty z dostupných surovin, nákupní seznam.
K zařízení pro sběr dat pak bude připojen menší displej, kde uživatel uvidí, co naskenoval a oznámení systému.

## Komunikace mezi zařízeními
Zařízení budou připojena na společnou Wi-Fi síť, kde mezi sebou budou komunikovat na základě HTTP protokolu.

## Hardwaree
 - ESP-32-CAM
 - Raspberry Pi
 - Teploměr
 - LCD displej

## Technologie
 - ESP-32 - C/C++ ESP-IDF framework
 - Raspberry Pi - Linux, C# web server
 - Knihovny: [ESP-WHO](https://github.com/espressif/esp-who), [Open-CV](https://github.com/opencv/opencv)

## Hlavní Funkce
 - Seznam potravin v lednici
 - Hlídání data spotřeby
 - Monitorování teploty
 - Vedení nákupního seznamu
 - Pravidelné upozornění (například prášky, které by měl uživatel brát každý den)
 - Recepty ze surovin v lednici

## Vývojové prostředí
Visual Studio Code + Espressiff IDE Extension

## Zdroje
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/) (2024, v5.1.2)
- [C# documentation](https://learn.microsoft.com/en-us/dotnet/csharp/) (2024)
- [C++ reference](https://en.cppreference.com/w/) (2024)
- Developing IoT Projects with ESP32 - Second Edition, Oner, V. O. (2023), ISBN: 1803237686
- ESP32 Formats and Communication, Cameron, N. (2023), ISBN: 1484293789
- Build Your Own IoT Platform. Develop a Flexible a Scalable Internet of Things Platform, Tamboli, A. (2022) ISBN: 9781484280720
- [OpenCV](https://github.com/opencv/opencv) (2024, v4.9.0)
