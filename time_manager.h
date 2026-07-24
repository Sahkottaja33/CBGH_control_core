#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

unsigned long getCurrentTimestamp();
void updateTimeManager();
void initTimeManager(unsigned long launchEpoch);
bool handleSerialTimeCorrection();
unsigned long getSkippedSeconds();
unsigned long getLastLoggedTimestamp();
void confirmLoggedTimestamp(unsigned long ts);

#endif
