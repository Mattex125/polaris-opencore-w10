# LamaDiLuce - Anima EVO
[![Build Status](https://img.shields.io/github/forks/LamaDiLuce/polaris-opencore.svg)](https://github.com/LamaDiLuce/polaris-opencore)
[![Build Status](https://img.shields.io/github/stars/LamaDiLuce/polaris-opencore.svg)](https://github.com/LamaDiLuce/polaris-opencore)
[![License](https://img.shields.io/github/license/LamaDiLuce/polaris-opencore.svg)](https://github.com/LamaDiLuce/polaris-opencore)
[![GitHub issues](https://img.shields.io/github/issues/LamaDiLuce/polaris-opencore)](https://github.com/LamaDiLuce/polaris-opencore/issues)
[![Donate](https://img.shields.io/badge/Donate-PayPal-blue.svg)](https://www.paypal.me/ludosport/)
[![GitHub issues](https://img.shields.io/github/issues/Naereen/StrapDown.js.svg)](https://GitHub.com/Naereen/StrapDown.js/issues/)

Polaris, the combat ready modular hilt born in 2015 becomes an open source project in 2020 powered with the new Anima EVO electronic module.

To read the entire story you can follow [this link](https://www.lamadiluce.it/polaris-evo/).

[![Polaris Anima EVO](https://www.lamadiluce.it/wp-content/uploads/2020/06/FBcover-01-e1596115323936.png)](https://www.lamadiluce.it/polaris-evo/)

---

## Table of contents
- [Effetti lama (IT)](#effetti-lama)
- [Introduction](#introduction)
- [Features](#features)
- [Setup](#setup)
- [Contributing](#contributing)
- [Coulcil members](#coulcil-members)
- [FAQ](#faq)
- [Donations](#donations)
- [License](#license)

---

## Effetti lama

Questa versione del firmware aggiunge due effetti animati alla lama. Tutti gli altri slot
funzionano esattamente come nel firmware originale.

| Slot | Effetto | Da fermo | In movimento (swing) | Colpo (clash) |
|---|---|---|---|---|
| 1 – ORANGE | **Arcobaleno caldo** | rosso → viola → blu → rosa, ciclo di circa 10 s | continua | colore clash dello slot, poi riprende |
| 2 – YELLOW | **Elettrico** | si accende e si spegne a tempi irregolari, con il colore principale dello slot | si ferma sul colore swing dello slot | colore clash dello slot, poi riprende |

### Come si mette sulla spada

Dalla cartella del progetto, con la spada collegata via USB e l'app di configurazione chiusa:

```sh
git checkout master
git pull
pio run              # deve finire con [SUCCESS]
pio run -t upload    # carica il firmware sulla spada
```

Colori, suoni e impostazioni salvati sulla spada non vengono toccati. La guida passo passo
(PlatformIO su Windows, macOS e Linux, problemi comuni) è in
[Documentation/GUIDA_FLASH_TERMINALE.md](Documentation/GUIDA_FLASH_TERMINALE.md).

### Come si usa

- Accendi la spada e scegli lo slot come fai di solito durante la selezione del colore:
  ORANGE è l'arcobaleno, YELLOW è l'elettrico. Durante la selezione questi due slot
  mostrano un'anteprima dell'effetto.
- In alternativa, con la spada collegata via USB, puoi scegliere lo slot da seriale con
  `B=1` (arcobaleno) o `B=2` (elettrico).
- La spada ricorda l'ultimo slot usato anche dopo lo spegnimento.

### Come si riavvia

- Dopo `pio run -t upload` la scheda si riavvia da sola con il nuovo firmware: scollega
  l'USB e la spada è pronta.
- Ogni volta che cambi qualcosa nel file di configurazione devi ricompilare e ricaricare
  con `pio run -t upload`.
- Se il caricamento resta in attesa, chiudi l'app di configurazione e i monitor seriali,
  scollega e ricollega l'USB, poi rilancia `pio run -t upload`.

### Come si cambia (slot, luminosità, colori)

Tutto si trova in un solo file, `libraries/CoreLed/BladeEffectsConfig.h`:

| Cosa | Riga |
|---|---|
| Slot dell'arcobaleno | `SLOT_WARM_RAINBOW = ORANGE;` |
| Slot dell'elettrico | `SLOT_ELECTRIC = YELLOW;` |
| Luminosità degli effetti | `EFFECT_BRIGHTNESS_PERCENT = 90;` (1–100) |
| Colori e tempi dell'arcobaleno | tabella `RAINBOW_STOPS` (c'è già pronta una variante con il rosa esplicito) |
| Velocità dell'elettrico | `ELECTRIC_ON_MIN_MS`, `ELECTRIC_ON_MAX_MS`, `ELECTRIC_OFF_MIN_MS`, `ELECTRIC_OFF_MAX_MS` |

Gli slot si indicano per nome (`RED`, `ORANGE`, `YELLOW`, `GREEN`, `WHITE`, `ACQUA`, `BLUE`,
`PURPLE`) o con il numero da 0 a 7. Se un valore non è valido, la compilazione si ferma con un
messaggio in italiano e sulla spada non arriva nulla.

### Come si toglie

| Cosa vuoi fare | Come |
|---|---|
| Tornare al comportamento originale senza modificare file | `pio run -e teensy31_stock -t upload` |
| Disattivare gli effetti nel codice | in `BladeEffectsConfig.h` metti `#define BLADE_EFFECTS_ENABLED 0`, poi `pio run -t upload` |
| Togliere un solo effetto | `SLOT_WARM_RAINBOW = NO_SLOT;` oppure `SLOT_ELECTRIC = NO_SLOT;`, poi `pio run -t upload` |
| Rimettere gli effetti | flag a 1, poi `pio run -t upload` |

Con gli effetti disattivati il LED si comporta al bit come nel firmware originale. In tutti
i casi colori, suoni e impostazioni salvati sulla spada restano invariati.

Cosa è cambiato nel codice e come è stato verificato:
[Documentation/MODIFICHE_EFFETTI_LAMA.md](Documentation/MODIFICHE_EFFETTI_LAMA.md).

---

## Introduction
[LudoSport](https://www.ludosport.net/) masters created in 2006 a combat discipline with the lightsaber that spread a common language among the people practicing it, however the athletes didn’t have a reference for their equipment yet.

In 2015 [LamaDiLuce](https://www.lamadiluce.it/) gave birth to Polaris, a new saber that would become the reference for hundreds of lightsaber enthusiasts who wanted to compete with an elegant weapon.

After 5 years, and after thousands of feedbacks from worldwide users, the new generation of saber is ready and you can contribute to the evolution of Polaris too.

---

## Features

- RGBW customization for the LED
- No pushbutton required
  - Gestures to arm / disarm
  - Blade color can be selected among pre-sets during arming
  - Kill switch
- Mute function activated by a tap on the hilt during arming
- Electronic powered by a Cortex-M4
- Dedicated [PC software](http://sabers.amazer.uk/?p=gilthoniel) for advanced customization and firmware upgrade
- Code built with [Automaton](https://github.com/tinkerspy/Automaton/wiki) state machine modules
- [Smoothswing experience](https://github.com/LamaDiLuce/polaris-opencore/wiki/SmoothSwing)

---

## Setup
In order to be able to contributre to this project, compile and upload the FW to your lightsaber, you need to:
- undestand C++
- be familiar with Arduino
- download and install a bunch of software
- get used to Git and GitHub in order to commit your changes
- get familiar with [Automaton framework](https://github.com/LamaDiLuce/polaris-opencore/wiki/Automaton)

We strongly recommend to install VS Code and PlatformIO extension.

- [Get ready with GitHub](https://github.com/LamaDiLuce/polaris-opencore/wiki/Git)
- [Configure VS Code plus PlatformIO](https://github.com/LamaDiLuce/polaris-opencore/wiki/VSCode-PlatformIO)

---

## Contributing
Any developer who wants to contribute to the project shall propose a completed merge into the master as a significant feature update for a release to the public (just one pull request with all the relevant details).

The changes are first reviewed in terms of coding, compatibility with previous versions and Scintilla SW. If the changes are ok and in line with the purpose of the lightsaber FW, the council gives the approval and a new release candidate is built.

This new release candidate is testes by selected developers and, if all the tests are passed, the master branch is updated and a new official FW version is released.

---

## Council members
| Daniele *"denken0bi"* | Andrea *"AnFive"* | Carlos *"karldeux"* | Fabio "*FlamingBear*" | Davide "*FarFlyer"* | Ian *"Nuntis"* |
| :---: | :---: | :---: | :---: | :---: | :---: |
| <img src="Documentation/Images/Council/Daniele.jpg" alt="Daniele" title="Daniele" height="80"/> | <img src="Documentation/Images/Council/Andrea.jpg" alt="Andrea" title="Andrea" height="80"/> | <img src="Documentation/Images/Council/Carlos.jpg" alt="Carlos" title="Carlos" height="80"/> | <img src="Documentation/Images/Council/Fabio.jpg" alt="Fabio" title="Fabio" height="80"/> |  <img src="Documentation/Images/Council/Davide.jpg" alt="Davide" title="Davide" height="80"/> | <img src="Documentation/Images/Council/Ian.jpg" alt="Ian" title="Ian" height="80"/> |

---

## FAQ
FAQ and troubleshooting can be found [here](https://github.com/LamaDiLuce/polaris-opencore/wiki/FAQ)

---

## Donations

You can still support the project with [development suggestions](https://bit.ly/Polaris-OpenCore-suggestion) or with a [free donation](https://www.paypal.me/ludosport/) that will be used for new hardware evolutions.

[![Donate](https://img.shields.io/badge/Donate-PayPal-blue.svg)](https://www.paypal.me/ludosport/)

---

## License
[![License](https://img.shields.io/github/license/LamaDiLuce/polaris-opencore.svg)](https://github.com/LamaDiLuce/polaris-opencore)

---
