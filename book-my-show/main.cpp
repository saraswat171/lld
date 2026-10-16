#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

enum SeatStatus
{
    AVAILABLE,
    HOLD,
    BOOKED
};

enum BookingStatus
{
    SUCCESS,
    FAILURE
};

enum SeatCategory
{
    SILVER,
    GOLD,
    PLATINUM
};

class Seat
{
private:
    int seatNumber;
    int seatId;
    int row;
    SeatCategory category;

public:
    int getSeatNumber() const
    {
        return seatNumber;
    }

    int getSeatId() const
    {
        return seatId;
    }

    int getRow() const
    {
        return row;
    }

    SeatCategory getCategory() const
    {
        return category;
    }
};

class Movie
{
private:
    int movieId;
    string movieName;
    int movieDuration;
    string movieLanguage;
    vector<string> movieCast;

public:
    Movie(
        int id,
        string name,
        int duration,
        string language,
        vector<string> cast)
        : movieId(id),
          movieName(name),
          movieDuration(duration),
          movieLanguage(language),
          movieCast(cast)
    {
    }
};

class Show
{
private:
    int showId;
    int movieId;
    int screenId;
    int showTime;
    int showDate;

public:
    int getShowId() const
    {
        return showId;
    }
};

class ShowSeat
{
private:
    int showSeatId;
    int showId;
    int seatId;
    int heldByBookingId;
    SeatStatus status;

public:
    int getShowSeatId() const
    {
        return showSeatId;
    }

    bool isAvailable() const
    {
        return status == SeatStatus::AVAILABLE;
    }

    void holdSeat(int bookingId)
    {
        if (status != SeatStatus::AVAILABLE)
        {
            throw runtime_error("Seat is not available");
        }

        status = SeatStatus::HOLD;
        heldByBookingId = bookingId;
    }

    void bookSeat(int bookingId)
    {
        if (status != SeatStatus::HOLD)
        {
            throw runtime_error("Seat is not held");
        }

        if (heldByBookingId != bookingId)
        {
            throw runtime_error("Seat is held by another booking");
        }

        status = SeatStatus::BOOKED;
        heldByBookingId = -1;
    }

    void releaseHold(int bookingId)
    {
        if (status != SeatStatus::HOLD)
        {
            throw runtime_error("Seat is not held");
        }

        if (heldByBookingId != bookingId)
        {
            throw runtime_error("Seat is held by another booking");
        }

        status = SeatStatus::AVAILABLE;
        heldByBookingId = -1;
    }
};

class User
{
private:
    int userId;
    string userName;
    string phoneNumber;

public:
    int getUserId() const
    {
        return userId;
    }
};


