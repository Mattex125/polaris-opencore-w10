#pragma once

// =============================================================================
//  EFFETTI LAMA - configurazione
//
//  Questo e' l'unico file da modificare per attivare/disattivare gli effetti,
//  cambiare slot, luminosita', colori e tempi.
//  Ogni valore viene controllato in compilazione (in fondo al file): se un
//  valore non e' valido il firmware non compila, quindi non arriva alla scheda.
// =============================================================================

#include <stdint.h>
#include "CoreCommon.h"

// -----------------------------------------------------------------------------
// 1) Interruttore generale
//    1 = effetti attivi
//    0 = comportamento identico al firmware originale
//    Senza toccare questo file si puo' compilare la versione originale con:
//      pio run -e teensy31_stock -t upload
// -----------------------------------------------------------------------------
#ifndef BLADE_EFFECTS_ENABLED
#define BLADE_EFFECTS_ENABLED 1
#endif

// -----------------------------------------------------------------------------
// 2) Slot a cui assegnare gli effetti (0..7)
//    0=RED 1=ORANGE 2=YELLOW 3=GREEN 4=WHITE 5=ACQUA 6=BLUE 7=PURPLE
//    NO_SLOT = effetto non assegnato a nessuno slot
// -----------------------------------------------------------------------------
static constexpr int NO_SLOT = -1;
static constexpr int SLOT_WARM_RAINBOW = ORANGE; // slot 1: arcobaleno caldo
static constexpr int SLOT_ELECTRIC = YELLOW;     // slot 2: elettrico (accendo/spengo)

// -----------------------------------------------------------------------------
// 3) Luminosita' degli effetti in percentuale (1..100)
//    Scala il colore senza cambiarne la tinta. Swing e clash usano i colori
//    dello slot esattamente come nel firmware originale.
// -----------------------------------------------------------------------------
static constexpr int EFFECT_BRIGHTNESS_PERCENT = 90;

// -----------------------------------------------------------------------------
// 4) Arcobaleno caldo
//    Ogni tappa sfuma verso la successiva in fadeMs millisecondi; l'ultima
//    sfuma di nuovo verso la prima. Colori a piena scala {R, G, B, W} 0..255.
// -----------------------------------------------------------------------------
struct RainbowStop
{
  ColorLed color;
  uint16_t fadeMs;
};

static constexpr RainbowStop RAINBOW_STOPS[] = {
  { { 255, 0,   0, 0 },  750 }, // rosso -> viola
  { { 255, 0, 255, 0 }, 7110 }, // viola -> blu
  { {   0, 0, 255, 0 }, 2380 }, // blu   -> rosso (la sfumatura finale e' il rosa)
  // Variante con tappa rosa esplicita: sostituisci la riga del blu con queste due
  // { {   0, 0, 255, 0 }, 1190 }, // blu  -> rosa
  // { { 255, 0, 128, 0 }, 1190 }, // rosa -> rosso
};

// Ogni quanto si aggiorna il colore (10 ms = 100 aggiornamenti al secondo)
static constexpr uint16_t RAINBOW_FRAME_MS = 10;

// -----------------------------------------------------------------------------
// 5) Elettrico
//    Accende e spegne il colore principale dello slot con tempi casuali,
//    per un effetto "scarica elettrica". In movimento smette di lampeggiare
//    e mostra il colore swing dello slot.
// -----------------------------------------------------------------------------
static constexpr uint16_t ELECTRIC_ON_MIN_MS = 20;
static constexpr uint16_t ELECTRIC_ON_MAX_MS = 160;
static constexpr uint16_t ELECTRIC_OFF_MIN_MS = 10;
static constexpr uint16_t ELECTRIC_OFF_MAX_MS = 60;

// =============================================================================
//  Controlli in compilazione - non modificare sotto questa riga
// =============================================================================

static constexpr int RAINBOW_STOP_COUNT = sizeof(RAINBOW_STOPS) / sizeof(RAINBOW_STOPS[0]);

static constexpr bool isValidEffectSlot(int slot)
{
  return slot == NO_SLOT || (slot >= 0 && slot <= COLORS);
}

static constexpr bool isValidChannel(int value)
{
  return value >= 0 && value <= 255;
}

static constexpr bool isValidRainbowStop(const RainbowStop& stop)
{
  return stop.fadeMs > 0 && isValidChannel(stop.color.red) && isValidChannel(stop.color.green) &&
         isValidChannel(stop.color.blue) && isValidChannel(stop.color.white);
}

static constexpr bool rainbowStopsValid(int i = 0)
{
  return i >= RAINBOW_STOP_COUNT || (isValidRainbowStop(RAINBOW_STOPS[i]) && rainbowStopsValid(i + 1));
}

static constexpr uint32_t rainbowPeriodMs(int i = 0)
{
  return i >= RAINBOW_STOP_COUNT ? 0 : RAINBOW_STOPS[i].fadeMs + rainbowPeriodMs(i + 1);
}

static constexpr uint32_t RAINBOW_PERIOD_MS = rainbowPeriodMs();

static_assert(BLADE_EFFECTS_ENABLED == 0 || BLADE_EFFECTS_ENABLED == 1,
              "BLADE_EFFECTS_ENABLED deve valere 0 oppure 1");
static_assert(isValidEffectSlot(SLOT_WARM_RAINBOW), "SLOT_WARM_RAINBOW deve essere 0..7 oppure NO_SLOT");
static_assert(isValidEffectSlot(SLOT_ELECTRIC), "SLOT_ELECTRIC deve essere 0..7 oppure NO_SLOT");
static_assert(SLOT_WARM_RAINBOW == NO_SLOT || SLOT_WARM_RAINBOW != SLOT_ELECTRIC,
              "Due effetti non possono usare lo stesso slot");
static_assert(EFFECT_BRIGHTNESS_PERCENT >= 1 && EFFECT_BRIGHTNESS_PERCENT <= 100,
              "EFFECT_BRIGHTNESS_PERCENT deve essere 1..100");
static_assert(rainbowStopsValid(), "Ogni tappa dell'arcobaleno deve avere colori 0..255 e fadeMs > 0");
static_assert(RAINBOW_FRAME_MS > 0, "RAINBOW_FRAME_MS deve essere > 0");
static_assert(ELECTRIC_ON_MIN_MS > 0 && ELECTRIC_ON_MIN_MS <= ELECTRIC_ON_MAX_MS,
              "Tempi ELECTRIC_ON non validi (serve 0 < MIN <= MAX)");
static_assert(ELECTRIC_OFF_MIN_MS > 0 && ELECTRIC_OFF_MIN_MS <= ELECTRIC_OFF_MAX_MS,
              "Tempi ELECTRIC_OFF non validi (serve 0 < MIN <= MAX)");
