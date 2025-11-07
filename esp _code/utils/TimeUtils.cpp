#include "TimeUtils.h"

String TimeUtils::getTimestamp() {
  time_t now = time(nullptr);
  struct tm* ptm = localtime(&now);
  char buffer[20];
  sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d", 
          ptm->tm_mday, ptm->tm_mon + 1, ptm->tm_year + 1900,
          ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  return String(buffer);
}

String TimeUtils::generateSessionId() {
  time_t now = time(nullptr);
  struct tm* ptm = localtime(&now);
  char buffer[16];
  sprintf(buffer, "%04d%02d%02d_%02d%02d%02d", 
          ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
          ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  return String(buffer);
}