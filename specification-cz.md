# smart-fridge
Projekt se zabývá implementací systému "chytré lednice." Jedná se o jednoduché zařízení, které bude mít uživatel u lednice/v lednici. Toto zařízení pak bude uživateli umožňovat skenovat produkty, které bude do lednice vkládat. Informace o produktech pak budou uživateli k dispozici ve webové aplikaci.
Projekt se skládá ze dvou částí: 
Microcontrolleru, který se stará o sběr dat
Server, který data zpracovává a poskytuje je uživateli

## Zařízení pro sběr dat
O sběr dat se stará deska ESP-32 s připojeným teploměrem, malým displejem a kamerou. 
Hlavní úkol této desky je skenování čárových kódů, skenování data spotřeby, kontrola teploty, zobrazování jednoduchých informací/upozornění na displeji.

## Server
Jako server bude pracovat Raspberry Pi. Bude se starat hlavně o databázi produktů a uživatelský interface ve formě webové aplikace.

### Uživatelský interface
Uživatel si bude moci zobrazit seznam produktů v lednici a jejich aktuální počet, uložené recepty z dostupných surovin, nákupní seznam

## Komunikace mezi zařízeními
Zařízení budou připojena na společnou Wi-Fi síť, kde mezi sebou budou komunikovat na základě HTTP protokolu.

## Funkce
