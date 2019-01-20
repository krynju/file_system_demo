Zadanie polegało na napisanie programu realizującego proste funkcje systemu plików w języku C.

Wymagana funkcjonalność to:

-   tworzenie wirtualnego dysku,

-   kopiowanie pliku z dysku systemu na dysk wirtualny,

-   kopiowanie pliku z dysku wirtualnego na dysk systemu,

-   wyświetlanie katalogu dysku wirtualnego,

-   usuwanie pliku z wirtualnego dysku,

-   usuwanie wirtualnego dysku,

-   wyświetlenie zestawienia z aktualną map± zajętości wirtualnego dysku

# Model systemu plików

System plików jest jednokatalogowy i może przechowywać maksymalnie 128 plików. Wpis pojedynczego pliku w metadanych zajmuje 32B. Zawiera on na początku bajt określający czy dany wpis jest ważny. Następnie zarezerwowane jest w nim 27 bajtów na nazwę pliku. Na końcu znajduje się miejsce zarezerwowane na dwie liczby typu UInt16, które określają offset bazowy pliku w części danych i rozmiar pliku.

<img src="/media/image1.png" style="width:4.20365in;height:1.38973in" />

Rysunek 1: metadane pliku

<img src="/media/image2.png" style="width:2.49444in;height:3.18403in" />

System plików jest w stanie przechowywać 64KiB danych. Pliki przechowywane są w sposób ciągły, w metadanych przechowywany jest offset startowy oraz rozmiar pliku. Dane te są 16 bitowe ze względu na to, że taka długość offsetu wystarcza na pełne zaadresowanie przestrzeni danych.

Model ten jest w stanie bez przeszkód realizować określoną funkcjonalność. Jest on również dobrym przykładem jak nie powinien być zaprojektowany system plików i można na nim pokazać kilka sytuacji, w których bardzo słabo sprawdza się zastosowanie takiego modelu. Jedyną jego zaletą jest prostota i szybkość implementacji i głównie z tego powodu został on zaimplementowany z celu wykonania zadania.

# Realizacja funkcjonalności

## tworzenie wirtualnego dysku

int create\_filesystem(char \*filesys\_name);  
./filesysexec -c filesys\_name

Tworzy plik system plików i alokuje w nim wystarczająco pamięci na przedział metadanych i danych.

## usuwanie wirtualnego dysku

int remove\_filesystem(char \*filesys\_name);  
./filesysexec -d filesys\_name

Usuwa plik z systemem plików.

## kopiowanie pliku z dysku systemu na dysk wirtualny

int add\_file(char \*filesys\_name, char \*file\_name);  
./filesysexec -f filesys\_name -a file\_name  
int add\_file\_as(char \*filesys\_name, char \*file\_name, char \*output\_name);  
./filesysexec -f filesys\_name -a file\_name -o output\_name

Kopiuje plik o podanej w argumencie nazwie do systemu plików. Pierwsza funkcja dodaje plik do systemu plików o tej samej nazwie, druga zaś dodaje do systemu plików plik o podanej nazwie, co przydaje się w trakcie testowania systemu plików.

## kopiowanie pliku z dysku wirtualnego na dysk systemu

int copy\_file\_from(char \*filesys\_name, char \*file\_name, char \*output\_name);  
./filesysexec -f filesystem -a file\_name -o output\_name

Kopiuje plik o podanej w argumencie nazwie z systemu plików i zapisuje go z podaną w argumencie nazwą.

## usuwanie pliku z wirtualnego dysku

int remove\_file(char \*filesys\_name, char \*file\_name);  
./filesysexec -f filesys\_name -d file\_name

Usuwa plik o podanej w argumencie nazwie z systemu plików.

## wyświetlanie katalogu dysku wirtualnego

int display\_catalogue(char \*filesys\_name);  
./filesysexec -l filesystem

Wyświetla katalog systemu plików o nazwie podanej w argumencie.

## wyświetlenie zestawienia z aktualną map zajętości wirtualnego dysku 

int display\_map(char \*filesys\_name);  
./filesysexec -lr filesystem

Wyświetla mapę pamięci systemu plików.

# Przygotowane skrypty testowe

## capacity\_error.sh

Pokazuje sytuacje, w której dodawany plik nie zmieści się w systemie plików przez brak miejsca w przedziale danych.

## files\_number\_error.sh

Pokazuje sytuacje, w której dodawany plik nie zmieści się w systemie plików, ponieważ brakuje miejsca na wpis w przedziale metadanych.

## copy\_to.sh

Pokazuje funkcjonalność kopiowania plików do systemu plików

## copy\_from.sh

Pokazuje funkcjonalność kopiowania pliku z systemu plików z powrotem do systemu.

## removing\_files.sh

Pokazuje funkcjonalność usuwania plików z systemu plików.

## fragmentation\_example.sh

Pokazuje dość ekstremalną sytuacje fragmentacji zewnętrznej. Sytuację, w której tylko \~50% przedziału danych jest zajęte a nie można dodać pliku ze względu na brak wystarczająco dużego miejsca w przedziale danych.

# Wnioski 

Ten system plików poza swoją prostotą i łatwością implementacji nie posiada żadnych innych zalet. Idealnie pokazuje, że stały przedział pamięci na ilość plików jest złym pomysłem, jak również, że alokacja plików ciągła wymaga sprawnego mechanizmu defragmentacji w celu wydajnego korzystania z pamięci. System plików realizuje jednak określoną funkcjonalność poprawnie i bez żadnych przeszkód związanych z samym modelem systemu plików.
