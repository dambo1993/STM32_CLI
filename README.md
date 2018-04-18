# STM32_CLI

Projekt umożliwia sterowanie podstawowymi funkcjami mikrokontrolerów STM32 z poziomu terminala uart. Przygotowany jako test biblioteki do parsowania komend oraz do testowania płytek po polutowaniu. 

## Zawartość repozytorium

W repozytorium znajdziemy:
* projekt (Atollic) dla STM32F0 z uartem skonfigurowanym na piny PA2 i PA3 i prędkości 115200
* jak dokończę konsolkę to dodam plik przycisków do niej do łatwego sterowania

### Dostępne komendy

* sterowanie GPIO
  * Konfiguracja danego pinu - AT+GPIO_INIT=<port_gpio>,<nr_pinu>,<tryb>,<parametr> gdzie:  
    * port_gpio -> GPIOA/GPIOB/GPIOC/GPIOD/GPIOF (w F030F4P6 nie ma E)  
    * nr_pinu -> 0-15  
    * tryb -> OUTPUT, INPUT, ALTERNATE, ANALOG  
    * parametr -> zalezny od trybu -> jeśli jest to:  
      - OUTPUT -> 0 - stan niski, 1 - stan wysoki  
      - INPUT -> 0 - no pullup/pulldown, 1 - pullup, 2 - pulldown  
      - ANALOG -> pomijalny  
      - ALTERNATE -> numer funkcji alternate, ale obecnie niewykorzystywane - może w przyszłości  
    Przykład:  
    AT+GPIO_INIT=GPIOA,6,OUTPUT,0  

  * Ustawienie stanu pinu: AT+GPIO_OUT=<gpio>,<numer_pinu>,<stan(0/1)>  
    tu chyba nie muszę tłumaczyć :)  
    Przykład:  
    AT+GPIO_OUT=GPIOA,5,1  


  * Odczyt pinu: AT+GPIO_IN=<gpio>,<numer_pinu>
    mozliwa odpowiedź od układu to:  
    * Stan pinu: HIGH  
    * Stan pinu: LOW
  
* sterowanie ADC
  * Inicjalizacja ADC: AT+ADC_INIT
  tu wszystko jasne.  
  Oczywiście musimy ustawić wybrany pin w tryb analog komendą:  
  AT+GPIO_INIT=GPIOA,5,ANALOG,0  

  * I pomiar robimy w taki sposób: AT+ADC_START=<numer_kanału_ADC>
  kanał odczytujemy z dokumentacji.  
  Przykład pomiaru:  
  Wynik pomiaru ADC to: 2417  
  
  * Ale idąc trochę dalej - sam wynik z przetwornika dla nas nie jest zbytnio przyjazny. Dlatego istnieje kolejna komenda:
AT+ADC_START_V=<numer_kanalu>
i ona zwraca wynik w takiej formie:  
Napięcie na ADC to: 1,946V  
Skąd to przeliczenie? Otóż wynik jest mnożony przez 3300 (bo napięcie odniesienia to 3v3), a potem dzielony przez 4096(12bitowy przetwornik). Wyświetlam najpierw ten wynik przez 1000, potem modulo i mamy ładny zapis z 3 miejscami po przecinku.  

  * Nie zawsze jednak mierzymy napięcia, które toleruje ADC procka - czasem potrzebny jest dzielnik. Jak więc wtedy to przeliczyć? Otóż wspomniane wyżej współczynniki są zapisane w dwóch zmiennych (A i B). Możemy zobaczyć ich wartości komendą:
  AT+ADC_GET_AB  
  A=3300 B=4096  

  * I możemy je ustawić jak tylko chcemy komendami:
    * AT+ADC_SET_A=<wartosc_wspolczynnika_A>
    * AT+ADC_SET_B=<wartosc_wspolczynnika_B>
    
## Autor

* **dambo** - [Blog](http://projektydmb.blogspot.com)

## Dyskusja na temat bibliotkeki
* wątek na forum microgeek.eu : [https://microgeek.eu/viewtopic.php?f=48&t=1634&p=10987#p10987]
