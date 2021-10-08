#ifndef TIMEZONE_H
#define TIMEZONE_H

const int UTCDifference = 1;

RtcDateTime ApplyTimezone(RtcDateTime utc)
{
	// normal timezone difference from UTC
	utc += UTCDifference * 3600;

	// Daylight Saving Time
	// determine last Sunday in March of current year
	RtcDateTime start(utc.Year(), 3, 25, 2, 0, 0);
	while (start.DayOfWeek() != 0) {
		start += 86400;
	}

	// determine last Sunday in October of current year
	RtcDateTime end(utc.Year(), 10, 25, 3, 0, 0);
	while (end.DayOfWeek() != 0) {
		end += 86400;
	}

	if (utc.Month() >= start.Month() && utc.Month() <= end.Month()) {

		if (utc.Month() == start.Month() && utc.Day() >= start.Day()
		    && utc.Hour() >= start.Hour()
		    || utc.Month() == end.Month() && utc.Day() <= end.Day()
		    && utc.Hour() <= end.Hour()) {
			utc += 3600;
		} else if (utc.Month() > start.Month() && utc.Month() < end.Month()) {
			utc += 3600;
		}
	}

	return utc;
}

#endif