# Guida: caricare questa versione sulla spada da terminale

Questa guida spiega come compilare e caricare il firmware con gli **effetti lama**
(Arcobaleno caldo ed Elettrico) usando PlatformIO da terminale, e come tornare al
comportamento originale con un solo comando. Cosa è cambiato nel codice è descritto
in [MODIFICHE_EFFETTI_LAMA.md](MODIFICHE_EFFETTI_LAMA.md).

> **In breve**
> ```sh
> git clone https://github.com/Mattex125/polaris-opencore-w10.git
> cd polaris-opencore-w10
> git checkout claude/confident-cerf-1aaiks
> pio run -t upload                      # versione con effetti
> pio run -e teensy31_stock -t upload    # versione originale (rollback)
> ```

---

## Cosa fanno gli effetti

| Slot | Da fermo | In movimento (swing) | Colpo (clash) |
|---|---|---|---|
| **1 – ORANGE: Arcobaleno caldo** | rosso → viola → blu → rosa, ciclo di circa 10 s | continua senza interruzioni | colore clash dello slot, poi riprende |
| **2 – YELLOW: Elettrico** | si accende e si spegne a tempi irregolari con il colore principale dello slot | smette di lampeggiare e mostra il colore swing dello slot | colore clash dello slot, poi riprende |
| tutti gli altri | come il firmware originale | come l'originale | come l'originale |

- Durante la scelta del colore gli slot con effetto mostrano un'anteprima: l'arcobaleno fa un giro veloce, l'elettrico lampeggia.
- I colori principale, swing e clash degli slot si cambiano come sempre, dall'app o da seriale.
- Le impostazioni salvate sulla spada (colori, suoni, `config.ini`) **non vengono toccate** dal flash: il formato è lo stesso del firmware originale.

---

## 1. PlatformIO

Se usi già PlatformIO in VS Code, hai già tutto il necessario. Serve solo che il
comando `pio` funzioni nel terminale:

- **Il modo più semplice:** in VS Code apri la palette comandi (`Ctrl+Shift+P`) e
  scegli **"PlatformIO: Open PlatformIO Core CLI"**. Si apre un terminale in cui
  `pio` funziona già.
- **In un terminale normale**, se `pio` dà "comando non trovato", usa il percorso completo:
  - Windows (PowerShell): `& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" --version`
  - macOS / Linux: `~/.platformio/penv/bin/pio --version`

Se invece non hai PlatformIO:

| Sistema | Comando |
|---|---|
| Windows | installa Python da python.org (spunta "Add to PATH"), poi `py -m pip install -U platformio` |
| macOS | `brew install platformio` |
| Linux | `pipx install platformio` (oppure `sudo apt install pipx` prima) |

**Solo su Linux** servono le regole udev, altrimenti il caricamento fallisce per mancanza di permessi:

```sh
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
sudo udevadm control --reload-rules && sudo udevadm trigger
```

Poi scollega e ricollega la spada.

---

## 2. Scaricare questa versione

Prima volta:

```sh
git clone https://github.com/Mattex125/polaris-opencore-w10.git
cd polaris-opencore-w10
git checkout claude/confident-cerf-1aaiks
```

Se hai già la cartella, per aggiornarla:

```sh
cd polaris-opencore-w10
git fetch origin
git checkout claude/confident-cerf-1aaiks
git pull
```

---

## 3. (Facoltativo) Cambiare le impostazioni degli effetti

Tutto si trova in **un solo file**: `libraries/CoreLed/BladeEffectsConfig.h`.

| Cosa | Riga da cambiare |
|---|---|
| Spegnere tutti gli effetti | `#define BLADE_EFFECTS_ENABLED 0` |
| Cambiare lo slot dell'arcobaleno | `SLOT_WARM_RAINBOW = ORANGE;` (es. `PURPLE`, oppure `NO_SLOT`) |
| Cambiare lo slot dell'elettrico | `SLOT_ELECTRIC = YELLOW;` |
| Luminosità degli effetti | `EFFECT_BRIGHTNESS_PERCENT = 90;` (1–100) |
| Colori e durate dell'arcobaleno | tabella `RAINBOW_STOPS` (c'è già una variante con il rosa esplicito, commentata) |
| Velocità dell'elettrico | `ELECTRIC_ON_MIN_MS`, `ELECTRIC_ON_MAX_MS`, `ELECTRIC_OFF_MIN_MS`, `ELECTRIC_OFF_MAX_MS` |

Se inserisci un valore non valido (slot fuori da 0–7, due effetti sullo stesso slot,
luminosità oltre 100, durata 0…), **la compilazione si ferma con un messaggio in
italiano** che spiega cosa correggere. Un errore di configurazione non può quindi
arrivare sulla scheda.

---

## 4. Compilare

Dalla cartella del progetto:

```sh
pio run
```

- La prima volta PlatformIO scarica la toolchain Teensy e ci mette qualche minuto.
- Alla fine devi vedere `[SUCCESS]`. Se vedi `[FAILED]`, **non caricare nulla**:
  leggi il messaggio di errore (per i valori di configurazione è in italiano).

---

## 5. Caricare sulla spada

Prima di iniziare:

- [ ] batteria carica;
- [ ] cavo USB **dati** (non solo ricarica);
- [ ] app di configurazione (Scintilla/Gilthoniel) e monitor seriali **chiusi**, perché occupano la porta;
- [ ] durante la scrittura (pochi secondi) **non scollegare il cavo**.

Poi:

```sh
pio run -t upload
```

PlatformIO compila, apre **Teensy Loader**, riavvia la scheda in modalità
programmazione e scrive il firmware. Alla fine la spada si riavvia da sola.

> Grazie a `default_envs = teensy31` in `platformio.ini`, questo comando carica
> **solo** la versione con effetti, mai entrambe.

### Alternativa: `tycmd` (lo stesso strumento del flasher ufficiale)

```sh
pio run                     # crea .pio/build/teensy31/firmware.hex
```

- Windows: `Utilities\flash-opencore-builder\files\tycmd.exe upload .pio\build\teensy31\firmware.hex`
- macOS: `Utilities/flash-opencore-builder/files/tycmd upload .pio/build/teensy31/firmware.hex`

---

## 6. Provare la spada

Prova ogni voce e controlla che sia come descritto:

- [ ] **Slot normali** (es. RED, GREEN, BLUE): accensione, swing, clash e spegnimento come prima.
- [ ] **Scelta colore**: passando su ORANGE vedi un giro veloce dell'arcobaleno; su YELLOW vedi il lampeggio.
- [ ] **ORANGE da fermo**: l'arcobaleno parte dal rosso e scorre in modo fluido.
- [ ] **ORANGE swing**: l'arcobaleno non si ferma.
- [ ] **ORANGE clash**: lampo del colore clash, poi l'arcobaleno riprende.
- [ ] **YELLOW da fermo**: si accende e si spegne a tempi irregolari.
- [ ] **YELLOW swing**: smette di lampeggiare e resta sul colore swing.
- [ ] **YELLOW clash**: lampo del colore clash, poi riprende a lampeggiare.
- [ ] **Spegnimento** su ORANGE e YELLOW: dissolvenza senza salti di colore.

> L'effetto Elettrico lampeggia diverse volte al secondo. Se usi la spada davanti a
> un pubblico, tieni presente che le luci lampeggianti possono dare fastidio alle
> persone fotosensibili.

---

## 7. Tornare al comportamento originale (rollback)

Senza modificare nessun file:

```sh
pio run -e teensy31_stock -t upload
```

Carica lo stesso firmware con gli effetti disattivati: tutti gli slot tornano
esattamente come nel firmware originale. Per rimettere gli effetti: `pio run -t upload`.

> **Non** usare i vecchi file `OpenCore.1.9.17_*.hex` in `Utilities/`: sono di una
> versione precedente con un formato di configurazione diverso, e potrebbero
> riportare colori e suoni alle impostazioni di fabbrica.

---

## Problemi comuni

| Problema | Soluzione |
|---|---|
| `pio: comando non trovato` | vedi il punto 1 (terminale "PlatformIO Core CLI" o percorso completo) |
| Teensy Loader resta in attesa ("press button") | chiudi l'app di configurazione e i monitor seriali, scollega e ricollega l'USB, poi rilancia `pio run -t upload` |
| Linux: `permission denied` sulla porta | installa le regole udev (punto 1) e ricollega la spada |
| La compilazione si ferma con `static assertion failed: …` | un valore in `BladeEffectsConfig.h` non è valido: il messaggio dice quale |
| L'arcobaleno è troppo veloce o lento | cambia i `fadeMs` nella tabella `RAINBOW_STOPS` (sono millisecondi) |
