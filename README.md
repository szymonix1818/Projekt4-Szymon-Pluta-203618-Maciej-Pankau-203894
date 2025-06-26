
# Projekt 4 – Szymon 203618, Maciek 203894

## Opis
Aplikacja Windows wykorzystująca GDI+ do wizualizacji i filtracji poruszających się figur geometrycznych po taśmie produkcyjnej (koła, kwadraty, trójkąty). Użytkownik może sterować prędkością ruchu, częstotliwością generowania oraz filtrować figury w zależności od ich typu.

## Funkcjonalności

- Rysowanie figur (koło, kwadrat, trójkąt)
- Dynamiczna animacja:
  - Ruch poziomy
  - Spadanie
  - Wspinanie się 
  - Zjeżdżanie po pochyłości
- Filtracja figur po typie (koło, kwadrat, trójkąt)
- Zliczanie poprawnych i błędnych produktów
- Interaktywne menu i suwaki do regulacji parametrów
- Tryby generowania figur:
  - Wszystkie
  - Tylko koła
  - Tylko kwadraty
  - Tylko trójkąty
  - Koło + kwadrat
  - Trójkąt + kwadrat
- Tryby filtracji:
  - brak filtra
  - Tylko koła
  - Tylko kwadraty
  - Tylko trójkąty
- Predefiniowane opcje filtrów + trybów generowania

## Wymagania

- System operacyjny Windows
- Visual Studio z obsługą GDI+ i WinAPI
- `comctl32.lib` i `Gdiplus.lib`

## Kompilacja

Upewnij się, że w pliku `.vcxproj` masz dołączone odpowiednie biblioteki:

```cpp
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Gdiplus.lib")
```

## Struktura projektu

- `Figure` – struktura reprezentująca figurę geometryczną
- `State` – stan animacji danej figury (ruch, spadanie, filtracja itp.)
- `Mode` – tryb generowania figur
- `FilterType` – typ aktualnie włączonego filtra

## Kluczowe zmienne

| Zmienna               | Opis                                      |
|-----------------------|-------------------------------------------|
| `movementSpeed`       | Prędkość ruchu figur                      |
| `generationRate`      | Częstotliwość generowania nowych figur    |
| `correctProducts`     | Liczba poprawnych produktów               |
| `incorrectProducts`   | Liczba błędnych produktów                 |
| `trianglesCompleted`  | Ukończone trójkąty                        |
| `squaresCompleted`    | Ukończone kwadraty                        |

## Obsługa GUI

- **Suwaki**: regulują prędkość ruchu i częstotliwość generowania
- **Menu**: zmiana trybu generowania i filtrowania (Te 2 osobne menu co są u góry programu, osobno dla filtra i dla trybu generowania)
- **Przyciski radiowe**: szybkie ustawienia predefiniowanych kombinacji (które były w treści zadania)
## Zrzut ekranu

![image](https://github.com/user-attachments/assets/b5879fdc-e20c-413c-a515-9e0fd3a77eee)

## Autorzy

- **Szymon Pluta** – 203618  
- **Maciek Pankau** – 203894
