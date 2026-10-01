#include "BladeEffects.h"

namespace
{
constexpr ColorLed BLACK = { 0, 0, 0, 0 };

int clampChannel(int value)
{
  return value < 0 ? 0 : (value > 255 ? 255 : value);
}

int scaleChannel(int value)
{
  return clampChannel(value) * EFFECT_BRIGHTNESS_PERCENT / 100;
}

ColorLed scaleBrightness(const ColorLed& c)
{
  return { scaleChannel(c.red), scaleChannel(c.green), scaleChannel(c.blue), scaleChannel(c.white) };
}

// Interpolazione lineare da 'from' a 'to': t va da 0 (from) a duration (to)
int lerpChannel(int from, int to, uint32_t t, uint32_t duration)
{
  return from + (to - from) * (int32_t)t / (int32_t)duration;
}

ColorLed lerpColor(const ColorLed& from, const ColorLed& to, uint32_t t, uint32_t duration)
{
  return { lerpChannel(from.red, to.red, t, duration), lerpChannel(from.green, to.green, t, duration),
           lerpChannel(from.blue, to.blue, t, duration), lerpChannel(from.white, to.white, t, duration) };
}
} // namespace

BladeEffect bladeEffectForBank(int bank)
{
  if (!BLADE_EFFECTS_ENABLED || bank < 0)
  {
    return BladeEffect::NONE;
  }
  if (bank == SLOT_WARM_RAINBOW)
  {
    return BladeEffect::WARM_RAINBOW;
  }
  if (bank == SLOT_ELECTRIC)
  {
    return BladeEffect::ELECTRIC;
  }
  return BladeEffect::NONE;
}

void BladeEffectPlayer::select(BladeEffect newEffect, const ColorLed& mainColor)
{
  effect = newEffect;
  baseColor = scaleBrightness(mainColor);
}

bool BladeEffectPlayer::isActive() const
{
  return effect != BladeEffect::NONE;
}

bool BladeEffectPlayer::runsDuringSwing() const
{
  return effect == BladeEffect::WARM_RAINBOW;
}

ColorLed BladeEffectPlayer::startColor() const
{
  switch (effect)
  {
    case BladeEffect::WARM_RAINBOW:
      return scaleBrightness(RAINBOW_STOPS[0].color);
    case BladeEffect::ELECTRIC:
      return baseColor;
    default:
      return BLACK;
  }
}

ColorLed BladeEffectPlayer::colorAt(uint32_t now) const
{
  switch (effect)
  {
    case BladeEffect::WARM_RAINBOW:
      return rainbowColorAt(now);
    case BladeEffect::ELECTRIC:
      return baseColor;
    default:
      return BLACK;
  }
}

void BladeEffectPlayer::restart(uint32_t now)
{
  origin = now;
  speedNum = 1;
  speedDen = 1;
  rngState ^= now;
  if (rngState == 0)
  {
    rngState = 0x9E3779B9u;
  }
}

void BladeEffectPlayer::restartPreview(uint32_t now, uint32_t durationMs)
{
  restart(now);
  if (durationMs > 0)
  {
    speedNum = RAINBOW_PERIOD_MS;
    speedDen = durationMs;
  }
}

ColorLed BladeEffectPlayer::resume(uint32_t now)
{
  lastChange = now;
  switch (effect)
  {
    case BladeEffect::WARM_RAINBOW:
      return rainbowColorAt(now);
    case BladeEffect::ELECTRIC:
      startElectricOn(now);
      return baseColor;
    default:
      return BLACK;
  }
}

bool BladeEffectPlayer::update(uint32_t now, ColorLed& out)
{
  switch (effect)
  {
    case BladeEffect::WARM_RAINBOW:
      if (now - lastChange < RAINBOW_FRAME_MS)
      {
        return false;
      }
      lastChange = now;
      out = rainbowColorAt(now);
      return true;

    case BladeEffect::ELECTRIC:
      if (now - lastChange < holdMs)
      {
        return false;
      }
      lit = !lit;
      lastChange = now;
      holdMs = lit ? randomBetween(ELECTRIC_ON_MIN_MS, ELECTRIC_ON_MAX_MS)
                   : randomBetween(ELECTRIC_OFF_MIN_MS, ELECTRIC_OFF_MAX_MS);
      out = lit ? baseColor : BLACK;
      return true;

    default:
      return false;
  }
}

ColorLed BladeEffectPlayer::rainbowColorAt(uint32_t now) const
{
  const uint64_t elapsed = (uint64_t)(uint32_t)(now - origin) * speedNum / speedDen;
  uint32_t t = (uint32_t)(elapsed % RAINBOW_PERIOD_MS);

  for (int i = 0; i < RAINBOW_STOP_COUNT; i++)
  {
    const RainbowStop& stop = RAINBOW_STOPS[i];
    if (t < stop.fadeMs)
    {
      const ColorLed& next = RAINBOW_STOPS[(i + 1) % RAINBOW_STOP_COUNT].color;
      return scaleBrightness(lerpColor(stop.color, next, t, stop.fadeMs));
    }
    t -= stop.fadeMs;
  }
  return scaleBrightness(RAINBOW_STOPS[0].color); // non raggiungibile: t < periodo
}

void BladeEffectPlayer::startElectricOn(uint32_t now)
{
  lit = true;
  lastChange = now;
  holdMs = randomBetween(ELECTRIC_ON_MIN_MS, ELECTRIC_ON_MAX_MS);
}

// Generatore xorshift32: veloce, deterministico, indipendente da random() di Arduino
uint32_t BladeEffectPlayer::randomBetween(uint32_t minValue, uint32_t maxValue)
{
  rngState ^= rngState << 13;
  rngState ^= rngState >> 17;
  rngState ^= rngState << 5;
  return minValue + rngState % (maxValue - minValue + 1);
}
