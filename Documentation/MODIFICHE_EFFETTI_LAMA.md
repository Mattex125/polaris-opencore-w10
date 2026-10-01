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
- **Fade-in e fade-out usavano il colore base dello slot**, con un salto visibile. Ora partono dal colore dell'effetto.
- **Log `snprintf` ogni 10 ms anche con `DEBUG` spento.** Rimossi.
- **Fade più preciso su tutti gli slot.** Prima arrivava a 240 invece di 255 e azzerava i canali sotto 30.

## Verifiche fatte

- Test sul PC del motore degli effetti (C++11/14/17, warning come errori):
  - continuità del colore millisecondo per millisecondo;
  - valori sempre entro i limiti;
  - rollover di `millis()`;
  - tempi dell'elettrico;
  - associazione slot → effetto con flag a 1 e a 0.
- L'arcobaleno nuovo coincide con quello della prima versione: al massimo 4 livelli
  di differenza su 229, dovuti all'arrotondamento della vecchia scala a 60 livelli.
- Ogni configurazione non valida provata blocca la compilazione con il messaggio giusto.
- `CoreLed.cpp` e `BladeEffects.cpp` compilati per Cortex-M4 (Teensy 3.2) con il core
  Teensy 1.59, in gnu++14 e gnu++17:
  - nessun warning nei file modificati;
  - stessi warning del core dell'originale;
  - tutti i simboli si risolvono.

## Non ancora verificato

- **Compilazione completa con PlatformIO:** il registro di PlatformIO non era raggiungibile
  dall'ambiente di sviluppo. Va fatta con `pio run` prima del flash.
- **Prova sulla spada:** checklist al punto 6 della guida.
