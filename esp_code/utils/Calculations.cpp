#include "Calculations.h"

float Calculations::calculateMoistureContent(float currentWeight, float initialWeight) {
  if (currentWeight <= 0 || initialWeight <= 0) {
    return 0;
  }
  
  float moisture = (currentWeight / initialWeight) * 100;
  
  if (moisture > 100) moisture = 100;
  if (moisture < 0) moisture = 0;
  
  return moisture;
}