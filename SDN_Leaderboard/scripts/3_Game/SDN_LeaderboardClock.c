class SDN_LeaderboardClock
{
    // Seconds since 2020-01-01 UTC, fitting a signed 32-bit integer through 2087.
    // Server wall clock is required; backwards movement suspends scoring.
    static bool Leap(int year)
    {
        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    }

    static int Now()
    {
        int year, month, day, hour, minute, second;
        GetYearMonthDayUTC(year, month, day);
        GetHourMinuteSecondUTC(hour, minute, second);
        int checkYear, checkMonth, checkDay;
        GetYearMonthDayUTC(checkYear, checkMonth, checkDay);
        if (year != checkYear || month != checkMonth || day != checkDay)
            return -1; // Midnight raced the two native reads; next event can retry.
        if (year < 2020 || year > 2087)
            return -1;
        int days;
        for (int y = 2020; y < year; y++)
        {
            days += 365;
            if (Leap(y)) days++;
        }
        array<int> lengths = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (Leap(year)) lengths[1] = 29;
        for (int m = 1; m < month; m++) days += lengths[m - 1];
        days += day - 1;
        return days * 86400 + hour * 3600 + minute * 60 + second;
    }
}
