# SAKK Fejlesztői Dokumentáció

Ez a dokumentum a SAKK sakkalkalmazás technikai áttekintését nyújtja. A dokumentum frissítve lett, hogy tükrözze a modern C++ objektumorientált architektúrát.

## Architektúra áttekintés

Az alkalmazás több modulra tagolódik, szigorú objektumorientált elvek alapján:

- **`main.cpp`**: A belépési pont. Definiálja a `Game` osztályt, amely fő vezérlőként működik. Ez kezeli az állapotgépet (menük, játékciklus, elemző mód) és közvetlenül birtokolja a `BoardTree`-t.
- **`chess.cpp` / `chess.h`**: A sakk alapvető logikáját tartalmazza. Rendelkezik egy polimorf `Piece` hierarchiával (absztrakt alap `Piece`, valamint `Pawn`, `Rook`, `Knight`, `Bishop`, `Queen`, `King` leszármazottak) a bábuk értékeinek és szimbólumainak lekérdezéséhez. Ide tartozik a lépések ellenőrzése, a sáncolás és a sakk felismerése is.
- **`display.cpp` / `display.h`**: A `Display` statikus osztály kezeli az összes felhasználói felületet és beviteli műveletet. Absztrahálja a konzolos rajzolást és a beviteli olvasást, kommunikálva az `econio` könyvtárral.
- **`storage.cpp` / `storage.h`**: Definiálja a `Board` struktúrát és a `BoardTree` osztályt. A faestruktúra lehetővé teszi a történet elágazását (az Elemző módhoz). A `BoardTree` teljesen kezeli a memóriafoglalást és -felszabadítást RAII segítségével.
- **`econio.c` / `econio.h`**: Egy egyetemi könyvtár a konzolos I/O műveletekhez, amely funkciókat biztosít a színes kimenethez és a nyers bevitel olvasásához.
- **`teszt.cpp`**: A tesztprogram váza (szkeleton), amely bemutatja a polimorf bábuk példányosítását és a táblatörténet-fa felépítését.

---

## Bábuk kódolási konvenciója

A tábla állapota belsőleg egy `Piece*[8][8]` tömbként van reprezentálva a `Board` csomóponton belül. Minden cella vagy egy polimorf `Piece` alosztályra mutató pointer, vagy `nullptr` (üres mező). A bábuk lemezre mentésekor és megjelenítésekor karakterkódolást használnak:
**A világos bábuk kisbetűk, a sötét bábuk nagybetűk.**

| Bábu   | Világos (kisbetű) | Sötét (nagybetű) |
|--------|:-----------------:|:----------------:|
| Király | `k`               | `K`              |
| Vezér  | `q`               | `Q`              |
| Bástya | `r`               | `R`              |
| Futó   | `b`               | `B`              |
| Huszár | `n`               | `N`              |
| Gyalog | `p`               | `P`              |
| Üres   | `.`               | `.`              |

A `SMALL_CAP_DISTANCE` (`32`) konstans egy kisbetű és a megfelelő nagybetű közötti ASCII-különbség (`'a' - 'A' == 32`).

---

## Főbb adatszerkezetek

### Polimorf `Piece` (a `chess.h` fájlban)
Az architektúra polimorfizmust használ a bábuk tulajdonságainak meghatározásához, ismétlődő switch-utasítások elkerülése érdekében.
```cpp
class Piece {
protected:
  char m_figure;
  bool m_isWhite;
public:
  virtual char symbol() const = 0;
  virtual int value() const = 0;
  virtual ~Piece() = default;
};
```

### `Board` és `BoardTree` (a `storage.h` fájlban)
A játéktörténet egy fa struktúra, amely támogatja a többszörös analitikai elágazásokat.

```cpp
struct Board {
    Piece *board[8][8];
    Board *prev;
    int numNext;
    Board **next; // Dinamikusan allokált tömb az elágazó történethez
    int id;
    int previd;
    int selectedBranch; // Jelenleg kiválasztott ág az elemző módban
    int en_passant_col; // Az oszlop, ahol en passant ütés lehetséges (-1, ha nincs)
};
```
A `BoardTree` osztály teljesen tokozva (encapsulate) tartalmazza ezt a struktúrát. Új elágazás hozzáadásakor az `addBoard()` kibővíti a `next` mutatótömböt a `new Board*[...]` segítségével. **Megjegyzés: A projekt szigorúan kerüli az olyan szabványos tárolók használatát, mint az `std::vector`, hogy megfeleljen az egyetemi korlátozásoknak.** A memóriakezelés manuálisan történik `new[]` és `delete[]` segítségével.

### Lépésellenőrzési adatok (`Input`, `Booleans`, `Distances`)
Ezek a struktúrák értékkel/referenciával adódnak át a motorban a geometriai logika egyszerűsítése (`is_rook_valid` stb.) és a sáncolás szabályosságának nyomon követése (király/bástya mozgott-e) céljából.

---

## Mentés/Betöltés Szerializáció

A játékállapot mentése a `Board` fa bináris fájlba való szerializálásával történik az `std::ofstream` és `std::ifstream` segítségével.
Mivel a mutatókat (`prev`, `next`) nem lehet lemezre menteni, a szerializálás lelapítja a fát egy `BoardElement` struktúrákból álló folyammá. Az `id` és `previd` egészek idegen kulcsként működnek.
- **Mentés**: Mélységi bejárással végigmegy a fán, és minden csomópont tömbjét és azonosítóit kiírja.
- **Betöltés**: Szekvenciálisan olvassa az elemeket, és a `previd` alapján újra hozzácsatolja őket a szülőjükhöz a memóriában.

---

## Fordítás és fejlesztés

A C++ projekt fordításához futtasd:

```bash
g++ -std=c++17 -Wall storage.cpp chess.cpp display.cpp main.cpp econio.c -o sakk
```

Az NHF3 Szkeleton Teszt fordításához a teljes játék helyett:
```bash
g++ -std=c++17 -Wall storage.cpp chess.cpp display.cpp teszt.cpp -o skeleton_test
```

### Memóriakezelés
A régi C-s `debugmalloc.h` **le lett cserélve a `memtrace`-re** (`memtrace.h` / `memtrace.cpp`), amely egy egyetemi memóriaszivárgás-detektor, ami futásidőben nyomon követi a `new` és `delete` hívásokat. Az architektúra a szabványos C++ `new` és `delete` operátorokra támaszkodik. A `Game` vezérlő birtokolja a `BoardTree`-t, és amikor megsemmisül vagy újraindul, a `BoardTree::~BoardTree()` rekurzívan meghívja a `freeAll()` függvényt a memóriafa törléséhez, így automatikusan megelőzve a memóriaszivárgásokat.