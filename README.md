# Viorica
Intelligen home assistant.

====== Catalin-Mihai Vajaiala-Tomici (87621) -- Viorica ======
==== Introducere ===
Viorica este un asistent personal pentru locuinte inteligente. Aceasta va raspunde la anumite comenzi vocale presetate.

==== Descriere Generala ====

Viorica se conecteaza la wifi cu ajutorul modulului ESP8266. Dupa ce se conecteaza, aceasta va trimite catre server un stream audio continuu.

Dupa ce se activeaza, aceasta va putea raspunde la cereri vocale. Printre cererile pe care ar trebui sa poata sa le rezolve se numara:
  - Controlul unor becuri la care e conectata
  - Sa caute si sa redea o anumita melodie ceruta.

Serverul primeste streamul audio si salveaza odata la cateva secunde ce a primit intr-un fisier .wav. 

Pe acest fisier, serverul aplica un algoritm de speech-to-text pentru a recunoaste comenzile vocale si in functie de ce comenzi au fost date, trimite
un anumit raspuns inapoi catre microcontroller.

{{:prj2019:apredescu:viorica-diagrama-2.png?600|}}


===== Informații hardware =====

=== Listă de piese: ===
  - Placă de bază PM 2019 (cu microcontroller Atmega324PA)
  - Microfon (senzor de sunet) Adafruit MAX9814
  - Amplificator de sunet
  - Difuzor
  - Modul Wifi ESP8266
  - Level shifter 5V - 3.3V
  - Stabilizator de tensiune 5V - 3.3V
  - Modul card SD
  - [Diverse] Fire, rezistențe, etc

=== Schema electrica ===
{{:prj2019:apredescu:viorica-electric.png?600|}}


===== Software Design =====

=== Tools ===

 * Pentru implementare am folosit ATMEL Studio 7 si Arduino IDE (pentru programarea modulului wifi ESP8266 si pentru monitorizarea interfetei seriale)
 * Pentru server Python3
 * Pentru debugging am avut un modul USB-TTL

=== Functionare Microcontroller ===

Microcontrolerul initiaza ADC-ul si interfata seriala USART. 
Dupa ce initializarile au fost facute, se citesc date de la ADC (microfon) cu un sample rate de 10KHz si se trimit datele prin USART catre modulul de wifi.

Modulul de wifi primeste amplitudini ale semnalului de la microfon si le trimite mai departe catre server si asteapta un raspuns, pe care il trimite catre
microcontroller. 

Cand microcontrollerul primeste un raspuns de la modulul wifi pe USART RX se produce o intrerupere care va decoda raspunsul si va actiona.

Raspunsurile si actiunile suportate la momentul actual sunt aprindere si stingere de led-uri.


=== Functionare Server Web === 

Serverul a fost scris in Python3. Acesta deschide un socket TCP pe care asteapta conexiuni de la clienti. 

Dupa ce un client se conecteaza, ii va trimite un stream audio (amplitudini ale semnalului de la microfonul conectat la microcontroller).

Odata la cateva secunde, serverul salveaza informatiile primite pana atunci intr-un fisier .wav si il analizeaza folosind un API de speech-to-text.
Biblioteca folosita pentru recunoastearea vocala este SpeechRecognition si foloseste API de la google. 

Daca in fisierul .wav a fost gasita o comanda vocala cunoscuta, atunci serverul trimite inapoi catre microcontroller un raspuns corespunzator comenzii.


Serverul a fost hostat pe o masina Amazon EC2.

===== Rezultate =====

Am reusit sa implementez comunicarea dintre microcontroller si server. Serverul primeste cu succes un stream audio, insa nu am reusit sa il salvez intr-un fisier .wav. Problema pe care am intampinat-o cel mai probabil a fost sample rate-ul care era gresit. Tot ce am reusit sa salvez in fisier au fost cateva secunde de noise (audio inteligibil). 

In rest, pe un fisier audio corect, recunoasterea vocala merge bine, microcontrolerul primeste raspunsuri corect de la server daca se recunoaste ceva bine si actioneaza pe cele 3 leduri legate la PB0, PB1 si PD7.

Nu am reusit sa implementez redarea de fisierelor audio si folosirea modulului SD. Probabil o sa completez in timpul liber proiectul.


===== Concluzie =====

Chiar daca nu am reusit sa duc la capat ce mi-am propus pentru proiect din cauza ca mi-am alocat prost timpul, consider ca a fost o experianta misto. A contribuit la lipsa de timp si faptul ca dupa ce imi cumparam o componenta, aflam ca mai am nevoie de inca una (de exemplu pentru modulul de wifi, am aflat ca trebuie stabilizatorul de tensiune si dupa cateva zile am aflat ca imi trebuie si un level shifter).

==== Download ====


==== Bibliografie/Resurse ====
Laboratoare PM: http://cs.curs.pub.ro/wiki/pm/start

https://cdn-learn.adafruit.com/downloads/pdf/adafruit-agc-electret-microphone-amplifier-max9814.pdf

https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/
