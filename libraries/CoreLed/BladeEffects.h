#pragma once

#include <stdint.h>
#include "BladeEffectsConfig.h"

// Effetti animati della lama.
// Il modulo non usa ne' Arduino ne' Automaton: riceve il tempo (millis) da chi
// lo chiama e restituisce i colori da mostrare. Tutti i calcoli sui tempi sono
// sicuri anche quando millis() ricomincia da zero (dopo ~49 giorni).

enum class BladeEffect : uint8_t
{
  NONE,
  WARM_RAINBOW,
  ELECTRIC
};

// Effetto assegnato allo slot (NONE se gli effetti sono disattivati)
BladeEffect bladeEffectForBank(int bank);

class BladeEffectPlayer
{
public:
  // Imposta l'effetto dello slot corrente: chiamato a ogni cambio di slot
  void select(BladeEffect newEffect, const ColorLed& mainColor);

  bool isActive() const;
  // true se l'effetto continua anche durante lo swing (arcobaleno)
  bool runsDuringSwing() const;

  // Colore a cui arriva il fade-in di accensione
  ColorLed startColor() const;
  // Colore "stabile" dell'effetto in questo istante (usato dal fade-out)
  ColorLed colorAt(uint32_t now) const;

  // Riparte dall'inizio: chiamato alla fine del fade-in di accensione
  void restart(uint32_t now);
  // Come restart(), ma l'arcobaleno compie un giro intero in durationMs (anteprima)
  void restartPreview(uint32_t now, uint32_t durationMs);
  // Rientro nello stato acceso (accensione, fine swing, fine clash):
  // restituisce il colore da mostrare subito
  ColorLed resume(uint32_t now);
  // Da chiamare a ogni ciclo: true se c'e' un nuovo colore da scrivere in 'out'
  bool update(uint32_t now, ColorLed& out);

private:
  ColorLed rainbowColorAt(uint32_t now) const;
  void startElectricOn(uint32_t now);
  uint32_t randomBetween(uint32_t minValue, uint32_t maxValue);

  BladeEffect effect = BladeEffect::NONE;
  ColorLed baseColor = { 0, 0, 0, 0 }; // colore principale dello slot, gia' scalato
  uint32_t origin = 0;                 // arcobaleno: istante di inizio del ciclo
  uint32_t speedNum = 1;               // arcobaleno: velocita' = speedNum / speedDen
  uint32_t speedDen = 1;
  uint32_t lastChange = 0;             // ultimo frame (arcobaleno) o commutazione (elettrico)
  uint32_t holdMs = 0;                 // elettrico: durata dello stato attuale
  bool lit = true;                     // elettrico: acceso o spento
  uint32_t rngState = 0x9E3779B9u;
};
