# Modifiche: effetti lama (Arcobaleno caldo ed Elettrico)

Riepilogo delle modifiche rispetto al firmware originale (`0c76117`) e alla prima
versione degli effetti (`0d9efe3`). Per compilare e caricare il firmware vedi
[GUIDA_FLASH_TERMINALE.md](GUIDA_FLASH_TERMINALE.md).

## Comportamento

| Slot | Da fermo | Swing | Clash |
|---|---|---|---|
| 1 – ORANGE: **Arcobaleno caldo** | rosso → viola → blu → (rosa) → rosso, ciclo di 10,24 s | continua | colore clash dello slot, poi riprende |
| 2 – YELLOW: **Elettrico** | colore principale dello slot acceso/spento a tempi casuali | colore swing dello slot, fisso | colore clash dello slot, poi riprende |
| altri slot | come l'originale | come l'originale | come l'originale |

- Luminosità degli effetti: 90%, ottenuta scalando il colore (la tinta non cambia).
- Accensione e spegnimento sfumano dal colore reale dell'effetto, senza salti.
- Nella scelta colore gli slot con effetto mostrano un'anteprima di 500 ms (stessa durata dell'originale).

## Come si configura

Tutto si trova in `libraries/CoreLed/BladeEffectsConfig.h`:

- `BLADE_EFFECTS_ENABLED`: 1 = effetti attivi, 0 = comportamento originale;
- slot assegnati a ciascun effetto;
- luminosità;
- tabella dei colori dell'arcobaleno (variante con rosa esplicito già pronta, commentata);
- tempi dell'elettrico.

I valori non validi bloccano la compilazione con un messaggio in italiano.

Rollback senza toccare file: `pio run -e teensy31_stock -t upload`.

## File modificati

| File | Modifica |
|---|---|
| `libraries/CoreLed/BladeEffectsConfig.h` | **nuovo**: tutte le impostazioni e i controlli in compilazione |
| `libraries/CoreLed/BladeEffects.h/.cpp` | **nuovo**: motore degli effetti, senza dipendenze da Arduino o Automaton |
| `libraries/CoreLed/CoreLed.h/.cpp` | integrazione dell'effetto negli stati ARMED/SWING/ARM/DISARM; rimossi strobo, timer dell'effetto, log per tick e codice commentato |
| `libraries/CoreCommon/CoreCommon.h` | tornato identico all'originale |
| `platformio.ini` | `default_envs = teensy31` e nuovo env `teensy31_stock` |
| `Documentation/GUIDA_FLASH_TERMINALE.md` | **nuovo**: guida al flash da terminale |

Non toccati: formato di `config.ini`, protocollo seriale (compatibile con
Scintilla/Gilthoniel), suoni, ricarica, batteria, movimento.

## Problemi della prima versione corretti

- **L'arcobaleno si bloccava durante lo swing.** I timer di Automaton contano dall'ingresso
  nello stato corrente, quindi il timer impostato in ARMED non scadeva in SWING.
  Ora il colore dipende dal tempo e non usa più quei timer.
- **Gli slot normali scendevano al 25% una volta accesi.** Tornano a piena luminosità come l'originale.
- **Il limite di luminosità tagliava i canali e cambiava la tinta** (es. arancione → giallo).
  Ora il colore viene scalato in proporzione.
- **Fade-in e fade-out usavano il colore base dello slot**, con un salto visibile. Ora partono dal colore dell'effetto
  e, sugli slot con effetto, arrivano esattamente a quel colore. Gli slot normali usano la formula originale.
- **Log `snprintf` ogni 10 ms anche con `DEBUG` spento.** Rimossi.

## Garanzia sul comportamento originale

Con `BLADE_EFFECTS_ENABLED 0` (o con l'env `teensy31_stock`), e su tutti gli slot senza
effetto, il LED si comporta **al bit come il firmware originale**: stessi colori, negli
stessi istanti, con gli stessi tempi di scelta colore e di blocco del loop. È verificato in
simulazione (vedi sotto).

## Verifiche fatte

**Compilazione completa con PlatformIO** (`pio run`, entrambi gli env):

- piattaforma Teensy 6.0.0 (GCC 15.2, Teensyduino 1.62) e piattaforma vecchia 4.17.0
  (GCC 5.4, Teensyduino 1.57): `[SUCCESS]` in entrambi i casi;
- nessun warning nei file modificati, con entrambe le piattaforme; con la piattaforma attuale
  gli altri warning sono gli stessi 10 dell'originale (`CoreComms`, `CoreSettings`, core Teensy);
- memoria: RAM 15032 byte (+64 rispetto all'originale), Flash 117232 byte su 262144;
- nell'env `teensy31_stock` la funzione che sceglie l'effetto restituisce sempre "nessuno"
  (controllato sul codice macchina).

**Simulazione della spada sul PC.** Il vero `CoreLed.cpp` e la vera macchina a stati
Automaton girano su un orologio simulato, con AddressSanitizer e UBSan attivi. Vengono
registrati tutti i colori scritti sui LED, su:

- 5 scenari: uso completo, scelta colore, 60 s accesa con swing di 20 s, accensione diretta,
  scelta colore interrotta;
- tutti gli 8 slot, con l'orologio che parte da zero oppure a 3 s dal giro di `millis()`
  (dopo 49 giorni);
- 400 sequenze casuali di eventi, anche non validi, con USB collegata e scollegata a caso.

Risultati:

- nessun crash e nessun errore dei sanitizer in 1520 esecuzioni;
- flag a 0 e slot normali: uguali al bit all'originale in tutti i 130 confronti sugli scenari
  e in tutte le 400 sequenze casuali;
- con gli effetti attivi: stessi tempi di scelta colore, stessi stati finali, stessi salvataggi
  in memoria e stesso blocco massimo del loop dell'originale (1833 ms, watchdog a 4000 ms);
- **arcobaleno:**
  - ogni colore coincide al millisecondo con la palette, anche attraverso swing, colpi e
    giro di `millis()`;
  - in uno swing di 20 s si aggiorna 2000 volte (la prima versione: 0 volte, ferma);
  - il colpo mostra il colore clash dello slot;
- **elettrico:**
  - acceso per 20–160 ms e spento per 10–60 ms, sempre alternato;
  - nello swing resta fisso sul colore swing;
  - dopo swing e colpo riparte acceso;
- fade di accensione e spegnimento senza salti; anteprime nella scelta colore come previsto.

**Test del motore degli effetti** (C++11/14/17, warning come errori):

- continuità e range dei colori;
- tempi dell'elettrico;
- associazione slot → effetto;
- ogni configurazione non valida blocca la compilazione con il messaggio giusto.

## Non ancora verificato

- **Prova sulla spada vera:** la simulazione riproduce il codice, non l'hardware. Segui la
  checklist al punto 6 della guida.
